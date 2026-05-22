# 0x20 -- coalesce

- **Category:** SingleOp
- **Enum:** `ByteCode::Coalesce`
- **Operand(s):** (none)
- **Reads:** acc, sp[0]
- **Writes:** acc
- **Side effects:** sets CPU flags (test acc)

## Semantics
Null-coalesce: if `acc` is nil (zero), replace it with the value at `sp[0]`; otherwise leave `acc` unchanged. Stack pointer is not adjusted.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %20h`

```asm
  test  rbx, rbx
  cmovz rbx, r10
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %20h`

```asm
  test   ebx, ebx
  cmovz  ebx, esi
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %20h`

```asm
  cmp     x10, #0
  csel    x10, x0, x10, eq
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %20h`

```asm
  cmpwi   r15, 0
  iseleq  r15, r3, r15
```

## Notes
- Null-check / null-coalesce pattern: returns `acc` if non-nil, else falls back to `sp[0]`.
- `index` register is preserved across this opcode -- only `acc` may change.
- Stack is not popped; `sp[0]` remains live for any subsequent opcode.
- Commonly emitted after method calls whose result may be `nil`, fused with the default-value push.
- CPU flags are clobbered (the `test`/`cmp` is part of the sequence) -- do not depend on previous flag state.
