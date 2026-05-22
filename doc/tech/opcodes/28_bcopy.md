# 0x28 -- bcopy

- **Category:** SingleOp
- **Enum:** `ByteCode::BCopy`
- **Operand(s):** (none)
- **Reads:** sp[0], acc
- **Writes:** memory at `[acc]`

## Semantics
Reads a zero-extended byte from `sp[0]` and stores it as an integer at `[acc]`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %28h`

```asm
  mov  rsi, r10
  xor  rax, rax
  mov  al, byte ptr [rsi]
  mov  [rbx], rax
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %28h`

```asm
  xor  eax, eax
  mov  al, byte ptr [esi]
  mov  dword ptr [ebx], eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %28h`

```asm
  ldrsb   x17, [x0]
  str     x17, [x10]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %28h`

```asm
  lbz     r17, 0(r3)
  std     r17, 0(r15)
```

## Notes
- Single-byte copy from `[sp[0]]` to `[acc]` (zero-extended into a word slot on the destination).
- The "copy" mnemonic is misleading -- this is a one-element load/store, not a bulk `rep movsb`. For block copies see the dedicated copy helpers in the runtime.
- aarch64 uses sign-extending `ldrsb`; amd64/x32/ppc64le use zero-extending loads -- same caveat as `bread` (0x23) for signed bytes.
- `index` register is **not** consulted -- this is the zero-stride version of `bread`.
- No alignment requirement (single byte access).
