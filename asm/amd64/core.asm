
// !! NOTE : R15 register must be preserved

// --- Predefined References  --
define GC_ALLOC	            10001h
define HOOK                 10010h
define INVOKER              10011h
define INIT_RND             10012h
define CALC_SIZE            1001Fh
define GET_COUNT            10020h
define THREAD_WAIT          10021h
define GC_ALLOCPERM	    10031h

define CORE_GC_TABLE        20002h
define CORE_STATICROOT      20005h
define CORE_TLS_INDEX       20007h
define THREAD_TABLE         20008h
define CORE_MESSAGE_TABLE   2000Ah
define CORE_ET_TABLE        2000Bh
define SYSTEM_ENV           2000Ch
define VOID           	    2000Dh
define VOIDPTR              2000Eh

// Object header fields
define elSizeOffset          0004h
define elVMTOffset           0010h 
define elObjectOffset        0010h

// VMT header fields
define elVMTSizeOffset       0008h
define elVMTFlagOffset       0018h
define elPackageOffset       0020h

define gc_header             0000h
define gc_start              0008h
define gc_end                0048h

define gc_yg_current         0018h
define gc_yg_end             0020h
define gc_mg_start           0038h
define gc_mg_current         0040h
define gc_mg_wbar            0050h
define gc_et_current         0058h 
define gc_stack_frame        0060h 
define gc_rootcount          0088h
define gc_perm_start         0090h 
define gc_perm_end           0098h 
define gc_perm_current       00A0h 

define page_size               20h
define page_ceil               2Fh
define page_size_order          5h
define page_mask        0FFFFFFE0h
define struct_mask_inv   3FFFFFFFh
define stuct_mask        40000000h

define ACTION_ORDER              9
define ARG_ACTION_MASK        1DFh
define ACTION_MASK            1E0h
define ARG_MASK               01Fh

// --- System Core Preloaded Routines --

structure % CORE_ET_TABLE

  dq 0 // ; critical_exception    ; +x00   - pointer to critical exception handler

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
  dq 0 // ; gc_et_current         : +58h 
  dq 0 // ; gc_stack_frame        : +60h 
  dd 0 // ; gc_lock               : +68h 
  dd 0 // ; gc_signal             : +6Ch 
  dd 0 // ; tt_ptr                : +70h 
  dd 0 // ; tt_lock               : +74h 
  dq 0 // ; dbg_ptr               : +78h 
  dq 0 // ; gc_roots              : +80h 
  dq 0 // ; gc_rootcount          : +88h 
  dq 0 // ; gc_perm_start         : +90h 
  dq 0 // ; gc_perm_end           : +98h 
  dq 0 // ; gc_perm_current       : +A0h 

end

// ; NOTE : the table is tailed with GCMGSize,GCYGSize,GCPERMSize and MaxThread fields
rstructure %SYSTEM_ENV

  dd 0
  dq data : %CORE_STATICROOT
  dq data : %CORE_GC_TABLE
  dq data : %CORE_TLS_INDEX
  dq data : %THREAD_TABLE
  dq code : %INVOKER

end

rstructure %VOID

  dq 0
  dq 0  // ; a reference to the super class class
  dq 0
  dq 0  
  dq 0

end

rstructure %VOIDPTR

  dq rdata : %VOID + elPackageOffset
  dq 0
  dq 0

end

// --- GC_ALLOC ---
// in: ecx - size ; out: ebx - created object
procedure %GC_ALLOC

  mov  rax, [data : %CORE_GC_TABLE + gc_yg_current]
  mov  rdx, [data : %CORE_GC_TABLE + gc_yg_end]
  add  rcx, rax
  cmp  rcx, rdx
  jae  short labYGCollect
  mov  [data : %CORE_GC_TABLE + gc_yg_current], rcx
  lea  rbx, [rax + elObjectOffset]
  ret

labYGCollect:
  // ; restore ecx
  sub  rcx, rax

  // ; save registers
  push rbp

  // ; lock frame
  mov  [data : %CORE_GC_TABLE + gc_stack_frame], rsp

  push rcx
  
  // ; create set of roots
  mov  rbp, rsp
  xor  ecx, ecx
  push rcx        // ; reserve place 
  push rcx
  push rcx

  // ; save static roots
  mov  rsi, data : %CORE_STATICROOT
  mov  rcx, [data : %CORE_GC_TABLE + gc_rootcount]
  push rsi
  push rcx

  // ; collect frames
  mov  rax, [data : %CORE_GC_TABLE + gc_stack_frame]  
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

  // ; restore rbp
  mov  rax, rbp
  mov  rbp, [rax+8]

  push rax
  sub  rsp, 20h
  call extern 'rt_dlls.GCCollect
  add  rsp, 20h

  mov  rbx, rax
  pop  rbp

  mov  rsp, rbp 
  pop  rcx 
  pop  rbp

  ret

end

// --- GC_ALLOCPERM ---
// in: ecx - size ; out: ebx - created object
procedure %GC_ALLOCPERM

  mov  rax, [data : %CORE_GC_TABLE + gc_perm_current]
  mov  rdx, [data : %CORE_GC_TABLE + gc_perm_end]
  add  rcx, rax
  cmp  rcx, rdx
  jae  short labPERMCollect
  mov  [data : %CORE_GC_TABLE + gc_perm_current], rcx
  lea  rbx, [rax + elObjectOffset]
  ret

labPERMCollect:
  // ; restore ecx
  sub  rcx, rax

  // ; lock frame
  mov  [data : %CORE_GC_TABLE + gc_stack_frame], rsp

  push rcx
  
  sub  rsp, 18h
  call extern 'rt_dlls.GCCollectPerm
  add  rsp, 20h

  mov  rbx, rax

  ret

end

// ; --- HOOK ---
// ; in: ecx - catch offset
procedure %HOOK

  mov  rax, [rsp]       
  lea  rcx, [rax + rcx - 5]               
  ret

end

// --- System Core Functions --

procedure % THREAD_WAIT

  ret
  
end

procedure % CALC_SIZE

  mov  ecx, edx
  add  ecx, page_ceil
  and  ecx, page_mask

  ret

end

procedure % GET_COUNT

  mov  edx, dword ptr [rbx - elSizeOffset]
  test edx, stuct_mask
  jnz  short labErr
  and  edx, struct_mask_inv
  shr  edx, 3
  ret

labErr:
  xor  edx, edx
  ret 

end

// ; ==== Command Set ==

// ; coalesce
inline % 2

  mov    rax, [rsp]
  test   rbx, rbx
  cmovz  rbx, rax

end

// ; peek
inline % 3

  mov  rax, [rsp]
  mov  rbx, [rax+rdx*8] 

end

// ; snop
inline % 4

  nop

end

// ; pushverb
inline % 5

  mov   rax, rdx
  mov   rcx, rdx
  shr   eax, ACTION_ORDER
  mov   rdi, rdata : % CORE_MESSAGE_TABLE
  test  rcx, rcx
  cmovs rax, [rdi + rdx]
  push  rax

end

// ; loadverb
inline % 6

  mov   rcx, rdx
  shr   rdx, ACTION_ORDER
  mov   rax, rdata : % CORE_MESSAGE_TABLE
  test  rcx, rcx
  cmovs rdx, [rax + rdx]

end

// ; throw
inline % 7

  mov  rax, [data : %CORE_GC_TABLE + gc_et_current]
  jmp  [rax]

end

// ; push
inline % 09h

   mov  rax, [rbx + rdx * 8]
   push rax

end

// ; xnew
inline % 0Ch

  mov  rax, [rsp]
  mov  ecx, page_ceil
  push rbx
  mov  rdx, [rax]
  lea  rcx, [rcx + rdx*8]
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  pop   rdi
  xor   edx, edx
  mov   rax, [rsp]
  mov   [rbx-elVMTOffset], rdi
  mov   rcx, [rax]
  mov   esi, stuct_mask
  test  ecx, ecx
  cmovz rdx, rsi
  shl   ecx, 3
  or    ecx, edx
  mov   dword ptr[rbx-elSizeOffset], ecx

end

// ; storev
inline % 0Dh

  mov eax, edx
  and eax, ACTION_MASK
  mov ecx, dword ptr [rsp]
  and ecx, ARG_MASK
  or  ecx, edx
  mov dword ptr [rsp], ecx

end

// ; bsredirect
inline % 0Eh // (rbx - object, rdx - message)

  mov  rdi, [rbx - elVMTOffset]
  xor  ecx, ecx
  mov  esi, dword ptr[rdi - elVMTSizeOffset]

labSplit:
  test esi, esi
  jz   short labEnd

