# 0x93 -- free i

- **Category:** DoubleOp
- **Enum:** `ByteCode::FreeI`
- **Operand(s):** `free i`
- **Reads:** --
- **Writes:** `sp`
- **Side effects:** Stack must remain aligned; no zero-fill. Inverse of `alloc i`.

## Semantics
`sp -= i` -- shrink the stack by `i` bytes. Caller must balance `alloc i` / `free i`; there is no runtime check.

## Compiler behaviour
`compileFree` (`elenasrc3/engine/jitcompiler.cpp:2963-2971`) clears `scope->inlineMode` if set and restores normal stack tracking. MUST follow a prior `alloc i` (0x92) -- emitting a `free` without a matching `alloc` leaves `inlineMode` toggled and corrupts subsequent stack-offset computations.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %93h`

```asm
  add  rsp, __arg32_1
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %93h`

```asm
  add  esp, __arg32_1
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %93h`

```asm
  add     sp, sp, __arg12_1
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %93h`

```asm
  addi    r1, r1, __arg16_1     ; free stack
```

## Notes
- MUST follow a matching `alloc i` (0x92); the JIT toggles inline-mode in step with the pair.
- No zero-fill -- slots are simply released; caller must not read them after.
- Single-instruction emission on all backends -- pure `sp` adjust.
- Preserves `acc`, `index`, and all stack values below the freed region.
