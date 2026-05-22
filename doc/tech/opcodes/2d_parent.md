# 0x2D -- parent

- **Category:** SingleOp
- **Enum:** `ByteCode::Parent`
- **Operand(s):** (none)
- **Reads:** acc (class VMT pointer)
- **Writes:** acc

## Semantics
Loads the parent class VMT pointer for the class referenced by `acc`. The parent slot lives at `[acc - elPackageOffset]` in the class header.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %2Dh`

```asm
  mov rbx, [rbx - elPackageOffset]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %2Dh`

```asm
  mov ebx, [ebx - elPackageOffset]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %2Dh`

```asm
  sub     x14, x10, elPackageOffset
  ldr     x10, [x14]              ; edi
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %2Dh`

```asm
  ld      r15, -elPackageOffset(r15)
```

## Notes
- Walks the class hierarchy by loading `acc.class.parent` (i.e. `[acc - elPackageOffset]`).
- Returns `nil` at the root (`system'Object`) -- callers must check before dereferencing to avoid faulting.
- `index` and stack are preserved; only `acc` is rewritten.
- The aarch64 sequence splits the negative-offset load into `sub` + `ldr` because `ldr` immediates are unsigned with limited range.
- Typically emitted in `isa`-test / VMT-walk loops where each iteration narrows toward `nil` or the target class.
