// main.cpp — W1 内存工具包（导师交付骨架，已编译验证）
// 编译：MSVC  cl /W4 /Od /EHsc main.cpp   或   g++ -Wall -Wextra -Wpedantic -O0 -std=c++17 main.cpp
// 目标：打印六类地址 + 递归探栈 + 堆单调性自检，建立"地址 / 内存四区"因果模型。
// 三个刻意设计见注释，别跳过；W1 一律 Debug / -Od，禁止 /O2（开了变量进寄存器，"地址"就不存在）。

#include <cstdio>
#include <cstdlib>

// .data 区：已初始化全局变量
int g_data_var = 42;

// .bss 区：未初始化全局变量（被清零）
int g_bss_var;

// .data 区：指针变量本身；它指向的字符串字面量在 .rdata 区（零基础最常混淆处）
const char* g_msg_ptr = "hello-from-rdata";

// 任意一个函数，用来取 .text（代码段）地址
static void text_marker() {}

// 递归探栈：每深一层打印局部变量地址，观察是否逐层变小（栈向下增长的证据，不是课本断言）
static void recurse(int depth, int* caller_local) {
    int local = depth;  // 本层局部变量（在栈上）
    std::printf("  [recurse depth=%d] &local = %p\n", depth, (void*)&local);
    if (caller_local) {
        std::printf("    -> 上一层 &local = %p  (本层更小? %s)\n",
                    (void*)caller_local,
                    ((void*)&local < (void*)caller_local) ? "是" : "否");
    }
    if (depth > 0) recurse(depth - 1, &local);
}

int main() {
    std::printf("=== 内存六区地址 ===\n");
    std::printf(".text  (代码段)   : %p  (函数 text_marker)\n", (void*)(void*)&text_marker);
    std::printf(".rdata (只读数据) : %p  (字符串字面量)\n", (void*)"hello-from-rdata");
    std::printf(".data  (已初始化) : %p  (g_data_var)\n", (void*)&g_data_var);
    std::printf(".bss   (未初始化) : %p  (g_bss_var)\n", (void*)&g_bss_var);
    std::printf(".data  (指针变量) : %p  (g_msg_ptr 本身，在 .data)\n", (void*)&g_msg_ptr);
    std::printf("    -> g_msg_ptr 指向的字符串在 .rdata: %p\n", (void*)g_msg_ptr);

    int stack_local = 0;  // 栈变量
    int* heap_arr[5];
    std::printf("栈 (.stack)      : %p  (main 局部 stack_local)\n", (void*)&stack_local);

    std::printf("\n=== 堆单调性自检（连续 5 次 new）===\n");
    bool monotonic = true;
    for (int i = 0; i < 5; ++i) {
        heap_arr[i] = new int(i);
        std::printf("  new #%d -> %p\n", i, (void*)heap_arr[i]);
        if (i > 0 && (void*)heap_arr[i] <= (void*)heap_arr[i - 1]) monotonic = false;
    }
    std::printf("结论：地址严格递增？ %s\n",
                monotonic ? "是" : "否（正常！空闲块复用+分级 bin 导致，解释它而非记结论）");
    for (int i = 0; i < 5; ++i) delete heap_arr[i];

    std::printf("\n=== 递归探栈（4 层）===\n");
    recurse(4, nullptr);

    std::printf("\n=== 验收三问（写进日报）===\n");
    std::printf("1) 栈为何向低地址增长？（push 让 rsp 递减，是 ABI 约定+历史设计）\n");
    std::printf("2) 堆为何不一定递增？（分配器策略，不是保证）\n");
    std::printf("3) 绝对高低由谁决定？（ASLR 随机化，每次启动都变，我不记结论）\n");
    return 0;
}
