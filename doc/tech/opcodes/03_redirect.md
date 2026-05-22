# 0x03 -- redirect

- **Category:** SingleOp
- **Enum:** `ByteCode::Redirect`
- **Operand(s):** (none)
- **Reads:** `acc` (object), `index` (message), `acc::VMT`
- **Writes:** PC (tail-jumps to the resolved handler)
- **Side effects:** Performs a binary search over `acc`'s VMT for an entry whose message id equals `index`, then tail-jumps to the matching handler. If no match is found, falls through to `labEnd`.

## Semantics
Dispatch helper. Treats `acc::VMT` as a sorted table of `(message, handler)` pairs; binary-searches it for the message currently in `index`. On a hit the handler is invoked as a tail call; on a miss control falls past the loop.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline % 03h`

```asm
  mov  r14, [rbx - elVMTOffset]
  xor  ecx, ecx
  mov  rsi, qword ptr[r14 - elVMTSizeOffset]

labSplit:
  test esi, esi
  jz   short labEnd

labStart:
  shr   esi, 1
  lea   r13, [rsi*2]
  setnc cl
  cmp   rdx, [r14+r13*8]
  je    short labFound
  lea   r8, [r14+r13*8]
  jb    short labSplit
  lea   r14, [r8+16]
  sub   esi, ecx
  jmp   labSplit
  nop
  nop
labFound:
  jmp   [r14+r13*8+8]

labEnd:
```

Header comment: `(rbx - object, rdx - message, r10 - arg0, r11 - arg1)`.

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline % 03h`

```asm
  mov   [esp+4], esi                      ; saving arg0
  xor   ecx, ecx
  mov   edi, [ebx - elVMTOffset]
  mov   esi, [edi - elVMTSizeOffset]

labSplit:
  test  esi, esi
  jz    short labEnd

labStart:
  shr   esi, 1
  setnc cl
  mov   eax, [edi+esi*8]
  cmp   edx, eax
  je    short labFound
  lea   eax, [edi+esi*8]
  jb    short labSplit
  lea   edi, [eax+8]
  sub   esi, ecx
  jmp   short labSplit
  nop
labFound:
  mov   eax, [edi+esi*8+4]
  mov   esi, [esp+4]
  jmp   eax

  rgw nop [eax + eax + 0]

labEnd:
  mov   esi, [esp+4]
```

Header comment: `(ebx - object, edx - message, esi - arg0, edi - arg1)`. `esi` (sp[0]) is spilled into the frame across the search and restored on both exits.

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline % 03h`

```asm
  sub     x14, x10, elVMTOffset
  ldr     x11, [x14]              ; edi
  mov     x12, #0                 ; ecx
  sub     x15, x11, elVMTSizeOffset
  ldr     x13, [x15]              ; esi

labSplit:
  cmp     x13, #0
  beq     labEnd

labStart:
  tst     x13, #1
  lsr     x13, x13, #1
  cset    x12, eq

  lsl     x14, x13, #4
  add     x14, x14, x11

  ldr     x15, [x14]         ; edx
  cmp     x9, x15
  beq     labFound
  add     x14, x14, #16
  ble     labSplit
  mov     x11, x14
  sub     x13, x13, x12
  b       labSplit

labFound:
  add     x14, x14, #8
  ldr     x15, [x14]
  br      x15

labEnd:
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline % 03h`

```asm
  ld      r16, -elVMTOffset(r15)      ; edi
  xor     r17, r17, r17               ; ecx
  ld      r7, -elVMTSizeOffset(r16)   ; esi
  li      r19, 1

labSplit:
  cmpwi   r7, 0
  beq     labEnd

labStart:
  andi.   r0, r7, 1
  srdi    r7, r7, 1
  iseleq  r21, r19, r17                  ; ecx

  sldi    r22, r7, 4
  add     r22, r22, r16                  ; edx

  ld      r23, 0(r22)
  cmpd    r14, r23
  beq     labFound
  addi    r22, r22, 16
  blt     labSplit
  mr      r16, r22
  subf    r7, r21, r7
  b       labSplit
labFound:
  ld      r23, 8(r22)
  mtctr   r23
  bctr

labEnd:
```

## Notes
- VMT entries are pre-sorted by `mssg_t` at link time (see Linker layout in README), so the inline body performs a true binary search in O(log n).
- On a hit, control tail-jumps to the handler -- caller's `acc` (object) and `index` (message) are preserved through dispatch.
- On x32 the per-iteration body spills `esi` (sp[0]) to `[esp+4]` and reloads it on both exit paths; amd64/aarch64/ppc64le keep sp[0] in a register across the loop.
- Miss falls through to `labEnd`; surrounding bytecode is expected to handle the unresolved-message case.
