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

// ; --- GC TABLE OFFSETS ---
define gc_header             0000h
define gc_start              0004h
define gc_yg_start           0008h
define gc_yg_current         000Ch
define gc_yg_end             0010h
define gc_shadow             0014h
define gc_shadow_end         0018h
define gc_mg_start           001Ch
define gc_mg_current         0020h
define gc_end                0024h
define gc_mg_wbar            0028h
define gc_perm_start         002Ch 
define gc_perm_end           0030h 
define gc_perm_current       0034h 
define gc_lock               0038h 
define gc_signal             0040h 

// ; THREAD CONTENT
define et_current            0004h
define tt_stack_frame        0008h
define tt_sync_event         000Ch
define tt_flags              0010h

define es_prev_struct        0000h
define es_catch_addr         0004h
define es_catch_level        0008h
define es_catch_frame        000Ch

// ; THREAD TABLE
define tt_slots             00004h

// ; --- Object header fields ---
define elSizeOffset          0004h
define elVMTOffset           0008h 
define elObjectOffset        0008h

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

// ; --- GC_ALLOC ---
// ; in: ecx - size ; out: ebx - created object
inline % GC_ALLOC

  // ; GCXT: set lock
labStart:
  mov  edi, data : %CORE_GC_TABLE + gc_lock

labWait:
  mov edx, 1
  xor eax, eax
  lock cmpxchg dword ptr[edi], edx
  jnz  short labWait

  mov  eax, [data : %CORE_GC_TABLE + gc_yg_current]
  add  ecx, eax
  cmp  ecx, [data : %CORE_GC_TABLE + gc_yg_end]
  jae  short labYGCollect
  mov  [data : %CORE_GC_TABLE + gc_yg_current], ecx

  // ; GCXT: clear sync field
  mov  edx, 0FFFFFFFFh
  lea  ebx, [eax + elObjectOffset]
  
  // ; GCXT: free lock
  // ; could we use mov [esi], 0 instead?
  lock xadd [esi], edx

  ret

labYGCollect:
  // ; save registers
  sub  ecx, eax

  // ; GCXT: find the current thread entry
  mov  edx, fs:[2Ch]
  mov  eax, [data : %CORE_TLS_INDEX]

  push esi

  // ; GCXT: find the current thread entry
  mov  eax, [edx+eax*4]

  push ebp

  // ; GCXT: lock frame
  // ; get current thread event
  mov  esi, [eax + tt_sync_event]
  mov  [eax + tt_stack_frame], esp

  push ecx

  // ; === GCXT: safe point ===
  mov  edx, [data : %CORE_GC_TABLE + gc_signal]
  // ; if it is a collecting thread, starts the GC
  test edx, edx                       
  jz   short labConinue
  // ; otherwise eax contains the collecting thread event

  // ; signal the collecting thread that it is stopped
  push edx
  push esi
  call extern "$rt.SignalStopGCLA"
  add  esp, 4

  // ; free lock
  // ; could we use mov [esi], 0 instead?
  mov  edi, data : %CORE_GC_TABLE + gc_lock
  mov  ebx, 0FFFFFFFFh
  lock xadd [edi], ebx

  // ; stop until GC is ended
  call extern "$rt.WaitForSignalGCLA"
  add  esp, 4

  // ; restore registers and try again
  pop  ecx
  pop  ebp
  pop  esi

  jmp  labStart

labConinue:
  mov  [data : %CORE_GC_TABLE + gc_signal], esi // set the collecting thread signal
  mov  ebp, esp

  // ; === thread synchronization ===

  // ; create list of threads need to be stopped
  mov  eax, esi
  // ; get tls entry address  
  mov  esi, data : %CORE_THREAD_TABLE + tt_slots
  mov  edi, [esi - 4]
labNext:
  mov  edx, [esi]
  test edx, edx
  jz   short labSkipTT
  cmp  eax, [edx + tt_sync_event]
  setz cl
  or   ecx, [edx + tt_flags]
  test ecx, 1
  // ; skip current thread signal / thread in safe region from wait list
  jnz  short labSkipSave
  push [edx + tt_sync_event]
labSkipSave:

  // ; reset all signal events
  push [edx + tt_sync_event]
  call extern "$rt.SignalClearGCLA"
  add  esp, 4

  lea  esi, [esi + 8]
  mov  eax, [data : %CORE_GC_TABLE + gc_signal]
labSkipTT:
  sub  edi, 1
  jnz  short labNext

  mov  esi, data : %CORE_GC_TABLE + gc_lock
  mov  edx, 0FFFFFFFFh
  mov  ebx, ebp

  // ; free lock
  // ; could we use mov [esi], 0 instead?
  lock xadd [esi], edx

  mov  ecx, esp
  sub  ebx, esp
  jz   short labSkipWait

  // ; wait until they all stopped
  shr  ebx, 2
  push ecx
  push ebx
  call extern "$rt.WaitForSignalsGCLA"

labSkipWait:
  // ; remove list
  mov  esp, ebp     

  // ==== GCXT end ==============

  // ; create set of roots
  mov  ebp, esp
  xor  ecx, ecx
  push ecx        // ; reserve place 
  push ecx
  push ecx

  // ;   save static roots
  mov  ecx, [rdata : %SYSTEM_ENV]
  mov  esi, stat : %0
  shl  ecx, 2
  push esi
  push ecx

  // ;   collect frames
  mov  eax, [data : %CORE_SINGLE_CONTENT + tt_stack_frame]  
  mov  ecx, eax

labYGNextFrame:
  mov  esi, eax
  mov  eax, [esi]
  test eax, eax
  jnz  short labYGNextFrame
  
  push ecx
  sub  ecx, esi
  neg  ecx
  push ecx  
  
  mov  eax, [esi + 4]
  test eax, eax
  mov  ecx, eax
  jnz  short labYGNextFrame

  mov [ebp-4], esp      // ; save position for roots

  mov  ebx, [ebp]
  mov  eax, esp

  // ; restore frame to correctly display a call stack
  mov  edx, ebp
  mov  ebp, [edx+4]

  // ; call GC routine
  push edx
  push ebx
  push eax
  call extern "$rt.CollectGCLA"

  mov  edi, eax
  mov  ebp, [esp+8] 
  add  esp, 12

  // ; GCXT: signal the collecting thread that GC is ended
  // ; should it be placed into critical section?
  xor  ecx, ecx
  mov  esi, [data : %CORE_GC_TABLE + gc_signal]
  // ; clear thread signal var
  mov  [data : %CORE_GC_TABLE + gc_signal], ecx
  push esi
  call extern "$rt.SignalStopGCLA"

  mov  ebx, edi

  mov  esp, ebp 
  pop  ecx 
  pop  ebp
  pop  esi
  ret

end

// ; --- System Core Preloaded Routines --

// ; ==== Command Set ==

// ; throw
inline %0Ah

  mov  ecx, fs:[2Ch]
  mov  eax, [data : %CORE_TLS_INDEX]
  mov  ecx, [ecx+eax*4]
  mov  edi, [ecx + et_current]
  jmp  [edi + es_catch_addr]

end

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
