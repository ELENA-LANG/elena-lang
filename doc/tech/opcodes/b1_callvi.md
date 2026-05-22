# 0xB1 -- call vt

- **Category:** DoubleOp
- **Enum:** `ByteCode::CallVI`
- **Operand(s):** `call vt:i`
- **Reads:** `acc` (receiver), `[acc - elVMTOffset]` (VMT pointer), `[VMT + i]` (handler)
- **Writes:** `pc`, return-address slot; on return `acc` holds the result
- **Side effects:** Callee may clobber any caller-saved register.

## Semantics
Call the method at slot `i` of the receiver's VMT -- `acc::VMT[i]`. Classic virtual dispatch through a VMT index resolved at JIT time. The argument `i` is the byte offset into the VMT (already scaled by the entry stride). `elVMTOffset` is the negative displacement from `acc` to the VMT pointer slot (object header layout). Compare with `jumpvi` (0xB5, tail-call form) and `callmr`/`vcallmr` (message-resolved dispatch).

## JIT (amd64)

**Selection rule:** dispatched through `loadVMTIndexOp` (`jitcompiler.cpp:933-978`) -- `retrieveCode(arg1)` picks the inline template, and the operand slot is rewritten as `((arg1 << 1) + 1) << indexPower`. VMT entries are stored as sorted `(message, addr)` pairs of size `2^(indexPower+1)`: the `<<1` selects the entry and the `+1` skips past the `msg` word so the relocation lands on the `addr` word directly.

**Template:** `asm/amd64/core60.asm` `inline % 0B1h`

```asm
  mov  rax, [rbx - elVMTOffset]
  call [rax + __arg32_1]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0B1h`

```asm
  mov  eax, [ebx - elVMTOffset]
  call [eax + __arg32_1]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0B1h`

```asm
  sub     x14, x10, elVMTOffset
  ldr     x17, [x14]
  add     x17, x17, __arg12_1
  ldr     x17, [x17]
  blr     x17
```

`elVMTOffset` is positive in the source; the negative displacement is applied via `sub`. Final `blr x17` is the indirect call-and-link.

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline % 0B1h`

```asm
  ld       r16, -elVMTOffset(r15)
  ld       r17, __arg16_1(r16)
  mtctr    r17            ; put code address into ctr
  bctrl                   ; and call it
```

Load VMT pointer into `r16`, load handler at `[r16 + i]` into `r17`, then `mtctr` + `bctrl`.

## Notes

- Virtual dispatch through VMT slot `i` of the receiver in `acc`. `elVMTOffset` is the negative displacement from `acc` to the VMT pointer slot in the object header.
- Tail-call sibling: `jump vt:i` (0xB5).
- Message-resolved sibling: `callmr m,r` / `vcallmr` use a class+message lookup table instead of a VMT index.
- The VMT entry layout is `(msg, addr)` pairs; the JIT skips the `msg` word at JIT time so the call instruction reads the `addr` word directly.
- Preserves nothing; `acc` holds the result on return.
