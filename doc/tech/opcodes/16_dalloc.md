# 0x16 -- dalloc

- **Category:** SingleOp
- **Enum:** `ByteCode::DAlloc`
- **Operand(s):** (none)
- **Reads:** `index`
- **Writes:** `sp` (grows downward by `index` slots, zero-filled)
- **Side effects:** Reserves stack with zero-fill; on 64-bit targets aligns the resulting `sp` to a 16-byte boundary.

## Semantics
`sp -= index*slot_size` (zero-filled). Index-driven counterpart of `alloci n`. The fixed counterpart `dfree` releases the same amount.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %16h`

```asm
  lea  rax, [rdx*8]
  add  eax, 8
  and  eax, 0FFFFFFF0h

  sub  rsp, rax
  mov  rcx, rdx
  xor  rax, rax
  mov  rdi, rsp
  rep  stos
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %16h`

```asm
  lea  eax, [edx*4]
  sub  esp, eax
  mov  ecx, edx
  xor  eax, eax
  mov  edi, esp
  rep  stos
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %16h`

```asm
  lsl     x12, x9, #3
  add     x12, x12, #8    ; rounding to 10h

  lsr     x12, x12, #4
  lsl     x12, x12, #4

  mov     x13, sp
  sub     x13, x13, x12   ; allocate stack
  mov     x11, 0
  mov     sp, x13

labLoop:
  cmp     x12, 0
  beq     labEnd
  sub     x12, x12, 8
  str     x11, [x13], #8
  b       labLoop

labEnd:
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %16h`

```asm
  sldi    r16, r14, 3

  addi    r16, r16, 8     ; rounding to 10h
  srdi    r16, r16, 4
  sldi    r16, r16, 4

  sub     r1, r1,  r16   ; allocate stack
  li      r17, 0
  mr      r18, r1

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:
```

## Notes
- Dynamic stack alloc with zero-fill; size in `index`. Pairs with `dfree` (0x35) which releases the same amount.
- 64-bit arches round the byte size up to 16 before subtracting from `sp` so the stack stays 16-byte aligned (ABI requirement for native callees). x32 does not round -- slot is 4 bytes and ESP needs only 4-byte alignment.
- Zero-fill is mandatory: GC scans the live stack as roots, so leaving stale values would create spurious references.
- Preserves `acc`. Clobbers `sp`, scratch registers (`rcx`, `rax`, `rdi` on amd64). `index` survives on aarch64/ppc64le but is consumed (`ecx <- edx`) on x86.
