// ; --- Predefined References  --
define INVOKER              10001h
define GC_ALLOC	            10002h
define VEH_HANDLER          10003h
define GC_COLLECT	    10004h
define GC_ALLOCPERM	    10005h

define CORE_TOC             20001h
define SYSTEM_ENV           20002h
define CORE_GC_TABLE   	    20003h
define CORE_TLS_INDEX       20004h
define CORE_SINGLE_CONTENT  2000Bh
define VOID           	    2000Dh
define VOIDPTR              2000Eh
define CORE_THREAD_TABLE    2000Fh

// ; THREAD CONTENT
define et_current            0004h
define tt_stack_frame        0008h

define es_prev_struct        0000h
define es_catch_addr         0004h
define es_catch_level        0008h
define es_catch_frame        000Ch

// ; THREAD TABLE
define tt_slots             00004h

// ; NOTE : the table is tailed with GCMGSize,GCYGSize and MaxThread fields
structure %SYSTEM_ENV

  dd 0
  dd data : %CORE_GC_TABLE
  dd 0
  dd data : %CORE_THREAD_TABLE
  dd code : %INVOKER
  dd code : %VEH_HANDLER
  // ; dd GCMGSize
  // ; dd GCYGSize
  // ; dd ThreadCounter

end

structure %CORE_THREAD_TABLE

  dd 0 // ; tt_length              : +00h

  dd 0 // ; tt_slots               : +04h
  dd 0

end

// ; --- System Core Preloaded Routines --

// ; ==== Command Set ==

// ; unhook
inline %0Bh

  // ; GCXT: get current thread frame
  mov  eax, [data : %CORE_TLS_INDEX]
  mov  ecx, fs:[2Ch]
  mov  ecx, [ecx+eax*4]
  mov  edi, [ecx + et_current]

  mov  eax, [edi + es_prev_struct]
  mov  ebp, [edi + es_catch_frame]
  mov  esp, [edi + es_catch_level]

  mov  [ecx + et_current], eax

end

// ; system 3
inline %3CFh

  mov  eax, [data : %CORE_TLS_INDEX]
  mov  ecx, fs: [2Ch]
  mov  eax, [ecx + eax * 4]
  mov  edi, data : %CORE_THREAD_TABLE + tt_slots
  mov  [edi + edx * 8], eax

end

// ; xhookdpr
inline %0E6h

  // ; GCXT: get current thread frame
  mov  eax, [data : %CORE_TLS_INDEX]
  mov  ecx, fs:[2Ch]
  lea  edi, [ebp + __arg32_1]
  mov  eax, [ecx+eax*4]

  mov  ecx, [eax + et_current]
  mov  [edi + es_catch_frame], ebp
  mov  [edi + es_prev_struct], ecx
  mov  [edi + es_catch_level], esp
  mov  [edi + es_catch_addr], __ptr32_2

  mov  [eax + et_current], edi

end
