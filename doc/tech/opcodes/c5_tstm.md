# 0xC5 -- tst mssg

- **Category:** DoubleOp
- **Enum:** `ByteCode::TstM`
- **Operand(s):** `tstm m` (M-cmd; message reference)
- **Reads:** `acc::VMT` (and its size word)
- **Writes:** `COMP.EQ`

## Semantics
Binary-search `m` in `acc::VMT`; `COMP.EQ := found`. VMT is a sorted (message -> handler) table; size word lives at `acc::VMT - elVMTSizeOffset`.

## JIT (amd64)
**Selection rule:** M-command -- dispatched through `loadMOp`. The message reference is rewritten via `importMessage` (mapping inter-module IDs to local ones) before the JIT inlines the search template. Variant prefix depends on the message reference kind (local, imported, extern).

**Template:** classic binary search; `r14` walks the base, `rsi` holds remaining count, `rax` is the needle. Loop labels are `labSplit/labStart/labFound/labEnd`.

```asm
mov  eax, __arg32_1
mov  r14, [rbx - elVMTOffset]
xor  ecx, ecx
mov  rsi, qword ptr [r14 - elVMTSizeOffset]

labSplit:
  test esi, esi
  jz   short labEnd

labStart:
  shr   esi, 1
  lea   r13, [rsi*2]
  setnc cl
  cmp   rax, [r14 + r13*8]
  je    short labFound
  lea   r8, [r14 + r13*8]
  jb    short labSplit
  lea   r14, [r8 + 16]
  sub   esi, ecx
  jmp   labSplit
  nop
  nop
labFound:
  mov  esi, 1

labEnd:
  cmp  esi, 1
```

## JIT (x32)
**Template:** same binary search; `esi` is spilled to a slot at `[esp+__n_2]` since x32 needs to preserve the sp[0] register convention.

```asm
mov   [esp+__n_2], esi      ; saving arg0
xor   ecx, ecx
mov   edi, [ebx - elVMTOffset]
mov   esi, [edi - elVMTSizeOffset]

labSplit:
  test  esi, esi
  jz    short labEnd

labStart:
  shr   esi, 1
  setnc cl
  mov   eax, __arg32_1
  cmp   eax, [edi + esi*8]
  je    short labFound
  lea   eax, [edi + esi*8]
  jb    short labSplit
  lea   edi, [eax + 8]
  sub   esi, ecx
  jmp   short labSplit

labFound:
  mov   esi, 1

labEnd:
  cmp   esi, 1
  mov   esi, [esp+__n_2]
```

## JIT (aarch64)
**Template:** binary search with x11/x13/x14/x15 as base/count/cursor/value; final `cmp x13, #1` materialises the EQ flag.

```asm
movz    x16, __arg32lo_1
movk    x16, __arg32hi_1, lsl #16

sub     x14, x10, elVMTOffset
ldr     x11, [x14]
mov     x12, #0
sub     x15, x11, elVMTSizeOffset
ldr     x13, [x15]

labSplit:
  cmp     x13, #0
  beq     labEnd

labStart:
  tst     x13, #1
  lsr     x13, x13, #1
  cset    x12, eq

  lsl     x14, x13, #4
  add     x14, x14, x11
  ldr     x15, [x14]
  cmp     x16, x15
  beq     labFound
  add     x14, x14, #16
  ble     labSplit
  mov     x11, x14
  sub     x13, x13, x12
  b       labSplit

labFound:
  mov     x13, #1

labEnd:
  cmp     x13, #1
```

## JIT (ppc64le)
**Template:** binary search; `iseleq` materialises the parity bit; `cmpwi r7, 1` sets CR0 at the end.

```asm
lis     r20, __arg32hi_1
addi    r20, r20, __arg32lo_1

ld      r16, -elVMTOffset(r15)
xor     r17, r17, r17
ld      r7, -elVMTSizeOffset(r16)
li      r19, 1

labSplit:
  cmpwi   r7, 0
  beq     labEnd

labStart:
  andi.   r0, r7, 1
  srdi    r7, r7, 1
  iseleq  r21, r19, r17

  sldi    r22, r7, 4
  add     r22, r22, r16
  ld      r23, 0(r22)
  cmp     r20, r23
  beq     labFound
  addi    r22, r22, 16
  blt     labSplit
  mr      r16, r22
  subf    r7, r21, r7
  b       labSplit

labFound:
  li      r7, 1

labEnd:
  cmpwi   r7, 1
```

## Notes
- Walks `acc`'s VMT for the given message; entries are 16 bytes each: `(message, handler-address)`.
- O(log n) via binary search since VMT entries are sorted by message id.
- The search progresses in two passes per iteration (`shr` then re-base), counting `setnc` / `iseleq` to compensate for odd sizes.
- Failure exits with `esi/x13/r7 == 0`, success with `1`, then the trailing `cmp ..., 1` sets `COMP.EQ`.
- Message reference goes through `importMessage` so cross-module IDs become local before the immediate is patched in.
