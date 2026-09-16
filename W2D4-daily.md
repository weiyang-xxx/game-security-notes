# W2D4 日报 ｜ 2026-09-10（周四）｜ 缓冲收口日

## 今日三件事（全绿 🟢）

### C · 回锅 D1–D3 裂缝 + 四程序实证复核 ✅
- 本机 x64 Native Tools 重新编译运行 4 个 cpp，输出全绿：
  - `verify.exe`：A:a@0 b@4 c@8 size=12 / B:b@0 a@4 c@5 size=8 / C:buf@0 d@104 size=112
  - `ptr_arith.exe`：S:d@0 p@8 i@16 size=24 / 步长 4 / (char*)&s.i+1 差 1
  - `memcpy_pitfall.exe`：raw b match=1 / **field-by-step: a=x b=0x12345678 c=0x9ABC**（修正后正确，旧版曾出 0x800）
  - `pack_demo.exe`：**Packed: a@0 b@1 c@5 size=7**（修正后通过，旧版曾 C2059 编不过）
- 暴露并修掉旧 bug：未对齐强转 + short 收缩 → c=0x800；中文编码 → C2059。重写纯 ASCII + 全 memcpy 后稳定。
- 教训：旧 exe 残留 + 双击 bat 在普通 cmd 跑 = 「假通过」。交付代码前必须自验。

### B · W2 weekly.md 草稿 ✅
- 在 `weekly/weekly.md` 追加「第 2 周 周报」段（契约 7.9，未覆盖 W1）。
- 含产出证据 / 一句话总结 / 卡点（含 bug 复盘）。

### A · W2 公开笔记骨架 ✅（待填肉）
- 建 `notes/w2-public-note.md`，5 节骨架 + 关键事实 + 真实踩坑已填，✍️ 处留用户自写「我的话」。
- 发布动作（git push 到 game-security-notes）待用户自做。

## 抽问 drill（已在 D3 存档）
- `S{double;char*;int}` = 24 字节，padding 在 [20,23]。

## 明日（W2D5 计划）
- 笔记定稿发博客 + weekly 补全归档 + 可选预热 W3 虚表概念（只读不写码）。
