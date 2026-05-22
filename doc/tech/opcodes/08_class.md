# 0x08 -- class

- **Category:** SingleOp
- **Enum:** `ByteCode::Class`
- **Operand(s):** (none)
- **Reads:** `[acc - elVMTOffset]`
- **Writes:** `acc` (now points to the VMT)

## Semantics
Replaces `acc` with the address of its class' VMT, read from the per-object header slot at offset `-elVMTOffset` from the object pointer.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %8`

```asm
  mov rbx, [rbx - elVMTOffset]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %8`

```asm
  mov ebx, [ebx - elVMTOffset]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %8`

```asm
  sub     x14, x10, elVMTOffset
  ldr     x10, [x14]              ; edi
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %8`

```asm
  ld      r15, -elVMTOffset(r15)
```

## Notes
- After `class`, `acc` points into rdata (the VMT); the original object pointer is lost.
- To walk the class hierarchy continue with `parent` (0x2D), which reads the parent VMT slot from the current VMT.
- Preserves `index`, `sp[0]`, `sp[1]`. Overwrites `acc` with the VMT pointer.
