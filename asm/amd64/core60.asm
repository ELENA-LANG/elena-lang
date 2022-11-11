// !! NOTE : R15 register must be preserved

// ; --- Predefined References  --
define INVOKER              10001h
define GC_ALLOC	            10002h
define VEH_HANDLER          10003h

define CORE_TOC             20001h
define SYSTEM_ENV           20002h
define CORE_GC_TABLE        20003h
define CORE_THREAD_TABLE    2000Bh
define VOID           	    2000Dh
define VOIDPTR              2000Eh

define ACTION_ORDER              9
define ACTION_MASK            1E0h
define ARG_MASK               01Fh

// ; --- Object header fields ---
define elSizeOffset          0004h
define elVMTOffset           0010h 
define elObjectOffset        0010h

// ; --- VMT header fields ---
define elVMTSizeOffset       0008h
define elVMTFlagOffset       0018h
define elPackageOffset       0020h

// ; --- GC TABLE OFFSETS ---
define gc_header             0000h
define gc_start              0008h
define gc_yg_current         0010h
define gc_yg_current         0018h
define gc_yg_end             0020h
define gc_mg_start           0038h
define gc_mg_current         0040h
define gc_end                0048h
define gc_mg_wbar            0050h

define et_current            0008h
define tt_stack_frame        0010h

define es_prev_struct        0000h
define es_catch_addr         0008h
define es_catch_level        0010h
define es_catch_frame        0018h

// ; --- Page Size ----
define page_ceil               2Fh
define page_mask        0FFFFFFE0h
define page_size_order          5h
define struct_mask       40000000h
define struct_mask_inv   3FFFFFFFh

// ; --- System Core Preloaded Routines --

structure % CORE_TOC

  dq 0         // ; reserved

end
 
structure % CORE_THREAD_TABLE

  dq 0 // ; crtitical_handler      ; +x00   - pointer to ELENA exception handler
  dq 0 // ; et_current             ; +x08   - pointer to the current exception struct
  dq 0 // ; tt_stack_frame         ; +x10   - pointer to the stack frame

end

structure %CORE_GC_TABLE

  dq 0 // ; gc_header             : +00h
  dq 0 // ; gc_start              : +08h
  dq 0 // ; gc_yg_start           : +10h
  dq 0 // ; gc_yg_current         : +18h
  dq 0 // ; gc_yg_end             : +20h
  dq 0 // ; gc_shadow             : +28h
  dq 0 // ; gc_shadow_end         : +30h
  dq 0 // ; gc_mg_start           : +38h
  dq 0 // ; gc_mg_current         : +40h
  dq 0 // ; gc_end                : +48h
  dq 0 // ; gc_mg_wbar            : +50h

end

// ; NOTE : the table is tailed with GCMGSize,GCYGSize and MaxThread fields
structure %SYSTEM_ENV

  dq 0
  dq data : %CORE_GC_TABLE
  dq data : %CORE_THREAD_TABLE
  dq code : %INVOKER
  dq code : %VEH_HANDLER
  // ; dd GCMGSize
  // ; dd GCYGSize

end

structure %VOID

  dq 0
  dq 0  // ; a reference to the super class class
  dq 0
  dq 0  
  dq 0

end

structure %VOIDPTR

  dq rdata : %VOID + elPackageOffset
  dq 0
  dq 0

end

// ; --- GC_ALLOC ---
// ; in: rcx - size ; out: ebx - created object
// ; note for linux - there is a separate copy
inline % GC_ALLOC

  mov  rax, [data : %CORE_GC_TABLE + gc_yg_current]
  mov  r12, [data : %CORE_GC_TABLE + gc_yg_end]
  add  rcx, rax
  cmp  rcx, r12
  jae  short labYGCollect
  mov  [data : %CORE_GC_TABLE + gc_yg_current], rcx
  lea  rbx, [rax + elObjectOffset]
  ret