labStart:
  shr   esi, 1
  lea   rax, [rsi*2]
  setnc cl
  cmp   rdx, [rdi+rax*8]
  je    short labFound
  lea   r8, [rdi+rax*8]
  jb    short labSplit
  lea   rdi, [r8+16]
  sub   esi, ecx
  jmp   labSplit
  nop
  nop
labFound:
  jmp   [rdi+rax*8+8]

labEnd:
                               
end

// ; setv
inline % 0Fh

  mov eax, dword ptr [rbx]
  and eax, ARG_ACTION_MASK
  mov ecx, edx
  shl ecx, ACTION_ORDER
  or  eax, ecx
  mov dword ptr [rbx], eax

end

// ; open
inline % 11h

  push rbp
  mov  rbp, rsp

end

// ; sub
inline % 13h

  sub  edx, dword ptr [rbx]
  
end

// ; swapd
inline % 14h

  mov  rax, [rsp]
  mov  [rsp], rdx
  mov  rdx, rax 

end

// ; close
inline % 15h

  mov  rsp, rbp
  pop  rbp
  
end

// ; rexp
inline % 16h

  mov   rax, [rsp]
  mov   rdx, 0
  fld   qword ptr [rax]   // ; Src

  fldl2e                  // ; ->log2(e)
  fmulp                   // ; ->log2(e)*Src
                                                              
  // ; the FPU can compute the antilog only with the mantissa
  // ; the characteristic of the logarithm must thus be removed
      
  fld   st(0)             // ; copy the logarithm
  frndint                 // ; keep only the characteristic
  fsub  st(1),st(0)       // ; keeps only the mantissa
  fxch                    // ; get the mantissa on top

  f2xm1                   // ; ->2^(mantissa)-1
  fld1
  faddp                   // ; add 1 back

  //; the number must now be readjusted for the characteristic of the logarithm

  fscale                  // ;, scale it with the characteristic
      
  fstsw ax                // ; retrieve exception flags from FPU
  shr   al,1              // ; test for invalid operation
  jc    short lErr        // ; clean-up and return if error
      
  // ; the characteristic is still on the FPU and must be removed
  
  fstp  st(1)             // ; get rid of the characteristic

  fstp  qword ptr [rbx]   // ; store result 
  mov   rdx, 1
  jmp   short labEnd
  
lErr:
  ffree st(1)
  
labEnd:

end

// ; get
inline % 18h

   mov  rbx, [rbx + rdx * 8]

end

// ; set
inline % 19h
                                
   // ; calculate write-barrier address
   mov  rcx, rbx
   mov  rsi, [data : %CORE_GC_TABLE + gc_header]
   sub  rcx, [data : %CORE_GC_TABLE + gc_start]
   mov  rax, [rsp]
   shr  ecx, page_size_order
   mov  [rbx + rdx*8], rax
   mov  byte ptr [rcx + rsi], 1  

end

// ; swap
inline % 1Ah

  mov rax, [rsp]
  mov [rsp], rbx
  mov rbx, rax 

end

// ; mquit
inline % 1Bh

  mov  rcx, rdx
  pop  rsi
  and  ecx, ARG_MASK
  lea  rsp, [rsp + rcx * 8]
  jmp  rsi
 
end

// ; count
inline % 1Ch

  mov  ecx, struct_mask_inv
  mov  rdx, [rbx-elSizeOffset]
  and  rdx, rcx
  shr  rdx, 3

end

// ; unhook
inline % 1Dh

  mov  rsi, [data : %CORE_GC_TABLE + gc_et_current]
  mov  rsp, [rsi + 8]
  mov  rbp, [rsi + 16]
  pop  rcx
  mov  [data : %CORE_GC_TABLE + gc_et_current], rcx
  
end

// ; rsin
inline % 1Eh

  mov   rax, [rsp]
  fld   qword ptr [rax]  
  fldpi
  fadd  st(0),st(0)       // ; ->2pi
  fxch

lReduce:
  fprem                   // ; reduce the angle
  fsin
  fstsw ax                // ; retrieve exception flags from FPU
  shr   al,1              // ; test for invalid operation
  // ; jc    short lErr        // ; clean-up and return error
  sahf                    // ; transfer to the CPU flags
  jpe   short lReduce     // ; reduce angle again if necessary
  fstp  st(1)             // ; get rid of the 2pi

  fstp  qword ptr [rbx]    // ; store result 

end

// ; allocd
inline % 1Fh

  mov  ecx, edx
  add  ecx, 1
  and  ecx, 0FFFFFFFEh
  mov  rdi,  rcx
  shl  rdi, 3
  sub  rsp, rdi
  xor  rax, rax
  mov  rdi, rsp
  rep  stos

end

// ; rcos
inline % 20h

  mov   rax, [rsp]
  fld   qword ptr [rax]  
  fcos
  fstp  qword ptr [rbx]    // store result 

end

// ; rarctan
inline % 21h

  mov   rax, [rsp]
  fld   qword ptr [rax]  
  fld1
  fpatan                  // i.e. arctan(Src/1)
  fstp  qword ptr [rbx]    // store result 

end

// ; xtrans
inline % 24h

  mov  rax, [rsp]
  mov  rcx, [rax+rdx*8]
  mov  [rbx+rdx*8], rcx                                                   

end

// ; include
inline % 25h

  add  rsp, 16

end

// ; exclude
inline % 26h
       
  push   0
  push   rbp   
  mov    [data : %CORE_GC_TABLE + gc_stack_frame], rsp

end

// ; trylock
inline % 27h

  xor  edx, edx

end

// ; freelock
inline % 28h

  nop

end


// ; freed
inline % 29h

  mov  rdi, rdx
  shl  rdi, 3
  add  rsp, rdi

end

// ; loadenv
inline % 2Ah

  mov  rdx, rdata : %SYSTEM_ENV

end

// ; store
inline % 2Bh

  mov  rax, [rsp]
  mov  [rax+rdx*8], rbx 

end

// ; rln
inline % 2Ch

  mov   rax, [rsp]
  mov   rdx, 0
  fld   qword ptr [rax]  
  
  fldln2
  fxch
  fyl2x                   // ->[log2(Src)]*ln(2) = ln(Src)

  fstsw ax                // retrieve exception flags from FPU
  shr   al,1              // test for invalid operation
  jc    short lErr        // clean-up and return error

  fstp  qword ptr [rbx]    // store result 
  mov   rdx, 1
  jmp   short labEnd

lErr:
  ffree st(0)

labEnd:

end

// ; read
inline % 2Dh

  mov  rdx, [rbx + rdx]

end

// ; clone
inline % 02Eh

  mov  ecx, dword ptr [rbx - elSizeOffset]
  mov  rsi, [rsp]
  and  ecx, struct_mask_inv
  mov  rdi, rbx
  add  ecx, 3
  shr  ecx, 2
  rep  movsd

end

// ; xset
inline % 2Fh
            
   mov  rax, [rsp]                  
   mov  [rbx + rdx * 8], rax

end

// ; rabs
inline %30h

  mov   rax, [rsp]
  fld   qword ptr [rax]  
  fabs
  fstp  qword ptr [rbx]    // ; store result 
  
end

// ; len
inline % 31h

  mov  edx, struct_mask_inv
  mov  rcx, [rbx-elSizeOffset]
  and  rdx, rcx

end

// ; rload
inline %32h

  fld   qword ptr [rbx]

end

// ; flag
inline % 33h

  mov  rax, [rbx - elVMTOffset]
  mov  edx, dword ptr [rax - elVMTFlagOffset]
  
end

// ; parent
inline % 35h

  mov rbx, [rbx - elPackageOffset]

end

// ; class
inline % 36h

  mov rbx, [rbx - elVMTOffset]

end

// ; mindex
inline % 37h

  mov  rcx, rdx
  mov  rdi, [rbx - elVMTOffset]
  xor  rdx, rdx
  mov  esi, dword ptr [rdi - elVMTSizeOffset]

labSplit:
  test esi, esi
  jz   short labEnd

labStart:
  mov   r9, rdi
  lea   r8, [rsi*8]
  shr   esi, 1
  setnc dl
  cmp   rdx, [r9+r8*2]
  jb    short labSplit
  nop
  nop
  jz    short labFound
  lea   rdi, [rdi+r8+16]
  lea   rdi, [r9+r8*2]
  sub   rsi, rdx
  jnz   short labStart
  nop
  nop
labEnd:
  mov  rdx, 0FFFFFFFFh
labFound:

end

