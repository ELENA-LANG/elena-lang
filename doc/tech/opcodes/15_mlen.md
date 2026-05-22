# 0x15 -- mlen

- **Category:** SingleOp
- **Enum:** `ByteCode::MLen`
- **Operand(s):** (none)
- **Reads:** `index`
- **Writes:** `index`

## Semantics
`index := index & ARG_MASK` -- extracts the argument-count bits of a message word currently in `index`. Action/flags components are discarded.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %15h`

```asm
  and   edx, ARG_MASK
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %15h`

```asm
  and   edx, ARG_MASK
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %15h`

```asm
  and   x9, x9, ARG_MASK
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %15h`

```asm
  andi.   r14, r14, ARG_MASK
```

## Notes
- Extracts argument count from a packed `mssg_t` already in `index`: `index := index & ARG_MASK` (ARG_MASK = 0x1F).
- Single-instruction on every arch. ppc64le's `andi.` additionally sets CR0, but the rest of the pipeline does not consume that side effect.
- Preserves `acc`, `sp[0]`, `sp[1]`. Clobbers `index`.
