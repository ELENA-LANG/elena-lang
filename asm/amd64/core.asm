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

// ; loadenv
inline % 2Ah

  mov  rdx, rdata : %SYSTEM_ENV

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

// ; alloci
inline %0D1h

  // ; generated in jit : sub  esp, __arg1*4
  mov  rcx, __arg1
  xor  rax, rax
  mov  rdi, rsp
  rep  stos

end