// ; rround
inline %3Dh

  mov   rax, [rsp]
  mov   edx, 0
  fld   qword ptr [rax]  

  push  rax               // ; reserve space on CPU stack

  fstcw word ptr [rsp]    // ;get current control word
  mov   rax, [rsp]
  and   ax,0F3FFh         // ; code it for rounding 
  push  rax
  fldcw word ptr [rsp]    // ; change rounding code of FPU to round

  frndint                 // ; round the number
  pop   rax               // ; get rid of last push
  fldcw word ptr [rsp]    // ; load back the former control word

  fstsw ax                // ; retrieve exception flags from FPU
  shr   al,1              // ; test for invalid operation
  pop   rcx               // ; clean CPU stack
  jc    short lErr        // ; clean-up and return error
  
  fstp  qword ptr [rbx]   // ; store result 
  mov   edx, 1
  jmp   short labEnd
  
lErr:
  ffree st(0)

labEnd:
  
end

// ; equal
inline % 3Eh

  mov  rax, [rsp]
  xor  rdx, rdx
  cmp  rax, rbx
  setz dl

end

// ; nequal
inline % 40h

  mov  rax, [rsp]
  xor  edx, edx
  mov  ecx, dword ptr [rbx]
  cmp  ecx, dword ptr [rax]
  setz dl

end

// ; nless
inline % 41h

  mov  rax, [rsp]
  xor  edx, edx
  mov  ecx, dword ptr [rbx]
  cmp  ecx, dword ptr [rax]
  setl dl

end

// ; lequal

inline % 43h

  mov  rax, [rsp]
  xor  edx, edx
  mov  rcx, [rbx]
  cmp  rcx, [rax]
  setz dl

end

// ; lless(lo, ro, tr, fr)
inline % 44h

  mov  rax, [rsp]
  xor  rdx, rdx
  mov  rcx, [rbx]
  cmp  rcx, [rax]
  setl dl

end

// ; rset (src, tgt)
inline % 45h

  push rdx
  fild dword ptr [rsp]
  pop  rdx

end

// ; rsave
inline % 46h

  fstp qword ptr [rbx]

end

// ; save
inline % 47h

  mov dword ptr [rbx], edx

end

// ; load
inline % 48h

  mov edx, dword ptr [rbx]

end

// ; rsaven
inline % 49h

  fistp dword ptr [rbx]

end
               
// ; rsavel
inline % 4Ah

  fistp qword ptr [rbx]

end

// ; lsave
inline % 4Bh

  mov  [rbx], rdx

end

// ; lload
inline % 4Ch

  mov  eax, edx
  cdq
  mov  dword ptr [rbx+4], edx
  mov  dword ptr [rbx], eax
  mov  edx, eax

end

// ; rint
inline % 4Fh

  mov   rax, [rsp]
  mov   ecx, 0
  fld   qword ptr [rax]

  push  rcx                // reserve space on stack
  fstcw word ptr [rsp]     // get current control word
  mov   dx, word ptr [rsp]
  or    dx,0c00h           // code it for truncating
  push  rdx
  fldcw word ptr [rsp]    // change rounding code of FPU to truncate

  frndint                  // truncate the number
  pop   rdx                // remove modified CW from CPU stack
  fldcw word ptr [rsp]     // load back the former control word
  pop   rdx                // clean CPU stack
      
  fstsw ax                 // retrieve exception flags from FPU
  shr   al,1               // test for invalid operation
  jc    short labErr       // clean-up and return error

labSave:
  fstp  qword ptr [rbx]    // store result
  mov   ecx, 1
  jmp   short labEnd
  
labErr:
  ffree st(1)
  
labEnd:
  mov  edx, ecx

end

// ; addf
inline % 050h

  lea  rsi, [rbp+__arg1]
  add  dword ptr [rsi], edx

end

// ; subf
inline % 051h

  lea  rsi, [rbp+__arg1]
  sub  dword ptr [rsi], edx

end

// ; nxorf
inline % 52h

  mov  ecx, dword ptr [rbx]
  xor  dword ptr [rbp+__arg1], ecx

end

// ; norf
inline % 53h

  mov  ecx, dword ptr [rbx]
  or   dword ptr [rbp+__arg1], ecx

end

// ; nandf
inline % 54h

  mov  ecx, dword ptr [rbx]
  and  dword ptr [rbp+__arg1], ecx

end

// ; movfipd
inline % 55h

  lea  rcx, [rdx*8]
  lea  rbx, [rbp+__arg1]
  sub  rbx, rcx 

end

// ; xsave
inline % 5Ah

  lea rax, [rbx+__arg1]
  mov dword ptr [rax], edx

end

// ; div
inline %05Bh

  mov  eax, edx
  mov  ecx, __arg1
  cdq
  idiv ecx
  mov  edx, eax

end

// ; xwrite
inline % 5Ch

  mov  rsi, [rsp]
  mov  ecx, __arg1
  lea  rdi, [rbx+rdx]
  rep  movsb

end

// ; xwrite
inline % 15Ch

  mov  rcx, [rsp]
  lea  rdi, [rbx+rdx]
  mov  rax, [rcx]
  mov  byte ptr [rdi], al

end

// ; xwrite
inline % 25Ch

  mov  rcx, [rsp]
  lea  rdi, [rbx+rdx]
  mov  rax, [rcx]
  mov  word ptr [rdi], ax

end

// ; xwrite
inline % 35Ch

  mov  rcx, [rsp]
  lea  rdi, [rbx+rdx]
  mov  rax, [rcx]
  mov  dword ptr [rdi], eax

end

// ; xwrite
inline % 45Ch

  mov  rcx, [rsp]
  lea  rdi, [rbx+rdx]
  mov  rax, [rcx]
  mov  [rdi], rax

end

// ; copyto
inline % 5Dh

  lea rdi, [rbx+rdx]
  mov  ecx, __arg1
  mov rsi, [rsp]
  rep movsd

end

// ; copyto
inline % 15Dh

  mov rsi, [rsp]
  lea rdi, [rbx+rdx]
  mov rax, [rsi]
  mov dword ptr [rdi], eax

end

// ; copyto
inline % 25Dh

  mov rsi, [rsp]
  lea rdi, [rbx+rdx]
  mov rax, [rsi]
  mov [rdi], rax

end

// ; copyto
inline % 35Dh

  mov rsi, [rsp]
  lea rdi, [rbx+rdx]
  mov rax, [rsi]
  mov [rdi], rax
  mov rcx, [rsi+8]
  mov dword ptr [rdi+8], ecx

end

// ; copyto
inline % 45Dh

  mov rsi, [rsp]
  lea rdi, [rbx+rdx]
  mov rax, [rsi]
  mov [rdi], rax
  mov rcx, [rsi+8]
  mov [rdi+8], rcx

end

// ; nshlf
inline % 5Eh

  mov eax, dword ptr [rbp+__arg1]
  mov ecx, dword ptr [rbx]
  shl eax, cl
  mov dword ptr [rbp+__arg1], eax

end

// ; nshrf
inline % 5Fh

  mov eax, dword ptr [rbp+__arg1]
  mov ecx, dword ptr [rbx]
  shr eax, cl
  mov dword ptr [rbp+__arg1], eax

end

// ; mul
inline %060h

  mov  rax, rdx
  mov  ecx, __arg1
  imul ecx
  mov  rdx, rax

end

// ; checksi
inline % 061h

  mov    rdi, [rsp+__arg1]
  xor    rdx, rdx
  mov    rsi, [rbx-elVMTOffset]
labNext:
  mov    rax, 0
  cmp    rsi, rdi
  mov    rsi, [rsi - elPackageOffset]
  setz   dl
  cmovnz rax, rsi
  and    rax, rax
  jnz    short labNext

end

// ; xredirect 
inline % 062h

  lea  rbx, [rbx + __arg2] // ; NOTE use __arg2 due to current implementation
  push rbx
  push rdx 

  mov  rsi, [rbx]   // ; get next overload list
  test rsi, rsi
  jz   labEnd

labNextList:
  xor  edx, edx
  mov  ebx, dword ptr[rsi] // ; message from overload list

labNextOverloadlist:
  shr  ebx, ACTION_ORDER
  mov  rdi, rdata : % CORE_MESSAGE_TABLE
  mov  rcx, [rsp]
  mov  rbx, [rdi + rbx * 8 + 4]
  and  ecx, ARG_MASK
  lea  rbx, [rdi + rbx - 4]
  inc  ecx 

labNextParam:
  sub  ecx, 1
  jnz  short labMatching

  pop  rax
  pop  rsi
  mov  rcx, rdx
  mov  rsi, [rsi]
  mov  rbx, [rsp]
  mov  rdx, [rsi + rcx * 8]
  jmp  [rsi + rcx * 8 + 4]

