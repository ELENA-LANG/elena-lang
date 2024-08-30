// !! NOTE : R15 register must be preserved

// ; --- Predefined References  --
define INVOKER              10001h
define GC_ALLOC	            10002h
define VEH_HANDLER          10003h
define GC_COLLECT	    10004h
define GC_ALLOCPERM	    10005h
define PREPARE	            10006h
define THREAD_WAIT          10007h

define CORE_TOC             20001h
define SYSTEM_ENV           20002h
define CORE_GC_TABLE        20003h
define CORE_SINGLE_CONTENT  2000Bh
define VOID           	    2000Dh
define VOIDPTR              2000Eh
define CORE_THREAD_TABLE    2000Fh

define ACTION_ORDER              9
define ACTION_MASK            1E0h
define ARG_MASK               01Fh
define ARG_ACTION_MASK        1DFh

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
define gc_yg_start           0010h
define gc_yg_current         0018h
define gc_yg_end             0020h
define gc_mg_start           0038h
define gc_mg_current         0040h
define gc_end                0048h
define gc_mg_wbar            0050h
define gc_perm_start         0058h 
define gc_perm_end           0060h 
define gc_perm_current       0068h 

define et_current            0008h
define tt_stack_frame        0010h
define tt_stack_root         0028h

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

// ; NOTE : the table is tailed with GCMGSize,GCYGSize and MaxThread fields
structure %SYSTEM_ENV

  dq 0
  dq 0
  dq data : %CORE_GC_TABLE
  dq 0
  dq data : %CORE_THREAD_TABLE
  dq code : %INVOKER
  dq code : %VEH_HANDLER
  // ; dd GCMGSize
  // ; dd GCYGSize
  // ; dd ThreadCounter

end

structure %CORE_THREAD_TABLE

  dq 0 // ; tt_length              : +00h

  dq 0 // ; tt_slots               : +08h
  dq 0

end

// ; --- GC_ALLOC ---
// ; in: ecx - size ; out: ebx - created object
inline % GC_ALLOC

  // ; GCXT: set lock
labStart:
  mov  rdi, data : %CORE_GC_TABLE + gc_lock

labWait:
  mov edx, 1
  xor eax, eax
  lock cmpxchg dword ptr[rdi], edx
  jnz  short labWait

  mov  rax, [data : %CORE_GC_TABLE + gc_yg_current]
  mov  r12, [data : %CORE_GC_TABLE + gc_yg_end]
  add  rcx, rax
  cmp  rcx, r12
  jae  short labYGCollect
  mov  [data : %CORE_GC_TABLE + gc_yg_current], rcx

  // ; GCXT: clear sync field
  mov  edx, 0FFFFFFFFh
  lea  rbx, [rax + elObjectOffset]
  
  // ; GCXT: free lock
  // ; could we use mov [esi], 0 instead?
  lock xadd [rdi], edx

  ret

labYGCollect:
  // ; save registers
  sub  rcx, rax
  xor  edx, edx
  call %GC_COLLECT
  ret

end

// ; --- GC_COLLECT ---
// ; in: ecx - fullmode (0, 1)
inline % GC_COLLECT

labStart:
  // ; GCXT: find the current thread entry
  mov  rdi, gs:[58h]
  mov  rax, [data : %CORE_TLS_INDEX]

  push r10
  push r11

  // ; GCXT: find the current thread entry
  mov  rax, [rdi+rax*8]

  push rbp

  // ; GCXT: lock frame
  // ; get current thread event
  mov  rsi, [rax + tt_sync_event]
  mov  [rax + tt_stack_frame], rsp

  push rdx
  push rcx

  // ; === GCXT: safe point ===
  mov  rdx, [data : %CORE_GC_TABLE + gc_signal]
  // ; if it is a collecting thread, starts the GC
  test rdx, rdx                       
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
