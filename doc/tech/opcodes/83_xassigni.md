# 0x83 -- xassign i

- **Category:** DoubleOp
- **Enum:** `ByteCode::XAssignI`
- **Operand(s):** `xassign i`
- **Reads:** `acc`, `sp[0]`
- **Writes:** `acc[i]`
- **Side effects:** No GC write barrier (direct store).

## Semantics
`acc[i] := sp[0]` -- write the cached `sp[0]` value into field `i` of the object pointed to by `acc`. Direct variant: no GC write barrier (caller must guarantee target is not in old generation or value is non-pointer). Compare with `assigni` (full version with barrier).

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %83h`

```asm
  mov  [rbx + __arg32_1], r10
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %83h`

```asm
  mov  [ebx + __arg32_1], esi
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %83h`

```asm
  add     x11, x10, __arg12_1
  str     x0, [x11]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %83h`

```asm
  addi    r16, r15, __arg16_1
  std     r3, 0(r16)
```

## Notes
- Direct, unguarded store -- no GC write barrier; callers must ensure the target is young-gen or the value is non-pointer.
- Companion full-barrier form is `assigni i` (0x49).
- Reads `sp[0]` from the cached register (no memory access on amd64/aarch64/ppc64le when cached).
- Preserves `acc`, `index`, `sp[0..1]`.