labYGCollect:
  // ; save registers
  sub  rcx, rax
  push r10
  push r11
  push rbp

  // ; lock frame
  mov  [data : %CORE_THREAD_TABLE + tt_stack_frame], rsp

  push rcx

  // ; create set of roots
  mov  rbp, rsp
  xor  ecx, ecx
  push rcx        // ; reserve place 
  push rcx
  push rcx

  // ;   save static roots
  mov  rax, rdata : %SYSTEM_ENV
  mov  rsi, stat : %0
  mov  ecx, dword ptr [rax]
  shl  ecx, 3
  push rsi
  push rcx

  // ;   collect frames
  mov  rax, [data : %CORE_THREAD_TABLE + tt_stack_frame]  
  mov  rcx, rax

labYGNextFrame:
  mov  rsi, rax
  mov  rax, [rsi]
  test rax, rax
  jnz  short labYGNextFrame

  push rcx
  sub  rcx, rsi
  neg  rcx
  push rcx  

  mov  rax, [rsi + 8]
  test rax, rax
  mov  rcx, rax
  jnz  short labYGNextFrame

  mov [rbp-8], rsp      // ; save position for roots

  mov  rdx, [rbp]
  mov  rcx, rsp

  // ; restore frame to correctly display a call stack
  mov  rax, rbp
  mov  rbp, [rax+8]

  // ; call GC routine
  sub  rsp, 30h
  mov  [rsp+28h], rax
  call extern "$rt.CollectGCLA"

  mov  rbp, [rsp+28h] 
  add  rsp, 30h
  mov  rbx, rax

  mov  rsp, rbp 
  pop  rcx
  pop  rbp
  pop  r11
  pop  r10

  ret

end

// ; ==== Command Set ==

// ; redirect
inline % 03h // (rbx - object, rdx - message, r10 - arg0, r11 - arg1)

  mov  r14, [rbx - elVMTOffset]
  xor  ecx, ecx
  mov  rsi, qword ptr[r14 - elVMTSizeOffset]

labSplit:
  test esi, esi
  jz   short labEnd

labStart:
  shr   esi, 1
  lea   r13, [rsi*2]
  setnc cl
  cmp   rdx, [r14+r13*8]
  je    short labFound
  lea   r8, [r14+r13*8]
  jb    short labSplit
  lea   r14, [r8+16]
  sub   esi, ecx
  jmp   labSplit
  nop
  nop
labFound:
  jmp   [r14+r13*8+8]

labEnd:
                               
end

// ; quit
inline %4

  ret

end

// ; movenv
inline %5

  mov  rdx, rdata64 : %SYSTEM_ENV

end

// ; load
inline %6

  mov  edx, dword ptr [rbx]

end

// ; len
inline %7

  mov  edx, struct_mask_inv
  mov  rcx, [rbx-elSizeOffset]
  and  rdx, rcx
  shr  edx, 3

end

// ; class
inline %8

  mov rbx, [rbx - elVMTOffset] 

end

// ; save
inline %9

  mov  dword ptr [rbx], edx

end

// ; throw
inline %0Ah

  mov  rax, [data : %CORE_THREAD_TABLE + et_current]
  jmp  [rax + es_catch_addr]

end

// ; unhook
inline %0Bh

  mov  rdi, [data : %CORE_THREAD_TABLE + et_current]

  mov  rax, [rdi + es_prev_struct]
  mov  rbp, [rdi + es_catch_frame]
  mov  rsp, [rdi + es_catch_level]

  mov  [data : %CORE_THREAD_TABLE + et_current], rax

end

// ; loadv
inline % 0Ch

  and  edx, ARG_MASK
  mov  ecx, dword ptr [rbx]
  and  ecx, ~ARG_MASK
  or   edx, ecx

end

// ; xcmp
inline % 0Dh

  mov  ecx, dword ptr [rbx]
  cmp  edx, ecx 

end

// ; bload
inline %0Eh

  mov  edx, dword ptr [ebx]
  and  edx, 0FFh 

end

// ; wload
inline %0Fh

  mov  eax, dword ptr [ebx]
  cwde
  mov  edx, eax

end

// ; exclude
inline % 10h

  push 0                                                     
  push rbp     
  mov  [data : %CORE_THREAD_TABLE + tt_stack_frame], rsp

