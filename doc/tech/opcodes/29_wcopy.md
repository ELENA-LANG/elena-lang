# 0x29 -- wcopy

- **Category:** SingleOp
- **Enum:** `ByteCode::WCopy`
- **Operand(s):** (none)
- **Reads:** sp[0], acc
- **Writes:** memory at `[acc]`

## Semantics
Reads a word (2 bytes) from `sp[0]` and stores it as an integer at `[acc]`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %29h`

```asm
  mov  rsi, r10
  xor  rax, rax
  mov  ax, word ptr [rsi]
  mov  [rbx], rax
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %29h`

```asm
  xor  eax, eax
  mov  ax, word ptr [esi]
  mov  dword ptr [ebx], eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %29h`

```asm
  ldrsw   x17, [x0]
  str     x17, [x10]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %29h`

```asm
  lwz     r17, 0(r3)
  stw     r17, 0(r15)
```

## Notes
- Single 2-byte copy from `[sp[0]]` to `[acc]`; the zero-stride sibling of `wread` (0x26).
- Eligible for aligned-load fast paths when source and destination are word-aligned; otherwise hardware handles the misalignment penalty.
- aarch64 uses sign-extending `ldrsw` (note: loads 4 bytes here despite the wcopy name -- verify intended semantics in the .asm).
- amd64/x32 zero-extend through `xor eax, eax` first; the destination is always a full machine word.
- `index` is ignored; only `acc` and `sp[0]` are inputs.
