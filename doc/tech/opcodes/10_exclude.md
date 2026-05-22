# 0x10 -- exclude

- **Category:** SingleOp
- **Enum:** `ByteCode::Exclude`
- **Operand(s):** (none)
- **Reads:** `fp`, `sp`, thread-table `tt_stack_frame`
- **Writes:** `sp` (pushes 2 slots), thread-table `tt_stack_frame`
- **Side effects:** Pushes the prior managed-frame link onto the stack and updates the thread table to record the new `sp` as the managed-frame boundary; in the MT variant also raises `tt_flags = 1` to mark the thread "in safe region" for the cooperative collector.

## Semantics
Save the current `tt_stack_frame` and `fp` onto the stack, then store `sp` into `tt_stack_frame`. Used around unmanaged work so the GC skips native frames; reversed by `0x11 include`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %10h`

```asm
  mov  rax, [data : %CORE_SINGLE_CONTENT + tt_stack_frame]
  push rax
  push rbp
  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_frame], rsp
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %10h`

```asm
  push [data : %CORE_SINGLE_CONTENT + tt_stack_frame]
  push ebp
  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_frame], esp
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %10h`

```asm
#if _MAC
  adrp    x14, data_page : %CORE_SINGLE_CONTENT
  add     x14, x14, data_pageoff : %CORE_SINGLE_CONTENT
#elif (_LNX || _FREEBSD)
  movz    x14,  data_ptr32lo : %CORE_SINGLE_CONTENT
  movk    x14,  data_ptr32hi : %CORE_SINGLE_CONTENT, lsl #16
#endif
  add     x14, x14, # tt_stack_frame
  ldr     x18, [x14]

  stp     x29, x18, [sp, #-16]!
  mov     x18, sp
  str     x18, [x14]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %10h`

```asm
  li      r18, 0

  std     r31, -10h(r1)  ; save frame pointer
  std     r18, -08h(r1)  ; save return address
  addi    r1, r1, -16    ; allocate raw stack

  ld      r16, toc_data(r2)
  addis   r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi    r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT
  std     r1, tt_stack_frame(r16)
```

## MT variant (corex60.asm / GCXT)

### amd64
```asm
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
  mov  dword ptr [rdi + tt_flags], 1
  mov  rax, [rdi + tt_stack_frame]
  push rax
  push rbp
  mov  [rdi + tt_stack_frame], rsp
```

### x32
```asm
#if _WIN
  mov  eax, fs:[2Ch]
  mov  edi, [eax]
#elif _LNX
  mov  eax, gs:[0]
  lea  edi, [eax-tt_size]
#endif
  mov  [edi + tt_flags], 1
  mov  eax, [edi + tt_stack_frame]
  push eax
  push ebp
  mov  [edi + tt_stack_frame], esp
```

## Notes
- Pairs with `include` (0x11). Bracket unmanaged work so the GC can skip native frames between the two markers.
- STA emits a frame-link save (push old `tt_stack_frame`, push `fp`, update boundary). MT additionally sets `tt_flags = 1` to mark the thread "in safe region" -- the cooperative collector treats such threads as park-able without waiting.
- Pushes 2 slots onto sp; `include` must symmetrically pop 2.
- Preserves `acc`, `index`. Clobbers a scratch (`rax`/`eax`/`x18`/`r18`).
