# 0x21 -- not

- **Category:** SingleOp
- **Enum:** `ByteCode::Not`
- **Operand(s):** (none)
- **Reads:** index
- **Writes:** index

## Semantics
Bitwise complement of `index` (treated as 32-bit on x86 / aarch64; full register on amd64/ppc64le in current backends).

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %21h`

```asm
  not    rdx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %21h`

```asm
  not    edx
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %21h`

```asm
  mvn    x9, x9
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %21h`

```asm
  nand    r14, r14, r14
```

## Notes
- Bitwise NOT operates on `index`; `acc` and stack are untouched.
- amd64 emits `not rdx` which inverts the full 64-bit register -- for a strict 32-bit semantics, follow with a zero-extension or mask the high half.
- For 64-bit complement on backends that treat `index` as 32-bit, pair with `lneg`-style sequences or emit `not edx; xor rdx, 0xFFFFFFFF00000000` manually.
- Does not set comparison flags meaningfully; do not rely on flags after this opcode.
- ppc64le uses `nand r,r,r` as the canonical complement (no dedicated NOT mnemonic).
