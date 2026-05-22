# 0x09 -- save

- **Category:** SingleOp
- **Enum:** `ByteCode::Save`
- **Operand(s):** (none)
- **Reads:** `index`, `acc`
- **Writes:** `[acc]` (32-bit store)

## Semantics
Stores the low 32 bits of `index` into the first cell of the object referenced by `acc`. Only the low word is written; the upper half on 64-bit targets is left untouched.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %9`

```asm
  mov  dword ptr [rbx], edx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %9`

```asm
  mov  dword ptr [ebx], edx
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %9`

```asm
  str    w9, [x10]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %9`

```asm
  stw      r14, 0(r15)
```

## Notes
- 32-bit store at `[acc]`; the upper half on 64-bit targets is left untouched. Use `lsave` (0x24) when a full qword store is required.
- Does NOT trigger a write barrier -- the GC barrier lives in `assign` (0x12). `save` is intended for primitive fields, not for assigning managed references.
- Preserves `acc`, `index`, `sp[0]`, `sp[1]`. Only memory at `[acc]` is mutated.
