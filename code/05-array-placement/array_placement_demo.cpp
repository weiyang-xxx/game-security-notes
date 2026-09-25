// W4D2 Task: array new[]/delete[] pairing + placement new
// (array_placement_demo.cpp)
//
// Two lessons today:
//   (1) WHY new[] MUST pair with delete[] (never plain delete).
//   (2) placement new: construct an object in MEMORY YOU ALREADY OWN
//       (a raw buffer), without asking the heap for more.
//
// Build (Debug build recommended):
//   cl /Od /EHsc /W4 /MDd array_placement_demo.cpp
// Run:
//   array_placement_demo.exe
//
// NOTE: Part B (mismatch) is UNDEFINED BEHAVIOR and may crash/assert.
//       That crash IS the lesson. See the study plan for the safe order.
#include <cstdio>
#include <new>        // placement new
#include <cstddef>
#include <cstdint>

#ifdef _DEBUG
#include <crtdbg.h>
#endif

struct Player {
    int hp;
    int x, y;
    Player(int h, int px, int py) : hp(h), x(px), y(py) {
        printf("Player ctor hp=%d\n", hp);
    }
    ~Player() {
        printf("Player dtor hp=%d\n", hp);
    }
    void print() const { printf("Player(hp=%d, x=%d, y=%d)\n", hp, x, y); }
};

// ---- Part A: CORRECT array new[] / delete[] ----
// The runtime stores a "count" just BEFORE the array memory, so delete[]
// knows how many destructors to call and how big the block is. Pair them.
void array_ok() {
    // Allocate an array of 3 Players, store the pointer, then FREE with delete[].
    Player* arr = new Player[3]{ Player(1,0,0), Player(2,0,0), Player(3,0,0) };
    arr[0].print(); arr[1].print(); arr[2].print();
    delete[] arr;   // MUST be delete[], not plain delete. Expected: 3 ctors + 3 dtors.
}

// ---- Part B: THE MISMATCH TRAP (deliberate bug, UNDEFINED BEHAVIOR) ----
// Calling plain `delete` (not delete[]) on a new[] result makes the runtime
// call only ONE destructor and free with the WRONG size -> heap corruption,
// crash, or silent leak. Running it is the lesson. It may abort at exit.
void array_mismatch() {
    Player* arr = new Player[3]{ Player(10,0,0), Player(20,0,0), Player(30,0,0) };
    arr[0].print(); arr[1].print(); arr[2].print();
    delete arr;     // WRONG on purpose: mismatch with new[]. Observe what happens.
}

// ---- Part C: placement new ----
// Placement new builds an object at an address YOU give. It does NOT allocate.
// So you MUST call the destructor by hand, and you must NEVER `delete p`
// (the memory was not from new). This is how you "rebuild" an object over
// raw bytes -- a skill you'll reuse when reading other processes in S1.
void placement_ok() {
    alignas(Player) char buffer[sizeof(Player)];   // raw storage, NOT from heap
    Player* p = new (buffer) Player(100, 5, 5);    // construct IN buffer (no allocation)
    p->print();
    p->~Player();   // mandatory: buffer wasn't allocated by new, so free by hand
}

int main() {
#ifdef _DEBUG
    // Debug heap bookkeeping ON (one dump source only).
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);
    // Route leak report to stderr so it shows in a plain console (cmd) run.
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
#endif

    printf("=== Part A: array new[] / delete[] (correct) ===\n");
    array_ok();

    printf("=== Part C: placement new ===\n");
    placement_ok();

    // Run Part B LAST and ALONE (see plan). It may crash; that's expected.
    printf("=== Part B: new[] / delete mismatch (UB) ===\n");
    array_mismatch();

#ifdef _DEBUG
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}
