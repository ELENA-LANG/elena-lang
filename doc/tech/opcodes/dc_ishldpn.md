# 0xDC -- ishl dp

- **Category:** TripleOp
- **Enum:** `ByteCode::IShlDPN`
- **Operand(s):** `ishl dp:disp, n` (n = 1, 2, 4, 8)
- **Reads:** `sp[0]` (count), `dp[disp]` (n bytes)
- **Writes:** `dp[disp]` (n bytes)

## Semantics
`dp[disp] := dp[disp] << index_from_sp[0]`. The shift count is loaded from `sp[0]`; the value at `dp[disp]` is shifted left at the chosen width.

## JIT (amd64)
**Selection rule:** dispatched through `loadDPNOp` -> `retrieveICode(scope, arg2)`. Width `n`  in  {1,2,4,8} -> slots `%1DCh, %2DCh, %0DCh, %4DCh`. On x32 the n=8 form emulates a 64-bit shift with `shld`/`shl` plus a 32-step boundary check.

**Template:** load count into `rcx` (so `cl` drives `shl`), value into `eax`, shift, store narrowed.

```asm
; %0DCh -- n = 4
lea  rdi, [rbp + __arg32_1]
mov  rcx, [r10]
mov  eax, dword ptr [rdi]
shl  eax, cl
mov  dword ptr [rdi], eax
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1DCh` | 1 | `and eax, 0FFh; shl eax, cl; mov byte ptr [rdi], al` |
| `%2DCh` | 2 | `and eax, 0FFFFh; shl eax, cl; mov word ptr [rdi], ax` |
| `%0DCh` | 4 | `shl eax, cl; mov dword ptr [rdi], eax` |
| `%4DCh` | 8 | `mov rax,[rdi]; shl rax, cl; mov [rdi], rax` |

## JIT (x32)
**Template:** identical pattern for 1/2/4; n=8 emulates a 64-bit shift with `shld/shl` and a 32-step boundary check.

```asm
; %0DCh -- n = 4
lea  edi, [ebp + __arg32_1]
mov  ecx, [esi]
mov  eax, [edi]
shl  eax, cl
mov  [edi], eax
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1DCh` | 1 | `and eax, 0FFh; shl eax, cl; mov byte ptr [edi], al` |
| `%2DCh` | 2 | `and eax, 0FFFFh; shl eax, cl; mov word ptr [edi], ax` |
| `%0DCh` | 4 | `shl eax, cl; mov [edi], eax` |
| `%4DCh` | 8 | software 64-bit shift via `shld eax, edx, cl` / `shl edx, cl`, with `cl >= 64` -> zero, `cl >= 32` -> swap halves |

The n=8 sequence on x32:

```asm
push edx
lea  edi, [ebp + __arg32_1]
mov  ecx, [esi]
mov  eax, [edi]
mov  edx, [edi+4]

cmp  cl, 40h
jae  short lErr
cmp  cl, 20h
jae  short LL32
shld eax, edx, cl
shl  edx, cl
jmp  short lEnd

LL32:
mov  edx, eax
xor  eax, eax
sub  cl, 20h
shl  eax, cl
jmp  short lEnd

lErr:
xor  eax, eax
xor  edx, edx
jmp  short lEnd2

lEnd:
mov  [edi], eax
mov  [edi+4], edx

lEnd2:
pop  edx
```

## JIT (aarch64)
**Template:** `lsl` for all widths; store narrows the result.

```asm
; %0DCh -- n = 4
add     x19, x29, __arg12_1
ldrsw   x17, [x0]
ldrsw   x18, [x19]
lsl     x18, x18, x17
str     w18, [x19]
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1DCh` | 1 | `ldrsw+ldrsw; lsl; strb` |
| `%2DCh` | 2 | `ldrsw+ldrsw; lsl; strh` |
| `%0DCh` | 4 | `ldrsw+ldrsw; lsl; str w` |
| `%4DCh` | 8 | `ldrsw+ldr; lsl; str` |

## JIT (ppc64le)
**Template:** `sld` (shift left doubleword), store narrowed.

```asm
; %0DCh -- n = 4
addi    r19, r31, __arg16_1
lwz     r17, 0(r3)
lwz     r18, 0(r19)
sld     r18, r18, r17
stw     r18, 0(r19)
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1DCh` | 1 | `lwz/lwz; sld; stb` (NB: stores the *count* register; verify expected behaviour) |
| `%2DCh` | 2 | `lwz/lwz; sld; sth` |
| `%0DCh` | 4 | `lwz/lwz; sld; stw` |
| `%4DCh` | 8 | `lwz/ld; sld; std` |

## Notes
- Width `n`  in  {1,2,4,8} via `retrieveICode` on `arg2`.
- Sibling of `ishr dp` (0xDD).
- The 8-byte path on x32 is the only width that diverges from the simple `shl reg, cl` pattern -- it uses `shld`/`shl` plus a `cl >= 64` / `cl >= 32` boundary check.
- aarch64 uses `lsl` (logical shift left) for all widths.
- Shift count is loaded from `sp[0]`; on x86 only the low 6 bits of `cl` (5 bits on x32 for 32-bit ops) matter -- host-level shift-count masking applies.
