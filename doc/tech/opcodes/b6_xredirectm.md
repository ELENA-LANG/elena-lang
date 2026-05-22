# 0xB6 -- xredirect (M-cmd)

- **Category:** DoubleOp (M-cmd)
- **Enum:** `ByteCode::XRedirectM`
- **Operand(s):** `xredirect m`
- **Reads:** `acc` (receiver), `[acc - elVMTOffset]` (VMT), `index` (message), `[VMT - elVMTSizeOffset]` (entry count)
- **Writes:** `pc` (on hit, tail-jumps to handler); falls through on miss
- **Side effects:** Binary-search dispatch loop. `index`/`acc` are preserved across the search; on success the original message is restored before the jump.

## Semantics
Binary-search the receiver's VMT for an entry whose key equals `(message.action | m.arg_count)` -- i.e. take the *action* bits of the current message (`index`) and OR in the *arg-count* bits of the immediate operand `m`. On a hit, tail-jump to the handler; on a miss, fall past the loop. The "x" form preserves `index` (original message) on both exits.

Compare with `redirect` (0x03), which uses the message in `index` unchanged. Compare also with `xdispatchmr` (0xFA) / `dispatchmr` (0xFB), which dispatch via a separate message table.

## JIT (amd64)

**Selection rule:** M-cmd dispatched through `loadMOp` -- the operand is the message immediate `m`, rewritten via `importMessage(arg)` so cross-module message identifiers are translated to the JIT-local module's numbering. The same inline template is reused for every call site; only the `__arg32_1` slot changes.

**Template:** `asm/amd64/core60.asm` `inline % 0B6h`

Header comment: `(rbx - object, rdx - message, r10 - arg0, r11 - arg1)`.

```asm
  mov  r15, rdx
  mov  r14, [rbx - elVMTOffset]
  xor  ecx, ecx

  and  edx, ARG_ACTION_MASK
  mov  eax, __arg32_1
  and  eax, ~ARG_MASK
  or   edx, eax
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
  mov   rdx, r15
  jmp   [r14+r13*8+8]

labEnd:
  mov   rdx, r15
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline % 0B6h`

Header comment: `(ebx - object, edx - message, esi - arg0, edi - arg1)`.

```asm
  mov   [esp+4], esi                      ; saving arg0
  xor   ecx, ecx
  push  edx
  mov   edi, [ebx - elVMTOffset]
  mov   eax, __arg32_1
  and   edx, ARG_ACTION_MASK
  and   eax, ~ARG_MASK
  mov   esi, [edi - elVMTSizeOffset]
  or    edx, eax

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
labFound:
  pop   edx
  mov   eax, [edi+esi*8+4]
  mov   esi, [esp+4]
  jmp   eax

labEnd:
  pop   edx
  mov   esi, [esp+4]
```

`esi` (`sp[0]` cache) is spilled to the frame across the search and restored on both exits; `edx` (message) is pushed and popped so it remains live across the loop.

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline % 0B6h`

Header comment: `(r15 - object, r14 - message)`.

```asm
  mov     x20, x9
  sub     x14, x10, elVMTOffset
  ldr     x11, [x14]              ; edi
  mov     x12, #0                 ; ecx
  sub     x15, x11, elVMTSizeOffset
  ldr     x13, [x15]              ; esi

  movz    x14,  __arg32lo_1
  movk    x14,  __arg32hi_1, lsl #16
  mov     x15, ARG_ACTION_MASK
  and     x9, x9, x15
  movz    x16,  ~ARG_MASK
  movk    x16,  #0FFFFh, lsl #16
  and     x14, x14, x16
  orr     x9, x9, x14

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
  mov     x9, x20
  add     x14, x14, #8
  ldr     x15, [x14]
  br      x15

labEnd:
  mov     x9, x20
```

Original `index` (`x9`) is saved in `x20` and restored on both exits.

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline % 0B6h`

Header comment: `(r15 - object, r14 - message)`.

```asm
  mr      r20, r14
  ld      r16, -elVMTOffset(r15)      ; edi
  xor     r17, r17, r17               ; ecx
  ld      r7, -elVMTSizeOffset(r16)   ; esi

  lis     r18, __arg32hi_1
  addi    r18, r18, __arg32lo_1

  andi.   r14, r14, ARG_ACTION_MASK

  li      r19, ~ARG_MASK
  andi.   r19, r19, 0FFFFh
  addis   r19, r19, 0FFFFh

  and     r18, r18, r19
  or      r14, r14, r18

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
  cmp     r14, r23
  beq     labFound
  addi    r22, r22, 16
  blt     labSplit
  mr      r16, r22
  subf    r7, r21, r7
  b       labSplit
labFound:
  mr      r14, r20
  ld      r23, 8(r22)
  mtctr   r23
  bctr

labEnd:
  mr      r14, r20
```

Original message (`r14`) is saved in `r20` and restored before exit.

## Notes

- Binary-search VMT dispatch over the receiver's class. Search key is `(current message.action) | (m.arg_count)` -- only the action bits of `index` are preserved, the immediate operand contributes the arg-count bits.
- On hit: tail-jump to the handler with the **original** `index` restored. On miss: falls through (no exception); the caller is expected to follow with a `dispatchmr`/`xdispatchmr` against a fallback table.
- Preserves `acc`, `sp[0]` (spilled and restored on x32) on both hit and miss paths.
- The "x" prefix means `index` is preserved across the search; the non-"x" sibling `redirect` (0x03) uses `index` unchanged as the search key (no OR-in of arg-count).
- VMT entries must be **sorted** by message key for the binary search to be correct -- the linker is responsible for emitting them sorted.
- Cross-module messages are translated through `importMessage(arg)` at JIT time.
