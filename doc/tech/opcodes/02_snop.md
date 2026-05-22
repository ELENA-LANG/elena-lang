# 0x02 -- snop

- **Category:** SingleOp
- **Enum:** `ByteCode::SNop`
- **Operand(s):** (none)
- **Reads:** (MT) `gc_signal`
- **Writes:** (none)
- **Side effects:** In multithreaded builds, acts as a GC safe-point: if the GC has signalled, the thread parks in `%THREAD_WAIT` until the collection completes.

## Semantics
Thread-safe nop. In the single-threaded (STA) core it is an empty placeholder. In the multithreaded (GCXT) core it polls the GC signal and yields to `%THREAD_WAIT` while a collection is in progress.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline % 2`

```asm
```

Empty body in STA (placeholder).

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline % 2`

```asm
```

Empty body in STA (placeholder).

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline % 2`

```asm
```

Empty body in STA (placeholder).

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline % 2`

```asm
```

Empty body in STA (placeholder).

## MT variant (corex60.asm / GCXT)

### amd64
```asm
  ; safe point
  mov  rdx, [data : %CORE_GC_TABLE + gc_signal]
  test rdx, rdx                       ; if it is a collecting thread, waits
  jz   short labConinue               ; otherwise goes on

  nop
  nop
  call %THREAD_WAIT                   ; waits until the GC is stopped

labConinue:
```

### x32
```asm
  ; safe point
  mov  edx, [data : %CORE_GC_TABLE + gc_signal]
  test edx, edx                       ; if it is a collecting thread, waits
  jz   short labConinue               ; otherwise goes on

  nop
  nop
  call %THREAD_WAIT                   ; waits until the GC is stopped

labConinue:
```

## Notes
- STA: empty body -- preserves all roles like 0x00 `nop`.
- MT (corex60.asm): GC safe-point. Compiler inserts at loop back-edges and other long-running constructs so the stop-the-world collector can rendezvous; clobbers a scratch register and may transfer to `%THREAD_WAIT`.
- The `nop`/`nop` pad before `call %THREAD_WAIT` exists so the call site can be patched atomically by the cooperative collector.
