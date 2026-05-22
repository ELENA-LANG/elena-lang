# 0xCD -- xloadarg fp

- **Category:** DoubleOp
- **Enum:** `ByteCode::XLoadArgFI`
- **Operand(s):** `xloadarg fp:i` (or `xloadarg sp:i` on aarch64/ppc64le)
- **Reads:** `fp[i]` / `sp[i]`
- **Writes:** `index`

## Semantics
Load an OS-passed argument into the data accumulator. The argument size depends on the platform calling convention (4 on x32, 8 on the 64-bit hosts).

## JIT (amd64)
**Selection rule:** dispatched through `loadFrameIndexOp` -> `retrieveCodeWithNegative`. The displacement is converted via `getFPOffset(arg << indexPower)`. The picker keys on the sign / magnitude of `i` (negative -> slot 5, positive in range -> slot 0, plus peepholes for `i = 0/1` that read the ABI argument registers directly on aarch64/ppc64le).

**Template:** 64-bit load from the frame.

```asm
mov  rdx, qword ptr [rbp + __arg32_1]
```

## JIT (x32)
**Template:** 32-bit load from the frame.

```asm
mov  edx, [ebp + __arg32_1]
```

## JIT (aarch64)
**Template:** load from `sp + imm` -- on aarch64 OS arguments live in the shadow stack region; peepholes use x0/x1.

```asm
add     x11, sp, __arg12_1
ldr     x9, [x11]
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0CDh` | generic | `add x11, sp, i; ldr x9, [x11]` |
| `%1CDh` | `sp:0` | `mov x9, x0` |
| `%2CDh` | `sp:1` | `mov x9, x1` |

## JIT (ppc64le)
**Template:** `ld` from `r1 + imm`; peepholes via r3/r4.

```asm
ld      r14, __arg16_1(r1)
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0CDh` | generic | `ld r14, i(r1)` |
| `%1CDh` | `sp:0` | `mr r14, r3` |
| `%2CDh` | `sp:1` | `mr r14, r4` |

## Notes
- Distinguished from plain `load sp:i` (0xCC) because the data is laid down by the host ABI (Win64 / SysV / AArch64 PCS / PPC ELF v2), not by ELENA's own `push`.
- On amd64/x32 it's a frame-relative load (Win64 shadow space + SysV register spill area); aarch64/ppc64le surface it as stack-relative since their PCS spill registers above `sp`.
- aarch64/ppc64le get peephole forms for `sp:0`/`sp:1` that read from the ABI argument registers directly.
- Slot index goes through `getFPOffset` so the patched displacement is in bytes, accounting for the saved-frame header.
