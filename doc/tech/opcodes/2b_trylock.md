# 0x2B -- trylock

- **Category:** SingleOp
- **Enum:** `ByteCode::TryLock`
- **Operand(s):** (none)
- **Reads:** acc (object pointer)
- **Writes:** lock byte at `[acc - elSyncOffset]` (MT build), CPU EQ flag
- **Side effects:** atomic RMW on MT build; stub on single-threaded build

## Semantics
Attempts to acquire the per-object lock byte stored at `[acc - elSyncOffset]`. On the MT (`corex60.asm` / GCXT) build this is a `lock cmpxchg` against zero with 1, leaving `EQ` set on success. On the ST build this is a no-op that simply clears `eax` so subsequent `EQ`-tested code treats the lock as acquired.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %2Bh`

```asm
  xor  eax, eax
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %2Bh`

```asm
  xor  eax, eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %2Bh`

*Empty body in this backend (single-threaded stub).*

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %2Bh`

*Empty body in this backend (single-threaded stub).*

## MT variant (corex60.asm / GCXT)

### amd64
**Template:** `asm/amd64/corex60.asm` `inline %2Bh`

```asm
  ; GCXT: try to lock
  xor  eax, eax
  mov  ecx, 1
  lock cmpxchg byte ptr[rbx - elSyncOffset], cl
  test eax, eax
```

### x32
**Template:** `asm/x32/corex60.asm` `inline %2Bh`

```asm
  ; GCXT: try to lock
  xor  eax, eax
  mov  ecx, 1
  lock cmpxchg byte ptr[ebx - elSyncOffset], cl
  test eax, eax
```

## Notes
- Pairs with `freelock` (0x2C) -- together they form the per-object lock acquire/release.
- STA build is a no-op stub (just clears `eax` so EQ tests pass); MT build emits a real `lock cmpxchg` on the byte at `[acc - elSyncOffset]`.
- Lock byte is **one byte per object**, embedded just before the object header at the `elSyncOffset` distance.
- Sets the EQ flag -- caller branches on it to determine success/failure of the acquisition.
- aarch64/ppc64le currently lack MT bodies; multi-threaded support on those backends is not wired up yet.