end

// ; include
inline % 11h

  add  rsp, 10h

end

// ; coalesce
inline % 20h

   test  rbx, rbx 
   cmovz rbx, r10

end

// ; setr
inline %80h

  mov  rbx, __ptr64_1

end 

// ; setr 0
inline %180h

  xor  rbx, rbx

end 

// ; setddisp
inline %81h

  lea  rbx, [rbp + __arg32_1]

end 

// ; nlen n
inline %82h

  mov  eax, struct_mask_inv
  and  eax, dword ptr [rbx-elSizeOffset]
  mov  ecx, __n_1
  cdq
  idiv ecx
  mov  rdx, rax

end

// ; nlen 1
inline %182h

  mov  edx, struct_mask_inv
  mov  ecx, dword ptr [rbx-elSizeOffset]
  and  edx, ecx

end

// ; nlen 2
inline %282h

  mov  edx, struct_mask_inv
  mov  ecx, dword ptr [rbx-elSizeOffset]
  and  edx, ecx
  shr  edx, 1

end

// ; nlen 4
inline %382h

  mov  edx, struct_mask_inv
  mov  ecx, dword ptr [rbx-elSizeOffset]
  and  edx, ecx
  shr  edx, 2

end

// ; nlen 8
inline %482h

  mov  edx, struct_mask_inv
  mov  ecx, dword ptr [rbx-elSizeOffset]
  and  edx, ecx
  shr  edx, 3

end

// ; xassigni
inline %83h

  mov  [rbx + __arg32_1], r10

end

// ; peekr
inline %84h

  mov  rax, __ptr64_1
  mov  rbx, [rax]

end 

// ; storer
inline %85h

  mov  rax, __ptr64_1
  mov  [rax], rbx

end 

// ; xswapsi
inline %86h

  mov  rax, [rsp+__arg32_1]
  mov  [rsp+__arg32_1], r10
  mov  r10, rax

end

// ; xswapsi 0
inline %186h

end

// ; xswapsi 1
inline %286h

  mov  rax, r11
  mov  r11, r10
  mov  r10, rax

end

// ; swapsi
inline %87h

  mov  rax, [rsp+__arg32_1]
  mov  [rsp+__arg32_1], rbx
  mov  rbx, rax

end

// ; swapsi 0
inline %187h

  mov  rax, r10
  mov  r10, rbx
  mov  rbx, rax

end

// ; swapsi 1
inline %287h

  mov  rax, r11
  mov  r11, rbx
  mov  rbx, rax

end

// ; movm
inline %88h

  mov  edx, __arg32_1

end

// ; movn
inline %89h

  mov  edx, __n_1

end

// ; loaddp
inline %8Ah

  mov  edx, dword ptr [rbp + __arg32_1]

end 

// ; xcmpdp
inline %8Bh

  mov  ecx, dword ptr [rbp + __arg32_1]
  cmp  edx, ecx 

end 

// ; subn
inline %8Ch

  sub  edx, __n_1

end

// ; addn
inline %8Dh

  add  edx, __n_1

end

// ; setfp
inline %8Eh

  lea  rbx, qword ptr [rbp + __arg32_1]

end 

// ; copy
inline %90h

  mov  rsi, r10
  mov  ecx, __n_1 
  mov  rdi, rbx
  rep  movsb

end

// ; closen
inline %91h

  add  rbp, __n_1
  mov  rsp, rbp
  pop  rbp
  
end

// ; closen 0
inline %191h

  mov  rsp, rbp
  pop  rbp
  
end

// ; alloci
inline %92h

  sub  rsp, __arg32_1
  xor  rax, rax
  mov  ecx, __n_1
  mov  rdi, rsp
  rep  stos

end

// ; alloci 0
inline %192h

end

// ; alloci 1
inline %292h

  push 0

end

// ; alloci 2
inline %392h

  push 0
  push 0

end

// ; alloci 3
inline %492h

  push 0
  push 0
  push 0

end

// ; alloci 4
inline %592h

  push 0
  push 0
  push 0
  push 0

end

// ; freei
inline %93h

  add  rsp, __arg32_1

