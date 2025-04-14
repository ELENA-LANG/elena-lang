// ; --- Predefined References  --
define GC_ALLOC	            10002h
define VEH_HANDLER          10003h
define GC_COLLECT	    10004h
define GC_ALLOCPERM	    10005h
define PREPARE	            10006h
define THREAD_WAIT          10007h

define CORE_TOC             20001h
define SYSTEM_ENV           20002h
define CORE_GC_TABLE   	    20003h
define CORE_SINGLE_CONTENT  2000Bh
define VOID           	    2000Dh
define VOIDPTR              2000Eh
define CORE_THREAD_TABLE    2000Fh

// ; --- sysenv offsets ---
define env_tls_size         0004h

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
define tt_stack_root         0014h

define tt_size               0018h

define es_prev_struct        0000h
define es_catch_addr         0004h
define es_catch_level        0008h
define es_catch_frame        000Ch

// ; THREAD TABLE
define tt_slots             00004h

// ; --- Object header fields ---
define elSyncOffset          0001h
define elSizeOffset          0004h
define elVMTOffset           0008h 
define elObjectOffset        0008h

// ; NOTE : the table is tailed with GCMGSize,GCYGSize and MaxThread fields
structure %SYSTEM_ENV

  dd 0
  dd 0
  dd data : %CORE_GC_TABLE
  dd 0
  dd data : %CORE_THREAD_TABLE
  dd 0
  dd code : %VEH_HANDLER
  // ; dd GCMGSize
  // ; dd GCYGSize
  // ; dd ThreadCounter
  // ; dd TLSSize

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
  lock xadd [edi], edx

  ret

labYGCollect:
  // ; save registers
  sub  ecx, eax
  xor  edx, edx
  call %GC_COLLECT
  ret

end

// ; --- GC_COLLECT ---
// ; in: ecx - fullmode (0, 1)
inline % GC_COLLECT

labStart:
  // ; GCXT: find the current thread entry

#if _WIN
  mov  eax, fs:[2Ch]
#elif _LNX
  lea  eax, gs:[-4]
#endif
  push esi

  // ; GCXT: find the current thread entry
  mov  eax, [eax]

  push ebp

  // ; GCXT: lock frame
  // ; get current thread event
  mov  esi, [eax + tt_sync_event]
  mov  [eax + tt_stack_frame], esp

  push edx
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
  pop  edx
  pop  ebp
  pop  esi

  test ecx, ecx
  jz   labStart

  // ; repeat the alloc operation if required
  call %GC_ALLOC
  ret

labConinue:
  mov  [data : %CORE_GC_TABLE + gc_signal], esi // set the collecting thread signal
  mov  ebp, esp

  // ; === thread synchronization ===

  // ; create list of threads need to be stopped
  mov  eax, esi
  // ; get tls entry address  
  mov  esi, data : %CORE_THREAD_TABLE + tt_slots
  xor  ecx, ecx
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

  // ; save perm roots
  mov  esi, [data : %CORE_GC_TABLE + gc_perm_start]
  mov  ecx, [data : %CORE_GC_TABLE + gc_perm_current]
  sub  ecx, esi
  push esi
  push ecx

  // ; == GCXT: save frames ==
  mov  eax, data : %CORE_THREAD_TABLE
  mov  ebx, [eax]

labYGNextThread:  
  sub  ebx, 1
  mov  eax, data : %CORE_THREAD_TABLE + tt_slots
  
  // ; get tls entry address
  mov  esi, [eax+ebx*8]            
  test esi, esi
  jz   short labYGNextThreadSkip

  // ; get the thread local roots
  lea  eax, [esi + tt_size]
  mov  ecx, [rdata : %SYSTEM_ENV + env_tls_size]
  push eax
  push ecx

  // ; get the top frame pointer
  mov  eax, [esi + tt_stack_frame]
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
  nop
  nop

labYGNextThreadSkip:
  test ebx, ebx
  jnz  short labYGNextThread
  // ; == GCXT: end ==

  mov [ebp-4], esp      // ; save position for roots

  mov  ebx, [ebp]
  mov  edx, [ebp+4]
  mov  eax, esp

  // ; restore frame to correctly display a call stack
  mov  ecx, ebp
  mov  ebp, [ecx+8]

  // ; call GC routine
  push ecx
  push edx
  push ebx
  push eax
  call extern "$rt.CollectGCLA"

  mov  edi, eax
  mov  ebp, [esp+12] 

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
  pop  edx 
  pop  ebp
  pop  esi
  ret

end

