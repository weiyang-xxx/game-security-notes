// W2D3 Task A: verify D2 hand-calc (verify.cpp)
// offsetof(Struct, member) returns the byte offset of 'member' from the start of Struct.
// sizeof(Struct) returns the total size in bytes (including tail padding).
// %zu is the printf format for size_t (what offsetof/sizeof return).
// NOTE: this is the filled reference for Task A only. Tasks B/C/D you must write yourself.
#include <cstdio>
#include <cstddef>

struct A { char a; int b; char c; };
struct B { int b; char a; char c; };
struct C { char buf[100]; double d; };

int main() {
    printf("A: a@%zu b@%zu c@%zu size=%zu\n",
           offsetof(A, a), offsetof(A, b), offsetof(A, c), sizeof(A));
    printf("B: b@%zu a@%zu c@%zu size=%zu\n",
           offsetof(B, b), offsetof(B, a), offsetof(B, c), sizeof(B));
    printf("C: buf@%zu d@%zu size=%zu\n",
           offsetof(C, buf), offsetof(C, d), sizeof(C));
    return 0;
}