labMatching:
  mov  rdi, [rax + rcx * 4]

  //; check nil
  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  test  rdi, rdi
  cmovz rdi, rsi

  mov  rdi, [rdi - elVMTOffset]
  mov  rsi, [rbx + rcx * 4]

labNextBaseClass:
  cmp  rsi, rdi
  jz   labNextParam
  mov  rdi, [rdi - elPackageOffset]
  and  rdi, rdi
  jnz  short labNextBaseClass

  mov  rsi, [rsp+4]
  add  rdx, 1
  mov  rsi, [rsi]
  mov  rbx, [rsi + rdx * 8] // ; message from overload list
  and  rbx, rbx
  jnz  labNextOverloadlist
  add  [rsp+4], 4
  mov  rsi, [rsp+4]
  mov  rdx, [rsi]
  test rdx, rdx
  jnz  labNextList

labEnd:
  pop  rdx
  pop  rbx

end

// ; xvredirect
inline % 063h

  lea  rbx, [rbx + __arg2] // ; NOTE use __arg2 due to current implementation
  push rbx
  push rdx 

  mov  rsi, [rbx]   // ; get next overload list
  test rsi, rsi
  jz   labEnd

labNextList:
  xor  rdx, rdx
  mov  rbx, rax
  xor  rcx, rcx

labCountParam:
  lea  rbx, [rbx+4]
  cmp  [rbx], -1
  lea  rcx, [rcx+1]
  jnz  short labCountParam

  push rcx
  mov  rbx, [rsi] // ; message from overload list

labNextOverloadlist:
  mov  rdi, rdata : % CORE_MESSAGE_TABLE
  shr  ebx, ACTION_ORDER
  mov  rcx, [rsp]              // ; param count
  mov  rbx, [rdi + rbx * 8 + 4]
  lea  rbx, [rdi + rbx - 4]

labNextParam:
  // ; check if signature contains the next class ptr
  lea  rsi, [rbx + 4]
  cmp [rsi], 0
  cmovnz rbx, rsi

  sub  ecx, 1
  jnz  short labMatching

  lea  rsp, [rsp + 8]
  pop  rax
  pop  rsi
  mov  rcx, rdx
  mov  rsi, [rsi]
  mov  rbx, [rsp]
  mov  rdx, [rsi + rcx * 8]
  jmp  [rsi + rcx * 8 + 4]

labMatching:
  mov  rsi, [rsp]
  sub  rsi, rcx
  mov  rdi, [rax + rsi * 4]

  //; check nil
  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  test  rdi, rdi
  cmovz rdi, rsi

  mov  rdi, [rdi - elVMTOffset]
  mov  rsi, [rbx]

labNextBaseClass:
  cmp  rsi, rdi
  jz   labNextParam
  mov  rdi, [rdi - elPackageOffset]
  and  rdi, rdi
  jnz  short labNextBaseClass

  mov  rsi, [rsp+8]
  add  rdx, 1
  mov  rsi, [rsi]
  mov  rbx, [rsi + rdx * 8] // ; message from overload list
  and  rbx, rbx
  jnz  labNextOverloadlist

  add  [rsp+8], 4
  mov  rsi, [rsp+8]
  mov  rdx, [rsi]
  test rdx, rdx
  jnz  labNextList

labEnd:
  lea  rsp, [rsp + 8]
  pop  rdx
  pop  rbx

end

// ; laddf
inline % 074h

  mov  rcx, [rbx]
  add  [rbp+__arg1], rcx

end

// ; lsubf
inline % 075h

  mov  rcx, [rbx]
  sub  [rbp+__arg1], rcx

end

// ; lmulf
inline % 076h

  mov  rax, qword ptr [rbp+__arg1]
  imul qword ptr [rbx]
  mov  qword ptr [rbp+__arg1], rax

end

// ; ldivf
inline % 077h

  mov  rax, qword ptr [rbp+__arg1]
  cqo
  idiv qword ptr [rbx]
  mov  [rbp+__arg1], rax

end

// ; landf
inline % 078h

  mov  rcx, qword ptr [rbx]
  and  qword ptr [rbp+__arg1], rcx

end

// ; lorf
inline % 079h

  mov  rcx, qword ptr [rbx]
  or   qword ptr [rbp+__arg1], rcx

end

// ; lxorf
inline % 07Ah

  mov  rcx, qword ptr [rbx]
  xor  qword ptr [rbp+__arg1], rcx

end

// ; lshlf
inline % 7Bh

  mov rax, [rbp+__arg1]
  mov ecx, dword ptr [rbx]
  shl rax, cl
  mov qword ptr [rbp+__arg1], rax

end

// ; lshrf
inline % 7Dh

  mov rax, [rbp+__arg1]
  mov ecx, dword ptr [rbx]
  shr rax, cl
  mov qword ptr [rbp+__arg1], rax

end

// ; raddnf
inline % 80h

  lea   rdi, [rbp+__arg1]
  fild  dword ptr [rbx]
  fadd  qword ptr [rdi] 
  fstp  qword ptr [rdi]

end

// ; rsubnf
inline % 81h

  lea   rdi, [rbp+__arg1]
  fld   qword ptr [rdi]
  fisub dword ptr [rbx] 
  fstp  qword ptr [rdi]

end

// ; rmulnf
inline % 82h

  lea   rdi, [rbp+__arg1]
  fld   qword ptr [rdi]
  fimul dword ptr [rbx] 
  fstp  qword ptr [rdi]

end

// ; requal
inline % 83h

  mov    rdi, [rsp]
  fld    qword ptr [rdi]
  fld    qword ptr [rbx]
  xor    edx, edx
  fcomip st, st(1)
  sete   dl
  fstp  st(0)

end

// ; rless(lo, ro, tr, fr)
inline % 84h

  mov    rdi, [rsp]
  fld    qword ptr [rdi]
  fld    qword ptr [rbx]
  xor    edx, edx
  fcomip st, st(1)
  setb   dl
  fstp  st(0)

end

// ; raddf
inline % 85h

  lea  rdi, [rbp+__arg1]
  fld  qword ptr [rbx]
  fadd qword ptr [rdi] 
  fstp qword ptr [rdi]

end

// ; rsubf
inline % 86h

  lea  rdi, [rbp+__arg1]
  fld  qword ptr [rdi]
  fsub qword ptr [rbx] 
  fstp qword ptr [rdi]

end

// ; rmulf
inline % 87h

  lea  rdi, [rbp+__arg1]
  fld  qword ptr [rdi]
  fmul qword ptr [rbx] 
  fstp qword ptr [rdi]

end

// ; rdivf
inline % 88h
                                                   
  lea  rdi, [rbp+__arg1]
  fld  qword ptr [rdi]
  fdiv qword ptr [rbx] 
  fstp qword ptr [rdi]

end

// ; rdivnf
inline % 89h
                                                   
  lea   rdi, [rbp+__arg1]
  fld   qword ptr [rdi]
  fidiv dword ptr [rbx] 
  fstp  qword ptr [rdi]

end

// ; rintf
inline % 8Eh

  lea  rdi, [rbp+__arg1]
  mov   rcx, 0
  fld   qword ptr [rbx]

  push  rcx                // reserve space on stack
  fstcw word ptr [rsp]     // get current control word
  mov   dx, word ptr [rsp]
  or    dx,0c00h           // code it for truncating
  push  rdx
  fldcw word ptr [rsp]    // change rounding code of FPU to truncate

  frndint                  // truncate the number
  pop   rdx                // remove modified CW from CPU stack
  fldcw word ptr [rsp]     // load back the former control word
  pop   rdx                // clean CPU stack
      
  fstsw ax                 // retrieve exception flags from FPU
  shr   al,1               // test for invalid operation
  jc    short labErr       // clean-up and return error

labSave:
  fstp  qword ptr [rdi]    // store result
  mov   rdx, 1
  jmp   short labEnd
  
labErr:
  ffree st(1)
  
labEnd:

end

// ; geti
inline % 91h

  mov  rbx, [rbx+__arg1]
  
end

// ; restore
inline % 92h

  add  rbp, __arg1
  
end

// ; peekfi
inline % 94h

  mov  rbx, [rbp+__arg1]

end

// ; peeksi
inline % 95h

  mov  rbx, [rsp+__arg1]

end

// ; ifheap - part of the command
inline % 96h

  xor    edx, edx
  mov    rax,[data : %CORE_GC_TABLE + gc_start]
  mov    esi, 1
  mov    rcx,[data : %CORE_GC_TABLE + gc_end]
  cmp    rbx, rax
  cmovl  rdx, rsi
  cmp    rbx, rcx
  cmovg  rdx, rsi
  and    rdx, rdx

