// --- Predefined References  --

// --- Predefined References  --
define GC_ALLOC	            10001h
define HOOK                 10010h
define INVOKER              10011h
define INIT_RND             10012h
define ENDFRAME             10016h
define RESTORE_ET           10017h
define CALC_SIZE            1001Fh
define GET_COUNT            10020h
define THREAD_WAIT          10021h
define BREAK                10026h
define EXPAND_HEAP          10028h

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
define elVMTOffset           000Ch 
define elObjectOffset        000Ch

// VMT header fields
define elVMTSizeOffset       0004h
define elVMTFlagOffset       0010h
define elPackageOffset       0018h

define gc_header             0000h
define gc_start              0008h
define gc_et_current         0058h 
define gc_stack_frame        0060h 

define page_size_order          4h
define struct_mask_inv     7FFFFFh

define ACTION_ORDER              9
define ARG_ACTION_MASK        1DFh
define ACTION_MASK            1E0h
define ARG_MASK               01Fh

// --- System Core Preloaded Routines --

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
  dd 0 // ; gc_rootcount          : +88h 

end

// ; NOTE : the table is tailed with GCMGSize,GCYGSize and MaxThread fields
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
  dd 0  // ; a reference to the super class class
  dq 0
  dd 0  
  dd 0

end

rstructure %VOIDPTR

  dq rdata : %VOID + elPackageOffset
  dd 0

end

// --- GC_ALLOC ---
// in: ecx - size ; out: ebx - created object
procedure %GC_ALLOC

//  mov  eax, [data : %CORE_GC_TABLE + gc_yg_current]
//  mov  edx, [data : %CORE_GC_TABLE + gc_yg_end]
//  add  ecx, eax
//  cmp  ecx, edx
//  jae  short labYGCollect
//  mov  [data : %CORE_GC_TABLE + gc_yg_current], ecx
//  lea  ebx, [eax + elObjectOffset]
  ret

//labYGCollect:
  // ; restore ecx
//  sub  ecx, eax

  // ; save registers
//  push ebp

  // ; lock frame
//  mov  [data : %CORE_GC_TABLE + gc_stack_frame], esp

//  push ecx
  
  // ; create set of roots
//  mov  ebp, esp
//  xor  ecx, ecx
//  push ecx        // ; reserve place 
//  push ecx
//  push ecx

//  // ; save static roots
//  mov  esi, data : %CORE_STATICROOT
//  mov  ecx, [data : %CORE_GC_TABLE + gc_rootcount]
//  push esi
//  push ecx

//  // ; collect frames
//  mov  eax, [data : %CORE_GC_TABLE + gc_stack_frame]  
//  mov  ecx, eax

//labYGNextFrame:
//  mov  esi, eax
//  mov  eax, [esi]
//  test eax, eax
//  jnz  short labYGNextFrame
  
//  push ecx
 // sub  ecx, esi
 // neg  ecx
 // push ecx  
  
//  mov  eax, [esi + 4]
//  test eax, eax
//  mov  ecx, eax
//  jnz  short labYGNextFrame

  // === Minor collection ===
//  mov [ebp-4], esp      // ; save position for roots

  // ; save mg -> yg roots 
//  mov  ebx, [data : %CORE_GC_TABLE + gc_mg_current]
//  mov  edi, [data : %CORE_GC_TABLE + gc_mg_start]
//  sub  ebx, edi                                        // ; we need to check only MG region
//  jz   labWBEnd                                        // ; skip if it is zero
//  mov  esi, [data : %CORE_GC_TABLE + gc_mg_wbar]
//  shr  ebx, page_size_order
//  // ; lea  edi, [edi + elObjectOffset]

//labWBNext:
//  cmp  [esi], 0
//  lea  esi, [esi+4]
//  jnz  short labWBMark
//  sub  ebx, 4
//  ja   short labWBNext
//  nop
//  nop
//  jmp  short labWBEnd