end

// ; andn
inline %94h

  and  edx, __n_1

end

// ; readn
inline %95h

  mov  ecx, __n_1 
  mov  eax, edx
  imul eax, ecx
  mov  rsi, r10
  add  rsi, rax
  mov  rdi, rbx
  rep  movsb

end

// ; writen
inline %96h

  mov  ecx, __n_1 
  mov  eax, edx
  imul eax, ecx
  mov  rdi, r10
  add  rdi, rax
  mov  rsi, rbx
  rep  movsb

end

// ; cmpn n
inline %097h

  cmp  edx, __n_1

end

// ; saveddisp
inline %0A0h

  mov  dword ptr[rbp + __arg32_1], edx

end

// ; storefp
inline %0A1h

  mov  qword ptr [rbp + __arg32_1], rbx

end

// ; savesi
inline %0A2h

  mov dword ptr [rsp + __arg32_1], edx

end 

// ; savesi 0
inline %1A2h

  mov r10, rdx

end 

// ; savesi 1
inline %2A2h

  mov r11, rdx

end 

// ; storesi
inline %0A3h

  mov qword ptr [rsp + __arg32_1], rbx

end 

// ; storesi 0
inline %1A3h

  mov r10, rbx

end 

// ; storesi 1
inline %2A3h

  mov r11, rbx

end 

// ; xflushsi i
inline %0A4h

end 

// ; xflushsi 0
inline %1A4h

  mov [rsp+8], r10

end 

// ; xflushsi 1
inline %2A4h

  mov [rsp+16], r11

end 

// ; geti
inline %0A5h

  mov  rbx, [rbx + __arg32_1]

end

// ; assigni
inline %0A6h

  mov  rax, rbx

  // calculate write-barrier address
  mov  rcx, [data : %CORE_GC_TABLE + gc_header]
  sub  rax, [data : %CORE_GC_TABLE + gc_start]
  shr  rax, page_size_order
  mov  [rbx + __arg32_1], r10
  mov  byte ptr [rax + rcx], 1  

end

// ; xrefreshsi i
inline %0A7h

end 

// ; xrefreshsi 0
inline %1A7h

  mov r10, [rsp+8]

end 

// ; xrefreshsi 1
inline %2A7h

  mov [rsp+16], r11

end 

// ; peekfi
inline %0A8h

  mov  rbx, qword ptr [rbp + __arg32_1]

end 

// ; peeksi
inline %0A9h

  mov rbx, qword ptr [rsp + __arg32_1]

end 

// ; peeksi 0
inline %1A9h

  mov rbx, r10

end 

// ; peeksi 1
inline %2A9h

  mov rbx, r11

end 

// ; callr
inline %0B0h

  call __relptr32_1

end

// ; callvi
inline % 0B1h

  mov  rax, [rbx - elVMTOffset]
  call [rax + __arg32_1]

end

// ; jumpvi
inline % 0B5h

  mov  rax, [rbx - elVMTOffset]
  jmp  [rax + __arg32_1]

end

// ; cmpr r
inline %0C0h

  mov  rax, __ptr64_1
  cmp  rbx, rax

end 

// ; cmpr 0
inline %1C0h

  mov  rax, 0
  cmp  rbx, rax

end 

// ; icmpn 4
inline %0C2h

  mov  rax, [r10]
  cmp  eax, dword ptr[rbx]

end

// ; icmpn 1
inline %1C2h

  mov  rax, [r10]
  cmp  al, byte ptr [rbx]

end

// ; icmpn 2
inline %2C2h

  mov  rax, [r10]
  cmp  ax, word ptr [rbx]

end

// ; icmpn 8
inline %4C2h

  mov  rax, [r10]
  cmp  rax, [rbx]

end

// ; tstflg
inline %0C3h

  mov  rcx, [rbx - elVMTOffset] 
  mov  rax, [rcx - elVMTFlagOffset]
  test eax, __n_1

end

// ; tstn
inline %0C4h

  test edx, __n_1

end

