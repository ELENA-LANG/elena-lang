# 0x11 -- include

- **Category:** SingleOp
- **Enum:** `ByteCode::Include`
- **Operand(s):** (none)
- **Reads:** stack-top 2 slots (saved `tt_stack_frame` and `fp`)
- **Writes:** `sp` (pops 2 slots), thread-table `tt_stack_frame`
- **Side effects:** Restores the prior managed-frame link; in the MT variant clears `tt_flags = 0` so the cooperative collector resumes treating this thread as a mutator.

## Semantics
Reverse of `0x10 exclude`: drop the saved `fp` slot, pop the previously saved `tt_stack_frame` value back into the thread table.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %11h`

```asm
  mov  rax, [rsp + 8]
  add  rsp, 10h
  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_frame], rax
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %11h`

```asm
  add  esp, 4
  pop  eax
  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_frame], eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %11h`

```asm
  ldr     x18, [sp, #8]
  add     sp, sp, 10h          ; free stack

#if _MAC
  adrp    x14, data_page : %CORE_SINGLE_CONTENT
  add     x14, x14, data_pageoff : %CORE_SINGLE_CONTENT
#elif (_LNX || _FREEBSD)
  movz    x14,  data_ptr32lo : %CORE_SINGLE_CONTENT
  movk    x14,  data_ptr32hi : %CORE_SINGLE_CONTENT, lsl #16
#endif
  add     x14, x14, # tt_stack_frame
  str     x18, [x14]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %11h`

```asm
  addi    r1, r1, 10h          ; free stack
```

## MT variant (corex60.asm / GCXT)

### amd64
```asm
  add  rsp, 8
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
  mov  dword ptr [rdi + tt_flags], 0
  pop  rax
  mov  [rdi + tt_stack_frame], rax
```

### x32
```asm
  add  esp, 4

#if _WIN
  mov  eax, fs:[2Ch]
  mov  edi, [eax]
#elif _LNX
  mov  eax, gs:[0]
  lea  edi, [eax-tt_size]
#endif

  mov  [edi + tt_flags], 0
  pop  eax
  mov  [edi + tt_stack_frame], eax
```

## Notes
- Reverse of `exclude` (0x10). Restores the saved `tt_stack_frame` chain link and pops the 2 slots `exclude` pushed.
- MT variant clears `tt_flags = 0` to put the thread back into "mutator" mode so the next GC rendezvous will wait for it.
- ppc64le STA path is simpler than the other arches: it only adds `10h` to `r1`, because the prior `exclude` already wrote to a fixed stack slot.
- Preserves `acc`, `index`. Clobbers a scratch register.