end

// ; xseti
inline %97h

  mov  rax, [rsp]                   
  mov [rbx + __arg1], rax

end

// ; create
inline % 9Ah

  mov  rax, [rsp]
  mov  ecx, page_ceil
  mov  rdx, [rax]
  lea  rcx, [rcx + rdx*8]
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  mov   rax, [rsp]
  xor   edx, edx
  mov   [rbx-elVMTOffset], __arg1
  mov   rcx, [rax]
  mov   esi, stuct_mask
  test  ecx, ecx
  cmovz rdx, rsi
  shl   ecx, 3
  or    ecx, edx
  mov   dword ptr[rbx-elSizeOffset], ecx

end

// ; ajumpvi
inline % 0A1h

  mov  rax, [rbx - elVMTOffset]
  jmp  [rax + __arg1]

end

// ; callvi (ecx - offset to VMT entry)
inline % 0A2h

  mov  rax, [rbx - elVMTOffset]
  call [rax + __arg1]

end

// ; hook label (ecx - offset)
// ; NOTE : hook calling should be the first opcode
inline % 0A6h

  call code : %HOOK

  push [data : %CORE_GC_TABLE + gc_et_current]

  mov  rdx, rsp 
  push rbp
  push rdx
  push rcx

  mov  [data : %CORE_GC_TABLE + gc_et_current], rsp
  
end

// ; address label (ecx - offset)
inline % 0A7h

  call code : %HOOK
  mov  rdx, rcx

end

// ; calli
inline % 0A8h

  mov  rsi, [rbx + __arg1]
  call rsi

end

// ; ifcount
// ; - partial opcode
inline % 0AFh

  mov  ecx, struct_mask_inv
  mov  eax, dword ptr [rbx-elSizeOffset]
  and  rax, rcx
  shr  rax, 3
  cmp  rax, rdx

end

// ; movn
inline % 0B1h

  mov  edx, __arg1

end

// ; equalfi
inline % 0B3h

  mov  rax, [rbp+__arg1]
  xor  rdx, rdx
  cmp  rax, rbx
  setz dl

end

// ; pushai
inline % 0B4h

  push [rbx+__arg1]

end

// ; loadf
inline % 0B5h

  movsxd rdx, dword ptr [rbp + __arg1]

end

// ; loadfi
inline % 0B7h

  movsxd rdx, dword ptr [rbp + __arg1]

end

// ; dloadsi
inline % 0B8h

  movsxd rdx, dword ptr[rsp + __arg1]

end

// ; savef
inline % 0B9h

  mov  dword ptr [rbp + __arg1], edx

end

// ; savesi
inline % 0BBh

  mov  dword ptr [rsp + __arg1], edx

end

// ; savefi
inline % 0BCh

  mov  rax, [rbp + __arg1]
  mov  dword ptr[rax], edx

end

// ; pushf
inline % 0BDh

  lea  rax, [rbp + __arg1]
  push rax

end

// ; pushsip
inline % 0BEh

  lea  rax, [rsp + __arg1]
  push rax

end

// ; reserve
inline % 0BFh

  sub  rsp, __arg1
  push rbp
  push 0
  mov  rbp, rsp

end

// ; seti
inline %0C0h

  mov  rsi, rbx
  mov  rax, [rsp]                   
  // calculate write-barrier address
  sub  rsi, [data : %CORE_GC_TABLE + gc_start]
  mov  rcx, [data : %CORE_GC_TABLE + gc_header]
  shr  rsi, page_size_order
  mov  byte ptr [rsi + rcx], 1  

  mov [rbx + __arg1], rax

end

// ; storesi
inline % 0C3h

  mov  [rsp+__arg1], rbx

end

// ; storefi
inline % 0C4h

  mov  [rbp+__arg1], rbx

end

// ; naddf
inline % 0C5h

  mov  ecx, dword ptr [rbx]
  add  dword ptr [rbp+__arg1], ecx

end

// ; nmulf
inline % 0C6h

  mov  eax, dword ptr [rbp+__arg1]
  imul dword ptr [rbx]
  mov  dword ptr [rbp+__arg1], eax

end

// ; xsetr
inline % 0C7h
        
   mov    ecx, __arg1
   movsxd rax, ecx 
   mov    [rbx + rdx * 8], rax

end

// ; nsubf
inline % 0C8h

  mov  ecx, dword ptr [rbx]
  sub  dword ptr [rbp+__arg1], ecx

end

// ; ndivf
inline % 0C9h

  mov  eax, dword ptr [rbp+__arg1]
  cdq
  idiv dword ptr [rbx]
  mov  dword ptr [rbp+__arg1], eax

end

// ; loadi
inline % 0CAh

  mov  rdx, [rbx + __arg1]

end

// ; savei
inline % 0CBh

  mov  [rbx + __arg1], rdx

end

// ; xor
inline % 0CDh

  xor    edx, __arg1

end

// ; clonef
inline % 0CEh

  mov  rcx, [rbx - elSizeOffset]
  and  ecx, struct_mask_inv
  lea  rsi, [rbp+__arg1]
  shr  rcx, 2
  mov  rdi, rbx
  rep  movsd

end

// ; xload
inline % 0CFh

  mov  edx, dword ptr [rbx + __arg1]

end

// ; alloci
inline %0D1h

  // ; generated in jit : sub  esp, __arg1*4
  mov  ecx, __arg1
  xor  eax, eax
  mov  rdi, rsp
  rep  stos

end

// ; xcreate
inline % 0D2h

  mov   rax, [rsp]
  mov   edx, struct_mask_inv
  mov   ecx, dword ptr[rax-elSizeOffset]
  and   edx, ecx
  mov   ecx, page_ceil
  add   ecx, edx
  and   ecx, page_mask 
 
  call  code : %GC_ALLOC

  mov   rax, [rsp]
  mov   [rbx-elVMTOffset], __arg1
  mov   edx, struct_mask_inv
  mov   ecx, dword ptr[rax-elSizeOffset]
  and   edx, ecx
  mov   dword ptr[rbx-elSizeOffset], edx
  
end

// ; inc
inline %0D6h

  add  rdx, __arg1

end

// ; coalescer
inline % 0D8h

  mov    rax, __arg1
  test   rbx, rbx
  cmovz  rbx, rax

end

// ; vjumprm
inline % 0DBh

  mov  ecx, __arg1
  shl  ecx, 4
  mov  rax, [rbx - elVMTOffset]
  jmp  [rax + rcx + 8]

end

// ; xsaveai (__arg1 - index, __arg2 - n)
inline % 0DCh

  mov  rax, __arg2
  mov  [rbx + __arg1], rax

end

// ; copyai (__arg1 - index, __arg2 - n)
inline % 0DDh

  mov  ecx, __arg2	
  lea  rsi, [rbx + __arg1]
  mov  rdi, [rsp]
  rep  movsd

end

inline % 01DDh

  lea  rsi, [rbx + __arg1]
  mov  rdi, [rsp]
  mov  eax, dword ptr [rsi]
  mov  dword ptr [rdi], eax

end

inline % 02DDh

  lea  rsi, [rbx + __arg1]
  mov  rdi, [rsp]
  mov  rax, [rsi]
  mov  [rdi], rax

end

inline % 03DDh

  lea  rsi, [rbx + __arg1]
  mov  rdi, [rsp]
  mov  rax, [rsi]
  mov  [rdi], rax
  mov  ecx, dword ptr [rsi+8]
  mov  dword ptr [rdi+8], ecx

end

inline % 04DDh

  lea  rsi, [rbx + __arg1]
  mov  rdi, [rsp]
  mov  rax, [rsi]
  mov  [rdi], rax
  mov  rcx, [rsi+8]
  mov  [rdi+8], rcx

end

// ; move
inline % 0DEh

  lea rsi, [rbx+__arg1]
  mov ecx, __arg2
  mov rdi, [rsp]
  rep movsb

end

// ; move
inline % 01DEh

  lea rsi, [rbx+__arg1]
  mov rdi, [rsp]
  mov rax, [rsi]
  mov byte ptr [rdi], al

end

inline % 02DEh

  lea rsi, [rbx+__arg1]
  mov rdi, [rsp]
  mov rax, [rsi]
  mov word ptr [rdi], ax

end

inline % 03DEh

  lea rsi, [rbx+__arg1]
  mov rdi, [rsp]
  mov rax, [rsi]
  mov dword ptr [rdi], eax

end

inline % 04DEh

  lea rsi, [rbx+__arg1]
  mov rdi, [rsp]
  mov rax, [rsi]
  mov [rdi], rax

