# 0xA5 -- get i

- **Category:** DoubleOp
- **Enum:** `ByteCode::GetI`
- **Operand(s):** `get i`
- **Reads:** `acc`, `[acc + i]`
- **Writes:** `acc`
- **Side effects:** None -- unchecked field load.

## Semantics
`acc := acc[i]` -- load field `i` of the object referenced by `acc`. The argument is the byte displacement of the field within the object body (already scaled by the bytecode writer).

## JIT (amd64)

**Selection rule:** `loadIndexOp` (`jitcompiler.cpp:1086`) calls `retrieveICode(scope, arg)` (`jitcompiler.cpp:2399`). The "I" code path picks a short-form variant when `arg` is a power-of-two field index (1, 2, 4, 8) -- those map to dedicated `inline %nA5h` templates that drop the `add` and inline the displacement directly. Otherwise the generic 32-bit form is emitted.

**Template:** `asm/amd64/core60.asm` `inline %0A5h`

```asm
  mov  rbx, [rbx + __arg32_1]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0A5h`

```asm
  mov  ebx, [ebx + __arg32_1]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0A5h`

```asm
  add     x11, x10, __arg12_1
  ldr     x10, [x11]
```

### Variants
| Prefix | When | Body |
|---|---|---|
| `%0A5h` | `i >= 0`              | `add x11, x10, i; ldr x10, [x11]` |
| `%5A5h` | `i < 0` (encoded `-i`) | `sub x11, x10, -i; ldr x10, [x11]` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0A5h`

```asm
  addi    r16, r15, __arg16_1
  ld      r15, 0(r16)
```

## Notes

- Unchecked field load -- there is **no bounds check**; if `acc` is nil or `i` is out of range, the result is whatever happens to live at `[acc + i]` (caller is responsible for guarding via `cmpr nil:0` / `peeknd` patterns).
- `acc` is overwritten with the loaded value; previous `acc` is lost.
- Companion: `xstore i:i` (0xAE) loads into the cached `sp[0]` instead of `acc`.
- aarch64 splits the template on sign of `i` to keep the `imm12` non-negative.
