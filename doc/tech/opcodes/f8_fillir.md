# 0xF8 -- fill

- **Category:** TripleOp
- **Enum:** `ByteCode::FillIR`
- **Operand(s):** `fill i, r` (R-cmd)

## Semantics
Fill `i` slots in `acc` with reference value `r`. Generic form is a `rep stos`; specialized variants unroll for small `i  in  {1, 2, 3, 4}` and the `r == 0` case.

## JIT (amd64)
**Selection rule:** R-cmd -- variant chosen by the ref mask of `r` and the small-`i` shape. The `r == 0` sentinel picks a dedicated zero-fill slot (avoids materialising a ptr); `i  in  {1..4}` unroll explicit `mov`s instead of `rep stos`.

**Template:** `asm/amd64/core60.asm` `inline %0F8h`
```asm
; generic
mov  rax, __ptr64_2
mov  rdi, rbx
mov  ecx, __arg32_1
rep  stos
```

### Variants
| Prefix | (i, r) | Body |
|---|---|---|
| `%0F8h` | generic | `rep stos` with `rax = r` |
| `%1F8h` | (i, 0) | `xor rax,rax; rep stos` |
| `%2F8h` | (1, r) | `mov [rbx], rax` |
| `%3F8h` | (1, 0) | `xor rax,rax; mov [rbx], rax` |
| `%4F8h` | (2, r) | 2x `mov [rbx+...]` |
| `%5F8h` | (2, 0) | unrolled zero |
| `%6F8h` | (3, r) | 3x `mov` |
| `%7F8h` | (3, 0) | unrolled zero |
| `%8F8h` | (4, r) | 4x `mov` |
| `%9F8h` | (4, 0) | unrolled zero |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0F8h`
```asm
; generic
mov  eax, __ptr32_2
mov  edi, ebx
mov  ecx, __arg32_1
rep  stos
```

x32 zero-fill variants for i=2/4 use SSE `xorps xmm0` + `movq qword ptr` pairs.

### Variants
| Prefix | (i, r) | Body |
|---|---|---|
| `%0F8h` | generic | `rep stos` |
| `%1F8h` | (i, 0) | `xor eax,eax; rep stos` |
| `%2F8h` | (1, r) | `mov [ebx], eax` |
| `%3F8h` | (1, 0) | `xor eax,eax; mov [ebx], eax` |
| `%4F8h` | (2, r) | 2x `mov` |
| `%5F8h` | (2, 0) | `xorps xmm0; movq [ebx], xmm0` |
| `%6F8h` | (3, r) | 3x `mov` |
| `%7F8h` | (3, 0) | unrolled zero |
| `%8F8h` | (4, r) | 4x `mov` |
| `%9F8h` | (4, 0) | SSE: 2x `movq [ebx+...]` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0F8h`
```asm
; generic
movz    x12, __ptr32lo_2
movk    x12, __ptr32hi_2, lsl #16

mov     x11, __arg12_1
mov     x13, x10

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 1
  str     x12, [x13], #8
  b       labLoop

labEnd:
```

### Variants
`%0F8h`, `%1F8h`, `%3F8h`, `%5F8h`, `%7F8h`, `%9F8h` -- all implemented as loops (no unrolled specialization on aarch64).

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0F8h`
```asm
; generic
li      r16, __arg16_1

ld      r17, toc_rdata(r2)
addis   r17, r17, __disp32hi_2
addi    r17, r17, __disp32lo_2

mr      r18, r15

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -1
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:
```

### Variants
`%0F8h`, `%1F8h`, `%3F8h`, `%5F8h`, `%7F8h`, `%9F8h` -- all loops.

## Notes
- Fills `i` ptr-sized slots starting at `acc` with reference value `r`.
- amd64/x32 unroll for small `i` (1..4) and use SSE on x32 for the 16-byte zero-fill cases.
- aarch64/ppc64le keep a loop body even for the unrolled prefixes (no SSE-equivalent unroll on these arches).
- The `(i, 0)` family is dedicated: `r == 0` uses `xor`-clear instead of materialising a zero ptr.
