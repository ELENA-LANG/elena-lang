# 0x0D -- xcmp

- **Category:** SingleOp
- **Enum:** `ByteCode::XCmp`
- **Operand(s):** (none)
- **Reads:** `index`, `[acc]` (32-bit cell)
- **Writes:** `COMP` (host condition flags / cmp result)

## Semantics
Compares `index` against the 32-bit value at `[acc]`, leaving the host condition flags reflecting `index - [acc]`. Used by the following conditional-jump opcodes to test for equality.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline % 0Dh`

```asm
  mov  ecx, dword ptr [rbx]
  cmp  edx, ecx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline % 0Dh`

```asm
  mov  ecx, [ebx]
  cmp  edx, ecx
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0Dh`

```asm
  ldrsw   x14, [x10]
  cmp     x9, x14
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0Dh`

```asm
  lwz      r18, 0(r15)
  cmpd     r14, r18
```

## Notes
- 32-bit compare of `index` against `[acc]`. 64-bit form is `xlcmp` (0x1C).
- Sets `COMP.EQ` and `COMP.LT` (host flags or condition register). Followed by a conditional jump opcode to act on the result.
- Preserves `acc`, `index`, `sp[0]`, `sp[1]`. Reads memory at `[acc]` only.