// ; tstm
inline % 0C5h

  mov  eax, __arg32_1
  mov  r14, [rbx - elVMTOffset]
  xor  ecx, ecx
  mov  rsi, qword ptr[r14 - elVMTSizeOffset]

labSplit:
  test esi, esi
  jz   short labEnd

labStart:
  shr   esi, 1
  lea   r13, [rsi*2]
  setnc cl
  cmp   rax, [r14+r13*8]
  je    short labFound
  lea   r8, [r14+r13*8]
  jb    short labSplit
  lea   r14, [r8+16]
  sub   esi, ecx
  jmp   labSplit
  nop
  nop
labFound:
  mov  esi, 1

labEnd:
  cmp  esi, 1
                               
end

// ; cmpfi
inline %0C8h

  cmp  rbx, qword ptr [rbp + __arg32_1]

end 

// ; cmpsi
inline %0C9h

  cmp rbx, qword ptr [rsp + __arg32_1]

end 

// ; cmpsi 0
inline %1C9h

  cmp rbx, r10

end 

// ; cmpsi 1
inline %2C9h

  cmp rbx, r11

end 

// ; ianddpn
inline %0D8h

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  and  dword ptr [rdi], eax

end

// ; ianddpn
inline %1D8h

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  and  byte ptr [rdi], al

end

// ; ianddpn
inline %2D8h

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  and  word ptr [rdi], ax

end

// ; ianddpn
inline %4D8h

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  and  [rdi], rax

end

// ; iordpn
inline %0D9h

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  or   dword ptr [rdi], eax

end

// ; iordpn
inline %1D9h

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  or   byte ptr [rdi], al

end

// ; iordpn
inline %2D9h

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  or   word ptr [rdi], ax

end

// ; iordpn
inline %4D9h

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  or   [rdi], rax

end

// ; ixordpn
inline %0DAh

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  xor  dword ptr [rdi], eax

end

// ; ixordpn
inline %1DAh

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  xor  byte ptr [rdi], al

end

// ; ixordpn
inline %2DAh

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  xor  word ptr [rdi], ax

end

// ; ixordpn
inline %4DAh

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  xor  [rdi], rax

end

// ; copydpn
inline %0E0h

  mov  rsi, r10
  lea  rdi, [rbp + __arg32_1]
  mov  ecx, __n_2
  rep  movsb

end

// ; copydpn dpn, 1
inline %1E0h

  mov  rax, [r10]
  mov  byte ptr [rbp + __arg32_1], al

end

// ; copydpn dpn, 2
inline %2E0h

  mov  rax, [r10]
  mov  word ptr [rbp + __arg32_1], ax

end

// ; copydpn dpn, 4
inline %3E0h

  mov  rax, [r10]
  mov  dword ptr [rbp + __arg32_1], eax

end

// ; copydpn dpn, 8
inline %4E0h

  mov  rax, [r10]
  mov  [rbp + __arg32_1], rax

end

// ; iaddndp
inline %0E1h

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  add  dword ptr [rdi], eax

end

// ; iaddndp
inline %1E1h

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  add  byte ptr [rdi], al

end

// ; iaddndp
inline %2E1h

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  add  word ptr [rdi], ax

end

// ; iaddndp
inline %4E1h

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  add  [rdi], rax

end

// ; isubndp
inline %0E2h

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  sub  dword ptr[rdi], eax

end

// ; isubndp
inline %1E2h

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  sub  byte ptr [rdi], al

end

// ; isubndp
inline %2E2h

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  sub  word ptr [rdi], ax

end

// ; isubndp
inline %4E2h

  lea  rdi, [rbp + __arg32_1]
  mov  rax, [r10]
  sub  [rdi], rax

end

// ; imulndp
inline %0E3h

  mov  rcx, [r10]
  mov  rax, [rbp+__arg32_1]
  imul ecx
  mov  dword ptr [rbp+__arg32_1], eax

end

// ; imulndp
inline %1E3h

  mov  rcx, [r10]
  mov  rax, [rbp+__arg32_1]
  imul cl
  mov  byte ptr [rbp+__arg32_1], al

end

