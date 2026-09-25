// W4D1 Task: new/delete & heap memory leak (heap_leak_demo.cpp)
// Goal: understand heap allocation via new/delete, and SEE a real memory
//       leak reported by MSVC's debug CRT (_CrtDumpMemoryLeaks).
//
// Build (Debug build is REQUIRED for leak detection):
//   cl /Od /EHsc /W4 /MDd heap_leak_demo.cpp
// Run:
//   heap_leak_demo.exe
// Watch the console at exit: Part A's leaked Player block will be reported.
#include <cstdio>
#include <cstdlib>

#ifdef _DEBUG
#include <crtdbg.h>   // MSVC debug heap: dumps leaks at exit
#endif

struct Player {
    int hp;
    int x, y;
    Player(int h, int px, int py) : hp(h), x(px), y(py) {}
    void print() const { printf("Player hp=%d at (%d,%d)\n", hp, x, y); }
};

// ---- Part A: a function that LEAKS memory on purpose ----
// WHY this leaks: `p` is a STACK variable (dies when the function returns),
// but the object it points to lives on the HEAP. Without delete, the heap
// block is orphaned -> memory leak.
void leaky_function() {
    Player* p = new Player(100, 0, 0);  // Part A: allocated on heap, intentionally NOT deleted
    p->print();                        // NOTE: no delete -> leak when function returns
}

// ---- Part B: the SAME logic, but FIXED ----
void fixed_function() {
    Player* p = new Player(200, 10, 20);  // Part B: same alloc...
    p->print();                           // ...use it...
    delete p;                            // ...then free it -> no leak
}

int main() {
#ifdef _DEBUG
    // ---- Turn on the debug heap (CRT now keeps bookkeeping per block) ----
    // NOTE: we deliberately do NOT pass _CRTDBG_LEAK_CHECK_DF. That flag makes
    // the CRT dump leaks AGAIN automatically at process exit, and we already
    // call _CrtDumpMemoryLeaks() ourselves below -> the SAME report would be
    // printed twice. Rule: keep ONE dump source only.
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);

    // ---- Route the leak report to STDERR so it shows in a plain console ----
    // WHY: by default the CRT sends its report through OutputDebugString,
    // which only appears in a DEBUGGER's Output window. If you just run the
    // .exe from cmd, nobody is listening -> you'd see nothing at all and
    // wrongly conclude "no leak". Send it to the file (stderr) as well.
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
#endif

    printf("=== Part A: leaked allocation ===\n");
    leaky_function();

    printf("=== Part B: fixed allocation ===\n");
    fixed_function();

#ifdef _DEBUG
    // THE one and only leak dump. You should see Part A's Player block listed
    // here, but NOT Part B's (it was deleted).
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}
