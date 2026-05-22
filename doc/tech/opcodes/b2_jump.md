# 0xB2 -- jump

- **Category:** DoubleOp
- **Enum:** `ByteCode::Jump`
- **Operand(s):** `jump label`
- **Reads:** none
- **Writes:** `pc`
- **Side effects:** None.

## Semantics
Unconditional jump to `label`. The argument is a signed bytecode offset relative to the current opcode; the JIT translates it into a native code displacement during label resolution. There is no `inline` template -- the JIT emits the branch instruction directly via `LabelHelper::writeJump{Forward,Back}` (see `jitcompiler.cpp::compileJump`).

## Compiler behaviour
`compileJump` (`jitcompiler.cpp:2984-2995`) resolves the operand as a *label-id delta* to the current bytecode position:

```cpp
pos_t label = scope->tapeReader->position() + scope->command.arg1;
if (scope->command.arg1 < 0)      lh->writeJumpBack(label, *codeWriter);     // back-edge
else if (scope->command.arg1 > 0) lh->writeJumpForward(label, *codeWriter, scope->command.arg1);
```

The label helper (one per backend in `x86helper.cpp` / `arm64helper.cpp` / `ppc64lehelper.cpp`) picks the short or near encoding based on the resolved native distance. Because `Jump` has **no inline template**, readers of just the `core60.asm` files will not see the actual emitted bytes -- they live in the helper class.

## JIT (amd64)
*Not implemented in this backend as an inline template.* The JIT emits directly via `X86LabelHelper::writeJumpForward`/`writeJumpBack` (`x86helper.cpp`):

```asm
  ; short forward / back-edge with |disp| < 0x80:
  EB ib                ; jmp rel8
  ; otherwise:
  E9 id                ; jmp rel32
```

Short form is chosen when the bytecode-level offset (`arg1`) is `< 0x10` (forward) or when the resolved native offset fits in a signed 8-bit (back).

## JIT (x32)
*Not implemented in this backend as an inline template.* Same encoding as amd64 (`EB ib` / `E9 id`); the helpers in `x86helper.cpp` are shared between the two x86 backends.

## JIT (aarch64)
*Not implemented in this backend as an inline template.* `ARM64LabelHelper::writeJump{Forward,Back}` emits a single `b` (unconditional branch, 26-bit signed offset; range checked against `0x3FFFF` words):

```asm
  b   target            ; B (unconditional, +/-128 MB)
```

## JIT (ppc64le)
*Not implemented in this backend as an inline template.* `PPC64LabelHelper::writeJump{Forward,Back}` emits a `b` via `writeBxx`:

```asm
  b   target            ; unconditional branch (AA=0, LK=0)
```

## Notes

- No inline template -- JIT emits arch-specific `jmp` / `b` resolved by `LabelHelper`.
- Distance picks short (8-bit displacement on x86, 19-bit on aarch64 `b.cc`) vs near (32-bit on x86, 26-bit on aarch64 unconditional `b`) encoding.
- Back-edge resolution is precise (native distance known); forward-edge uses the bytecode-level `arg1` as a heuristic for short vs near selection.
- Preserves all registers and flags.
