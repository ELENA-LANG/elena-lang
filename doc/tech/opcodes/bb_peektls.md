# 0xBB -- peek tls

- **Category:** DoubleOp
- **Enum:** `ByteCode::PeekTLS`
- **Operand(s):** `peek tls:i`
- **Reads:** per-thread context block (STA: `CORE_SINGLE_CONTENT[i]`; MT: TLS-anchored block at `[i]`)
- **Writes:** `acc`
- **Side effects:** None.

## Semantics
`acc := TLS[i]` -- load the per-thread slot at byte offset `i` into `acc`. In the single-thread runtime the "TLS" reduces to a global `CORE_SINGLE_CONTENT` struct (the inline template is empty in `core60.asm`); the multi-threaded GCXT runtime in `corex60.asm` reads a real TLS-anchored per-thread block. Companion store: `store tls:i` (0xBC).

## JIT (amd64)

**Selection rule:** dispatched through `loadTLSOp` -- the operand is the TLS slot offset and is encoded via `mskTLSRef` so the relocation refers to a slot in the per-thread context-block layout (not a code-section address). In the STA build the template is empty: the loader resolves `CORE_SINGLE_CONTENT` once at boot and patches direct accesses elsewhere. In the GCXT/MT build (`corex60.asm`) the template loads through the platform TLS anchor.

**Template:** `asm/amd64/core60.asm` `inline %0BBh`

```asm
```

Empty in the STA build -- the loader patches the single-thread context address in elsewhere, so no per-call indirection is needed. Live emit is in `corex60.asm` (see MT variant below).

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0BBh`

```asm
```

Empty in the STA build (same reasoning as amd64).

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0BBh`

```asm
```

*Not implemented in this backend* -- the multi-threaded build does not currently target aarch64.

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0BBh`

```asm
```

*Not implemented in this backend* -- same as aarch64.

## MT variant (corex60.asm / GCXT)

### amd64
**Template:** `asm/amd64/corex60.asm` `inline %0BBh`

```asm
  mov  rcx, gs:[58h]
  mov  rax, [rcx]
  lea  rdi, [rax + __arg32_1]
  mov  rbx, [rdi]
```

`gs:[58h]` is the Linux x86_64 TLS-slot convention used by the ELENA runtime to anchor the per-thread context block; the second `mov` dereferences it. `__arg32_1` is the field offset inside that block.

### x32
**Template:** `asm/x32/corex60.asm` `inline %0BBh`

```asm
#if _WIN
  mov  eax, fs:[2Ch]
  mov  eax, [eax]
  lea  edi, [eax + __arg32_1]
#elif _LNX
  mov  eax, gs:[0]
  lea  edi, [eax - __arg32_1]
  lea  edi, [edi-4]
#endif

  mov  ebx, [edi]
```

Windows uses the FS-relative TLS table at `fs:[2Ch]`; Linux uses `gs:[0]` (the segment base of the thread struct) and addresses the field with a negative displacement adjusted by `-4`.

## Notes

- Variant by TLS slot index (`mskTLSRef`); the slot is a field in the per-thread context block, not a TLS segment offset directly.
- In **MT mode** the body lives in `corex60.asm` and chains through `gs:[58h]` (Linux x86_64) / `fs:[2Ch]` (Windows x86) / `gs:[0]` (Linux x86).
- In **STA mode** the body is empty: the loader patches each `peek tls:i` site with a direct `CORE_SINGLE_CONTENT[i]` access at link time.
- aarch64/ppc64le do not implement the MT path yet -- multi-threaded ELENA currently targets x86 only.
- Companion: `store tls:i` (0xBC).
