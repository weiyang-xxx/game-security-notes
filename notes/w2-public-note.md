# W2 笔记（公开版）：把结构体对齐这把尺子磨出来

> 写给未来的自己 + 同路的人。内部完整版在 `notes/w2-align-note.md`，这篇是能发的版本。

## 0. 一句话先给结论
x64 上：成员对齐到「自身大小」的整数倍；总大小取整到「最大对齐值」的倍数；padding 是编译器塞的空隙，**内容未定义**。

## 1. 对齐一句话版（事实）
- char=1 / short=2 / int=4 / 指针=8 / double=8（x64 ABI）
- 我的话：对齐就是——每个字段的起始偏移必须是它「对齐值」的整数倍；结构体的总大小也必须是「最大对齐值」的整数倍。中间多出来的那些缝就是 padding，它不是数据，内容编译器不保证（Debug 可能是 0xCC，Release 是随机）。

## 2. 换顺序大小就变（最小反例）
- A {char; int; char} = 12 字节
- B {int; char; char} = 8 字节
- 为什么：高对齐字段（int）落点决定 padding 浪费量。A 在 char 后补 3 字节缝给 int；B 把两个 char 塞进 int 留的空当，padding 从 3 字节降到 1 字节。
- 我的布局图（周会必考）：

  ```
  A { char a; int b; char c; }   size = 12, align = 4
   偏移:   0    1    2    3    4    5    6    7    8    9   10   11
   字节:  [ a ] [pad][pad][pad][ b ][ b ][ b ][ b ][ c ] [pad][pad][pad]
          1B        3B            4B              1B        3B
   总大小补到 4 的倍数：8 + 1 = 9，补到 12。

  B { int b; char a; char c; }   size = 8, align = 4
   偏移:   0    1    2    3    4    5    6    7
   字节:  [ b ][ b ][ b ][ b ][ a ][ c ] [pad][pad]
          4B            1B   1B     2B
   总大小 6，补到 4 的倍数：8。
  ```

## 3. 指针算术步长
- `p+1` 地址差 = `sizeof(指向类型)`，不是 1。
- int* +4；double* +8；要 +1 字节用 `(char*)p+1`。
- 我的话（推翻认知那一刻）：我用 `int* p` 打印 `p` 和 `p+1`，以为地址差 1，结果差 4。那一刻我才懂——指针算术的单位是「元素」，不是「字节」。

## 4. memcpy 反模式（重点，S3 的 FTransform 会救命）
- 整结构体 memcpy + 裸偏移读 = **赌布局不变**。padding 未初始化（Debug=0xCC, Release=随机），跨编译器/平台就错位。
- 正确姿势：逐字段读写（用 memcpy 按字段拷，不假设布局）。
- 真实踩坑：我初版 `memcpy_pitfall.cpp` 用了 `*(short*)(safe+5)=s.c` 这种未对齐强转，加上 short 截断，跑出 `c=0x800` 而不是 `0x9ABC`。改成全 memcpy 后才稳定出 `0x9ABC`。
- 我的话（为什么在游戏安全里致命）：游戏安全经常要跨进程读内存、注入、hook、解析封包。你本地声明的结构体只是你猜的布局；目标进程可能是另一个编译器、另一个版本、另一个 pack 设置。整块 memcpy + 裸偏移，等于把 padding 和字段顺序当成协议。一旦错位，你读到的可能不是坐标 / 血量，而是 padding 或别的字段；写错会崩游戏、触发异常；封包两端布局不同会解析出垃圾。正确做法：固定线格式，逐字段读写，带版本校验，偏移用扫描 / 符号，不赌 ABI。

## 5. #pragma pack(1) 的代价
- 去 padding 省字节，但换来非对齐访问：x86 慢，ARM/SPARC 直接 Bus Error 崩。
- 我的话（pack(1) 用法）：
  - **会用到的场景**：定义网络包、文件头、硬件寄存器映射、磁盘格式，而且我只通过「逐字节序列化 / 反序列化」访问它（不把它当内存结构体直接解引用）。
  - **绝对不用的场景**：直接当跨模块 ABI 接口、频繁解引用、取地址传给原子操作 / SIMD / 系统调用、性能敏感路径。

## 发布
- 存 `notes/w2-public-note.md` → `git add` / `commit` / `push` 到 `game-security-notes` → GitHub Pages 自动更新。
- 链接：`https://weiyang-xxx.github.io/game-security-notes/`
- 注意：别 `git add .` 整目录（`.gitignore` 已挡 `.workbuddy/` 与编译产物）。
