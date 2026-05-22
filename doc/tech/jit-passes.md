# ELENA JIT -- Tape Passes & Optimizer

The bytecode tape goes through several passes before any inline template is
copied. These passes can delete, rewrite, or reorder opcodes -- so the
sequence that ends up in the JIT is not always the one written by the
front-end.

Companion documents:
- [`opcodes/README.md`](opcodes/README.md) -- per-opcode JIT bodies
- [`runtime.md`](runtime.md) -- runtime architecture
- [`linker.md`](linker.md) -- VMT layout, message encoding, ref masks

## Accumulator liveness sets (`bytecode.cpp`)

Two static tables drive a simple register-liveness analysis on `acc`:

- **`opSetAcc[]`** (18 entries) -- opcodes that **write** to `acc`. Membership
  means: after this opcode, the previous value of `acc` is dead.
  Examples: `SetR (0x80)`, `SetDP (0x81)`, `PeekR (0x84)`, `SetFP (0x8E)`,
  `CreateR (0x8F)`, `XSetFP (0x9E)`, `PeekFI (0xA8)`, `PeekSI (0xA9)`,
  `SetSP (0xAF)`, `PeekTLS (0xBB)`, `XCreateR (0xCE)`, `SelGrRR (0xD7)`,
  `SelEqRR (0xEE)`, `SelLtRR (0xEF)`, `SelULtRR (0xDF)`, `NewIR (0xF4)`,
  `NewNR (0xF5)`, `CreateNR (0xF7)`, `XNewNR (0xE7)`.

- **`opNotUsingAcc[]`** (77 entries) -- opcodes that **do not read** `acc`.
  Includes most of `Nop`, `Breakpoint`, `SNop`, `MovEnv`, `Unhook`, `MLen`,
  `DAlloc`, `ConvL`, `LNeg`, `Not`, `Neg`, `AltMode`, `XNop`, `XQuit`,
  `Shl`, `Shr`, the float-DP ops, the DP-arith family, `Open` / `ExtOpen`,
  `XStoreSIR` / `XStoreFIR`, `XLAddDP`, etc.

`ByteCodeUtil::isAccFree(it)` walks forward from `it` and returns `true` if
it hits an `opSetAcc` opcode before any opcode missing from `opNotUsingAcc`.
The `ByteCodeTransformer` uses this to gate `IfAccFree` patterns -- peephole
rewrites that are only legal if the accumulator's current value isn't read
downstream.

## Jump optimization (`CommandTape::optimizeJumps`)

Three passes:
1. **Idle-jump elision** -- a `Jump`/`Jeq`/... whose target label is the very
   next instruction is dropped.
2. **Unreachable-block removal** -- after an unconditional `Jump`,
   instructions up to the next reachable label are zeroed.
3. **Reachability map** -- labels not reached by any jump or fall-through
   become candidates for removal.

This runs **before** the JIT copies any inline body, so the eliminated
instructions don't appear in the output asm.

## `ByteCodeTransformer` (peephole trie)