end

// ; moveto
inline % 0DFh

  lea rdi, [rbx+__arg1]
  mov ecx, __arg2
  mov rsi, [rsp]
  rep movsb

end

inline % 01DFh

  mov rsi, [rsp]
  lea rdi, [rbx+__arg1]
  mov rax, [rsi] 
  mov byte ptr [rdi], al

end

inline % 02DFh

  mov rsi, [rsp]
  lea rdi, [rbx+__arg1]
  mov rax, [rsi] 
  mov word ptr [rdi], ax

end

inline % 03DFh

  mov rsi, [rsp]
  lea rdi, [rbx+__arg1]
  mov rax, [rsi] 
  mov dword ptr[rdi], eax

end

inline % 04DFh

  mov rsi, [rsp]
  lea rdi, [rbx+__arg1]
  mov rax, [rsi] 
  mov [rdi], rax

end

// ; readtof (__arg1 - index, __arg2 - n)
inline % 0E0h

  mov  ecx, __arg2	
  lea  rdi, [rbp + __arg1]
  lea  rsi, [rbx+rdx]                                                    
  rep  movsd

end

// ; readtof (__arg1 - index, __arg2 - n)
inline % 1E0h

  mov  rcx, [rbx+rdx]
  lea  rdi, [rbp + __arg1]
  mov  dword ptr [rdi], ecx
end

// ; readtof (__arg1 - index, __arg2 - n)
inline % 2E0h

  mov  rcx, [rbx+rdx]
  lea  rdi, [rbp + __arg1]
  mov  [rdi], rcx

end

// ; readtof (__arg1 - index, __arg2 - n)
inline % 3E0h

  mov  rcx, [rbx+rdx]
  lea  rdi, [rbp + __arg1]
  mov  [rdi], rcx
  mov  rax, [rbx+rdx+8]
  mov  dword ptr [rdi+8], eax

end

// ; readtof (__arg1 - index, __arg2 - n)
inline % 4E0h

  mov  rcx, [rbx+rdx]
  lea  rdi, [rbp + __arg1]
  mov  [rdi], rcx
  mov  rax, [rbx+rdx+8]
  mov  [rdi+8], rax

end

// ; createn (__arg1 - item size)
inline % 0E1h                                                                  

  mov  rax, [rsp]
  mov  ecx, page_ceil
  mov  rax, [rax]
  mov  ebx, __arg1
  imul ebx
  add  ecx, eax
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  mov   rax, [rsp]
  mov   ecx, stuct_mask
  mov   rax, [rax]
  mov   esi, __arg1
  imul  esi
  or    ecx, eax
  mov   dword ptr [rbx-elSizeOffset], ecx
  
end

// ; createn (__arg1 = 1)
inline % 1E1h

  mov  rax, [rsp]
  mov  ecx, page_ceil
  add  ecx, dword ptr[rax]
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  mov   rax, [rsp]
  mov   ecx, stuct_mask
  or    ecx, dword ptr[rax]
  mov   dword ptr[rbx-elSizeOffset], ecx
  
end

// ; createn (__arg1 = 2)
inline % 2E1h

  mov  rax, [rsp]
  mov  ecx, page_ceil
  mov  eax, dword ptr [rax]
  shl  eax, 1
  add  ecx, eax
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  mov   rax, [rsp]
  mov   ecx, stuct_mask
  mov   rax, [rax]
  shl   eax, 1
  or    ecx, eax
  mov   dword ptr[rbx-elSizeOffset], ecx
  
end

// ; createn (__arg1 = 4)
inline % 3E1h

  mov  rax, [rsp]
  mov  ecx, page_ceil
  mov  eax, dword ptr [rax]
  shl  eax, 2
  add  ecx, eax
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  mov   rax, [rsp]
  mov   ecx, stuct_mask
  mov   rax, [rax]
  shl   eax, 2
  or    ecx, eax
  mov   dword ptr[rbx-elSizeOffset], ecx
  
end

// ; createn (__arg1 = 8)
inline % 4E1h

  mov  rax, [rsp]
  mov  rcx, page_ceil
  mov  eax, dword ptr [rax]
  shl  eax, 3
  add  ecx, eax
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  mov   rax, [rsp]
  mov   rcx, stuct_mask
  mov   rax, [rax]
  shl  eax, 3
  or    ecx, eax
  mov   dword ptr[rbx-elSizeOffset], ecx
  
end

// ; xsetfi (__arg1 - index, __arg2 - index)
inline % 0E2h

  mov  rax, [rbp + __arg1]
  mov  [rbx + __arg2], rax

end

// ; copytoai (__arg1 - index, __arg2 - n)
inline % 0E3h

  mov  ecx, __arg2	
  lea  rdi, [rbx + __arg1]
  mov  rsi, [rsp]
  rep  movsd

end

inline % 01E3h

  mov  rsi, [rsp]
  lea  rdi, [rbx + __arg1]
  mov  rax, [rsi]
  mov  dword ptr[rdi], eax

end

inline % 02E3h

  mov  rsi, [rsp]
  lea  rdi, [rbx + __arg1]
  mov  rax, [rsi]
  mov  [rdi], rax

end

inline % 03E3h

  mov  rsi, [rsp]
  lea  rdi, [rbx + __arg1]
  mov  rax, [rsi]
  mov  [rdi], rax
  mov  rcx, [rsi+8]
  mov  dword ptr[rdi+8], ecx

end

inline % 04E3h

  mov  rsi, [rsp]
  lea  rdi, [rbx + __arg1]
  mov  rax, [rsi]
  mov  [rdi], rax
  mov  rcx, [rsi+8]
  mov  [rdi+8], rcx

end

// ; copytofi (__arg1 - index, __arg2 - n)
inline % 0E4h

  mov  ecx, __arg2	
  mov  rdi, [rbp + __arg1]
  mov  rsi, rbx
  rep  movsd

end

inline % 1E4h

  mov  rdi, [rbp + __arg1]
  mov  rax, [rbx]
  mov  dword ptr[rdi], eax

end

inline % 2E4h

  mov  rdi, [rbp + __arg1]
  mov  rax, [rbx]
  mov  [rdi], rax

end

inline % 3E4h

  mov  rdi, [rbp + __arg1]
  mov  rax, [rbx]
  mov  rcx, [rbx+8]
  mov  [rdi], rax
  mov  dword ptr[rdi+8], ecx

end

inline % 4E4h

  mov  rdi, [rbp + __arg1]
  mov  rax, [rbx]
  mov  rcx, [rbx+8]
  mov  [rdi], rax
  mov  [rdi+8], rsi

end

// ; copytof (__arg1 - index, __arg2 - n)
inline % 0E5h

  mov  ecx, __arg2	
  lea  rdi, [rbp + __arg1]
  mov  rsi, rbx
  rep  movsd

end

// ; copytof (__arg1 - index, __arg2 - n)
inline % 1E5h

  lea  rdi, [rbp + __arg1]
  mov  rax, [rbx]
  mov  dword ptr[rdi], eax

end

// ; copytof (__arg1 - index, __arg2 - n)
inline % 2E5h

  lea  rdi, [rbp + __arg1]
  mov  rax, [rbx]
  mov  [rdi], rax

end

// ; copytof (__arg1 - index, __arg2 - n)
inline % 3E5h

  lea  rdi, [rbp + __arg1]
  mov  rax, [rbx]
  mov  [rdi], rax
  mov  rcx, [rbx+8]
  mov  dword ptr[rdi+8], ecx

end

// ; copytof (__arg1 - index, __arg2 - n)
inline % 4E5h

  lea  rdi, [rbp + __arg1]
  mov  rax, [rbx]
  mov  [rdi], rax
  mov  rcx, [rbx+8]
  mov  [rdi+8], rcx

end

// ; copyfi (__arg1 - index, __arg2 - n)
inline % 0E6h

  mov  ecx, __arg2	
  mov  rsi, [rbp + __arg1]
  mov  rdi, rbx
  rep  movsd

end

inline % 01E6h

  mov  rsi, [rbp + __arg1]
  mov  rax, [rsi]
  mov  dword ptr[rbx], eax

end

inline % 02E6h

  mov  rsi, [rbp + __arg1]
  mov  rax, [rsi]
  mov  [rbx], rax

end

inline % 03E6h

  mov  rsi, [rbp + __arg1]
  mov  rax, [rsi]
  mov  [rbx], rax
  mov  rcx, [rsi+8]
  mov  dword ptr [rbx+8], ecx

end

inline % 04E6h

  mov  rsi, [rbp + __arg1]
  mov  rax, [rsi]
  mov  [rbx], rax
  mov  rcx, [rsi+8]
  mov  [rbx+8], rcx

