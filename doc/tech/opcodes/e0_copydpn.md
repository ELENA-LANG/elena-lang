# 0xE0 -- copy dp

- **Category:** TripleOp
- **Enum:** `ByteCode::CopyDPN`
- **Operand(s):** `copy dp:disp, n`

## Semantics
Copy `n` bytes from the structure pointed to by `sp[0]` into the data-frame slot at `dp[disp]`. Generic form is `rep movsb`; fixed widths (1/2/4/8) inline a single sized `mov`.

## JIT (amd64)
**Selection rule:** `retrieveICode(n)` picks the variant by byte count `n`. n  in  {1,2,4,8} pick `%1E0h..%4E0h`; any other n falls back to the generic `%0E0h` `rep movsb` body.

**Template:** `asm/amd64/core60.asm` `inline %0E0h`
```asm
; generic %0E0h
mov  rsi, r10
lea  rdi, [rbp + __arg32_1]
mov  ecx, __n_2
rep  movsb
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%0E0h` | arbitrary | `rep movsb` (rsi=r10, rdi=rbp+disp) |
| `%1E0h` | 1 | `mov al,[r10]; mov byte [rbp+disp], al` |
| `%2E0h` | 2 | `mov ax,[r10]; mov word [rbp+disp], ax` |
| `%3E0h` | 4 | `mov eax,[r10]; mov dword [rbp+disp], eax` |
| `%4E0h` | 8 | `mov rax,[r10]; mov [rbp+disp], rax` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0E0h`
```asm
; generic %0E0h
mov  eax, esi
lea  edi, [ebp + __arg32_1]
mov  ecx, __n_2
rep  movsb
mov  esi, esi
```

On x32, `esi` is the live `sp[0]` register and must be saved/restored across `rep movsb`. The n=8 case uses SSE because GPRs are 32-bit.

### Variants
| Prefix | n | Body |
|---|---|---|
| `%0E0h` | arbitrary | `rep movsb` (saves/restores `esi`) |
| `%1E0h` | 1 | `mov al,[esi]; mov byte [ebp+disp], al` |
| `%2E0h` | 2 | `mov ax,[esi]; mov word [ebp+disp], ax` |
| `%3E0h` | 4 | `mov eax,[esi]; mov [ebp+disp], eax` |
| `%4E0h` | 8 | `movq xmm0,[esi]; movq [ebp+disp], xmm0` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0E0h`
```asm
; generic %0E0h
mov     x11, __n16_2
mov     x12, x0
add     x13, x29, __arg12_1

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 1
  ldrb    w14, [x12], #1
  strb    w14, [x13], #1
  b       labLoop

labEnd:
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%0E0h` | arbitrary | byte-by-byte loop |
| `%1E0h` | 1 | `ldr x14,[x0]; strb w14,[x13]` |
| `%2E0h` | 2 | `ldr x14,[x0]; strh w14,[x13]` |
| `%3E0h` | 4 | `ldr x14,[x0]; str w14,[x13]` |
| `%4E0h` | 8 | `ldr x14,[x0]; str x14,[x13]` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0E0h`
```asm
; generic %0E0h
li      r16, __n16_2
addi    r18, r31, __arg16_1
mr      r19, r3

labLoop:
  cmpwi   r16,0
  beq     labEnd
  ld      r17, 0(r19)
  addi    r16, r16, -1
  stb     r17, 0(r18)
  addi    r18, r18, 1
  addi    r19, r19, 1
  b       labLoop

labEnd:
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%0E0h` | arbitrary | byte-by-byte loop |
| `%1E0h` | 1 | `ld r17,0(r3); stb r17,0(r18)` |
| `%2E0h` | 2 | `ld r17,0(r3); sth r17,0(r18)` |
| `%3E0h` | 4 | `ld r17,0(r3); stw r17,0(r18)` |
| `%4E0h` | 8 | `ld r17,0(r3); std r17,0(r18)` |

## Notes
- Width `n` is the byte count of the value to copy.
- Source is `sp[0]` (stack temp register), destination is `dp[disp]` (data-frame slot).
- amd64/x32 use `rep movsb` for the generic path; the sized variants inline a single `mov` of the matching width.
- aarch64/ppc64le emit a byte-by-byte loop for the generic case and unrolled sized loads/stores for n  in  {1,2,4,8}.
- On x32 the n=8 variant goes through `xmm0` because the GPRs are 32-bit.
