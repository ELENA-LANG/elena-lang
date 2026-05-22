# 0x13 -- mov frm

- **Category:** SingleOp
- **Enum:** `ByteCode::MovFrm`
- **Operand(s):** (none)
- **Reads:** `fp`
- **Writes:** `index`

## Semantics
`index := fp` -- copies the current frame pointer into `index`. On 64-bit targets, store with `lsave` / `lsavedp` to keep the full pointer.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %13h`

```asm
  mov  rdx, rbp
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %13h`

```asm
  mov  edx, ebp
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %13h`

```asm
  mov  x9, x29
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %13h`

```asm
  mr  r14, r31
```

## Notes
- Captures the current frame pointer into `index`. Used for setjmp-style operations and to record an unwind anchor.
- Preserves `acc`, `sp[0]`, `sp[1]`. Clobbers only `index`.
- Persist with `lsave` (0x24) on 64-bit targets to keep the full pointer width; `save` (0x09) would truncate to 32 bits.
