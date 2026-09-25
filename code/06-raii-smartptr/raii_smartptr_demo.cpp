// W4D3 Task: RAII and smart pointers (raii_smartptr_demo.cpp)
//
// Two lessons today:
//   (1) RAII = Resource Acquisition Is Initialization:
//       tie a resource's cleanup to an object's lifetime. When the object
//       dies (scope exit, early return, exception), its destructor runs
//       automatically and frees the resource. No manual delete to forget.
//   (2) The standard tools that do this for you:
//       std::unique_ptr<T>  -> owns ONE heap object, deletes it on death.
//       std::vector<T>      -> owns a dynamic array, deletes it on death.
//       These replace the manual new/delete and new[]/delete[] you wrote
//       in W4D1/D2, and make leaks and mismatches basically impossible.
//
// Build (Debug build required for the leak check):
//   cl /Od /EHsc /W4 /MDd raii_smartptr_demo.cpp
// Run:
//   raii_smartptr_demo.exe
#include <cstdio>
#include <memory>    // std::unique_ptr, std::make_unique
#include <vector>    // std::vector

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

// ---- Part A: RAW pointer, DELIBERATELY leak on early return ----
// This is the "what goes wrong" demo. A raw pointer only frees when YOU
// remember to call delete. If the function returns early (or throws) before
// reaching delete, the object is orphaned -> memory leak.
void raw_leak_on_early_return() {
     //TODO(learner): allocate a Player with new, print it, then return
       //EARLY without delete. Observe the leak report at program exit.
       Player* p = new Player(1, 0, 0);
       p->print();
       return;   // BUG on purpose: no delete -> leak
}

// ---- Part B: unique_ptr, SAME early return, but NO leak ----
// unique_ptr owns the Player. Its destructor calls delete automatically,
// no matter HOW the scope is left (normal end, early return, exception).
void unique_no_leak_on_early_return(bool early) {
    //TODO(learner): create the Player via std::make_unique, print it, and
     //  if early is true, return early. Watch the dtor STILL fire.
     auto p = std::make_unique<Player>(2, 0, 0);
      p->print();
       if (early) return;   // dtor runs anyway -> no leak
       p->print();
}

// ---- Part C: vector<Player> replaces new[]/delete[] ----
// vector owns a growable array. You push_back objects; vector calls each
// destructor and frees the buffer on scope exit. No new[]/delete[] to pair.
void vector_demo() {
    //TODO(learner): build a std::vector<Player>, push_back 3 Players,
       //then print each. No manual delete anywhere.
    std::vector<Player> v;
       v.push_back(Player(3, 0, 0));
      v.push_back(Player(4, 0, 0));
     v.push_back(Player(5, 0, 0));
   for (const auto& e : v) e.print();
}

int main() {
#ifdef _DEBUG
    // Debug heap bookkeeping ON (one dump source only).
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);
    // Route leak report to stderr so it shows in a plain console (cmd) run.
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
#endif

    printf("=== Part A: raw pointer, deliberate early-return leak ===\n");
    raw_leak_on_early_return();

    printf("=== Part B: unique_ptr, early return (no leak) ===\n");
    unique_no_leak_on_early_return(true);

    printf("=== Part C: vector<Player> ===\n");
    vector_demo();

#ifdef _DEBUG
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}