// --- GC_ALLOCPERM ---
// in: ecx - size ; out: ebx - created object
procedure %GC_ALLOCPERM

  // ; GCXT: set lock
labStart:
  mov  edi, data : %CORE_GC_TABLE + gc_lock

labWait:
  mov edx, 1
  xor eax, eax
  lock cmpxchg dword ptr[edi], edx
  jnz  short labWait

  mov  eax, [data : %CORE_GC_TABLE + gc_perm_current]
  add  ecx, eax
  cmp  ecx, [data : %CORE_GC_TABLE + gc_perm_end]
  jae  short labPERMCollect
  mov  [data : %CORE_GC_TABLE + gc_perm_current], ecx
  lea  ebx, [eax + elObjectOffset]

  // ; GCXT: clear sync field
  mov  edx, 0FFFFFFFFh
  lea  ebx, [eax + elObjectOffset]
  
  // ; GCXT: free lock
  // ; could we use mov [esi], 0 instead?
  lock xadd [edi], edx

  ret

labPERMCollect:
  // ; save registers
  sub  ecx, eax

  // ; GCXT: find the current thread entry

#if _WIN
  mov  eax, fs:[2Ch]
#elif _LNX
  lea  eax, gs:[-4]
#endif

  push esi

  // ; GCXT: find the current thread entry
  mov  eax, [eax]

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

  call extern "$rt.CollectPermGCLA"

  mov  edi, eax

  // ; GCXT: signal the collecting thread that GC is ended
  // ; should it be placed into critical section?
  xor  ecx, ecx
  mov  esi, [data : %CORE_GC_TABLE + gc_signal]
  // ; clear thread signal var
  mov  [data : %CORE_GC_TABLE + gc_signal], ecx
  push esi
  call extern "$rt.SignalStopGCLA"

  mov  ebx, edi
  add  esp, 8

  pop  ebp
  pop  esi
  ret

end

// --- THREAD_WAIT ---
// GCXT: it is presumed that gc lock is on, edx - contains the collecting thread event handle

procedure % THREAD_WAIT

  push ebx
  push ebp
  mov  edi, esp

  push edx                  // hHandle

  // ; set lock
  mov  ebx, data : %CORE_GC_TABLE + gc_lock
labWait:
  mov edx, 1
  xor eax, eax  
  lock cmpxchg dword ptr[ebx], edx
  jnz  short labWait

  // ; find the current thread entry
#if _WIN
  mov  eax, fs:[2Ch]
#elif _LNX
  lea  eax, gs:[-4]
#endif

  mov  eax, [eax]

  mov  esi, [eax+tt_sync_event]   // ; get current thread event
  mov  [eax+tt_stack_frame], edi  // ; lock stack frame

  // ; signal the collecting thread that it is stopped
  push esi
  mov  edi, data : %CORE_GC_TABLE + gc_lock

  // ; signal the collecting thread that it is stopped
  call extern "$rt.SignalStopGCLA"
  add  esp, 4

  // ; free lock
  // ; could we use mov [esi], 0 instead?
  mov  ebx, 0FFFFFFFFh
  lock xadd [edi], ebx

  // ; stop until GC is ended
  call extern "$rt.WaitForSignalGCLA"

  add  esp, 8
  pop  ebx

  ret

end

// ; --- System Core Preloaded Routines --

// ; ==== Command Set ==

// ; snop
inline % 2

  // ; safe point
  mov  edx, [data : %CORE_GC_TABLE + gc_signal]
  test edx, edx                       // ; if it is a collecting thread, waits
  jz   short labConinue               // ; otherwise goes on

  nop
  nop
  call %THREAD_WAIT                   // ; waits until the GC is stopped

labConinue:

end

// ; throw
inline %0Ah

#if _WIN
  mov  eax, fs:[2Ch]
#elif _LNX
  lea  eax, gs:[-4]
#endif

  mov  ecx, [eax]
  mov  edi, [ecx + et_current]
  jmp  [edi + es_catch_addr]

end

// ; unhook
inline %0Bh

  // ; GCXT: get current thread frame
#if _WIN
  mov  eax, fs:[2Ch]
#elif _LNX
  lea  eax, gs:[-4]
#endif

  mov  ecx, [eax]
  mov  edi, [ecx + et_current]

  mov  eax, [edi + es_prev_struct]
  mov  ebp, [edi + es_catch_frame]
  mov  esp, [edi + es_catch_level]

  mov  [ecx + et_current], eax

end

// ; exclude
inline % 10h
     
#if _WIN
  mov  eax, fs:[2Ch]