//labWBMark:
//  lea  eax, [esi-4]
//  sub  eax, [data : %CORE_GC_TABLE + gc_mg_wbar]
//  mov  edx, [esi-4]
//  shl  eax, page_size_order
//  lea  eax, [edi + eax + elObjectOffset]
  
//  test edx, 0FFh
//  jz   short labWBMark2
//  mov  ecx, [eax-elSizeOffset]
//  push eax
//  and  ecx, 0FFFFFh
//  push ecx

//labWBMark2:
//  lea  eax, [eax + page_size]
//  test edx, 0FF00h
//  jz   short labWBMark3
//  mov  ecx, [eax-elSizeOffset]
//  push eax
//  and  ecx, 0FFFFFh
//  push ecx

//labWBMark3:
//  lea  eax, [eax + page_size]
//  test edx, 0FF0000h
//  jz   short labWBMark4
//  mov  ecx, [eax-elSizeOffset]
//  push eax
//  and  ecx, 0FFFFFh
//  push ecx

//labWBMark4:
//  lea  eax, [eax + page_size]
//  test edx, 0FF000000h
//  jz   short labWBNext
//  mov  ecx, [eax-elSizeOffset]
//  push eax
//  and  ecx, 0FFFFFh
//  push ecx
//  jmp  short labWBNext
  
//labWBEnd:
//  mov  ebx, [ebp]
//  mov  eax, esp
//  push ebx
//  push eax
//  call extern 'rt_dlls.GCCollect

//  mov  ebx, eax

//  mov  esp, ebp 
//  pop  ecx 
//  pop  ebp

//  ret

end

// ; --- HOOK ---
// ; in: ecx - catch offset
procedure %HOOK

  //mov  eax, [esp]       
  //lea  ecx, [eax + ecx - 5]               
  // ; add  ecx, [esp]
  // ; sub  ecx, 5             // ; call command size should be excluded
  ret

end

// --- System Core Functions --

procedure % ENDFRAME

//  // ; save return pointer
//  pop  ecx  
  
//  xor  edx, edx
//  lea  esp, [esp+8]
//  pop  ebp

//  pop  eax
//  mov  fs:[0], eax
//  lea  esp, [esp+4]

//  // ; restore return pointer
//  push ecx   
  ret

end

procedure % THREAD_WAIT

  ret
  
end

procedure % CALC_SIZE

//  mov  ecx, edx
//  add  ecx, page_ceil
//  and  ecx, page_mask

  ret

end

procedure % GET_COUNT

//  mov  edx, [ebx - elSizeOffset]
//  test edx, 0800000h
//  jnz  short labErr
//  and  edx, 0FFFFFFh
//  shr  edx, 2
//  ret

//labErr:
//  xor  edx, edx
//  ret 

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
  mov  rbx, [rax+rdx*4] 

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
  mov  rsi, [rdi - elVMTSizeOffset]
  xor  rcx, rcx

labSplit:
  test rsi, rsi
  jz   short labEnd

labStart:
  mov   r9, rdi
  lea   r8, [rsi*8]
  shr   rsi, 1
  lea   r8, [r8*2]
  setnc cl
  cmp   rdx, [r9+r8*2]
  je    short labFound
  lea   rax, [r9+r8*2]
  jb    short labSplit
  lea   rdi, [rax+16]
  sub   rsi, rcx
  jmp   labSplit
  nop
  nop
labFound:
  jmp   [rdi+rsi*8+4]

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

// ; close
inline % 15h

  mov  rsp, rbp
  pop  rbp
  
end

// ; swapd
inline % 14h

  mov  rax, [rsp]
  mov  [rsp], rdx
  mov  rdx, rax 

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

   mov  rbx, [rbx + rdx * 4]

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
  lea  rsp, [rsp + rcx * 4]
  jmp  rsi
 
end

// ; count
inline % 1Ch

  mov  ecx, 0FFFFFh
  mov  rdx, [rbx-elSizeOffset]
  and  rdx, rcx
  shr  rdx, 2

end

