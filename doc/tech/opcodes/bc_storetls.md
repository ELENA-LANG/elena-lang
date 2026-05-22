# 0xBC -- store tls

- **Category:** DoubleOp
- **Enum:** `ByteCode::StoreTLS`
- **Operand(s):** `store tls:i`
- **Reads:** `acc`
- **Writes:** per-thread context block at field offset `i` (STA: `CORE_SINGLE_CONTENT[i]`; MT: TLS-anchored)
- **Side effects:** None -- touches only the caller's per-thread block, so race-free without locks.

## Semantics
`TLS[i] := acc` -- store `acc` into the per-thread slot at byte offset `i`. Inverse of `peek tls:i` (0xBB). Used for thread-local variables, current-frame caching, GC state, and exception-handler chains. The thread block is initialized by `system n=3` (thread-table entry assignment) at thread creation.

## JIT (amd64)

**Selection rule:** dispatched through `loadTLSOp` -- the operand is encoded via `mskTLSRef`; same slot-layout indirection as `peek tls:i` (0xBB). STA template is empty (loader patches direct accesses); GCXT/MT template lives in `corex60.asm`.

**Template:** `asm/amd64/core60.asm` `inline %0BCh`

```asm
```

Empty in the STA build (single-thread context is patched in elsewhere). Live emit is in `corex60.asm`.

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0BCh`

```asm
```

Empty in the STA build.

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0BCh`

```asm
```

*Not implemented in this backend* -- multi-threaded build does not currently target aarch64.

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0BCh`

```asm
```

*Not implemented in this backend* -- same as aarch64.

## MT variant (corex60.asm / GCXT)

### amd64
**Template:** `asm/amd64/corex60.asm` `inline %0BCh`

```asm
  mov  rcx, gs:[58h]
  mov  rax, [rcx]
  lea  rdi, [rax + __arg32_1]
  mov  [rdi], rbx
```

Two indirections -- TLS self-slot at `gs:[58h]` -> per-thread context block -> field. Mirror of `peektls`.

### x32
**Template:** `asm/x32/corex60.asm` `inline %0BCh`

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

  mov  [edi], ebx
```

Windows: TLS-table at `fs:[2Ch]`. Linux: `gs:[0]` is the thread struct base; the field is at a negative offset adjusted by `-4`.

## Notes

- Mirror of `peek tls:i` (0xBB) -- writes only the calling thread's context block, so the operation is lock-free.
- Used for thread-local variables, current-frame caching, GC state, and exception-handler chains.
- The thread block is allocated by `system n=3` at thread creation; until then `store tls:i` to an unallocated slot is undefined.
- STA template is empty -- loader patches direct accesses to `CORE_SINGLE_CONTENT[i]`.
- aarch64/ppc64le do not implement the MT path.
