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

// ; snop
inline % 4

  nop

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
  shr   rsi, 1
  setnc cl
  cmp   rdx, [rdi+rsi*8]
  je    short labFound
  lea   rax, [rdi+rsi*8]
  jb    short labSplit
  lea   rdi, [rax+8]
  sub   rsi, rcx
  jmp   labSplit
  nop
  nop
labFound:
  jmp   [rdi+rsi*8+4]

labEnd:
                                                                
end

// ; close
inline % 15h

  mov  rsp, rbp
  pop  rbp
  
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

// ; class
inline % 36h

  mov rbx, [rbx - elVMTOffset]

end

// ; equal
inline % 3Eh

  mov  rax, [rsp]
  xor  rdx, rdx
  cmp  eax, ebx
  setz dl

end

// ; nless
inline % 41h

  mov  rax, [rsp]
  xor  rdx, rdx
  mov  rcx, [rbx]
  cmp  rcx, [rax]
  setl dl

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

// ; open
inline % 98h

  push rbp
  mov  rbp, rsp

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

// ; alloci
inline %0D1h

  // ; generated in jit : sub  esp, __arg1*4
  mov  ecx, __arg1
  xor  eax, eax
  mov  rdi, rsp
  rep  stos

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