// ; unhook
inline % 1Dh

  mov  rsi, [data : %CORE_GC_TABLE + gc_et_current]
  mov  rsp, [rsi + 4]
  mov  rbp, [rsi + 8]
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

// ; rarctan
inline % 21h

  mov   rax, [rsp]
  fld   qword ptr [rax]  
  fld1
  fpatan                  // i.e. arctan(Src/1)
  fstp  qword ptr [rbx]    // store result 

end

// ; include
inline % 25h

  add  rsp, 8

end

// ; exclude
inline % 26h
                                                       
  push rbp     
  mov  [data : %CORE_GC_TABLE + gc_stack_frame], rsp

end

// ; freelock
inline % 28h

  nop

end

// ; loadenv
inline % 2Ah

  mov  rdx, rdata : %SYSTEM_ENV

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
   mov  [rbx + rdx * 4], rax

end

// ; len
inline % 31h

  mov  edx, 0FFFFFh
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

// ; equal
inline % 3Eh

  mov  rax, [rsp]
  xor  rdx, rdx
  cmp  eax, ebx
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

// ; rsave
inline % 46h

  fstp qword ptr [rbx]

end

// ; save
inline % 47h

  mov [rbx], rdx

end

// ; load
inline % 48h

  mov rdx, [rbx]

end

// ; rsaven
inline % 49h

  fistp dword ptr [rbx]

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

// ; xsave
inline % 5Ah

  lea rax, [rbx+__arg1]
  mov dword ptr [rax], edx

end

// ; div
inline %05Bh

  mov  eax, edx
  mov  ecx, __arg1
  xor  edx, edx
  idiv ecx
  mov  edx, eax

end

// ; nshlf
inline % 5Eh

  mov eax, [rbp+__arg1]
  mov ecx, dword ptr [rbx]
  shl eax, cl
  mov dword ptr [rbp+__arg1], eax

end

// ; nshrf
inline % 5Fh

  mov eax, [rbp+__arg1]
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

// ; xseti
inline %97h

  mov  rax, [rsp]                   
  mov [rbx + __arg1], rax

end

// ; open
inline % 98h

  push rbp
  mov  rbp, rsp

end

// ; callvi (ecx - offset to VMT entry)
inline % 0A2h

  mov  rax, [rbx - elVMTOffset]
  call [rax + __arg1]

end

// ; movn
inline % 0B1h

  mov  edx, __arg1

end

// ; pushai
inline % 0B4h

  push [rbx+__arg1]

end

// ; loadf
inline % 0B5h

  mov  edx, dword ptr [rbp + __arg1]

end

// ; loadfi
inline % 0B7h

  mov  edx, dword ptr [rbp + __arg1]

end

// ; savef
inline % 0B9h

  mov  dword ptr [rbp + __arg1], edx

end

// ; savesi
inline % 0BBh

  mov  dword ptr [rsp + __arg1], edx

end

// ; pushs
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

// ; nsubf
inline % 0C8h

  mov  ecx, dword ptr [rbx]
  sub  dword ptr [rbp+__arg1], ecx

end

// ; ndivf
inline % 0C9h

  mov  eax, dword ptr [rbp+__arg1]
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

// ; inc
inline %0D6h

  add  rdx, __arg1

end

// ; pushf
inline % 0BDh

  lea  rax, [rbp + __arg1]
  push rax

end

// ; mtredirect (__arg3 - number of parameters, eax - points to the stack arg list)
inline % 0E8h

  mov  rsi, __arg1
  mov  r8,  rbx
  xor  edx, edx
  mov  rbx, [rsi] // ; message from overload list

labNextOverloadlist:
  lea  r9, [rsi*8]
  mov  r10, rdata : % CORE_MESSAGE_TABLE
  shr  ebx, ACTION_ORDER
  lea  r9,  [r10 + r9*2]
  mov  r9, [r9 + 4]
  mov  rcx, __arg3
  lea  rbx, [r10 + r9 - 4]

