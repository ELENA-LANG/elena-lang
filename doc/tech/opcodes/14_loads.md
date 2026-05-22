# 0x14 -- loads

- **Category:** SingleOp
- **Enum:** `ByteCode::LoadS`
- **Operand(s):** (none)
- **Reads:** `[acc]` (message word), message-remap table (`mdata : %0`)
- **Writes:** `index`

## Semantics
`index := [acc].action` -- loads the message word at `[acc]`, extracts the action component, looks it up in the remap table, applies the remap if non-zero, then re-positions the result in the action field.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %14h`

```asm
  mov    edx, dword ptr [rbx]
  shr    edx, ACTION_ORDER
  mov    rax, mdata : %0
  lea    rsi, [rdx*8]
  mov    ecx, dword ptr [rax + rsi * 2]
  test   ecx, ecx
  cmovnz edx, ecx
  shl    edx, ACTION_ORDER
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %14h`

```asm
  mov    edx, [ebx]
  shr    edx, ACTION_ORDER
  mov    eax, mdata : %0
  mov    ecx, [eax + edx * 8]
  test   ecx, ecx
  cmovnz edx, ecx
  shl    edx, ACTION_ORDER
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %14h`

```asm
  ldr     x22, [x10]
#if _MAC
  adrp    x24, mdata_page : #0               ;--
  add     x24, x24, mdata_pageoff : #0
#elif (_LNX || _FREEBSD)
  movz    x24,  mdata_ptr32lo : #0               ;--
  movk    x24,  mdata_ptr32hi : #0, lsl #16
#endif
  lsr     x22, x22, # ACTION_ORDER
  add     x22, x22, x24
  ldr     x9, [x22]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %14h`

```asm
  ld      r22, 0(r15)
  ld      r24, toc_mdata(r2)
  srdi    r22, r22, ACTION_ORDER
  add     r24, r24, r22
  ld      r14, 0(r24)
```

## Notes
- Loads the message action at `[acc]` and translates it through the per-module subject table at `mdata`. The translation lets messages from different modules share a global namespace at runtime.
- amd64/x32 use `cmovnz` to skip the remap when the table entry is zero (action keeps its original value); aarch64/ppc64le emit the unconditional load -- empty slots in the table must therefore be initialised to identity values on those arches.
- Preserves `acc`, `sp[0]`, `sp[1]`. Clobbers `index` and scratch (`ecx`/`rcx`, `rax`, `x22`/`x24`, `r22`/`r24`).
