# 0x18 -- dtrans

- **Category:** SingleOp
- **Enum:** `ByteCode::DTrans`
- **Operand(s):** (none)
- **Reads:** `sp[0]` (source pointer), `index` (count), `acc` (destination pointer)
- **Writes:** `[acc .. acc + index*slot_size]`
- **Side effects:** Raw pointer copy -- no write barrier.

## Semantics
`sp[0] >> acc, index ptrs` -- copies `index` reference-sized slots from the buffer pointed to by `sp[0]` into the buffer pointed to by `acc`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %18h`

```asm
  mov  rsi, r10
  mov  rcx, rdx
  mov  rdi, rbx
  rep  movsq
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %18h`

```asm
  mov  eax, esi
  mov  ecx, edx
  mov  edi, ebx
  rep  movsd
  mov  esi, eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %18h`

```asm
  mov     x11, x9
  mov     x12, x0
  mov     x13, x10

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 1
  ldr     x14, [x12], #8
  str     x14, [x13], #8
  b       labLoop

labEnd:
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %18h`

```asm
  mr      r16, r14
  mr      r19, r3
  mr      r18, r15

labLoop:
  cmpwi   r16,0
  beq     labEnd
  ld      r17, 0(r19)
  addi    r16, r16, -1
  std     r17, 0(r18)
  addi    r18, r18, 8
  addi    r19, r19, 8
  b       labLoop

labEnd:
```

## Notes
- Pointer-by-pointer copy from `sp[0]` to `acc` for `index` slots; no write barrier (used during migration of boxed values when destination is fresh memory).
- amd64 uses `rep movsq`, x32 uses `rep movsd` (and saves/restores `esi` so sp[0] is preserved); aarch64/ppc64le emit explicit loops.
- After execution `sp[0]` is preserved on x32 (explicitly restored) but on amd64 `rsi`/`r10` is left pointing past the source buffer -- callers must reload sp[0] if needed. Verify against your arch.
- Clobbers `index` (count goes to 0), scratch registers, and `rdi`/`rcx`/`rsi` on x86.
