# W3 公开笔记 · 虚表与对象内存布局全景

> 学员：韦扬 ｜ 阶段：S1（W1–W12）C++ 内存模型 + Windows 进程/内存 ｜ 周期：2026-09-13 ~ 2026-09-16
> 性质：W3D4 收口笔记，把 D1（单继承 vptr）/ D2（多继承双 vptr）/ D3（虚表三连）三份证据串成一张「任意 C++ 对象布局全图」。
> 验收：三节齐全 + 三张自画布局图 + 数字与 MSVC 实测一致。

---

## 一句话

W3 你从 W2「能算死的偏移」升级到「能看懂活的对象布局」：对象头里塞一个 **vptr** 指向一张 **vtable**；多继承下变出**双 vptr + this 调整**；虚表自己躺在**只读段 `.rdata`**。这篇把三天证据钉成一张全图，并接回 W1 的进程内存四区图。

---

## 1. 单继承：一个 vptr 指向一张 vtable

**直觉**：带虚函数的类，编译器在对象头部（offset 0）自动塞 8 字节指针（vptr），指向一张「虚函数跳转表」（vtable）。vtable 是只读（`.rdata`）的函数指针数组，按虚函数声明顺序排，全类共享一份。

**虚调用四步**（x64）：
1. 读对象头 8 字节 → 拿到 vptr
2. 跳到 vptr 指向的 vtable
3. 按虚函数声明顺序按下标取 slot（如 `foo` 是 slot 0）
4. `call` 该地址，并把 `this` 作隐藏参数走 `rcx`

**大小怎么来的**（你亲手实测，曾纠正导师错值）：
- `sizeof(Base)=16` = vptr(8) + x(4) + 尾部 padding(4)（最大对齐值 8 的倍数）
- `sizeof(Derived)=24` = **Base 子对象整块(16)** + y@16(4) + 尾部 padding(4)

> ⚠️ **关键坑（D1 实测）**：基类子对象在派生类里是**整块搬运**，其尾部 padding **不能被**派生类新成员复用。导师曾误算成 Derived=16/y@12，你实测证明 **24/y@16 才对**。

**手动经 vptr 调虚函数**（D1 核心动手，已跑通）：
```cpp
typedef void(*VFunc)(Base*);
VFunc f = (VFunc)vtable[0];
printf("manual: "); f(&b);   // 输出与 b.foo() 一字不差
```

**布局图（单继承）**：
```
offset 0  : [vptr 8B][x 4B][pad 4B]     <- Base 子对象 16B
offset 16 : [y 4B][pad 4B]              <- Derived 自己的 8B
总 24 字节
```

---

## 2. 多继承：双 vptr + 双 vtable + this 调整

**直觉**：`C : A, B`（A、B 都有虚函数）时，外部代码可能拿 `A*`（把 C 当 A）或 `B*`（把 C 当 B）。C++ 保证两种视角都像纯正的基类，但 A、B 的虚表内容不同，无法合成一张。于是编译器让 C 背着**两个 vptr**：
- A 是主基类摆最前，vptrA @ **offset 0**
- B 子对象 @ **offset 16**，vptrB @ **offset 16**

**this 调整（最反直觉）**：`B* bp = &c;` 时 `bp` 指向 B 子对象开头（offset 16），不是整个 C 开头（offset 0）。所以经 vptrB 手动调虚函数，**this 必须传 `(char*)&c + 16`**——传错就读到错位内存的垃圾。

**MSVC 铁证**（`cl /d1reportSingleClassLayoutC`）：
- 两个 `vftable` 块
- 第二个（B 的）`offset-to-top = -16`（负数 = this 调整的机器级证据）
- `this adjustor: a=0, b=16`

**手动调**（D2 核心动手，已跑通）：
```cpp
fa((void*)&c);            // 经 A 的 vptr，this=&c（offset 0）
fb((char*)&c + 16);       // 经 B 的 vptr，this= B 子对象起点（offset 16）
```

