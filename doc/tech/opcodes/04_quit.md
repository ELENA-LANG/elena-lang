# 0x04 -- quit

- **Category:** SingleOp
- **Enum:** `ByteCode::Quit`
- **Operand(s):** (none)
- **Reads:** return address on the stack
- **Writes:** PC
- **Side effects:** Returns from the current procedure. `acc` and `index` are preserved across the return.

## Semantics
Function return. Pops the return address (or branches to `lr` on PPC/AArch64) and resumes the caller. Both `acc` and `index` flow through unchanged so the caller can observe the result.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %4`

```asm
  ret
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %4`

```asm
  ret
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %4`

```asm
  ret x30
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %4`

```asm
  blr
```

## Notes
- Function return; the caller's contract is "result in `acc`". Both `acc` and `index` flow through unchanged so the caller can also observe the message/int result if needed.
- sp[0]/sp[1] are undefined after -- anything below the new `sp` is discarded.
- ppc64le uses `blr` (branch via link register) and aarch64 uses `ret x30`; on x86/amd64 the bare `ret` consumes the saved RIP off the stack.
