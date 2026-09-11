// W2D3 Task B: pointer arithmetic step (ptr_arith.cpp)
// Goal: verify p+1 advances by sizeof(pointed-type), NOT 1 byte.
// This is the filled reference for Task B. Tasks C/D you must write yourself.
#include <cstdio>
#include <cstddef>

struct S { double d; char* p; int i; };

int main() {
    S s;
    // Step 1: show member offsets + total size
    printf("S: d@%zu p@%zu i@%zu size=%zu\n",
           offsetof(S, d), offsetof(S, p), offsetof(S, i), sizeof(S));

    // Step 2: int* arithmetic advances by sizeof(int)=4, not 1
    int* ip = &s.i;
    printf("sizeof(int) = %zu\n", sizeof(int));
    printf("(char*)(ip+1) - (char*)ip = %td  (expect 4)\n",
           (char*)(ip + 1) - (char*)ip);

    // Step 3: char* advances by 1 byte (real byte address +1)
    printf("(char*)&s.i + 1 - (char*)&s.i = %td  (expect 1)\n",
           ((char*)&s.i + 1) - (char*)&s.i);

    return 0;
}
