# 0x17 -- tststck

- **Category:** SingleOp
- **Enum:** `ByteCode::TstStck`
- **Operand(s):** (none)
- **Reads:** `acc`, `sp`, thread-table `tt_stack_root`
- **Writes:** `COMP` flags

## Semantics
`COMP.EQ := (acc within current stack range)`. The test combines `acc < sp` and `acc > tt_stack_root` into a single condition: `COMP.EQ` is true precisely when neither holds, i.e. `sp <= acc <= tt_stack_root`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %17h`

```asm
  xor  ecx, ecx
  mov  rax, [data : %CORE_SINGLE_CONTENT + tt_stack_root]
  cmp  rbx, rsp
  setl cl
  cmp  rbx, rax
  setg ch
  test ecx, ecx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %17h`

```asm
  xor  ecx, ecx
  mov  eax,[data : %CORE_SINGLE_CONTENT + tt_stack_root]
  cmp  ebx, esp
  setl cl
  cmp  ebx, eax
  setg ch
  test ecx, ecx
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %17h`

```asm
#if _MAC
  adrp    x14, data_page : %CORE_SINGLE_CONTENT
  add     x14, x14, data_pageoff : %CORE_SINGLE_CONTENT
#elif (_LNX || _FREEBSD)
  movz    x14,  data_ptr32lo : %CORE_SINGLE_CONTENT
  movk    x14,  data_ptr32hi : %CORE_SINGLE_CONTENT, lsl #16
#endif
  add     x14, x14, # tt_stack_root
  ldr     x14, [x14]

  cmp     x10, sp
  cset    x12, lt
  cmp     x10, x14
  cset    x13, gt
  orr     x12, x12, x13

  cmp     x12, 0
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %17h`

```asm
  ld      r16, toc_data(r2)
  addis   r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi    r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT
  ld      r16, tt_stack_root(r16)

  li      r17, 0
  li      r18, 1
  cmpld   r15, r1
  isellt  r19, r18, r17
  cmpld   r15, r16
  iselgt  r20, r18, r17
  or      r19, r19, r20

  cmpwi   r19, 0
```

## MT variant (corex60.asm / GCXT)

### amd64
```asm
  ; COREX
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
  mov  rax, [rdi + tt_stack_root]

  xor  ecx, ecx
  cmp  rbx, rsp
  setl cl
  cmp  rbx, rax
  setg ch
  test ecx, ecx
```

### x32
```asm
  ; COREX
#if _WIN
  mov  eax, fs:[2Ch]
  mov  edi, [eax]
#elif _LNX
  mov  eax, gs:[0]
  lea  edi, [eax-tt_size]
#endif

  mov  eax, [edi + tt_stack_root]

  xor  ecx, ecx
  cmp  ebx, esp
  setl cl
  cmp  ebx, eax
  setg ch
  test ecx, ecx
```

## Notes
- STA path consults the global `tt_stack_root`; MT path reads the per-thread `tt_stack_root` via TLS (`gs:[58h]` / `fs:[2Ch]` / `gs:[0]-tt_size`). Used by closures to verify "is this captured pointer on the currently-live stack?".
- COMP.EQ is set when `sp <= acc <= tt_stack_root` (neither boundary crossed). The x86 implementation packs the two `setX` results into `cl` and `ch` then `test`s the combined byte -- a single conditional jump afterwards covers both bounds.
- Preserves `acc`, `index`, `sp[0]`. Clobbers COMP flags and scratch (`ecx`, `eax`/`rax`).