#elif _LNX
  lea  eax, gs:[-4]
#endif
  mov  edi, [eax]
  mov  [edi + tt_flags], 1
  mov  eax, [edi + tt_stack_frame]
  push eax
  push ebp     
  mov  [edi + tt_stack_frame], esp

end

// ; include
inline % 11h

  add  esp, 4

#if _WIN
  mov  eax, fs:[2Ch]
#elif _LNX
  lea  eax, gs:[-4]
#endif

  mov  edi, [eax]
  mov  [edi + tt_flags], 0
  pop  eax
  mov  [edi + tt_stack_frame], eax

end

// ; tststck
inline %17h

  // ; COREX
#if _WIN
  mov  eax, fs:[2Ch]
#elif _LNX
  lea  eax, gs:[-4]
#endif

  mov  edi, [eax]
  mov  eax, [edi + tt_stack_root]

  xor  ecx, ecx
  cmp  ebx, esp
  setl cl
  cmp  ebx, eax
  setg ch
  test ecx, ecx

end

// ; trylock
inline %02Bh

  // ; GCXT: try to lock
  xor  eax, eax
  mov  ecx, 1
  lock cmpxchg byte ptr[ebx - elSyncOffset], cl
  test eax, eax 

end

// ; freelock
inline %02Ch

  mov  ecx, -1

  // ; free lock
  lock xadd byte ptr [ebx - elSyncOffset], cl

end

// ; peektls
inline %0BBh

#if _WIN
  mov  eax, fs:[2Ch]
#elif _LNX
  lea  eax, gs:[-4]
#endif

  mov  eax, [eax]
  lea  edi, [eax + __arg32_1]
  mov  ebx, [edi]

end

// ; storetls
inline %0BCh

#if _WIN
  mov  eax, fs:[2Ch]
#elif _LNX
  lea  eax, gs:[-4]
#endif

  mov  eax, [eax]
  lea  edi, [eax + __arg32_1]
  mov  [edi], ebx

end

// ; extclosen
inline %0CAh

  add  ebp, __n_1
  mov  esp, ebp
  pop  ebp
  
  add  esp, 8

  pop  ebx

#if _WIN
  mov  ecx, fs:[2Ch]
#elif _LNX
  lea  ecx, gs:[-4]
#endif

  mov  edi, [ecx]
  mov  [edi + tt_stack_frame], ebx

  pop  ebp

  pop  ebx
  pop  ecx
  pop  edi
  pop  esi

end

// ; extclosen 0
inline %1CAh

  mov  esp, ebp
  pop  ebp

  add  esp, 8

  pop  ebx

#if _WIN
  mov  ecx, fs:[2Ch]
#elif _LNX
  lea  ecx, gs:[-4]
#endif

  mov  edi, [ecx]
  mov  [edi + tt_stack_frame], ebx

  pop  ebp

  pop  ebx
  pop  ecx
  pop  edi
  pop  esi
  
end

// ; system minor collect
inline %1CFh

  mov  edi, data : %CORE_GC_TABLE + gc_lock

labWait:
  mov edx, 1
  xor eax, eax
  lock cmpxchg dword ptr[edi], edx
  jnz  short labWait

  xor  ecx, ecx
  xor  edx, edx
  call %GC_COLLECT

end

// ; system full collect
inline %2CFh

  mov  edi, data : %CORE_GC_TABLE + gc_lock

labWait:
  mov edx, 1
  xor eax, eax
  lock cmpxchg dword ptr[edi], edx
  jnz  short labWait

  xor  ecx, ecx
  mov  edx, 1
  call %GC_COLLECT

end

// ; system 3 (thread startup)
inline %3CFh

#if _WIN
  mov  eax, fs:[2Ch]
#elif _LNX
  lea  eax, gs:[-4]
#endif

  mov  eax, [eax]
  mov  edi, data : %CORE_THREAD_TABLE + tt_slots
  mov  [edi + edx * 8], eax

  mov  [eax + tt_stack_root], esp

end

// ; system startup
inline %4CFh

#if _LNX

  lea  eax, gs:[-4]
  lea  ecx, gs:[0]

#endif

  finit

  mov  eax, esp
  call %PREPARE

end

// ; system : enter GC critical section
inline %6CFh

  mov  edi, data : %CORE_GC_TABLE + gc_lock
  mov  ecx, 1
labWait:
  xor  eax, eax
  lock cmpxchg dword ptr[edi], ecx
  jnz  short labWait

end

