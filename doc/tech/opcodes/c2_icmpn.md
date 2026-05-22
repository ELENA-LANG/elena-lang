# 0xC2 -- icmp n

- **Category:** DoubleOp
- **Enum:** `ByteCode::ICmpN`
- **Operand(s):** `icmp n` (n = 1, 2, 4, 8)
- **Reads:** `acc -> [acc]` (n bytes), `sp[0] -> [sp[0]]` (n bytes)
- **Writes:** `COMP.EQ`, `COMP.LT`

## Semantics
Width-selected integer compare: load `temp1 : n << acc` and `temp2 : n << sp[0]`, then `COMP.EQ := (temp2 == temp1)`, `COMP.LT := (temp2 < temp1)`. Prefix encodes the width.

## JIT (amd64)
**Selection rule:** dispatched through `loadDPNOp` -> `retrieveICode(scope, arg)` (`jitcompiler.cpp:2399`). Variant by `n`  in  {1,2,4,8} -> slots `%1C2h, %2C2h, %4C2h, %8C2h` respectively (n=4 is the default slot 0). On x32, n=8 expands to a paired 32-bit `sub`/`sbb` sequence (high then low) since the host lacks a 64-bit GPR.

**Template:** load value at `[r10]` (sp[0]) into `rax`, then `cmp` against `[rbx]` (acc) at the chosen width.

```asm
; %0C2h -- n = 4
mov  rax, [r10]
cmp  eax, dword ptr [rbx]
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1C2h` | 1 | `mov rax,[r10]; cmp al, byte ptr [rbx]` |
| `%2C2h` | 2 | `mov rax,[r10]; cmp ax, word ptr [rbx]` |
| `%0C2h` | 4 | `mov rax,[r10]; cmp eax, dword ptr [rbx]` |
| `%4C2h` | 8 | `mov rax,[r10]; cmp rax, [rbx]` |

## JIT (x32)
**Template:** load `[esi]` (sp[0]) then `cmp` against `[ebx]`. The 64-bit case is open-coded with `sub`/`sbb` to materialise both equal and signed-less.

```asm
; %0C2h -- n = 4
mov  eax, [esi]
cmp  eax, [ebx]
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1C2h` | 1 | `mov eax,[esi]; cmp al, byte ptr [ebx]` |
| `%2C2h` | 2 | `mov eax,[esi]; cmp ax, word ptr [ebx]` |
| `%0C2h` | 4 | `mov eax,[esi]; cmp eax, [ebx]` |
| `%4C2h` | 8 | dual `sub`/`sbb` over `[esi..]` and `[ebx..]`, packs eq+sign into `eax`, then `cmp ecx, eax` |

For n=8 on x32 the sequence is:

```asm
xor   eax, eax
mov   edi, [esi]
sub   edi, [ebx]
mov   ecx, [esi+4]
sbb   ecx, [ebx+4]
sets  ah
or    ecx, edi
setz  al
mov   ecx, 1
cmp   ecx, eax
```

## JIT (aarch64)
**Template:** sign-extend both operands at the chosen width into x17/x18, `cmp`.

```asm
; %0C2h -- n = 4
ldrsw   x17, [x0]
ldrsw   x18, [x10]
cmp     x17, x18
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1C2h` | 1 | `ldrsb x17,[x0]; ldrsb x18,[x10]; cmp x17,x18` |
| `%2C2h` | 2 | `ldrsh x17,[x0]; ldrsh x18,[x10]; cmp x17,x18` |
| `%0C2h` | 4 | `ldrsw x17,[x0]; ldrsw x18,[x10]; cmp x17,x18` |
| `%4C2h` | 8 | `ldr x17,[x0]; ldr x18,[x10]; cmp x17,x18` |

## JIT (ppc64le)
**Template:** load both at the chosen width and compare.

```asm
; %0C2h -- n = 4
lwz      r17, 0(r3)
lwz      r18, 0(r15)
cmp      r17, r18
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1C2h` | 1 | `lbz r17,0(r3); lbz r18,0(r15); cmp r17,r18` |
| `%2C2h` | 2 | `lhz r17,0(r3); lhz r18,0(r15); cmp r17,r18` |
| `%0C2h` | 4 | `lwz r17,0(r3); lwz r18,0(r15); cmp r17,r18` |
| `%4C2h` | 8 | `ld r17,0(r3); ld r18,0(r15); cmpd r17,r18` |

## Notes
- Width `n`  in  {1,2,4,8}.
- Signed comparison (`cmp` on x86/amd64, `cmp` on aarch64, `cmp`/`cmpd` on ppc64le).
- For unsigned semantics see `selult` (0xDF) and friends.
- On x32, n=8 emulates 64-bit compare by hand (`sub`/`sbb` over both halves, then packs eq+sign into a single flag word) since the host has no 64-bit GPRs.
- Selection rule routes through `retrieveICode`; n must be one of the four widths or the JIT falls back to slot 0 (n=4) which silently truncates.