**布局图（多继承 C : A, B）**：
```
offset 0  : [A-vptr 8B][A::x 4B][pad 4B]     <- A 子对象 16B
offset 16 : [B-vptr 8B][B::y 4B][pad 4B]     <- B 子对象 16B，第二个 vptr 在这！
offset 32 : [C::z 4B][pad 4B]
总 40 字节
```
> 注意：B 的 vptr 在 **offset 16，不是 8**（前面整块 A 子对象占了 16 字节）。

---

## 3. 虚表三连（接回 W1 进程内存四区图）

### 3.1 基类析构必须 virtual（否则派生部分泄漏）
同一个 `BaseA* p = new DerivedA(); delete p;`，只改 `~BaseA` 是否 virtual：

| `~BaseA` | 输出 | 含义 |
|---|---|---|
| **非** virtual | 只有 `~BaseA` | `~DerivedA` 没跑 → `DerivedA::buf` 那块堆**泄漏（UB）** |
| **virtual** | 先 `~DerivedA (frees buf)` 再 `~BaseA` | 派生析构先收口，基类收尾 → 干净 |

**机制**：`delete` 经基类指针时，非 virtual 只静态调 `~BaseA`（看指针表面类型），不知道底下压着 Derived 层。
**游戏安全落点**：逆向认一个类，必须顺着继承链查每层析构是否 virtual——有大量 `Base*` 指向的基类，析构不 virtual 就会在「经基类指针删除」处悄悄漏掉派生资源。

### 3.2 构造/析构中调虚函数不多态
在 `BaseB::BaseB()` 构造体里加 `foo();`，再 `DerivedB d;`：
```
BaseB::foo        <- 构造期间调用，走的是【基类版】！（非 DerivedB::foo）
```
**机制**：vptr 是**逐层盖楼**设的——基类构造先把 vptr 指基类表，基类构造跑完、派生构造才覆盖成派生表。所以 `BaseB` 构造体执行那一刻，vptr 还在基类表上。析构对称、方向相反。
**游戏安全落点**：反汇编里看到构造函数内的虚调用，**别假设跳最终派生版**，它只跳「当前正在构造那一层」的版本。

### 3.3 vtable 落在 `.rdata`（只读数据段）
区分两个常被混淆的东西：
- **vtable（虚表）**：编译器给**每个类**生成的**全局唯一、只读**函数指针表 → 存在 `.rdata`
- **vptr（虚指针）**：**每个对象头里**那 8 字节，指向自己类的 vtable → 随对象存在栈/堆上

**D3 实测四地址（本机 MSVC）**：

| 符号 | 地址 | 所属段 |
|---|---|---|
| **vtable addr** | `00007FF616E49570` | **`.rdata`（只读）** |
| string literal addr | `00007FF616E493F8` | `.rdata` |
| global `g_x` addr | `00007FF616E55C00` | `.data` |
| `free_fun` addr | `00007FF616E31000` | `.text` |

**关键点**：vtable 与字符串字面量地址只差 **0x178（376 字节）**——同一片内存、同属 `.rdata`，远离栈（高地址）与堆。
**为什么放只读段**：vtable 是虚函数跳转表，只要有人改写表里任一函数指针，所有经该 vptr 的虚调用就被劫持——这正是 **vtable hijack 攻击**与 S4 vtable hook 的攻击面。编译器放 `.rdata`，从段权限层面禁止改写。
**接回 W1**：vtable 落在 W1 进程内存分区图的 **`.rdata`（静态只读数据区）**，与字符串字面量同片；W3 虚表图由此拼进 W1 内存全景图。

---

## 全图总结（周日周会要能脱稿讲清 + 手绘）

任意 C++ 对象布局 = **各基类子对象（含各自 vptr）从 offset 0 依次堆叠 + 派生自有成员**；
- vptr 在各自子对象头，指向 `.rdata` 里的 vtable；
- 多继承下第二个基类子对象有**独立 vptr** 且调用时 **this 要 +offset**；
- 基类析构**必须 virtual** 防派生部分泄漏；
- 构造/析构期虚调用**不多态**（vptr 逐层盖楼）。

把这三把尺子和 W1 四区图拼在一起，W3 虚表这一关就真收口了。