// ; system : leave GC critical section
inline %7CFh

  // ; GCXT: clear sync field
  mov  edi, data : %CORE_GC_TABLE + gc_lock
  mov  ecx, 0FFFFFFFFh
  
  // ; GCXT: free lock
  // ; could we use mov [esi], 0 instead?
  lock xadd [edi], ecx

end

// ; xhookdpr
inline %0E6h

  // ; GCXT: get current thread frame
#if _WIN
  mov  eax, fs:[2Ch]
#elif _LNX
  lea  eax, gs:[-4]
#endif

  lea  edi, [ebp + __arg32_1]
  mov  eax, [eax]

  mov  ecx, [eax + et_current]
  mov  [edi + es_catch_frame], ebp
  mov  [edi + es_prev_struct], ecx
  mov  [edi + es_catch_level], esp
  mov  [edi + es_catch_addr], __ptr32_2

  mov  [eax + et_current], edi

end

// ; extopenin
inline %0F2h

  push esi
  push edi
  push ecx
  push ebx

  push ebp     

#if _WIN
  mov  eax, fs:[2Ch]
#elif _LNX
  lea  eax, gs:[-4]
#endif

  mov  edi, [eax]
  mov  eax, [edi + tt_stack_frame]
  push eax 

  mov  ebp, eax
  xor  eax, eax
  push ebp
  push eax
  mov  ebp, esp

  push ebp
  xor  eax, eax
  mov  ebp, esp
  sub  esp, __n_2
  push ebp
  push eax
  mov  ebp, esp
  mov  ecx, __n_1
  sub  esp, __arg32_1
  mov  edi, esp
  rep  stos
  mov  esi, eax

end 

// ; extopenin 0, n
inline %1F2h

  push esi
  push edi
  push ecx
  push ebx

  push ebp     
#if _WIN
  mov  ecx, fs:[2Ch]
#elif _LNX
  lea  ecx, gs:[-4]
#endif

  mov  edi, [ecx]
  mov  eax, [edi + tt_stack_frame]
  push eax 

  mov  ebp, eax
  xor  eax, eax
  push ebp
  push eax
  mov  ebp, esp

  push ebp
  xor  eax, eax
  mov  ebp, esp
  sub  esp, __n_2
  push ebp
  push eax
  mov  ebp, esp
  mov  esi, eax

end 

// ; extopenin 1, n
inline %2F2h

  push esi
  push edi
  push ecx
  push ebx

  push ebp     
#if _WIN
  mov  ecx, fs:[2Ch]
#elif _LNX
  lea  ecx, gs:[-4]
#endif
  mov  edi, [ecx]
  mov  eax, [edi + tt_stack_frame]
  push eax 

  mov  ebp, eax
  xor  eax, eax
  push ebp
  push eax
  mov  ebp, esp

  push ebp
  xor  eax, eax
  mov  ebp, esp
  sub  esp, __n_2
  push ebp
  push eax
  mov  ebp, esp
  push eax
  mov  esi, eax

end 

// ; extopenin 2, n
inline %3F2h

  push esi
  push edi
  push ecx
  push ebx

  push ebp     
#if _WIN
  mov  ecx, fs:[2Ch]
#elif _LNX
  lea  ecx, gs:[-4]
#endif
  mov  edi, [ecx]
  mov  eax, [edi + tt_stack_frame]
  push eax 

  mov  ebp, eax
  xor  eax, eax
  push ebp
  push eax
  mov  ebp, esp

  push ebp
  xor  eax, eax
  mov  ebp, esp
  sub  esp, __n_2
  push ebp
  push eax
  mov  ebp, esp
  push eax
  push eax
  mov  esi, eax

end 

// ; extopenin 3, n
inline %4F2h

  push esi
  push edi
  push ecx
  push ebx

  push ebp     
#if _WIN
  mov  ecx, fs:[2Ch]
#elif _LNX
  lea  ecx, gs:[-4]
#endif
  mov  edi, [ecx]
  mov  eax, [edi + tt_stack_frame]
  push eax 

  mov  ebp, eax
  xor  eax, eax
  push ebp
  push eax
  mov  ebp, esp

  push ebp
  xor  eax, eax
  mov  ebp, esp
  sub  esp, __n_2
  push ebp
  push eax
  mov  ebp, esp
  push eax
  push eax
  push eax
  mov  esi, eax

end 

// ; extopenin 4, n
inline %5F2h

  push esi
  push edi
  push ecx
  push ebx

  push ebp     
#if _WIN
  mov  ecx, fs:[2Ch]
#elif _LNX
  lea  ecx, gs:[-4]
