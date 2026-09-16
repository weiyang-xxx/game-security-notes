// W3D2 Task: multiple inheritance => ONE object carries TWO vptrs
// Goal: with two bases, the derived object has a separate vptr for EACH
//       base, each pointing to its OWN vtable. Manual dispatch must use
//       the RIGHT vptr AND the RIGHT 'this' (adjusted to that base).
//
// Build (MSVC): cl /Od /EHsc /W4 vtable_multi.cpp
// Build (g++):  g++ -O0 -o vtable_multi vtable_multi.cpp
//
// NOTE: the TWO-vptr fact and the offsets below are stable on x64 for both
//       compilers. Exact vtable bytes (RTTI / offset-to-top) are MSVC-specific.
//       To SEE them compile with:  cl /d1reportSingleClassLayoutC vtable_multi.cpp

#include <cstdio>
#include <cstddef>

struct A {
    int x = 0;
    virtual void a() { printf("A::a x=%d\n", x); }
};

struct B {
    int y = 0;
    virtual void b() { printf("B::b y=%d\n", y); }
};

struct C : A, B {        // inherit A (primary base) then B (secondary base)
    int z = 0;
    void a() override { printf("C::a x=%d z=%d\n", x, z); }
    void b() override { printf("C::b y=%d z=%d\n", y, z); }
};

int main() {
    C c; c.x = 1; c.y = 2; c.z = 3;

    // (1) sizes & key offsets (mentor). Expected on x64 MSVC:
    //     sizeof(A)=16  sizeof(B)=16  sizeof(C)=40
    //     A-subobj @0  (vptr@0, x@8)
    //     B-subobj @16 (vptr@16, y@24)
    //     z @32
    printf("sizeof(A)=%zu sizeof(B)=%zu sizeof(C)=%zu\n", sizeof(A), sizeof(B), sizeof(C));
    printf("offsetof(A,x)=%zu  offsetof(B,y)=%zu  offsetof(C,z)=%zu\n",
           offsetof(A, x), offsetof(B, y), offsetof(C, z));

    // (2) read BOTH vptrs. The second vptr is NOT at offset 8 -- it sits at the
    //     start of the B base subobject (here offset 16). Guessing "8" is wrong.
    void** vptrA = *(void***)&c;                 // A's vptr @ offset 0
    void** vptrB = *(void***)((char*)&c + 16);   // B's vptr @ offset 16
    printf("vptrA (C-as-A, @0)  = %p\n", (void*)vptrA);
    printf("vptrB (C-as-B, @16) = %p\n", (void*)vptrB);
    printf("vtableA[0]=%p  vtableB[0]=%p\n", (void*)vptrA[0], (void*)vptrB[0]);

    // (3) normal virtual calls (mentor). Same object, two base views.
    printf("normal C-as-A: "); ((A*)&c)->a();    // -> C::a (this = &c)
    printf("normal C-as-B: "); ((B*)&c)->b();    // -> C::b (compiler adjusts this to +16)

    // ===== manual dispatch via A's vptr (learner core) =====
    // vptrA[0] holds the address of C::a. Treat it as void(*)(void*) and call it
    // with (void*)&c as the hidden 'this'. Must print "C::a x=1 z=3".
    {
        typedef void(*VF)(void*);
        VF fa = (VF)vptrA[0];
        printf("manual via A-vptr: ");
        fa((void*)&c);
    }

    // ===== manual dispatch via B's vptr (learner core, this adjustment!) =====
    // vptrB[0] holds C::b. C::b reads y (B-subobj relative +8) and z (relative +16),
    // so 'this' MUST point at the B subobject start (offset 16), NOT at &c.
    // Pass (char*)&c + 16. Wrong 'this' => reads garbage (or crashes).
    // Must print "C::b y=2 z=3".
    {
        typedef void(*VF)(void*);
        VF fb = (VF)vptrB[0];
        printf("manual via B-vptr: ");
        fb((char*)&c + 16);
    }

    return 0;
}
