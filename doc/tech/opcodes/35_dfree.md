# 0x35 -- dfree

- **Category:** SingleOp
- **Enum:** `ByteCode::DFree`
- **Operand(s):** (none)
- **Reads:** index, sp
- **Writes:** sp

## Semantics
Releases `index` dynamic stack slots: computes `index * word_size`, rounds up to 16-byte ABI alignment, and adds the result to `sp`. Mirror of `dalloc` for an index-controlled size.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %35h`

```asm
  lea  rax, [rdx*8]
  add  eax, 7
  and  eax, 0FFFFFFF0h

  add  rsp, rax
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %35h`

```asm
  lea  eax, [edx*4]
  add  esp, eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %35h`

```asm
  lsl     x12, x9, #3
  add     x12, x12, #8    ; rounding to 10h

  lsr     x12, x12, #4
  lsl     x12, x12, #4

  mov     x13, sp
  add     x13, x13, x12   ; allocate stack
  mov     sp, x13
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %35h`

```asm
  sldi    r16, r14, 3

  addi    r16, r16, 8     ; rounding to 10h
  srdi    r16, r16, 4
  sldi    r16, r16, 4

  add     r1, r1,  r16   ; allocate stack
```

## Notes
- Pairs strictly with `dalloc` (0x16) -- the byte count derived from `index` **must** match what the corresponding `dalloc` reserved.
- 64-bit backends round the byte count up to 16 to honor the ABI's stack-alignment requirement; x32 skips the rounding because its ABI allows 4-byte alignment.
- Mismatches between `dalloc` and `dfree` corrupt the stack silently -- there is no runtime check.
- `index` register is consumed (its value is read) but not preserved across the alignment math on most arches; treat as clobbered.
- aarch64 routes the new SP via a GPR (`mov x13, sp ... mov sp, x13`) because direct `add sp, sp, xN` is restricted by encoding.