#endif
  mov  edi, [ecx]
  mov  eax, [edi + tt_stack_frame]
  push eax 

  mov  ebp, eax
  xor  eax, eax
  push ebp
  push eax
  mov  ebp, esp

  push ebp
  xor  eax, eax
  mov  ebp, esp
  sub  esp, __n_2
  push ebp
  push eax
  mov  ebp, esp
  push eax
  push eax
  push eax
  push eax
  mov  esi, eax

end 

// ; extopenin i, 0
inline %6F2h

  push esi
  push edi
  push ecx
  push ebx

  push ebp     
#if _WIN
  mov  ecx, fs:[2Ch]
#elif _LNX
  lea  ecx, gs:[-4]
#endif
  mov  edi, [ecx]
  mov  eax, [edi + tt_stack_frame]
  push eax 

  mov  ebp, eax
  xor  eax, eax
  push ebp
  push eax
  mov  ebp, esp

  push ebp
  xor  eax, eax
  mov  ebp, esp
  mov  ecx, __n_1
  sub  esp, __arg32_1
  mov  edi, esp
  rep  stos
  mov  esi, eax

end 

// ; extopenin 0, 0
inline %7F2h

  push esi
  push edi
  push ecx
  push ebx

  push ebp     
#if _WIN
  mov  ecx, fs:[2Ch]
#elif _LNX
  lea  ecx, gs:[-4]
#endif
  mov  edi, [ecx]
  mov  eax, [edi + tt_stack_frame]
  push eax 

  mov  ebp, eax
  xor  eax, eax
  push ebp
  push eax
  mov  ebp, esp

  push ebp
  mov  ebp, esp
  mov  esi, eax

end 

// ; extopenin 1, 0
inline %8F2h

  push esi
  push edi
  push ecx
  push ebx

  push ebp     
#if _WIN
  mov  ecx, fs:[2Ch]
#elif _LNX
  lea  ecx, gs:[-4]
#endif
  mov  edi, [ecx]
  mov  eax, [edi + tt_stack_frame]
  push eax 

  mov  ebp, eax
  xor  eax, eax
  push ebp
  push eax
  mov  ebp, esp

  push ebp
  mov  ebp, esp
  push 0
  mov  esi, eax

end 

// ; extopenin 2, 0
inline %9F2h

  push esi
  push edi
  push ecx
  push ebx

  push ebp     
#if _WIN
  mov  ecx, fs:[2Ch]
#elif _LNX
  lea  ecx, gs:[-4]
#endif
  mov  edi, [ecx]
  mov  eax, [edi + tt_stack_frame]
  push eax 

  mov  ebp, eax
  xor  eax, eax
  push ebp
  push eax
  mov  ebp, esp

  push ebp
  xor  eax, eax
  mov  ebp, esp
  push eax
  push eax
  mov  esi, eax

end 

// ; extopenin 3, 0
inline %0AF2h

  push esi
  push edi
  push ecx
  push ebx

  push ebp     
#if _WIN
  mov  ecx, fs:[2Ch]
#elif _LNX
  lea  ecx, gs:[-4]
#endif
  mov  edi, [ecx]
  mov  eax, [edi + tt_stack_frame]
  push eax 

  mov  ebp, eax
  xor  eax, eax
  push ebp
  push eax
  mov  ebp, esp

  push ebp
  xor  eax, eax
  mov  ebp, esp
  push eax
  push eax
  push eax
  mov  esi, eax

end 

// ; extopenin 4, 0
inline %0BF2h

  push esi
  push edi
  push ecx
  push ebx

  push ebp     
#if _WIN
  mov  ecx, fs:[2Ch]
#elif _LNX
  lea  ecx, gs:[-4]
#endif
  mov  edi, [ecx]
  mov  eax, [edi + tt_stack_frame]
  push eax 

  mov  ebp, eax
  xor  eax, eax
  push ebp
  push eax
  mov  ebp, esp

  push ebp
  xor  eax, eax
  mov  ebp, esp
  push eax
  push eax
  push eax
  push eax
  mov  esi, eax

end 

// VEH_HANDLER() 
procedure % VEH_HANDLER

#if _WIN

  mov  esi, edx
  mov  edx, eax   // ; set exception code

  mov  ecx, fs:[2Ch]
  mov  ecx, [ecx]
  jmp  [ecx]

#elif _LNX

  mov  esi, edx
  mov  edx, eax   // ; set exception code

  lea  eax, gs:[-4]
  mov  eax, [eax]
  jmp  [eax]

#endif

end