A `MemoryTrie<ByteCodePattern>` is loaded from a serialized rule dump
(`bcwriter.cpp`'s init reads `_bcTransformer.trie.load(...)`). Patterns
match multi-opcode sequences and rewrite them -- typical cases:

- Coalescing `MovN n; AddN m` -> `MovN (n+m)`
- Folding compare-then-branch into a single emit
- Replacing `SetR r; CallR r` with the resolved call form

Pattern argument types (`ByteCodePatternType` in `bytecode.h`):
- `Set` -- record the matched arg into a slot (`arg1` or `arg2`)
- `Match` -- require the slot's value to equal the opcode's arg
- `MatchArg` -- for `Jump`/`Jne`: require the target label to match a stored
  slot
- `IfAccFree` -- apply only when `isAccFree()` is true at this point

The `loaded` flag protects against running without a rule file (the trie is
silently bypassed in that case).

## Variant selection (per-opcode dispatch)

Most opcodes are emitted by the default `loadOp` -- a straight copy of the
matching `inline %NNh` body. ~40 opcodes go through specialised
`load*Op` / `compile*` functions that pick a **variant slot** based on
properties of the operand the asm cannot see by itself. The per-opcode files
in `opcodes/` document the specific rule in a `**Selection rule:**`
paragraph and (when present) a `## Compiler behaviour` section. The most
common selector families are:

| Selector | Rule | Where it's used |
|---|---|---|
| `retrieveICode(arg)` | `1->1, 2->2, 4->3, 8->4, <0&\|x\|>med->9, <0->8, >med->10, else->0` | width-typed arith / cmp (ICmpN, IAddDPN, FCmpN, Shl, Shr...) |
| `retrieveCode(arg)` | `0->1, 1->2, 2->3, 3->4, 4->5, 8->7, >med->10, <0->9, else->0` | R-cmds, FrameDisp / FrameIndex / StackIndex ops |
| `retrieveCodeWithNegative(arg)` | `0->1, 1->2, 2->3, 3->4, <0->5, else->0` | FieldIndex (negative = supertype slot) |
| `retrieveNOpIndex(arg)` | `\|arg\|<=ext->0, >ext->1` (2 slots only) | XSaveDispN, XSaveDisp |
| `retrieveIRCode(arg1, arg2)` | `arg1  in  {1..4}->{2,4,6,8}; arg2=0 -> +1` | DCopyDPN |
| `retrieveStackIndexRCode(arg1, arg2)` | 9-slot table -- `arg1  in  {0,1}` x `arg2  in  {-1, 0, other}` | dual-arg stack-index ops |
| `loadCallOp` | bit 31 of arg2 -> prefix `6`; lower bits 0..4 -> inline 1..5 | `CallExtR (0xFE)` extended form |
| `compileDispatchMR` | 11 slots from `MESSAGE_FLAG_MASK` + `scope->altMode` | DispatchMR, XDispatchMR |

## Argument rewrites

The JIT also rewrites the *value* of the argument before passing it to the
inline template, transparently to anyone reading the asm. The most common
rewrites:

| Transform | Affected ops |
|---|---|
| `getFPOffset(arg, dataOffset)` -- abstract dp index -> frame-relative byte offset | LoadDP, SaveDP, LSaveDP, LLoadDP, XCmpDP, XLAddDP, XHookDPR, XLabelDPR, all DPN family |
| `scope->stackOffset + arg` -- JIT-tracked stack offset | LoadSI, SaveSI, StoreSI, PeekSI, LSaveSI, LLoadSI, XCmpSI, CmpSI, SetSP |
| `importMessage(arg)` -- module-local msg ref -> globally stable id | MovM, TstM, XRedirectM, CallMR, VCallMR, JumpMR, VJumpMR, DispatchMR, XDispatchMR |
| `((arg << 1) + 1) << indexPower` -- VMT-entry stride (skip msg word to land on addr word) | CallVI (0xB1), JumpVI (0xB5) |
| `(arg << indexPower) - vmtSize` (when arg < 0) -- supertype slot | FieldIndex ops (StoreR, XAssignI when used with negative index) |

`scope->stackOffset` is maintained across the tape by `compileAlloc` /
`compileFree` / `compileOpen` / `compileClose` / `compileExtOpen` etc. --
so the JIT knows how many bytes the function has currently allocated on
the stack, and `sp:i` operands address the right physical slot.

## State-machine opcodes

A few opcodes don't emit code at all; they only update JIT scope state and
influence the next opcode's emission:

| Opcode | JIT state change | Consumed by |
|---|---|---|
| `0x31 AltMode` | `scope->altMode = true` | next `DispatchMR` / `XDispatchMR` / `VCallMR` / `VJumpMR` -- picks alt-dispatch fast-path slots |
| `0x92 AllocI` | `scope->inlineMode = true`, resets `stackOffset` | paired with `FreeI` (0x93) which clears it |
| `0xF0 OpenIN` | sets `frameOffset`, zeroes `stackOffset` | paired with `CloseN` (0x91) which restores |
| `0xF2 ExtOpenIN` | same as OpenIN + extended-mode flag | paired with `ExtCloseN` (0xCA) |
| `0xDE XOpenIN` | zero-emission; updates `frameOffset` and resets `stackOffset` if arg1==0 | continues into a normal body |
