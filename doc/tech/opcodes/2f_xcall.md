# 0x2F -- xcall

- **Category:** SingleOp
- **Enum:** `ByteCode::XCall`
- **Operand(s):** (none)
- **Reads:** acc
- **Writes:** PC, link register (indirect call)
- **Side effects:** pushes return address

## Semantics
Indirect call to the address in `acc`. Execution resumes at the next opcode on return.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %2Fh`

```asm
  call rbx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %2Fh`

```asm
  call ebx
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %2Fh`

```asm
  blr     x10
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %2Fh`

```asm
  mtctr    r15            ; put code address into ctr
  bctrl                   ; and call it
```

## Notes
- Indirect call through `acc` (the address held in the acc register). Result lands back in `acc`.
- Counterpart to `xjump` (0x27) which is the no-return tail-jump form.
- Stack slots `sp[0]`/`sp[1]` are **undefined** after return -- the callee may consume them as arguments.
- ppc64le requires `mtctr` + `bctrl` (the `l` suffix saves the link register).
- aarch64 `blr` saves the return address in `x30`; amd64/x32 use the standard `call` push semantics.
