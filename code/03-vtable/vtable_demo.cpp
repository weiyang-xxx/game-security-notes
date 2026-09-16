// W3D1 Task: virtual functions & vtable (vtable_demo.cpp)
// Goal: prove a class with virtual functions has a hidden vptr at offset 0,
//       and that virtual dispatch = dereference vptr -> vtable -> function pointer.
#include <cstdio>
#include <cstddef>

struct Base {
    int x;
    virtual void foo() { printf("Base::foo x=%d\n", x); }
    virtual void bar() { printf("Base::bar x=%d\n", x); }
};

struct Derived : Base {
    int y;
    void foo() override { printf("Derived::foo x=%d y=%d\n", x, y); }
};

int main() {
    Base b; b.x = 42;
    Derived d; d.x = 1; d.y = 2;

    // (1) object size & member offsets — vptr lives at offset 0, 8 bytes on x64
    printf("sizeof(Base)=%zu  offsetof(Base,x)=%zu\n", sizeof(Base), offsetof(Base, x));
    printf("sizeof(Derived)=%zu  offsetof(Derived,x)=%zu  offsetof(Derived,y)=%zu\n",
           sizeof(Derived), offsetof(Derived, x), offsetof(Derived, y));

    // (2) read the vptr: the object's first 8 bytes hold the vtable address
    void** vtable = *(void***)&b;
    printf("Base vptr (vtable addr) = %p\n", (void*)vtable);

    // (3) walk the vtable: it is an array of function pointers
    printf("vtable[0]=%p  vtable[1]=%p\n", (void*)vtable[0], (void*)vtable[1]);

    // (4) normal virtual calls (dynamic dispatch)
    printf("normal: "); b.foo();   // -> Base::foo
    printf("normal: "); d.foo();   // -> Derived::foo (polymorphism)

    // ===== TODO(learner): manual dispatch via vptr =====
    // A virtual function has a HIDDEN 'this' parameter (on x64, this goes in rcx).
    // So treat vtable[0] as void(*)(Base*), and call it with &b as 'this'.
    // Result must match b.foo() exactly.
    // Fill the 3 lines below:
    typedef void(*VFunc)(Base*);
    VFunc f = (VFunc)vtable[0];
    printf("manual: "); f(&b);


    return 0;
}
