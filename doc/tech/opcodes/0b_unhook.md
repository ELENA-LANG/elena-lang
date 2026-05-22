# 0x0B -- unhook

- **Category:** SingleOp
- **Enum:** `ByteCode::Unhook`
- **Operand(s):** (none)
- **Reads:** top exception record (`et_current` -> `{es_prev_struct, es_catch_frame, es_catch_level}`)
- **Writes:** `fp`, `sp`, `et_current`
- **Side effects:** Pops the topmost exception record off the chain and restores the frame pointer and stack pointer captured when the catch was installed.

## Semantics
Removes the active try/catch handler. Reads the previous chain link, the saved frame pointer, and the saved stack pointer, restores them, and stores the previous record back into `et_current`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %0Bh`

```asm
  mov  rdi, [data : %CORE_SINGLE_CONTENT + et_current]

  mov  rax, [rdi + es_prev_struct]
  mov  rbp, [rdi + es_catch_frame]
  mov  rsp, [rdi + es_catch_level]

  mov  [data : %CORE_SINGLE_CONTENT + et_current], rax
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0Bh`

```asm
  mov  edi, [data : %CORE_SINGLE_CONTENT + et_current]

  mov  eax, [edi + es_prev_struct]
  mov  ebp, [edi + es_catch_frame]
  mov  esp, [edi + es_catch_level]

  mov  [data : %CORE_SINGLE_CONTENT + et_current], eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0Bh`

```asm
#if _MAC
  adrp    x14, data_page : %CORE_SINGLE_CONTENT
  add     x14, x14, data_pageoff : %CORE_SINGLE_CONTENT
#elif (_LNX || _FREEBSD)
  movz    x14,  data_ptr32lo : %CORE_SINGLE_CONTENT
  movk    x14,  data_ptr32hi : %CORE_SINGLE_CONTENT, lsl #16
#endif

  add     x14, x14, # et_current
  ldr     x13, [x14]

  ldr     x15, [x13]
  ldr     x16, [x13, #8]!
  ldr     x17, [x13, #8]!
  ldr     x29, [x13, #8]!

  str     x15, [x14]

  mov     sp, x17
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0Bh`

```asm
  ld      r16, toc_data(r2)
  addis   r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi    r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT

  ld      r19, et_current(r16)

  ld      r17, es_prev_struct(r19)
  ld      r1, es_catch_level(r19)
  ld      r31, es_catch_frame(r19)

  std     r17, et_current(r16)
```

## MT variant (corex60.asm / GCXT)

### amd64
```asm
  ; GCXT: get current thread frame
  mov  rcx, gs:[58h]
  mov  rcx, [rcx]
  mov  rdi, [rcx + et_current]

  mov  rax, [rdi + es_prev_struct]
  mov  rbp, [rdi + es_catch_frame]
  mov  rsp, [rdi + es_catch_level]

  mov  [rcx + et_current], rax
```

### x32
```asm
  ; GCXT: get current thread frame
#if _WIN
  mov  eax, fs:[2Ch]
  mov  ecx, [eax]
#elif _LNX
  mov  eax, gs:[0]
  lea  ecx, [eax-tt_size]
#endif

  mov  edi, [ecx + et_current]

  mov  eax, [edi + es_prev_struct]
  mov  ebp, [edi + es_catch_frame]
  mov  esp, [edi + es_catch_level]

  mov  [ecx + et_current], eax
```

## Notes
- Pairs with `throw` (0x0A). Used in BOTH normal exit from a try-block and on the handler entry after an exception -- it pops the exception record and restores fp + sp from it.
- Order of slot reads differs slightly per arch (aarch64 uses pre-indexed `[x13, #8]!` walk; ppc64le reads by named field offset), but the net effect is identical: chain link, frame ptr, sp restore.
- Clobbers `fp`, `sp`, and a scratch register; preserves `acc` (carries the in-flight exception or normal result) and `index`.
