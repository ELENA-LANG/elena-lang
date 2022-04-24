// !! NOTE : R15 register must be preserved

// ; --- Predefined References  --
define INVOKER              10001h
define GC_ALLOC	            10002h

define CORE_TOC             20001h
define SYSTEM_ENV           20002h
define CORE_GC_TABLE        20003h
define VOID           	    2000Dh
define VOIDPTR              2000Eh

define ACTION_ORDER              9

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

// ; --- System Core Preloaded Routines --

structure % CORE_TOC

  dq 0         // ; reserved

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

  dq data : %CORE_GC_TABLE
  dq code : %INVOKER

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
  xor  rbx, rbx  // !! temporal stub
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

// ; movm
inline %88h

  mov  edx, __arg32_1

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

// ; peekfi
inline %0A8h

  mov  rbx, qword ptr [rbp + __arg32_1]

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
  mov  eax, __ptr32_2
  mov  [rbx - elVMTOffset], rax
  mov  dword ptr [rbx - elSizeOffset], ecx

end

// ; xstorefir
inline %0F9h

  mov  rax, __ptr64_2
  mov  [rbp+__arg32_1], rax

end

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
  mov  r13, [r9 + r13 * 2 + 8] 
  mov  rcx, [rbx - elVMTOffset]
  lea  rax, [r13 * 8]
  mov  rdx, [r9 + r13 * 2]
  jmp  [rcx + rax * 2 + 8]

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
  mov  r13, __ptr32_2
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
