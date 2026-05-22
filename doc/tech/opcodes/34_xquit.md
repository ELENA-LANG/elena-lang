# 0x34 -- xquit

- **Category:** SingleOp
- **Enum:** `ByteCode::XQuit`
- **Operand(s):** (none)
- **Reads:** index
- **Writes:** ABI return register, PC (return)
- **Side effects:** returns from current native frame; result is `index` (not `acc`)

## Semantics
Returns from the current native frame using `index` as the return value (placed in the ABI return register).

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %34h`

```asm
  mov  rax, rdx
  ret
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %34h`

```asm
  mov  eax, edx
  ret
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %34h`

```asm
  mov     x0, x9
  ret     x30
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %34h`

```asm
  mr      r3, r14
  blr
```

## Notes
- Counterpart to `quit` (0x04): returns from the native frame, but the return value is `index` instead of `acc`.
- Used in integer-returning leaf functions where the result already lives in the `index` register.
- The ABI return register varies per arch (`rax` on amd64, `eax` on x32, `x0` on aarch64, `r3` on ppc64le).
- ppc64le `blr` returns via the link register saved at call time; aarch64 explicitly names `x30`.
- Does not unwind any try/catch hooks -- only the simple frame is collapsed.
