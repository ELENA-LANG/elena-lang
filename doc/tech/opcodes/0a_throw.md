# 0x0A -- throw

- **Category:** SingleOp
- **Enum:** `ByteCode::Throw`
- **Operand(s):** (none)
- **Reads:** `et_current` of the active exception chain (`%CORE_SINGLE_CONTENT` for STA; per-thread TLS struct for MT)
- **Writes:** PC
- **Side effects:** Unwinds to the current catch handler by reading `es_catch_addr` of the top exception record and jumping to it. The exception payload is expected to be in `acc`.

## Semantics
Raises an exception. Looks up the topmost catch record via the `et_current` field and tail-jumps to its handler. Frame/SP restoration is performed by the corresponding `unhook` site at the handler.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %0Ah`

```asm
  mov  rax, [data : %CORE_SINGLE_CONTENT + et_current]
  jmp  [rax + es_catch_addr]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0Ah`

```asm
  mov  eax, [data : %CORE_SINGLE_CONTENT + et_current]
  jmp  [eax + es_catch_addr]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0Ah`

```asm
#if _MAC
  adrp    x14, data_page : %CORE_SINGLE_CONTENT
  add     x14, x14, data_pageoff : %CORE_SINGLE_CONTENT
#elif (_LNX || _FREEBSD)
  movz    x14,  data_ptr32lo : %CORE_SINGLE_CONTENT
  movk    x14,  data_ptr32hi : %CORE_SINGLE_CONTENT, lsl #16
#endif

  ldr     x14, [x14, # et_current]!
  ldr     x17, [x14, # es_catch_addr]!

  br      x17
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0Ah`

```asm
  ld      r16, toc_data(r2)
  addis   r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi    r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT

  ld      r17, et_current(r16)
  ld      r0, es_catch_addr(r17)
  mtctr   r0
  bctr
```

## MT variant (corex60.asm / GCXT)

### amd64
```asm
  mov  rcx, gs:[58h]
  mov  rcx, [rcx]
  mov  rdi, [rcx + et_current]
  jmp  [rdi + es_catch_addr]
```

### x32
```asm
#if _WIN
  mov  eax, fs:[2Ch]
  mov  ecx, [eax]
#elif _LNX
  mov  eax, gs:[0]
  lea  ecx, [eax-tt_size]
#endif

  mov  edi, [ecx + et_current]
  jmp  [edi + es_catch_addr]
```

## Notes
- Pairs with `unhook` (0x0B). The exception payload lives in `acc`; frame/sp restoration is the handler's responsibility (the handler site is preceded by `unhook` which pops `et_current`).
- This opcode does NOT touch fp/sp itself -- it only tail-jumps to `es_catch_addr`. The handler does the unwind.
- MT variant reads the per-thread `et_current` via TLS (`gs:[58h]` on amd64 Windows; `fs:[2Ch]` on x32 Windows; `gs:[0] - tt_size` on Linux x32). STA uses the global `%CORE_SINGLE_CONTENT`.