end

// ; copyf (__arg1 - index, __arg2 - n)
inline % 0E7h

  mov  ecx, __arg2	
  lea  rsi, [rbp + __arg1]
  mov  rdi, rbx
  rep  movsd

end

inline % 01E7h

  lea  rsi, [rbp + __arg1]
  mov  rax, [rsi]
  mov  dword ptr[rbx], eax

end

inline % 02E7h

  lea  rsi, [rbp + __arg1]
  mov  rax, [rsi]
  mov  [rbx], rax

end

inline % 03E7h

  lea  rsi, [rbp + __arg1]
  mov  rax, [rsi]
  mov  [rbx], rax
  mov  rcx, [rsi+8]
  mov  dword ptr[rbx+8], ecx

end

inline % 04E7h

  lea  rsi, [rbp + __arg1]
  mov  rax, [rsi]
  mov  [rbx], rax
  mov  rcx, [rsi+8]
  mov  [rbx+8], rcx

end

// ; mtredirect (__arg3 - number of parameters, eax - points to the stack arg list)
inline % 0E8h

  mov  rsi, __arg1
  mov  r8,  rbx
  xor  edx, edx
  mov  rbx, [rsi] // ; message from overload list

labNextOverloadlist:
  mov  r9, rdata : % CORE_MESSAGE_TABLE
  shr  ebx, ACTION_ORDER
  lea  r10, [rbx*8]
  mov  r10, [r9 + r10 * 2 + 8]
  mov  ecx, __arg3
  lea  rbx, [r9 + r10 - 8]

labNextParam:
  sub  ecx, 1
  jnz  short labMatching

  mov  r9, __arg1
  lea  r10, [rdx * 8]
  mov  rbx, r8
  mov  r10, [r9 + r10 * 2 + 8] 
  mov  rcx, [rbx - elVMTOffset]
  lea  rax, [r10 * 8]
  mov  rdx, [r9 + r10 * 2]
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
  mov  r10, __arg1
  lea  r9, [rdx * 8]
  mov  rbx, [r10 + r9 * 2] // ; message from overload list
  and  rbx, rbx
  jnz  labNextOverloadlist

end

// ; xmtredirect (__arg3 - number of parameters, eax - points to the stack arg list)
inline % 0E9h

  mov  rsi, __arg1
  mov  r8, rbx
  xor  edx, edx
  mov  rbx, [rsi] // ; message from overload list
          	
labNextOverloadlist:
  mov  r9, rdata : % CORE_MESSAGE_TABLE
  shr  ebx, ACTION_ORDER
  lea  r10, [rbx*8]
  mov  r10, [r9 + r10 * 2 + 8]
  mov  ecx, __arg3
  lea  rbx, [r9 + r10 - 8]

labNextParam:
  sub  ecx, 1
  jnz  short labMatching

  mov  r9, __arg1
  lea  r10, [rdx * 8]
  mov  rbx, r8
  mov  rdx, [r9 + r10 * 2]
  jmp  [r9 + r10 * 2 + 8]

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
  mov  r10, __arg1
  lea  r9, [rdx * 8]
  mov  rbx, [r10 + r9 * 2] // ; message from overload list
  and  rbx, rbx
  jnz  labNextOverloadlist

end

// ; mtredirect<1>
inline % 1E8h

  mov  rcx, __arg1
  xor  edx, edx
  mov  rax, [rax + 8]
  mov  rcx, [rcx] // ; message from overload list

  //; check nil
  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  test  rax, rax
  cmovz rax, rsi

  mov  rax, [rax - elVMTOffset]

labNextOverloadlist:
  mov  r9, rdata : % CORE_MESSAGE_TABLE
  shr  ecx, ACTION_ORDER
  lea  r10, [rcx*8]
  mov  r10, [r9 + r10 * 2 + 8]
  lea  rcx, [r9 + r10]

labMatching:
  mov  rdi, rax
  mov  rsi, [rcx]

labNextBaseClass:
  cmp  rsi, rdi
  jnz  short labContinue

  lea  r9, [rdx*8]  
  mov  r10, __arg1
  mov  rax, [r10 + r9 * 2 + 8]
  mov  rcx, [rbx - elVMTOffset]
  shl  rax, 1
  mov  rdx, [r10 + r9 * 2]
  jmp  [rcx + rax * 8 + 8]

labContinue:
  mov  rdi, [rdi - elPackageOffset]
  and  rdi, rdi
  jnz  short labNextBaseClass

  add  rdx, 1
  mov  r10, __arg1
  lea  r9, [rdx * 8]
  mov  rcx, [r10 + r9 * 2] // ; message from overload list
  and  rcx, rcx
  jnz  labNextOverloadlist

labEnd:

end

// ; xmtredirect<1>
inline % 1E9h

  mov  rcx, __arg1
  xor  edx, edx
  mov  rax, [rax + 8]
  mov  rcx, [rcx] // ; message from overload list

  //; check nil
  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  test  rax, rax
  cmovz rax, rsi

  mov  rax, [rax - elVMTOffset]

labNextOverloadlist:
  mov  r9, rdata : % CORE_MESSAGE_TABLE
  shr  ecx, ACTION_ORDER
  lea  r10, [rcx*8]
  mov  r10, [r9 + r10 * 2 + 8]
  lea  rcx, [r9 + r10]

labMatching:
  mov  rdi, rax
  mov  rsi, [rcx]

labNextBaseClass:
  cmp  rsi, rdi
  jnz  short labContinue

  lea  r9, [rdx*8]  
  mov  r10, __arg1
  mov  rdx, [r10 + r9 * 2]
  jmp  [r10 + r9 * 2 + 8]

labContinue:
  mov  rdi, [rdi - elPackageOffset]
  and  rdi, rdi
  jnz  short labNextBaseClass

  add  rdx, 1
  mov  r10, __arg1
  lea  r9, [rdx * 8]
  mov  rcx, [r10 + r9 * 2] // ; message from overload list
  and  rcx, rcx
  jnz  labNextOverloadlist

labEnd:

end

// ; mtredirect<2> (eax - refer to the stack)
inline % 2E8h 

  mov  rcx, __arg1
  xor  rdx, rdx
  mov  rcx, [rcx] // ; message from overload list

labNextOverloadlist:
  mov  r9, rdata : % CORE_MESSAGE_TABLE
  shr  ecx, ACTION_ORDER
  lea  r10, [rcx*8]
  mov  r10, [r9 + r10 * 2 + 8]
  lea  rcx, [r9 + r10]

labMatching:
  mov  rdi, [rax+8]

  //; check nil
  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  test  rdi, rdi
  cmovz rdi, rsi

  mov  rdi, [rdi-elVMTOffset]
  mov  rsi, [rcx]

labNextBaseClass:
  cmp  rsi, rdi
  jnz  labContinue

  mov  rdi, [rax+16]

  //; check nil
  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  test  rdi, rdi
  cmovz rdi, rsi

  mov  rdi, [rdi-elVMTOffset]
  mov  rsi, [rcx + 8]

labNextBaseClass2:
  cmp  rsi, rdi
  jnz  short labContinue2

  lea  r9, [rdx*8]  
  mov  r10, __arg1
  mov  rax, [r10 + r9 * 2 + 8]
  mov  rcx, [rbx - elVMTOffset]
  lea  rax, [rax * 8]
  mov  rdx, [r10 + r9 * 2]
  jmp  [rcx + rax * 2 + 8]

labContinue2:
  mov  rdi, [rdi - elPackageOffset]
  and  rdi, rdi
  jnz  short labNextBaseClass2
  nop
  nop
  jmp short labNext

labContinue:
  mov  rdi, [rdi - elPackageOffset]
  and  rdi, rdi
  jnz  short labNextBaseClass

labNext:
  add  rdx, 1
  mov  r10, __arg1
  lea  r9, [rdx * 8]
  mov  rcx, [r10 + r9 * 2] // ; message from overload list
  and  rcx, rcx
  jnz  labNextOverloadlist

end

// ; xmtredirect<2>  (eax - refer to the stack)
inline % 2E9h

  mov  rcx, __arg1
  xor  rdx, rdx
  mov  rcx, [rcx] // ; message from overload list

labNextOverloadlist:
  mov  r9, rdata : % CORE_MESSAGE_TABLE
  shr  ecx, ACTION_ORDER
  lea  r10, [rcx*8]
  mov  r10, [r9 + r10 * 2 + 8]
  lea  rcx, [r9 + r10]

