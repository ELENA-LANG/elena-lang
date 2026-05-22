# 0x8A -- load dp

- **Category:** DoubleOp
- **Enum:** `ByteCode::LoadDP`
- **Operand(s):** `load dp:disp`
- **Reads:** `fp`, `dp[disp]`
- **Writes:** `index`

## Semantics
`index := dp[disp]` -- load a 32-bit slot from the data frame, sign-extended into the data accumulator on 64-bit targets. Use `lload dp:disp` (0xAC) for the full 64-bit load.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %8Ah`

**Selection rule:** `retrieveCode` selects the offset-encoding variant; the argument is then rewritten as `getFPOffset(arg1, dataOffset)` -- the JIT translates the abstract dp index into the actual frame-relative byte offset (negative for previous frame, positive for current).

```asm
  movsxd rdx, dword ptr [rbp + __arg32_1]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %8Ah`

```asm
  mov  edx, [ebp + __arg32_1]
```

Plain 32-bit load -- `edx` is the native word size, no sign extension needed.

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %8Ah`

```asm
  add     x11, x29, __arg12_1
  ldrsw   x9,  [x11]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %8Ah`

```asm
  addi    r16, r31, __arg16_1
  lwz     r14, 0(r16)
```

## Notes
- Loads 32-bit slot and sign-extends to 64-bit on amd64/aarch64/ppc64le; on x32 the load is naturally 32-bit.
- Companion `lload dp` (0xAC) performs the full 64-bit load instead.
- Argument is the abstract dp index; JIT rewrites it to a frame-relative byte offset via `getFPOffset`.
- Preserves `acc`, `sp[0..1]`; only `index` is written.
