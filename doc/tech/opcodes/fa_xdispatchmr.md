# 0xFA -- xdispatch

- **Category:** TripleOp
- **Enum:** `ByteCode::XDispatchMR`
- **Operand(s):** `xdispatch m, r` (M+R)

## Semantics
Searches the overload list `r` for a message matching `m` against the parameter signature on the stack. If found, jumps **directly** to the resolved entry (no VMT lookup). Compare with `0xFB dispatchmr`, which dispatches through the VMT.

The opcode walks each overload entry, looking up the parameter class chain via the `mdata` action table and matching against the stacked argument types (using `elPackageOffset` to traverse base classes). Multiple specialized variants exist for different combinations (fixed-arg count, variadic, alt-mode).

## Compiler behaviour
`compileDispatchMR` (`jitcompiler.cpp:3075-3130`) implements an **11-slot variant selection** keyed by the `MESSAGE_FLAG_MASK` bits of the message argument plus the `scope->altMode` flag (set by a preceding 0x31 `altmode`).
- Variadic messages with `arg2` present -> `inlines[5 + altModifier]`.
- Binary selector -> `inlines[9]`.
- Fixed-arity messages -> `inlines[0..4]` depending on the action class.

Operands include the immediate signature and an arg-count field that are written via relocation entries (the JIT must patch the inline body with the resolved action-table address). After the dispatch is emitted, `scope->altMode` is cleared so the next opcode starts in normal mode.

## JIT (amd64)
**Selection rule:** M+R command. The JIT picks one of 11 inline slots based on `MESSAGE_FLAG_MASK` bits and `altMode`. Direct-`x`-prefixed dispatch jumps to the resolved entry without going through the VMT -- contrast with `dispatch mssg` (0xFB) which always indirects.

**Template:** `asm/amd64/core60.asm` `inline %0FAh`
```asm
; default (fixed arg count from __n_1)
mov  r8,  rbx
mov  [rsp+8], r10
lea  rax, [rsp + __n_2]
mov  [rsp+16], r11

mov  rsi, __ptr64_2
xor  edx, edx
mov  rbx, [rsi]                ; message from overload list

labNextOverloadlist:
  mov  r9, mdata : %0
  shr  ebx, ACTION_ORDER
  lea  r13, [rbx*8]
  mov  r13, [r9 + r13 * 2 + 8]
  mov  ecx, __n_1
  lea  rbx, [r13 - 8]

labNextParam:
  sub  ecx, 1
  jnz  short labMatching

  mov  r9, __ptr64_2
  lea  r13, [rdx * 8]
  mov  rbx, r8
  mov  rax, [r9 + r13 * 2 + 8]
  mov  rdx, [r9 + r13 * 2]
  jmp  rax                     ; DIRECT jump to entry

labMatching:
  mov  rdi, [rax + rcx * 8]
  ; nil check + walk class chain via elPackageOffset, compare
  ; against [rbx + rcx*8]; on match -> labNextParam; else advance edx

labNextBaseClass:
  ...
```

### Variants
| Prefix | Form |
|---|---|
| `%0FAh` | fixed arg count (`__n_1`) |
| `%5FAh` | variadic: counts args at runtime (terminator = -1) |
| `%9FAh` | indexed table walk (multiple overload lists, list index = `__n_1`) |
| `%0AFAh` | variadic + indexed table walk |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0FAh`
```asm
; default
mov  [esp+4], esi
lea  eax, [esp + __n_2]

mov  esi, __ptr32_2
push ebx
xor  edx, edx
mov  ebx, [esi]

labNextOverloadlist:
  shr  ebx, ACTION_ORDER
  mov  edi, mdata : %0
  mov  ebx, [edi + ebx * 8 + 4]
  mov  ecx, __n_1
  lea  ebx, [ebx - 4]

labNextParam:
  sub  ecx, 1
  jnz  short labMatching

  mov  esi, __ptr32_2
  pop  ebx
  mov  eax, [esi + edx * 8 + 4]
  mov  edx, [esi + edx * 8]
  mov  esi, [esp+4]
  jmp  eax
  ; ... matching loop omitted
```

### Variants
Same shape as amd64: `%0FAh`, `%5FAh` (variadic), `%9FAh` (indexed), `%0AFAh` (variadic + indexed).

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0FAh`
```asm
str     x0, [sp]
add     x17, sp, __n12_2
str     x1, [sp, #8]
sub     x17, x17, #8                  ; HOTFIX: caller addr not on stack

movz    x21, __ptr32lo_2
movk    x21, __ptr32hi_2, lsl #16

mov     x25, #0
ldr     x22, [x21, #0]

labNextOverloadlist:
  movz    x24, mdata_ptr32lo : #0
  movk    x24, mdata_ptr32hi : #0, lsl #16
  lsr     x22, x22, # ACTION_ORDER
  lsl     x23, x22, #4
  add     x23, x23, x24
  ldr     x23, [x23, #8]
  mov     x16, __n16_1
  sub     x22, x23, #8

labNextParam:
  sub     x16, x16, #1
  cmp     x16, #0
  bne     labMatching
  ; ... direct branch via x17
```

### Variants
| Prefix | Form |
|---|---|
| `%0FAh` | fixed arg count |
| `%5FAh` | variadic (counts args) |
| `%9FAh` | placeholder (`// !! temporally commented`) |
| `%0AFAh` | placeholder (`// !! temporally commented`) |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0FAh`
```asm
std     r3, 0(r1)                    ; saving arg0
addi    r17, r1, __n16_2
std     r4, 8(r1)                    ; saving arg1
addi    r17, r17, -8

ld      r21, toc_rdata(r2)
addis   r21, r21, __disp32hi_2
addi    r21, r21, __disp32lo_2

li      r25, 0
ld      r22, 0(r21)

labNextOverloadlist:
  ld      r24, toc_mdata(r2)
  srdi    r22, r22, ACTION_ORDER
  sldi    r23, r22, 4
  add     r23, r23, r24
  ld      r23, 8(r23)
  li      r16, __n16_1
  addi    r22, r23, -8

labNextParam:
  addi    r16, r16, -1
  cmpwi   r16,0
  bne     labMatching
  ; ... direct branch via ctr
```

## Notes
- 11 variant slots: fixed-arity (`%0FAh`), variadic (`%5FAh`), indexed table walk (`%9FAh`), variadic + indexed (`%0AFAh`), binary selector (slot 9), plus alt-mode counterparts.
- `altmode` (0x31) gates the secondary-table path -- must be emitted immediately before this opcode, and is auto-cleared after use.
- The final branch is **direct** (no VMT indirection): jumps straight to the resolved entry once the parameter signature matches.
- Operand relocation patches the immediate signature + arg-count fields into the inline body at JIT time.
- aarch64 has placeholder slots (`%9FAh`, `%0AFAh`) marked "temporally commented" in the asm -- not all paths are implemented on every arch.
