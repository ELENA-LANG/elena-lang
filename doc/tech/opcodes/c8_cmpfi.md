# 0xC8 -- cmp fp

- **Category:** DoubleOp
- **Enum:** `ByteCode::CmpFI`
- **Operand(s):** `cmp fp:i`
- **Reads:** `acc`, `fp[i]`
- **Writes:** `COMP.EQ`, `COMP.LT`

## Semantics
`COMP.EQ := (acc == fp[i])`; `COMP.LT := (acc < fp[i])` -- signed pointer-width compare against a frame slot.

## JIT (amd64)
**Selection rule:** dispatched through `loadFrameIndexOp` -> `retrieveCodeWithNegative` (`jitcompiler.cpp:980, 634`). The displacement is converted via `getFPOffset(arg << indexPower)` to the actual byte offset from the frame pointer. The picker keys on the **sign** of `i` -- negative indices pick slot 5 (e.g. `%0C8h`), positive indices pick slot 0 (e.g. `%5C8h`). aarch64 needs this distinction because `add`/`sub` take an unsigned 12-bit immediate; amd64 collapses to a single template.

**Template:** memory operand on a 64-bit `cmp`.

```asm
cmp  rbx, qword ptr [rbp + __arg32_1]
```

## JIT (x32)
**Template:** materialise the slot into a scratch and `cmp`.

```asm
mov  eax, [ebp + __arg32_1]
cmp  ebx, eax
```

## JIT (aarch64)
**Template:** sign of `i` decides whether to subtract or add to `x29` (frame pointer); two prefixes encode the direction.

```asm
; %0C8h -- arg1 < 0 (the JIT pre-inverts the sign)
sub     x11, x29, -__arg12_1
ldr     x11, [x11]
cmp     x10, x11
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0C8h` | `i < 0` | `sub x11, x29, -i; ldr x11,[x11]; cmp x10, x11` |
| `%5C8h` | `i > 0` | `add x11, x29, i; ldr x11,[x11]; cmp x10, x11` |

## JIT (ppc64le)
**Template:** `ld` from `r31 + i`, `cmpd r15, r16`.

```asm
ld      r16, __arg16_1(r31)
cmpd    r15, r16
```

## Notes
- Differs from `cmp sp:i` (0xC9) only in the base register (`fp` vs `sp`); both produce signed pointer-width compares.
- On aarch64 the JIT splits by the sign of `i` to use `add`/`sub` with an unsigned 12-bit immediate -- two prefixes (`%0C8h`/`%5C8h`) cover negative and positive frame offsets.
- The slot index is transformed via `getFPOffset` before patching, so the patched displacement is in *bytes*, not slots.
- Reads `acc` and the frame slot; both `acc`/`index` survive -- only COMP flags are written.
