// W2D3 Task D: #pragma pack(1) comparison (pack_demo.cpp)
// Goal: compare default alignment (with padding) vs pack(1) (no padding).
#include <cstdio>
#include <cstddef>

#pragma pack(1)
struct Packed { char a; int b; short c; };
#pragma pack()   // restore default alignment, avoid affecting later code

int main() {
    // Print Packed offsets and total size. Expected: a@0 b@1 c@5 size=7 (no padding).
    printf("Packed: a@%zu b@%zu c@%zu size=%zu\n",
           offsetof(Packed, a), offsetof(Packed, b), offsetof(Packed, c), sizeof(Packed));
    // Cost of pack(1): b (int) sits at offset 1, which is not 4-byte aligned.
    // On x86 it still runs but slower (extra memory cycle); on ARM/SPARC it crashes (Bus Error).
    printf("COST: pack(1) removes padding, but b (int) at offset 1 is not 4-byte aligned.\n"
           "On x86 it works but is slower; on ARM/SPARC it crashes with a bus error.\n");
    return 0;
}