labNextParam:
  sub  ecx, 1
  jnz  short labMatching

  lea  r9, [rdx*8]
  mov  r10, __arg1
  mov  rbx, r8
  mov  rax, [r10 + r9 * 2 + 4]
  mov  rcx, [rbx - elVMTOffset]
  lea  rax, [rax*8]
  mov  rdx, [r10 + r9 * 2]
  jmp  [rcx + rax * 2 + 4]

labMatching:
  mov  rdi, [rax + rcx * 4]

  //; check nil
  mov   rsi, rdata : %VOIDPTR + 4
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

  mov  rsi, __arg1
  add  rdx, 1
  mov  rbx, [rsi + rdx * 8] // ; message from overload list
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
  shr  ebx, ACTION_ORDER
  mov  r10, rdata : % CORE_MESSAGE_TABLE
  lea  r9, [ebx * 8]
  mov  rcx, __arg3
  mov  r9, [r10 + r9 * 2 + 8]
  lea  rbx, [r10 + r9 - 4]

labNextParam:
  sub  ecx, 1
  jnz  short labMatching

  mov  r9, __arg1
  lea  r10, [rdx * 8]
  mov  rbx, r8
  mov  rdx, [r9 + r10 * 2]
  jmp  [r9 + r10 * 2 + 8]

labMatching:
  mov  rdi, [rax + rcx * 4]

  //; check nil
  mov   rsi, rdata : %VOIDPTR + 4
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

  mov  rsi, __arg1
  add  rdx, 1
  mov  rbx, [rsi + rdx * 8] // ; message from overload list
  and  rbx, rbx
  jnz  labNextOverloadlist

end

// ; mtredirect<1>
inline % 1E8h

  mov  rcx, __arg1
  xor  edx, edx
  mov  rax, [rax + 4]
  mov  rcx, [rcx] // ; message from overload list

  //; check nil
  mov   rsi, rdata : %VOIDPTR + 4
  test  rax, rax
  cmovz rax, rsi

  mov  rax, [rax - elVMTOffset]

labNextOverloadlist:
  mov  r9, rdata : % CORE_MESSAGE_TABLE
  shr  ecx, ACTION_ORDER
  lea  r10, [ecx*8]
  mov  r10, [r9 + r10 * 2 + 8]
  lea  rcx, [r9 + r10]

labMatching:
  mov  rdi, rax
  mov  rsi, [rcx]

labNextBaseClass:
  cmp  rsi, rdi
  jnz  short labContinue

  lea  r9, [rdx*2]  
  mov  r10, __arg1
  mov  rax, [r10 + r9 * 2 + 8]
  mov  rcx, [rbx - elVMTOffset]
  mov  rdx, [r10 + r9 * 2]
  jmp  [rcx + rax * 8 + 4]

labContinue:
  mov  rdi, [rdi - elPackageOffset]
  and  rdi, rdi
  jnz  short labNextBaseClass

  mov  rcx, __arg1
  add  rdx, 1
  mov  rcx, [rcx + rdx * 8] // ; message from overload list
  and  rcx, rcx
  jnz  labNextOverloadlist

labEnd:

end

// ; xmtredirect<1>
inline % 1E9h

  mov  rcx, __arg1
  mov  rax, [rax + 4]
  xor  edx, edx
  mov  rcx, [rcx] // ; message from overload list

  //; check nil
  mov   rsi, rdata : %VOIDPTR + 4
  test  rax, rax
  cmovz rax, rsi

  mov  rax, [rax - elVMTOffset]

labNextOverloadlist:
  shr  rcx, ACTION_ORDER
  mov  rdi, rdata : % CORE_MESSAGE_TABLE
  mov  rcx, [rdi + rcx * 8 + 4]
  lea  rcx, [rdi + rcx]

labMatching:
  mov  rdi, rax
  mov  rsi, [rcx]

labNextBaseClass:
  cmp  rsi, rdi
  jnz  short labContinue

  mov  rcx, rdx
  mov  rsi, __arg1
  mov  rdx, [rsi + rdx * 8]
  jmp  [rsi + rcx * 8 + 4]

