# ELENA JIT Linker -- Layout & Reference Encoding

The JIT linker (`elenasrc3/engine/jitlinker.cpp`) resolves cross-module
references and lays out VMTs, message tables and constants as modules are
loaded. Several layouts and encoding conventions are visible in the per-arch
opcode templates as symbolic offsets and ref kinds; this document captures
them.

Companion documents:
- [`runtime.md`](runtime.md) -- heap, object header, frame, GCXT
- [`opcodes/README.md`](opcodes/README.md) -- per-opcode JIT bodies
- [`jit-passes.md`](jit-passes.md) -- optimizer & tape transformer

## VMT layout

```
                 vmt-pointer (stored in object header at [acc - elVMTOffset])
                     |
                     v
+-----------------+-----------------+-----------------+
|  parentRef      |  flags          |  classRef       |   (header)
+-----------------+-----------------+-----------------+
|  count (entries)                                    |
+-----------------------------------------------------+
|  (message_0, address_0)                             |   (sorted entries)
|  (message_1, address_1)                             |
|  ...                                                |
|  (message_{count-1}, address_{count-1})             |
+-----------------------------------------------------+
|  0                                                  |   (index table -- zero-terminated)
|  alt_message_0                                      |
|  alt_message_1                                      |
|  ...                                                |
|  0                                                  |
+-----------------------------------------------------+
|  static-field map (optional)                        |
+-----------------------------------------------------+
```

**Header negative offsets** (the VMT pointer lands on the entry array; the
header lives below it):
- `[vmt + elVMTSizeOffset]` = `[vmt + 8]` -- entry count
- `[vmt + elVMTFlagOffset]` = `[vmt + 0x18]` -- class flags (tested by
  `tstflg` 0xC3)
- `[vmt + elPackageOffset]` = `[vmt + 0x20]` -- package metadata

**Entry size:**
- 32-bit arches: 8 bytes per entry (`mssg_t` 4B + `pos_t` 4B). Verified in
  the asm -- e.g. x32 `redirect` advances by 8: `lea edi, [eax+8]`.
- 64-bit arches: 16 bytes per entry (`mssg_t` 4B + 4B alignment pad + `pos_t`
  8B). Verified in the asm -- amd64 `redirect` advances by 16:
  `lea r14, [r8+16]`.

Entries are **sorted by `mssg_t`** ascending -- this is why `Redirect (0x03)`,
`XRedirect (0xB6)`, `Dispatch (0xFB)` can all do **binary search**.

The **index table** is the alt-dispatch fast path: when a class has
variadic/property messages, the linker pre-computes a per-class table of
just those messages so the runtime can `XDispatch` straight to the matching
handler without scanning the full VMT.

## Message encoding (`mssg_t`)

`mssg_t` is a 32-bit packed value:

```
 31              9 8     5 4         0
+-----------------+-------+-----------+
|  action ref     | flags | arg count |
+-----------------+-------+-----------+
```

- bits **0-4** (`ARG_MASK = 0x1F`): argument count (0..31)
- bits **5-8** (`MESSAGE_FLAG_MASK`):
  - `STATIC_MESSAGE   = 0x100` (static / class method)
  - `FUNCTION_MESSAGE = 0x020`
  - `VARIADIC_MESSAGE = 0x040`
  - `PROPERTY_MESSAGE = 0x080`
  - `CONVERSION_MESSAGE = 0x0C0` (overload-pattern flag)
- bits **9-31** (`ACTION_ORDER = 9`): action (selector) reference into the
  module's message table

Encoding helper: `flags | ((actionRef << ACTION_ORDER) + argCount)`.

The `mlen` (0x15) opcode extracts the arg count via `index & ARG_MASK`; the
`loads` (0x14) opcode reads `[acc].action` and translates it through the
per-module subject table at `mdata` so message refs become globally stable
across module boundaries.

## Reference masks (`mskXxx` in `elena.h` / `elenaconst.h`)

R-command operands carry a 32-bit reference where the **high byte** identifies
the section/type and the rest is the section-local index. The JIT looks at
the mask to pick the right inline variant **and** the right relocation kind.

### Section masks (operand kinds)

| Mask | Meaning |
|---|---|
| `mskSymbolRef` | code symbol (procedure entry) |
| `mskVMTRef` | class VMT |
| `mskClassRef` | class metadata (the "class class") |
| `mskRDataRef` | rdata section (constants, VMTs live here) |
| `mskMDataRef` | method-managed data |
| `mskStatDataRef` | static (per-process) data |
| `mskTLSRef` | thread-local slot |
| `mskExternalRef` | imported external (C) symbol |
| `mskMssgLiteralRef` | message-literal constant |
| `mskConstArray` | constant array |
| `mskPackageRef` | package descriptor |

### Encoding masks (how the address gets written)

| Mask | Encoding |
|---|---|
| `mskRef32` | absolute 32-bit |
| `mskRef64` | absolute 64-bit |
| `mskRelRef32` | PC-relative 32-bit (used for x86/amd64 `call`/`jmp` near, RIP-relative loads) |
| `mskRef32Hi` / `mskRef32Lo` | high/low 16 bits -- used by aarch64 `movz`/`movk` pairs and ppc64le `addis`/`ori` |
| `mskDisp32Hi` / `mskDisp32Lo` | displacement field split across two instructions |

### Which masks each R-command accepts

| Opcode | Accepted masks |
|---|---|
| `0x80 SetR` | `mskSymbolRef`, `mskVMTRef`, `mskClassRef`, `mskConstArray`, `mskRDataRef`, `mskStatDataRef` |
| `0x84 PeekR` | same as `SetR` |
| `0x85 StoreR` | same as `SetR` |
| `0x8F CreateR` / `0xCE XCreateR` / `0xE7 XNewNR` / `0xF4 NewIR` / `0xF5 NewNR` / `0xF7 CreateNR` | `mskVMTRef` (the class to instantiate) |
| `0xB0 CallR` | `mskSymbolRef`, `mskExternalRef` |
| `0xC0 CmpR` | `mskSymbolRef`, `mskVMTRef`, `mskRDataRef` |
| `0xFE CallExtR` | `mskExternalRef` only |
| `0xFD CallMR` / `0xFC VCallMR` / `0xED JumpMR` / `0xEC VJumpMR` / `0xFB DispatchMR` / `0xFA XDispatchMR` | M+R: message + `mskVMTRef` |

The inline-variant prefix that the JIT picks for each R-command depends on
which mask the operand carries (e.g. `SetR` to a symbol uses one body, to a
VMT another, because they encode different relocation widths and may need
different addressing modes on aarch64/ppc64le).
