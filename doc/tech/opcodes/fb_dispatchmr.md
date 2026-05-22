# 0xFB -- dispatch mssg

- **Category:** TripleOp
- **Enum:** `ByteCode::DispatchMR`
- **Operand(s):** `dispatch m, r` (M+R)

## Semantics
Same overload-list traversal as `0xFA xdispatchmr`, but the final jump goes through the VMT instead of directly to the entry. In alt mode (`%06FBh`/`%0BFBh`), the secondary VMT (located after the primary using `elVMTSizeOffset` as stride) is used.

## Compiler behaviour
Like `xdispatch`, the dispatch logic flows through `compileDispatchMR` (`jitcompiler.cpp:3075-3130`) with the 11-slot variant selection keyed off `MESSAGE_FLAG_MASK` and `scope->altMode`. The difference vs. `xdispatch` is the inline body: each slot emits a VMT-indirect final branch (`jmp [rcx + rax + 8]` on amd64) instead of a direct jump. `altMode` is read, then cleared after the dispatch is emitted.

## JIT (amd64)
**Selection rule:** M+R command. Same 11-slot scheme as `xdispatch` (0xFA) keyed on `MESSAGE_FLAG_MASK` + `altMode`, but each slot emits a VMT-indirect final branch instead of a direct jump.

**Template:** `asm/amd64/core60.asm` `inline %0FBh`
```asm
; default
mov  r8,  rbx
mov  [rsp+8], r10
lea  rax, [rsp + __n_2]
mov  [rsp+16], r11

mov  rsi, __ptr64_2
xor  edx, edx
mov  rbx, [rsi]

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
  mov  rcx, [rbx - elVMTOffset]
  jmp  [rcx + rax + 8]              ; VMT-INDIRECT jump

; ... matching loop
```

### Variants
| Prefix | Form |
|---|---|
| `%0FBh` | default (fixed arg count) |
| `%5FBh` | variadic |
| `%6FBh` | alt-mode default (secondary VMT) |
| `%0BFBh` | alt-mode variadic |

Alt-mode tail:
```asm
mov  rcx, [rbx - elVMTOffset]
mov  rdi, [rcx - elVMTSizeOffset]
shl  rdi, 4
lea  rcx, [rcx + rdi]
jmp  [rcx + rax + 8]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0FBh`
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
  mov  ecx, [ebx - elVMTOffset]
  mov  esi, [esp+4]
  jmp  [ecx + eax + 4]
```

### Variants
Same set: `%0FBh`, `%5FBh` (variadic), `%6FBh` (alt-mode), `%0BFBh` (alt-mode variadic).

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0FBh`
```asm
str     x0, [sp]
add     x17, sp, __n12_2
str     x1, [sp, #8]
sub     x17, x17, #8

movz    x21, __ptr32lo_2
movk    x21, __ptr32hi_2, lsl #16

mov     x25, #0
ldr     x22, [x21, #0]

labNextOverloadlist:
  ; ... action-table lookup
  mov     x16, __n16_1
  sub     x22, x23, #8

labNextParam:
  sub     x16, x16, #1
  cmp     x16, #0
  bne     labMatching

  lsl     x23, x25, #4
  add     x25, x21, x23
  ldr     x9,  [x25]
  ldr     x23, [x25, #8]

  sub     x16, x10, elVMTOffset
  ldr     x16, [x16, #0]

  add     x20, x16, x23
  ldr     x17, [x20, #8]
  br      x17                            ; VMT-INDIRECT
```

### Variants
| Prefix | Form |
|---|---|
| `%0FBh` | default |
| `%5FBh` | variadic |
| `%6FBh` | alt-mode |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0FBh`
```asm
; default; same matching structure as 0xFA but final jump is:
ld       r17, -elVMTOffset(r15)
add      r17, r17, rax
ld       r17, 8(r17)
mtctr    r17
bctr
```

## Notes
- VMT-indirect counterpart of `xdispatch` (0xFA) -- same overload-list walk, but the final jump reads through the VMT slot for late binding.
- 11 inline slots selected via `MESSAGE_FLAG_MASK` + `altMode` (set by 0x31 `altmode`); alt-mode slots (`%6FBh`, `%0BFBh`) use the secondary VMT after the primary (size from `elVMTSizeOffset`).
- After emission `altMode` is cleared.
- Used when the resolved entry must respect override semantics (most virtual method dispatches with overload sets).
