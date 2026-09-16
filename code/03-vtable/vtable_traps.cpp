// W3D3 Task: virtual destructor + ctor/dtor polymorphism trap + vtable landing (.rdata)
// Goal:
//   (A) Prove a BASE destructor MUST be virtual, else the Derived part LEAKS / UB.
//   (B) Prove a virtual call made INSIDE a ctor/dtor is NOT polymorphic.
//   (C) Locate the vtable in memory and confirm it lives in .rdata (read-only data),
//       tying W3's "vtable picture" back to W1's process memory four-region map.
//
// Build (MSVC): cl /Od /EHsc /W4 vtable_traps.cpp
// Build (g++):  g++ -O0 -o vtable_traps vtable_traps.cpp
//
// RED-LINE (plan MEMORY.md, S1 AI<=40%): the experiments marked TODO(learner)
//   are the CORE hand's-on. Mentor gives class definitions + printing helpers only.
//   NEVER pre-write the bold experiment lines -- you (learner) must type them,
//   because the red line exists so the core gets written by hand the first time.

#include <cstdio>
#include <cstddef>

// ---------- Part A: virtual destructor ----------
struct BaseA {
    // TODO(learner) #A0: toggle 'virtual' here and re-observe Part A.
    virtual~BaseA() { printf("~BaseA\n"); }
    virtual void who() { printf("BaseA::who\n"); }
};
struct DerivedA : BaseA {
    int* buf = new int[4];                 // a resource we can SEE leak if ~DerivedA never runs
    ~DerivedA() { printf("~DerivedA (frees buf)\n"); delete[] buf; }
    void who() override { printf("DerivedA::who\n"); }
};

// ---------- Part B: ctor/dtor polymorphism trap ----------
struct BaseB {
    BaseB() {
    foo();// TODO(learner) #B0: add exactly one line here:  foo();
        //   (member-init + the rest stay empty). Then build Part B in main().
    }
    virtual void foo() { printf("BaseB::foo\n"); }
};
struct DerivedB : BaseB {
    void foo() override { printf("DerivedB::foo\n"); }
};

// ---------- Part C: vtable landing point ----------
struct C {
    virtual void f() { printf("C::f\n"); }
};
int g_x = 0;                                // mentor: a .data global
void free_fun() { printf("free_fun (in .text)\n"); }

int main() {
    // ===== Part A: is the base destructor virtual? =====
    printf("--- Part A: Base* p = new DerivedA; delete p; ---\n");
    // TODO(learner) #A1 (CORE): write these two lines, FIRST with ~BaseA NON-virtual:
     BaseA* p = new DerivedA();
     delete p;
    // Observe: only "~BaseA" prints -> DerivedA::buf LEAKS (UB on the Derived slice).
    // Then add 'virtual' to ~BaseA (TODO #A0) and rebuild: "~DerivedA" then "~BaseA".
    // (Optional, stronger proof: wrap with _CrtDumpMemoryLeaks() in Debug, W4 tool.)
    // Screenshot BOTH outputs as acceptance evidence.

    // ===== Part B: virtual call inside a constructor =====
    printf("--- Part B: construct DerivedB (BaseB ctor calls foo) ---\n");
    // TODO(learner) #B1 (CORE): first add `foo();` inside BaseB::BaseB() (TODO #B0),
    DerivedB d;//   then write here:  DerivedB d;
    // Observe: during BaseB construction, foo() resolves to BaseB::foo -- NOT
    //   DerivedB::foo. Reason: vptr is still pointing at BaseB's vtable at that
    //   moment (it gets re-pointed to DerivedB's vtable only AFTER the base ctor
    //   finishes). Same trap fires in the destructor, in reverse. Screenshot output.

    // ===== Part C: where does the vtable live? =====
    printf("--- Part C: vtable region vs W1 four-region map ---\n");
    C c;
    void** vptr = *(void***)&c;             // mentor: read the vptr (object offset 0)
    const char* lit = "i_live_in_rdata";    // mentor: a string literal lives in .rdata
    printf("vtable addr        = %p   (stored in object's vptr)\n", (void*)vptr);
    printf("string literal addr= %p   (.rdata)\n", (void*)lit);
    printf("global g_x addr     = %p   (.data)\n",  (void*)&g_x);
    printf("free_fun addr       = %p   (.text)\n",  (void*)(void*)free_fun);
    // TODO(learner) #C1 (CORE): cross-check the four addresses above against the
    //   SIX regions you printed in W1 (main.cpp): .text / .rdata / .data / stack / heap.
    //   Confirm the vtable sits in the SAME static low range as the string literal
    //   (.rdata), FAR from stack (high) and heap. State in your daily note WHICH W1
    //   region the vtable lands in, and WHY a table of function pointers being
    //   read-only makes sense (you never want code to rewrite the vtable).

    return 0;
}