labMatching:
  mov  rdi, [rax+8]

  //; check nil
  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  test  rdi, rdi
  cmovz rdi, rsi

  mov  rdi, [rdi-elVMTOffset]
  mov  rsi, [rcx]

labNextBaseClass:
  cmp  rsi, rdi
  jnz  labContinue

  mov  rdi, [rax+16]

  //; check nil
  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  test  rdi, rdi
  cmovz rdi, rsi

  mov  rdi, [rdi-elVMTOffset]
  mov  rsi, [rcx + 8]

labNextBaseClass2:
  cmp  rsi, rdi
  jnz  short labContinue2

  lea  r9, [rdx*8]  
  mov  r10, __arg1
  mov  rdx, [r10 + r9 * 2]
  jmp  [r10 + r9 * 2 + 8]

labContinue2:
  mov  rdi, [rdi - elPackageOffset]
  and  rdi, rdi
  jnz  short labNextBaseClass2
  nop
  nop
  jmp short labNext

labContinue:
  mov  rdi, [rdi - elPackageOffset]
  and  rdi, rdi
  jnz  short labNextBaseClass

labNext:
  add  rdx, 1
  mov  r10, __arg1
  lea  r9, [rdx * 8]
  mov  rcx, [r10 + r9 * 2] // ; message from overload list
  and  rcx, rcx
  jnz  labNextOverloadlist

end

// ; mtredirect<12> (__arg3 - number of parameters, eax - points to the stack arg list)
inline % 0CE8h

  mov  r8, rbx
  xor  rdx, rdx
  mov  rbx, rax
  xor  rcx, rcx

labCountParam:
  lea  rbx, [rbx+8]
  cmp  qword ptr [rbx], -1
  lea  rcx, [rcx+1]
  jnz  short labCountParam

  mov  rsi, __arg1
  mov  r11, rcx
  mov  rbx, [rsi] // ; message from overload list

labNextOverloadlist:
  mov  r9, rdata : % CORE_MESSAGE_TABLE
  shr  rbx, ACTION_ORDER
  mov  rcx, r11              // ; param count
  lea  r10, [rbx*8]
  mov  r10, [r9 + r10 * 2 + 8]
  lea  rbx, [r9 + r10 - 8]

labNextParam:
  // ; check if signature contains the next class ptr
  lea  rsi, [rbx + 8]
  cmp [rsi], 0
  cmovnz rbx, rsi

  sub  rcx, 1
  jnz  short labMatching

  mov  r9, __arg1
  lea  r10, [rdx * 8]
  mov  rbx, r8
  mov  r10, [r9 + r10 * 2 + 8] 
  mov  rcx, [rbx - elVMTOffset]
  lea  rax, [r10 * 8]
  mov  rdx, [r9 + r10 * 2]
  jmp  [rcx + rax * 2 + 8]

labMatching:
  mov  rsi, r11
  sub  rsi, rcx
  mov  rdi, [rax + rsi * 8]

  //; check nil
  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  test  rdi, rdi
  cmovz rdi, rsi

  mov  rdi, [rdi - elVMTOffset]
  mov  rsi, [rbx]

labNextBaseClass:
  cmp  rsi, rdi
  jz   labNextParam
  mov  rdi, [rdi - elPackageOffset]
  and  rdi, rdi
  jnz  short labNextBaseClass

  add  rdx, 1
  mov  r10, __arg1
  lea  r9, [rdx * 8]
  mov  rbx, [r10 + r9 * 2] // ; message from overload list
  and  rbx, rbx
  jnz  labNextOverloadlist
  mov  rbx, r8

end

// ; xmtredirect<12>
inline % 0CE9h

  mov  r8, rbx
  xor  rdx, rdx
  mov  rbx, rax
  xor  rcx, rcx

labCountParam:
  lea  rbx, [rbx+8]
  cmp  qword ptr [rbx], -1
  lea  rcx, [rcx+1]
  jnz  short labCountParam

  mov  rsi, __arg1
  mov  r11, rcx
  mov  rbx, [rsi] // ; message from overload list

labNextOverloadlist:
  mov  r9, rdata : % CORE_MESSAGE_TABLE
  shr  rbx, ACTION_ORDER
  mov  rcx, r11              // ; param count
  lea  r10, [rbx*8]
  mov  r10, [r9 + r10 * 2 + 8]
  lea  rbx, [r9 + r10 - 8]

labNextParam:
  // ; check if signature contains the next class ptr
  lea  rsi, [rbx + 8]
  cmp [rsi], 0
  cmovnz rbx, rsi

  sub  rcx, 1
  jnz  short labMatching

  mov  r9, __arg1
  lea  r10, [rdx * 8]
  mov  rbx, r8
  mov  rdx, [r9 + r10 * 2]
  jmp  [r9 + r10 * 2 + 8]

labMatching:
  mov  rsi, r11
  sub  rsi, rcx
  mov  rdi, [rax + rsi * 8]

  //; check nil
  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  test  rdi, rdi
  cmovz rdi, rsi

  mov  rdi, [rdi - elVMTOffset]
  mov  rsi, [rbx]

labNextBaseClass:
  cmp  rsi, rdi
  jz   labNextParam
  mov  rdi, [rdi - elPackageOffset]
  and  rdi, rdi
  jnz  short labNextBaseClass

  add  rdx, 1
  mov  r10, __arg1
  lea  r9, [rdx * 8]
  mov  rbx, [r10 + r9 * 2] // ; message from overload list
  and  rbx, rbx
  jnz  labNextOverloadlist
  mov  rbx, r8

end

// ; xrsavef (__arg1 - index, __arg2 - n)
inline % 0EDh

  push  __arg2
  fild  dword ptr [rsp]
  lea   rdi, [rbp+__arg1]
  fstp  qword ptr [rdi]
  lea   rsp, [rsp+8]

end

// ; xaddf (__arg1 - index, __arg2 - n)
inline % 0EEh

  add dword ptr [rbp + __arg1], __arg2

end

// ; xsavef (__arg1 - index, __arg2 - n)
inline % 0EFh

  mov dword ptr [rbp + __arg1], __arg2

end

// ; new (__arg1 - size)
inline % 0F0h
	
  mov  ecx, __arg1
  call code : %GC_ALLOC

end

// ; fillr (__arg1 - r)
inline % 09Bh
  mov  rsi, [rsp]	
  mov  rax, __arg1	
  mov  rdi, rbx
  mov  ecx, dword ptr[rsi]
  rep  stos

end

// ; fillri (__arg1 - count)
inline % 0F2h

  mov  rdi, rbx
  mov  ecx, __arg1
  rep  stos

end

// ; xselectr (eax - r1, __arg1 - r2)
inline % 0F3h

  test   rbx, rbx
  mov    rbx, __arg1
  cmovnz rbx, rax

end

// ; vcallrm
inline % 0F4h

  mov  ecx, __arg1
  mov  rax, [rbx - elVMTOffset]
  shl  ecx, 4
  call [rax + rcx + 8]
  
end

// ; jumprm
inline % 0F5h

  cmp [rbx], rbx
  jmp __arg1

end

// ; selectr (ebx - r1, __arg1 - r2)
inline % 0F6h

  mov    rcx, __arg1
  test   rdx, rdx
  cmovnz rbx, rcx

end

// ; allocn (__arg1 - size)
inline % 0F8h
	
  mov  ecx, __arg1
  call code : %GC_ALLOCPERM

end

// ; xsavesi (__arg1 - index, __arg2 - n)
inline % 0F9h

  mov  eax, __arg2
  mov  [rsp + __arg1], rax

end

// ; xsavesi (__arg1 - index, __arg2 - n)
inline % 01F9h

  mov    eax, __arg2
  movsxd rax, eax 
  mov    [rsp + __arg1], rax

end

// callrm (edx contains message, __arg1 contains vmtentry)
inline % 0FEh

   call code : __arg1

end

// ; callextr
inline % 0FFh

  mov  rcx, [rsp]
  mov  rdx, [rsp+8]
  mov  r8, [rsp+16]
  mov  r9, [rsp+24]
  call extern __arg1
  mov  rdx, rax

end

// ; callextr
inline % 1FFh

  mov  rcx, [rsp]
  sub  rsp, 20h
  call extern __arg1
  mov  rdx, rax

end

// ; callextr
inline % 2FFh

  mov  rcx, [rsp]
  mov  rdx, [rsp+8]
  sub  rsp, 10h
  call extern __arg1
  mov  rdx, rax

end

// ; callextr
inline % 3FFh

  mov  rcx, [rsp]
  mov  rdx, [rsp+8]
  mov  r8,  [rsp+16]
  sub  rsp, 10h
  call extern __arg1
  mov  rdx, rax

end
