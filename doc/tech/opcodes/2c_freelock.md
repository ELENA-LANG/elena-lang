# 0x2C -- freelock

- **Category:** SingleOp
- **Enum:** `ByteCode::FreeLock`
- **Operand(s):** (none)
- **Reads:** acc (object pointer)
- **Writes:** lock byte at `[acc - elSyncOffset]` (MT build only)
- **Side effects:** atomic RMW on MT build; no-op on ST build

## Semantics
Releases the per-object lock byte at `[acc - elSyncOffset]`. On the MT (`corex60.asm` / GCXT) build this is an atomic `lock xadd` of -1. On the ST build the opcode is empty.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %2Ch`

*Empty body (single-threaded build).*

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %2Ch`

*Empty body (single-threaded build).*

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %2Ch`

*Empty body in this backend (single-threaded stub).*

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %2Ch`

*Empty body in this backend (single-threaded stub).*

## MT variant (corex60.asm / GCXT)

### amd64
**Template:** `asm/amd64/corex60.asm` `inline %2Ch`

```asm
  mov  ecx, -1

  ; free lock
  lock xadd byte ptr [rbx - elSyncOffset], cl
```

### x32
**Template:** `asm/x32/corex60.asm` `inline %2Ch`

```asm
  mov  ecx, -1

  ; free lock
  lock xadd byte ptr [ebx - elSyncOffset], cl
```

## Notes
- Releases the lock byte acquired by `trylock` (0x2B); must always be paired with a successful `trylock`.
- STA build emits nothing -- single-threaded code carries no lock state to release.
- MT build uses `lock xadd ..., -1` rather than a plain store to keep the memory operation visible to other cores and to support fairness counters if extended.
- Per-object lock byte lives at `[acc - elSyncOffset]` (same address used by `trylock`).
- aarch64/ppc64le lack MT bodies; mirrors the gap in `trylock`.