// ; imulndp
inline %2E3h

  mov  rcx, [r10]
  mov  rax, [rbp+__arg32_1]
  imul cx
  mov  word ptr [rbp+__arg32_1], ax

end

// ; imulndp
inline %4E3h

  mov  rcx, [r10]
  mov  rax, [rbp+__arg32_1]
  imul rcx
  mov  [rbp+__arg32_1], rax

end

// ; idivndp
inline %0E4h

  mov  rcx, [r10]
  mov  rax, [rbp+__arg32_1]
  xor  edx, edx
  idiv ecx
  mov  dword ptr [rbp+__arg32_1], eax

end

// ; idivndp
inline %1E4h

  mov  rcx, [r10]
  mov  rax, [rbp+__arg32_1]
  idiv cl
  mov  byte ptr [rbp+__arg32_1], al

end

// ; idivndp
inline %2E4h

  mov  rcx, [r10]
  mov  rax, [rbp+__arg32_1]
  idiv cx
  mov  word ptr [rbp+__arg32_1], ax

end

// ; idivndp
inline %4E4h

  mov  rcx, [r10]
  mov  rax, [rbp+__arg32_1]
  idiv rcx
  mov  [rbp+__arg32_1], rax

end

// ; nsavedpn
inline %0E5h

  mov  eax, __n_2
  mov  dword ptr [rbp+__arg32_1], eax

end

// ; xhookdpr
inline %0E6h

  lea  rdi, [rbp + __arg32_1]
  mov  rcx, __ptr64_2
  mov  rax, [data : %CORE_THREAD_TABLE + et_current]

  mov  [rdi + es_prev_struct], rax
  mov  [rdi + es_catch_frame], rbp
  mov  [rdi + es_catch_level], rsp
  mov  [rdi + es_catch_addr], rcx

  mov  [data : %CORE_THREAD_TABLE + et_current], rdi

end

// ; xnewnr n, r
inline %0E7h

  lea  rbx, [rbx + elObjectOffset]

  mov  ecx, __n_1
  mov  rax, __ptr64_2
  mov  [rbx - elVMTOffset], rax
  mov  dword ptr [rbx - elSizeOffset], ecx

end

// ; nadddpn
inline %0E8h

  mov  eax, __n_2
  add  dword ptr [rbp+__arg32_1], eax

end

// ; xwriteon
inline %0EAh

  mov  rdi, r10
  mov  ecx, __n_2
  lea  rsi, [rbx + __arg32_1]
  rep  movsb

end

// ; xcopyon
inline %0EBh

  mov  rsi, r10
  mov  ecx, __n_2
  lea  rdi, [rbx + __arg32_1]
  rep  movsb

end

// ; vjumpmr
inline % 0ECh

  mov  rax, [rbx - elVMTOffset]
  jmp  [rax + __arg32_1]

end

// ; jumpmr
inline %0EDh

  jmp __relptr32_2

end

// ; seleqrr
inline %0EEh

  mov   rax, __ptr64_1
  mov   rbx, __ptr64_2
  cmovz rbx, rax

end

// ; selltrr
inline %0EFh

  mov   rax, __ptr64_1
  mov   rbx, __ptr64_2
  cmovl rbx, rax

end

// ; openin
inline %0F0h

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  sub  rsp, __n_2
  push rbp
  push rax
  mov  rbp, rsp
  mov  rcx, __n_1
  sub  rsp, __arg32_1
  mov  rdi, rsp
  rep  stos

end 

// ; openin 0, 0
inline %1F0h

  push rbp
  mov  rbp, rsp

end 

// ; openin 1, 0
inline %2F0h

  push rbp
  mov  rbp, rsp
  push 0
  push 0

end 

// ; openin 2, 0
inline %3F0h

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  push rax
  push rax

end 

// ; openin 3, 0
inline %4F0h

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  push rax
  push rax
  push rax
  push rax

end 

// ; openin 0, n
inline %5F0h

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  sub  rsp, __n_2
  push rbp
  push rax
  mov  rbp, rsp

end 

// ; openin i, 0
inline %6F0h

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  mov  rcx, __n_1
  sub  rsp, __arg32_1
  mov  rdi, rsp
  rep  stos

