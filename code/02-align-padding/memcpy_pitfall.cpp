// W2D3 Task C: memcpy anti-pattern (memcpy_pitfall.cpp)
// Goal: show "memcpy + raw offset read" works locally but layout is untrustworthy;
//       correct way is field-by-field read/write via memcpy (no raw pointer casts).
#include <cstdio>
#include <cstring>
#include <cstddef>

struct S { char a; int b; unsigned short c; };   // size=12, padding at [1,3] and [10,11]

int main() {
    S s{ 'x', 0x12345678, 0x9ABC };
    char buf[sizeof(S)];
    memcpy(buf, &s, sizeof(S));

    // ===== TODO(learner) #1 - raw offset read (safe via memcpy) =====
    int b_read;
    memcpy(&b_read, buf + offsetof(S, b), sizeof(int));
    printf("raw b = 0x%X, s.b = 0x%X, match=%d\n", b_read, s.b, (b_read == s.b));

    // ===== TODO(learner) #2 - correct field-by-field serialize =====
    // Custom compact layout: a@0, b@1, c@5, total 7 bytes.
    // Use memcpy for both write and read -> no unaligned / aliasing UB.
    char safe[7];
    safe[0] = s.a;
    memcpy(safe + 1, &s.b, sizeof(int));
    memcpy(safe + 5, &s.c, sizeof(unsigned short));
    char a2 = safe[0];
    int  b2;
    unsigned short c2;
    memcpy(&b2, safe + 1, sizeof(int));
    memcpy(&c2, safe + 5, sizeof(unsigned short));
    printf("field-by-step: a=%c b=0x%X c=0x%X\n", a2, b2, c2);

    // ===== TODO(learner) #3 - root-cause =====
    printf("ROOT CAUSE: memcpy + raw offset read relies on compiler-specific padding;\n"
           "padding bytes are uninitialized (Debug=0xCC, Release=random),\n"
           "and layout changes across platforms/compilers/struct member order.\n"
           "Field-by-field serialization via memcpy fixes the wire format explicitly.\n");

    return 0;
}
