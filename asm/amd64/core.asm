// --- Predefined References  --

define INVOKER              10011h

define CORE_GC_TABLE        20002h
define CORE_STATICROOT      20005h
define CORE_TLS_INDEX       20007h
define THREAD_TABLE         20008h
define SYSTEM_ENV           2000Ch

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

  dq 0
  dq data : %CORE_STATICROOT
  dq data : %CORE_GC_TABLE
  dq data : %CORE_TLS_INDEX
  dq data : %THREAD_TABLE
  dq code : %INVOKER

end

// ; ==== Command Set ==

// ; close
inline % 15h

  mov  rsp, rbp
  pop  rbp
  
end

// ; loadenv
inline % 2Ah

  mov  rdx, rdata : %SYSTEM_ENV

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

// ; restore
inline % 92h

  add  rbp, __arg1
  
end

// ; open
inline % 98h

  push rbp
  mov  rbp, rsp

end

// ; callextr
inline % 0A5h

  call extern __arg1
  mov  rdx, rax

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
  mov  rcx, __arg1
  xor  rax, rax
  mov  rdi, rsp
  rep  stos

end