end 

// ; xstoresir
inline %0F1h

  mov  rax, __ptr64_2
  mov  qword ptr [rsp+__arg32_1], rax

end

// ; xstoresir :0, ...
inline %1F1h

  mov  r10, __ptr64_2

end

// ; xstoresir :1, ...
inline %2F1h

  mov  r11, __ptr64_2

end

// ; xstoresir :0, 0
inline %6F1h

  mov  r10, 0

end

// ; xstoresir :1, 0
inline %7F1h

  mov  r11, 0

end

// ; openheaderin
inline %0F2h

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  sub  rsp, __n_2
  push rbp
  push rax
  mov  rbp, rsp
  mov  rcx, __n_1
  sub  rsp, __arg32_1
  mov  rdi, rsp
  rep  stos

end 

// ; openheaderin 0, 0
inline %1F2h

  push rbp
  mov  rbp, rsp

end 

// ; openheaderin 1, 0
inline %2F2h

  push rbp
  mov  rbp, rsp
  push 0

end 

// ; openheaderin 2, 0
inline %3F2h

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  push rax
  push rax

end 

// ; openheaderin 3, 0
inline %4F2h

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  push rax
  push rax
  push rax

end 

// ; openheaderin 0, n
inline %5F2h

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  sub  rsp, __n_2
  push rbp
  push rax
  mov  rbp, rsp

end 

// ; openheaderin i, 0
inline %6F2h

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  mov  rcx, __n_1
  sub  rsp, __arg32_1
  mov  rdi, rsp
  rep  stos

end 

// ; movsifi
inline %0F3h

  mov  rax, qword ptr [rbp+__arg32_2]
  mov  qword ptr [rsp+__arg32_1], rax

end

// ; movsifi sp:0, fp:i2
inline %1F3h

  mov  r10, qword ptr [rbp+__arg32_2]

end

// ; movsifi sp:1, fp:i2
inline %2F3h

  mov  r11, qword ptr [rbp+__arg32_2]

end

// ; newir i, r
inline %0F4h

  mov  ecx, __arg32_1
  call %GC_ALLOC

  mov  ecx, __n_1
  mov  rax, __ptr64_2
  mov  dword ptr [rbx - elSizeOffset], ecx
  mov  [rbx - elVMTOffset], rax

end

// ; newnr n, r
inline %0F5h

  mov  ecx, __arg32_1
  call %GC_ALLOC

  mov  ecx, __n_1
  mov  rax, __ptr64_2
  mov  [rbx - elVMTOffset], rax
  mov  dword ptr [rbx - elSizeOffset], ecx

end

// ; xmovsisi
inline %0F6h

  mov  rax, [rsp+__arg32_2]
  mov  [rsp+__arg32_1], rax

end

// ; xmovsisi 0, n
inline %1F6h

  mov  r10, [rsp+__arg32_2]

end

// ; xmovsisi n, 0
inline %2F6h

  mov  [rsp+__arg32_1], r10

end

// ; xmovsisi 1, n
inline %3F6h

  mov  r11, [rsp+__arg32_2]

end

// ; xmovsisi n, 1
inline %4F6h

  mov  [rsp+__arg32_1], r11

end

// ; xmovsisi 0, 1
inline %5F6h

  mov  r10, r11

end

// ; xmovsisi 1, 0
inline %6F6h

  mov  r11, r10

end

// ; createnr n,r
inline %0F7h

  mov  rax, [r10]
  mov  ecx, page_ceil
  imul eax, __n_1
  add  ecx, eax
  and  ecx, page_mask 
  call %GC_ALLOC

  mov  rcx, [r10]
  mov  eax, __n_1
  imul ecx, eax
  or   ecx, struct_mask

  mov  rax, __ptr64_2
  mov  [rbx - elSizeOffset], rcx
  mov  [rbx - elVMTOffset], rax

end

// ; xstorefir
inline %0F9h

  mov  rax, __ptr64_2
  mov  [rbp+__arg32_1], rax

end

