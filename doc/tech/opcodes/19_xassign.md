# 0x19 -- xassign

- **Category:** SingleOp
- **Enum:** `ByteCode::XAssign`
- **Operand(s):** (none)
- **Reads:** `acc`, `index`, `sp[0]`
- **Writes:** `acc[index]`

## Semantics
`acc[index] := sp[0]` -- direct store, no write barrier. The "x"-prefix denotes the unbarriered counterpart of `0x12 assign`. Use only when the destination is known not to hold a managed reference (struct fields, stack-allocated objects, freshly allocated arrays under construction).

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %19h`

```asm
  mov  [rbx + rdx*8], r10
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %19h`

```asm
  mov  [ebx + edx*4], esi
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %19h`

```asm
  lsl     x14, x9, 3
  add     x14, x14, x10
  str     x0, [x14]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %19h`

```asm
  sldi    r16, r14, 3
  add     r16, r16, r15
  std     r3, 0(r16)
```

## Notes
- Like `assign` (0x12) but WITHOUT a write barrier. Caller must guarantee the target slot is not a managed-pointer cell, OR that the target lives outside the GC-tracked heap (PERM/static memory, struct fields, freshly allocated arrays before they go live).
- Stride is `*8` on 64-bit, `*4` on x32 (slot-indexed, not byte-indexed).
- Preserves `acc`, `index`, `sp[0]`. Mutates only `acc[index]`.
