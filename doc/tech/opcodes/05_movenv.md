# 0x05 -- mov env

- **Category:** SingleOp
- **Enum:** `ByteCode::MovEnv`
- **Operand(s):** (none)
- **Reads:** `%SYSTEM_ENV` reference
- **Writes:** `index`

## Semantics
Loads the address of the `%SYSTEM_ENV` constant block into `index`. Used as a small accessor to the VM's process-wide environment record.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %5`

```asm
  mov  rdx, rdata64 : %SYSTEM_ENV
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %5`

```asm
  mov  edx, rdata32 : %SYSTEM_ENV
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %5`

```asm
#if _MAC
  adrp    x9, rdata_page : %SYSTEM_ENV
  add     x9, x9, rdata_pageoff : %SYSTEM_ENV
#elif (_LNX || _FREEBSD)
  movz    x9,  rdata_ptr32lo : %SYSTEM_ENV
  movk    x9,  rdata_ptr32hi : %SYSTEM_ENV, lsl #16
#endif
```

macOS uses PC-relative `adrp`+`add`; Linux/FreeBSD materialise the 32-bit absolute via `movz`/`movk`.

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %5`

```asm
  ld      r14, toc_rdata(r2)
  addis   r14, r14, rdata_disp32hi : %SYSTEM_ENV
  addi    r14, r14, rdata_disp32lo : %SYSTEM_ENV
```

Uses the TOC (`r2`) base plus a 32-bit displacement.

## Notes
- Loads the address of `%SYSTEM_ENV` (rdata pointer) into `index`. Use `lsave dp:disp` when persisting on 64-bit, since the destination is 8-byte wide.
- Preserves `acc`, `sp[0]`, `sp[1]`. Clobbers `index`.
- aarch64 has two address-materialisation paths: macOS uses PC-relative `adrp`+`add`; Linux/FreeBSD use absolute `movz`/`movk`. ppc64le goes through the TOC.
