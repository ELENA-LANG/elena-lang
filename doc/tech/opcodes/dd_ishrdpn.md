# 0xDD -- ishr dp

- **Category:** TripleOp
- **Enum:** `ByteCode::IShrDPN`
- **Operand(s):** `ishr dp:disp, n` (n = 1, 2, 4, 8)
- **Reads:** `sp[0]` (count), `dp[disp]` (n bytes)
- **Writes:** `dp[disp]` (n bytes)

## Semantics
`dp[disp] := dp[disp] >> index_from_sp[0]`. Right-shift the slot by the count loaded from `sp[0]`. Logical (unsigned) on amd64/x32 (`shr`); aarch64 uses `lsr`; ppc64le uses `srd`.

## JIT (amd64)
**Selection rule:** dispatched through `loadDPNOp` -> `retrieveICode(scope, arg2)`. Width `n`  in  {1,2,4,8} -> slots `%1DDh, %2DDh, %0DDh, %4DDh`. On x32 the n=8 form emulates a 64-bit shift right with `shrd`/`sar`.

**Template:** count -> `cl`, `shr` at the chosen width, store narrowed.

```asm
; %0DDh -- n = 4
lea  rdi, [rbp + __arg32_1]
mov  rcx, [r10]
mov  eax, dword ptr [rdi]
shr  eax, cl
mov  dword ptr [rdi], eax
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1DDh` | 1 | `and eax, 0FFh; shr eax, cl; mov byte ptr [rdi], al` |
| `%2DDh` | 2 | `and eax, 0FFFFh; shr eax, cl; mov word ptr [rdi], ax` |
| `%0DDh` | 4 | `shr eax, cl; mov dword ptr [rdi], eax` |
| `%4DDh` | 8 | `mov rax,[rdi]; shr rax, cl; mov [rdi], rax` |

## JIT (x32)
**Template:** identical pattern; n=8 emulates 64-bit shift right.

```asm
; %0DDh -- n = 4
lea  edi, [ebp + __arg32_1]
mov  ecx, [esi]
mov  eax, [edi]
shr  eax, cl
mov  [edi], eax
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1DDh` | 1 | `and eax, 0FFh; shr eax, cl; mov byte ptr [edi], al` |
| `%2DDh` | 2 | `and eax, 0FFFFh; shr eax, cl; mov word ptr [edi], ax` |
| `%0DDh` | 4 | `shr eax, cl; mov [edi], eax` |
| `%4DDh` | 8 | `shrd eax, edx, cl` / `sar edx, cl` with `cl >= 64` -> zero, `cl >= 32` -> swap halves (uses `sar` for the high half) |

The n=8 sequence on x32:

```asm
push edx
lea  edi, [ebp + __arg32_1]
mov  ecx, [esi]
mov  eax, [edi]
mov  edx, [edi+4]

cmp  cl, 64
jae  short lErr

cmp  cl, 32
jae  short LR32
shrd eax, edx, cl
sar  edx, cl
jmp  short lEnd

LR32:
mov  eax, edx
xor  edx, edx
sub  cl, 20h
shr  eax, cl
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
**Template:** `lsr` (logical shift right); narrow widths mask before shifting.

```asm
; %0DDh -- n = 4
add     x19, x29, __arg12_1
ldrsw   x17, [x0]
ldrsw   x18, [x19]
lsr     x18, x18, x17
str     w18, [x19]
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1DDh` | 1 | `ldrsw/ldrsw; and 0FFh; lsr; strb` |
| `%2DDh` | 2 | `ldrsw/ldrsw; and 0FFFFh; lsr; strh` |
| `%0DDh` | 4 | `ldrsw/ldrsw; lsr; str w` |
| `%4DDh` | 8 | `ldrsw/ldr; lsr; str` |

## JIT (ppc64le)
**Template:** `srd` (shift right doubleword) -- logical.

```asm
; %0DDh -- n = 4
addi    r19, r31, __arg16_1
lwz     r17, 0(r3)
lwz     r18, 0(r19)
srd     r18, r18, r17
stw     r18, 0(r19)
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1DDh` | 1 | `lwz/lwz; andi 0FFh; srd; stb` (stores count -- verify) |
| `%2DDh` | 2 | `lwz/lwz; andi 0FFFFh; srd; sth` |
| `%0DDh` | 4 | `lwz/lwz; srd; stw` |
| `%4DDh` | 8 | `lwz/ld; srd; std` |

## Notes
- Width `n`  in  {1,2,4,8} via `retrieveICode` on `arg2`.
- Mirror of `ishl dp` (0xDC).
- amd64/x32 use `shr` (logical); aarch64 uses `lsr`; ppc64le uses `srd` (logical doubleword).
- x32 uses `sar` (arithmetic) for the high half of the 64-bit emulation, which subtly differs from a pure logical right shift -- verify that the JIT only emits the n=8 form for signed operands.
- Shift count loaded from `sp[0]`; host shift-count masking applies on x86.
