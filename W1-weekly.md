# 第 1 周 周报 ｜ S1 ｜ 完成

> 契约 7.9 格式 · 验收看证据不看感觉
> 周期：2026-08-30（周日 / W1D1）～ 2026-09-05（周六 / W1D7 收口）
> 阶段：S1（W1–W12）= C++ 内存模型 + Windows 进程/内存
> 成员：韦扬（learner #1，进度隔离存档）

## 产出证据

- **main.cpp 编译运行**（D1 环境 + D2 编译）
  - 配置 Debug / x64，**0 错误 0 警告**
  - 证据文件：`code/01-memory-toolkit/main.cpp` + `run_out.txt`（已 push 公开仓库）
- **六区地址实测**（本机 Win11 x64 + ASLR，量级从低到高）
  - 栈 ≈ 825 GB（最低）< 堆 ≈ 2.85 TB < 模块 ≈ 140 TB（最高）
  - 证据：D3 推翻教科书布局图，实测数据见 `notes/d3-worksheet.md`
  - 结论：教科书"栈在最高、紧贴内核"的 32 位模型在 Win x64+ASLR 下不成立
- **三句解释**（D2–D3 验证）
  1. 栈向低地址增长（每层 `sub rsp, 0x130` = 304 字节，x64 ABI 约定，非物理定律）
  2. 堆不严格递增（分配器复用空闲块 + 按大小分 bin）
  3. 指针变量（.data）与字面量（.rdata）是两个地址
- **费曼记录**（D4）
  - 讲解稿 v4 定稿：`notes/d4-explain.md`
  - 追问清单 v6 终稿：`notes/d4-qa-v6.md`
  - 核心突破：局部变量相对帧底的"房间号"是编译期数死的相对量，ASLR 只洗绝对基址、洗不掉相对量
- **GitHub 公开仓库**（W12 硬门槛提前达标）
  - `https://github.com/weiyang-xxx/game-security-notes`（已确认 **Public**）
  - 已含 `main.cpp` + `run_out.txt` + `.gitignore`（挡 `.workbuddy/` 与编译产物）
- **博客站**
  - `https://weiyang-xxx.github.io/game-security-notes/`（GitHub Pages，D5 已开）
- **第一周公开笔记初稿**
  - `notes/w1-memory-note.md`（W12 硬门槛：12 周 ≥12 篇，此为第 1 篇）

## 一句话总结本周

从环境搭建一路打到能费曼讲清栈帧机制、自建 Public 仓库 + 博客，D1–D6 全绿；核心认知升级 = **亲手用实测推翻教科书布局图** + 想通"相对量（房间号）洗不掉"这一关键直觉。

## 卡在哪

- D4 卡点表第四栏「知识重述」初写偏题（写成感受而非知识重述），D6 已补自己的话版本（禁"偏移"二字、三要素齐：谁定/何时定/为何 ASLR 洗不掉），🟡 已关闭。
- git 三连坑（远程 URL 占位符未换 / 只 add 没 commit / 站错目录误 init 仓库）D5 实操踩过，D6 费曼回锅向同学口头陈述通过。
- 无未解决阻塞；W1 收口干净。
