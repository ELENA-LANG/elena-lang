# 0xB5 -- jump vt

- **Category:** DoubleOp
- **Enum:** `ByteCode::JumpVI`
- **Operand(s):** `jump vt:i`
- **Reads:** `acc` (receiver), `[acc - elVMTOffset]` (VMT pointer), `[VMT + i]`
- **Writes:** `pc`
- **Side effects:** Tail-call -- the current return address is reused by the callee.

## Semantics
Tail-call form of VMT-indirect dispatch: branch (no return-address push) to method slot `i` of the receiver's VMT -- `acc::VMT[i]`. Same lookup as `call vt:i` (0xB1) but with an unconditional jump rather than a call.

## JIT (amd64)

**Selection rule:** dispatched through `loadVMTIndexOp` (`jitcompiler.cpp:933-978`) -- same arg transformation as `call vt:i` (0xB1): the VMT-index operand is rewritten as `((arg1 << 1) + 1) << indexPower` so the indirect branch reads the `addr` word of the `(msg, addr)` VMT pair.

**Template:** `asm/amd64/core60.asm` `inline % 0B5h`

```asm
  mov  rax, [rbx - elVMTOffset]
  jmp  [rax + __arg32_1]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0B5h`

```asm
  mov  eax, [ebx - elVMTOffset]
  jmp  [eax + __arg32_1]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0B5h`

```asm
  sub     x14, x10, elVMTOffset
  ldr     x17, [x14]
  add     x17, x17, __arg12_1
  ldr     x17, [x17]
  br      x17
```

Indirect branch via `br x17` (no link register update).

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline % 0B5h`

```asm
  ld       r16, -elVMTOffset(r15)
  ld       r17, __arg16_1(r16)
  mtctr    r17            ; put code address into ctr
  bctr                    ; and jump to it
```

`bctr` is `bctrl` without the link bit.

## Notes

- Tail-call form of `call vt:i` (0xB1) -- branches without pushing a return address.
- The current return address is reused: the dispatched method returns to *the original caller* of this frame.
- Used by the trailing dispatch of polymorphic methods and by `^ self.foo()`-style tail returns.
- aarch64 uses `br` (no link); ppc64le uses `bctr` (no link).
