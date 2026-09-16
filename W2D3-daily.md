# D3 日报 ｜ W2 ｜ 2026-09-09 周三

【D3 ｜ W2 ｜ 🟢】

## 今天任务：写码验证 D2 手算 + 深化三件事（指针算术 / memcpy 反模式 / pack(1)）

### 任务 A · offsetof 验证 D2 手算（W2 验收①）— 🟢
手算 vs 实测对照表（verify.cpp 学员本机跑通，输出与手算 100% 一致）：

| 结构体 | 手算偏移/大小 | 实测（verify.cpp） | 一致 |
|---|---|---|---|
| A `{char a; int b; char c;}` | a@0 b@4 c@8 size=12 | 同左 | ✅ |
| B `{int b; char a; char c;}` | b@0 a@4 c@5 size=8 | 同左 | ✅ |
| C `{char buf[100]; double d;}` | buf@0 d@104 size=112 | 同左 | ✅ |

注：C 的 d@104 是**偏移**非**总大小**，今天没再犯"偏移当大小"的错。

### 任务 B · 指针算术步长（W2 验收②）— 🟢
ptr_arith.cpp 学员本机跑通：S`{double d; char* p; int i}` → d@0 p@8 i@16 size=24；
`(char*)(ip+1)-(char*)ip = 4`（步长=`sizeof(int)` 不是 1）；
`(char*)&s.i+1-(char*)&s.i = 1`（char* 才是 +1 字节）。
口述过关："`p+1` 跳整个对象，因为指针算术以指向类型为单位；要 +1 字节得强转 `char*`"。

### 任务 C · memcpy 反模式（W2 头号坑）— 🟢（代码自检）
memcpy_pitfall.cpp 由学员手填，逐行核对无误：
- 裸偏移读：`memcpy(&b_read, buf + offsetof(S, b), 4)` → `b_read == s.b`，`match=1`（本地一致 = 陷阱）
- 逐字段写 `safe[7]`：a@0, b@1, c@5；读回 `a2='x' b2=0x12345678 c2=0x9ABC`
- ROOT CAUSE 归因已写（padding 未初始化 + 布局依赖编译器/平台/成员顺序）
进阶点已提示：写入也该用 `memcpy(safe+1, &s.b, 4)` 而非 `*(int*)(safe+1)=s.b`，读写都避未对齐。
（沙箱无编译器，按 x86 布局手算确认输出与预期一致；你本机跑通截图即为铁证。）

### 任务 D · #pragma pack(1) 对照 — 🟢（代码自检）
pack_demo.cpp 由学员手填，逐行核对无误：
- `Packed{a@0 b@1 c@5 size=7}`（无 padding）
- COST 归因：b 在 offset 1 非 4 字节对齐，x86 变慢、ARM/SPARC 直接 Bus Error。

### 抽问 drill（自问自答）— 🟢
- **题**：`S3 {double d; char* p; int i;}` 24 字节，指出 padding 在哪两个字节区间，为什么。
- **答**：padding 在 **[20,23]**（4 字节）。d@0 占 0..7，p@8 占 8..15，i@16 占 16..19；总大小须取整到最大对齐值 8 → 20 非 8 倍数，补到 24，pad 20–23。

## 明天第一件事（D4 缓冲/收口）
- 写 W2 公开笔记（对齐/padding/指针算术 + memcpy 反模式教训，优先发博客）
- 起 W2 `weekly.md` 草稿（契约 7.9）
- 回锅 D1–D3 任何裂缝

## 备注
- AI 写码占比：四个 cpp 主逻辑均学员自写，导师只给骨架 + 定位 bug，符合 ≤40% 红线。
- GitHub：今天动了 `code/02-align-padding/` 下 4 个 cpp，记得 `add/commit/push` 保持连续链（仓库已 Public，勿 `git add .` 整目录）。
- 红线：靶场只限自写程序 / Lyra / CrackMe / 公开样本静态 / CTF / SRC，禁止去任何在线游戏试。
