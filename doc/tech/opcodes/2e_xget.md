# 0x2E -- xget

- **Category:** SingleOp
- **Enum:** `ByteCode::XGet`
- **Operand(s):** (none)
- **Reads:** acc, index
- **Writes:** acc

## Semantics
Loads a pointer-sized field at dynamic offset `index` from object `acc`: `acc := acc[index]` (offset is `index * word_size`).

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %2Eh`

```asm
  mov  rbx, [rbx + rdx*8]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %2Eh`

```asm
  mov  ebx, [ebx + edx*4]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %2Eh`

```asm
  lsl     x14, x9, #3
  add     x17, x10, x14
  ldr     x10, [x17]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %2Eh`

```asm
  sldi    r22, r14, 3
  add     r22, r22, r15
  ld      r15, 0(r22)
```

## Notes
- Indexed field load: `acc := acc[index]` with pointer-sized stride.
- Stride is `*8` on 64-bit targets (amd64/aarch64/ppc64le) and `*4` on x32 -- driven by the per-arch `indexPower` constant.
- `index` register is preserved (it is only read); only `acc` is rewritten.
- aarch64 must materialize the shifted offset (`lsl x14, x9, #3`) because LDR with register-scale is restricted in this template.
- No null check on `acc` -- caller emits one when needed.
