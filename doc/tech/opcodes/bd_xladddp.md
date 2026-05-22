# 0xBD -- xladd dp

- **Category:** DoubleOp
- **Enum:** `ByteCode::XLAddDP`
- **Operand(s):** `xladd dp:disp`
- **Reads:** `dp[disp]` (full word), `index`
- **Writes:** `index`
- **Side effects:** Sets native flags on amd64/x32; aarch64/ppc64le do not implicitly set flags from `add`.

## Semantics
`long:index += long:[dp[disp]]` -- load the full-width value at the frame slot `dp[disp]` and add it to `index`. 64-bit signed counterpart of `xadd dp:disp` (0x9D, 32-bit). Compare with `iadd dp:disp, n` (0xE1) which updates the *memory* slot, not the index register.

## JIT (amd64)

**Selection rule:** dispatched through `loadFrameDispOp` -- `retrieveCode(arg1)` selects the inline template and the operand slot is rewritten as `getFPOffset(arg1 << indexPower, frameOffset)` so the displacement is frame-pointer-relative bytes, not a logical bytecode index.

**Template:** `asm/amd64/core60.asm` `inline %0BDh`

```asm
  mov  rax, qword ptr [rbp+__arg32_1]
  add  rdx, rax
```

Sets ZF/SF/CF/OF -- usable by a following `jeq`/`jlt`/etc. without an explicit `cmp`.

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0BDh`

```asm
  lea  edi, [ebp+__arg32_1]
  mov  ecx, [edi]
  add  eax, ecx
  mov  ecx, [edi + 4]
  adc  edx, ecx
```

x32 emulates 64-bit add with `add`/`adc` across the `eax:edx` (low/high) long pair; both source halves are read from consecutive frame slots.

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0BDh`

```asm
  add     x11, x29, __arg12_1
  ldr     x12,  [x11]
  add     x9, x9, x12
```

Plain 64-bit `add`; no flag side effect (use `adds` if a follow-up branch depends on flags -- not the case here since `xladd` is normally followed by a separate compare).

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0BDh`

```asm
  addi    r16, r31, __arg16_1
  ld      r16, 0(r16)
  add     r14, r14, r16
```

No flag side effect (use `add.` to set CR0 -- not used here).

## Notes

- 64-bit signed add: `long:index += long:[dp[disp]]`. Updates the index register, not the memory slot.
- On amd64/x32 the add sets EFLAGS -- a following `jeq`/`jlt` can skip an explicit `cmp`. On aarch64/ppc64le the plain `add`/`add` instruction does **not** set flags; a separate compare is required if the next op is a conditional branch.
- 32-bit truncating sibling: `xadd dp:disp` (0x9D).
- Use `iadd dp:disp, n` (0xE1) to update the memory slot instead of `index`.
- x32 emulates 64-bit add with `add` + `adc` across the `eax:edx` long pair, reading both halves from consecutive frame words.