labContinue:
  mov  rdi, [rdi - elPackageOffset]
  and  rdi, rdi
  jnz  short labNextBaseClass

  mov  rcx, __arg1
  add  rdx, 1
  mov  rcx, [rcx + rdx * 8] // ; message from overload list
  and  rcx, rcx
  jnz  labNextOverloadlist

labEnd:

end

// ; mtredirect<2> (eax - refer to the stack)
inline % 2E8h 

  mov  rcx, __arg1
  xor  edx, edx
  mov  rcx, [rcx] // ; message from overload list

labNextOverloadlist:
  mov  rdi, rdata : % CORE_MESSAGE_TABLE
  shr  rcx, ACTION_ORDER
  mov  rcx, [rdi + rcx * 8 + 4]
  lea  rcx, [rdi + rcx]

labMatching:
  mov  rdi, [rax+4]

  //; check nil
  mov   rsi, rdata : %VOIDPTR + 4
  test  rdi, rdi
  cmovz rdi, rsi

  mov  rdi, [rdi-elVMTOffset]
  mov  rsi, [rcx]

labNextBaseClass:
  cmp  rsi, rdi
  jnz  labContinue

  mov  rdi, [rax+8]

  //; check nil
  mov   rsi, rdata : %VOIDPTR + 4
  test  rdi, rdi
  cmovz rdi, rsi

  mov  rdi, [rdi-4]
  mov  rsi, [rcx + 4]

labNextBaseClass2:
  cmp  rsi, rdi
  jnz  short labContinue2

  mov  rsi, __arg1
  mov  rax, [rsi + rdx * 8 + 4]
  mov  rcx, [rbx - elVMTOffset]
  mov  rdx, [rsi + rdx * 8]
  jmp  [rcx + rax * 8 + 4]

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
  mov  rcx, __arg1
  add  rdx, 1
  mov  rcx, [rcx + rdx * 8] // ; message from overload list
  and  rcx, rcx
  jnz  labNextOverloadlist

end

// ; xmtredirect<2>  (eax - refer to the stack)
inline % 2E9h

// ecx -> eax
// ebx -> ecx

  mov  rcx, __arg1
  xor  rdx, rdx
  mov  rcx, [rcx + rdx * 8] // ; message from overload list

labNextOverloadlist:
  mov  rdi, rdata : % CORE_MESSAGE_TABLE
  shr  rcx, ACTION_ORDER
  mov  rcx, [rdi + rcx * 8 + 4]
  lea  rcx, [rdi + rcx]

labMatching:
  mov  rdi, [rax+4]

  //; check nil
  mov   rsi, rdata : %VOIDPTR + 4
  test  rdi, rdi
  cmovz rdi, rsi

  mov  rdi, [rdi-elVMTOffset]
  mov  rsi, [rcx]

labNextBaseClass:
  cmp  rsi, rdi
  jnz  labContinue

  mov  rdi, [rax+8]

  //; check nil
  mov   rsi, rdata : %VOIDPTR + 4
  test  rdi, rdi
  cmovz rdi, rsi

  mov  rdi, [rdi-elVMTOffset]
  mov  rsi, [rcx + 4]

labNextBaseClass2:
  cmp  rsi, rdi
  jnz  short labContinue2

  mov  rsi, __arg1
  mov  rcx, rdx
  mov  rdx, [rsi + rcx * 8]
  jmp  [rsi + rcx * 8 + 4]

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
  mov  rcx, __arg1
  add  rdx, 1
  mov  rcx, [rcx + rdx * 8] // ; message from overload list
  and  rcx, rcx
  jnz  labNextOverloadlist

end

// ; mtredirect<12> (__arg3 - number of parameters, eax - points to the stack arg list)

inline % 0CE8h

  push rbx
  xor  rdx, rdx
  mov  rbx, rax
  xor  rcx, rcx

