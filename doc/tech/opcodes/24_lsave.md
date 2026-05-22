# 0x24 -- lsave

- **Category:** SingleOp
- **Enum:** `ByteCode::LSave`
- **Operand(s):** (none)
- **Reads:** index (low) / eax (high on x32), acc
- **Writes:** qword at `[acc]`

## Semantics
Stores `index` as a 64-bit value at `[acc]`. On 32-bit targets the high half comes from `eax`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %24h`

```asm
  mov  qword ptr [rbx], rdx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %24h`

```asm
  mov  [ebx + 4], edx
  mov  [ebx], eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %24h`

```asm
  str    x9, [x10]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %24h`

```asm
  std     r14, 0(r15)
```

## Notes
- 64-bit qword store at `[acc]` -- the canonical "write a long" primitive.
- On x32 the value is composed of two registers (`eax` high, `edx` low) and emitted as two 32-bit stores; both halves must be set up before this opcode.
- Pairs naturally with `lload` (0x1A) for 64-bit value transport.
- Caller must guarantee that `[acc]` has at least 8 bytes available; no alignment fault recovery is provided.
- `acc` and `index` are preserved (their values are read, not written).
