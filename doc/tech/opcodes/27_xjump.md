# 0x27 -- xjump

- **Category:** SingleOp
- **Enum:** `ByteCode::XJump`
- **Operand(s):** (none)
- **Reads:** acc
- **Writes:** PC (indirect jump)

## Semantics
Indirect jump to the address in `acc`. No return.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %27h`

```asm
  jmp rbx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %27h`

```asm
  jmp ebx
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %27h`

```asm
  br      x10
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %27h`

```asm
  mtctr    r15            ; put code address into ctr
  bctr                    ; and jump to it
```

## Notes
- Indirect tail-jump through `acc` -- no link register saved, no return path.
- Caller must clear method arguments off the stack **before** emitting `xjump`; the callee inherits a clean frame.
- Distinct from `xcall` (0x2F) which preserves the return address.
- ppc64le requires the two-instruction `mtctr` + `bctr` sequence because there is no register-direct branch.
- Falls through to whatever code begins at `[acc]`; verifying that `acc` points to executable code is the JIT/runtime's responsibility.