labCountParam:
  lea  rbx, [rbx+4]
  cmp  [rbx], -1
  lea  rcx, [rcx+1]
  jnz  short labCountParam

  mov  rsi, __arg1
  push rcx
  mov  rbx, [rsi] // ; message from overload list

labNextOverloadlist:
  mov  rdi, rdata : % CORE_MESSAGE_TABLE
  shr  rbx, ACTION_ORDER
  mov  rcx, [rsp]              // ; param count
  mov  rbx, [rdi + rbx * 8 + 4]
  lea  rbx, [rdi + rbx - 4]

labNextParam:
  // ; check if signature contains the next class ptr
  lea  rsi, [rbx + 4]
  cmp [rsi], 0
  cmovnz rbx, rsi

  sub  rcx, 1
  jnz  short labMatching

  mov  rsi, __arg1
  lea  rsp, [rsp + 4]
  pop  rbx
  mov  rax, [rsi + rdx * 8 + 4]
  mov  rcx, [rbx - elVMTOffset]
  mov  rdx, [rsi + rdx * 8]
  jmp  [rcx + rax * 8 + 4]

labMatching:
  mov  rsi, [rsp]
  sub  rsi, rcx
  mov  rdi, [rax + rsi * 4]

  //; check nil
  mov   rsi, rdata : %VOIDPTR + 4
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

  mov  rsi, __arg1
  add  rdx, 1
  mov  rbx, [rsi + rdx * 8] // ; message from overload list
  and  rbx, rbx
  jnz  labNextOverloadlist

  lea  rsp, [rsp + 4]
  pop  rax

end

// ; xmtredirect<12>

inline % 0CE9h

  push rbx
  xor  rdx, rdx
  mov  rbx, rax
  xor  rcx, rcx

labCountParam:
  lea  rbx, [rbx+4]
  cmp  [rbx], -1
  lea  rcx, [rcx+1]
  jnz  short labCountParam

  mov  rsi, __arg1
  push rcx
  mov  rbx, [rsi] // ; message from overload list

labNextOverloadlist:
  mov  rdi, rdata : % CORE_MESSAGE_TABLE
  shr  rbx, ACTION_ORDER
  mov  rcx, [rsp]              // ; param count
  mov  rbx, [rdi + rbx * 8 + 4]
  lea  rbx, [rdi + rbx - 4]

labNextParam:
  // ; check if signature contains the next class ptr
  lea  rsi, [rbx + 4]
  cmp [rsi], 0
  cmovnz rbx, rsi

  sub  rcx, 1
  jnz  short labMatching

  mov  rsi, __arg1
  mov  rcx, rdx
  lea  rsp, [rsp + 4]
  pop  rbx
  mov  rdx, [rsi + rcx * 8]
  jmp  [rsi + rcx * 8 + 4]

labMatching:
  mov  rsi, [rsp]
  sub  rsi, rcx
  mov  rdi, [rax + rsi * 4]

  //; check nil
  mov   rsi, rdata : %VOIDPTR + 4
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

  mov  rsi, __arg1
  add  rdx, 1
  mov  rbx, [rsi + rdx * 8] // ; message from overload list
  and  rbx, rbx
  jnz  labNextOverloadlist

  lea  rsp, [rsp + 4]
  pop  rax

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
  call [rax + rcx + 4]
  
end

// ; selectr (ebx - r1, __arg1 - r2)
inline % 0F6h

  mov    rcx, __arg1
  test   rdx, rdx
  cmovnz rbx, rcx

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
  sub  rbp, 18h
  call extern __arg1
  mov  rdx, rax

end

// ; callextr
inline % 2FFh

  mov  rcx, [rsp]
  mov  rdx, [rsp+8]
  sub  rbp, 10h
  call extern __arg1
  mov  rdx, rax

end

// ; callextr
inline % 3FFh

  mov  rcx, [rsp]
  mov  rdx, [rsp+8]
  mov  r8, [rsp+16]
  sub  rbp, 08h
  call extern __arg1
  mov  rdx, rax

end