// ; xdispatchmr
// ; NOTE : __arg32_1 - message; __n_1 - arg count; __ptr32_2 - list, __n_2 - argument list offset
inline % 0FAh

  mov  r8,  rbx
  mov  [rsp+8], r10                      // ; saving arg0
  lea  rax, [rsp + __n_2]
  mov  [rsp+16], r11                     // ; saving arg0

  mov  rsi, __ptr64_2
  xor  edx, edx
  mov  rbx, [rsi] // ; message from overload list

labNextOverloadlist:
  mov  r9, mdata : %0
  shr  ebx, ACTION_ORDER
  lea  r13, [rbx*8]
  mov  r13, [r9 + r13 * 2 + 8]
  mov  ecx, __n_1
  lea  rbx, [r13 - 8]

labNextParam:
  sub  ecx, 1
  jnz  short labMatching

  mov  r9, __ptr64_2
  lea  r13, [rdx * 8]
  mov  rbx, r8
  mov  rax, [r9 + r13 * 2 + 8]
  mov  rdx, [r9 + r13 * 2]
  jmp  rax

labMatching:
  mov  rdi, [rax + rcx * 8]

  //; check nil
  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  test  rdi, rdi                                              
  cmovz rdi, rsi

  mov  rdi, [rdi - elVMTOffset]
  mov  rsi, [rbx + rcx * 8]

labNextBaseClass:
  cmp  rsi, rdi
  jz   labNextParam
  mov  rdi, [rdi - elPackageOffset]
  and  rdi, rdi
  jnz  short labNextBaseClass

  add  edx, 1
  mov  r13, __ptr64_2
  lea  r9, [rdx * 8]
  mov  rbx, [r13 + r9 * 2] // ; message from overload list
  and  rbx, rbx
  jnz  labNextOverloadlist

end

// ; dispatchmr
// ; NOTE : __arg32_1 - message; __n_1 - arg count; __ptr32_2 - list, __n_2 - argument list offset
inline % 0FBh

  mov  r8,  rbx
  mov  [rsp+8], r10                      // ; saving arg0
  lea  rax, [rsp + __n_2]
  mov  [rsp+16], r11                     // ; saving arg0

  mov  rsi, __ptr64_2
  xor  edx, edx
  mov  rbx, [rsi] // ; message from overload list

labNextOverloadlist:
  mov  r9, mdata : %0
  shr  ebx, ACTION_ORDER
  lea  r13, [rbx*8]
  mov  r13, [r9 + r13 * 2 + 8]
  mov  ecx, __n_1
  lea  rbx, [r13 - 8]

labNextParam:
  sub  ecx, 1
  jnz  short labMatching

  mov  r9, __ptr64_2
  lea  r13, [rdx * 8]
  mov  rbx, r8
  mov  rax, [r9 + r13 * 2 + 8] 
  mov  rdx, [r9 + r13 * 2] 
  mov  rcx, [rbx - elVMTOffset]
  jmp  [rcx + rax + 8]

labMatching:
  mov  rdi, [rax + rcx * 8]

  //; check nil
  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  test  rdi, rdi                                              
  cmovz rdi, rsi

  mov  rdi, [rdi - elVMTOffset]
  mov  rsi, [rbx + rcx * 8]

labNextBaseClass:
  cmp  rsi, rdi
  jz   labNextParam
  mov  rdi, [rdi - elPackageOffset]
  and  rdi, rdi
  jnz  short labNextBaseClass

  add  rdx, 1
  mov  r13, __ptr64_2
  lea  r9, [rdx * 8]
  mov  rbx, [r13 + r9 * 2] // ; message from overload list
  and  rbx, rbx
  jnz  labNextOverloadlist

end

// ; vcallmr
inline % 0FCh

  mov  rax, [rbx - elVMTOffset]
  call [rax + __arg32_1]

end

// ; callmr
inline %0FDh

  call __relptr32_2

end

// ; callext
inline %0FEh

  mov  rcx, r10
  mov  rdx, r11
  mov  r8, [rsp+16]
  mov  r9, [rsp+24]
  call extern __relptr32_1
  mov  rdx, rax

end
