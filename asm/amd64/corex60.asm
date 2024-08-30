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

  sub  rsp, 30h

  // ; signal the collecting thread that it is stopped
  mov  r12, rdx

  mov  rcx, rsi
  call extern "$rt.SignalStopGCLA"

  // ; free lock
  // ; could we use mov [esi], 0 instead?
  mov  rdi, data : %CORE_GC_TABLE + gc_lock
  mov  ebx, 0FFFFFFFFh
  lock xadd [rdi], ebx

  // ; stop until GC is ended
  mov  rcx, r12
  call extern "$rt.WaitForSignalGCLA"
  add  rsp, 30h
  // ; restore registers and try again

  pop  rcx
  pop  rdx
  pop  rbp
  pop  r11
  pop  r10

  test rcx, rcx
  jz   labStart

  // ; repeat the alloc operation if required
  call %GC_ALLOC
  ret

labConinue:
  mov  [data : %CORE_GC_TABLE + gc_signal], rsi // set the collecting thread signal
  mov  rbp, rsp

  // ; === thread synchronization ===

  // ; create list of threads need to be stopped
  mov  rax, rsi
  // ; get tls entry address  
  mov  rsi, data : %CORE_THREAD_TABLE + tt_slots
  xor  ecx, ecx
  mov  rdi, [rsi - 8]
labNext:
  mov  rdx, [rsi]
  test rdx, rdx
  jz   short labSkipTT
  cmp  rax, [rdx + tt_sync_event]
  setz cl
  or   ecx, [rdx + tt_flags]
  test ecx, 1
  // ; skip current thread signal / thread in safe region from wait list
  jnz  short labSkipSave
  push [rdx + tt_sync_event]
labSkipSave:

  // ; reset all signal events
  sub  rsp, 30h
  mov  rcx, [rdx + tt_sync_event]
  call extern "$rt.SignalClearGCLA"
  add  rsp, 30h

  lea  rsi, [rsi + 16]
  mov  rax, [data : %CORE_GC_TABLE + gc_signal]
labSkipTT:
  sub  edi, 1
  jnz  short labNext

  mov  rsi, data : %CORE_GC_TABLE + gc_lock
  mov  edx, 0FFFFFFFFh
  mov  rbx, rbp

  // ; free lock
  // ; could we use mov [esi], 0 instead?
  lock xadd [rsi], edx

  mov  rdx, rsp
  sub  rbx, rsp
  jz   short labSkipWait

  // ; wait until they all stopped
  shr  ebx, 3
  sub  rsp, 30h
  mov  ecx, ebx
  call extern "$rt.WaitForSignalsGCLA"
  add  rsp, 30h

labSkipWait:
  // ; remove list
  mov  rsp, rbp     

  // ==== GCXT end ==============

  // ; create set of roots
  mov  rbp, rsp
  xor  ecx, ecx
  push rcx        // ; reserve place 
  push rcx        
  push rcx
  push rcx

  // ;   save static roots
  mov  rax, rdata : %SYSTEM_ENV
  mov  rsi, stat : %0
  mov  ecx, dword ptr [rax]
  shl  ecx, 3
  push rsi
  push rcx

  // ; save perm roots
  mov  rsi, [data : %CORE_GC_TABLE + gc_perm_start]
  mov  rcx, [data : %CORE_GC_TABLE + gc_perm_current]
  sub  rcx, rsi
  push rsi
  push rcx

  // ; == GCXT: save frames ==
  mov  rax, data : %CORE_THREAD_TABLE
  mov  rbx, [rax]

labYGNextThread:  
  sub  ebx, 1
  mov  rax, data : %CORE_THREAD_TABLE + tt_slots
  
  // ; get tls entry address
  mov  r8, rbx
  shl  r8, 4
  add  r8, rax
  mov  rsi, [r8]            
  test rsi, rsi
  jz   short labYGNextThreadSkip

  // ; get the thread local roots
  lea  rax, [rsi + tt_size]
  mov  rcx, [rdata : %SYSTEM_ENV + env_tls_size]
  push rax
  push rcx

  // ; get the top frame pointer
  mov  rax, [rsi + tt_stack_frame]
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
  nop
  nop

labYGNextThreadSkip:
  test rbx, rbx
  jnz  short labYGNextThread
  // ; == GCXT: end ==

  mov [rbp-8], rsp      // ; save position for roots

  mov  r8,  [rbp+8]
  mov  rdx, [rbp]
  mov  rcx, rsp

  // ; restore frame to correctly display a call stack
  mov  rax, rbp
  mov  rbp, [rax+16]

  // ; call GC routine
  sub  rsp, 30h
  mov  [rsp+28h], rax
  call extern "$rt.CollectGCLA"

  mov  rbp, [rsp+28h] 
  mov  rdi, rax

  mov  rbp, [rsp+28h]

  // ; GCXT: signal the collecting thread that GC is ended
  // ; should it be placed into critical section?
  xor  ebx, ebx
  mov  rcx, [data : %CORE_GC_TABLE + gc_signal]
  // ; clear thread signal var
  mov  [data : %CORE_GC_TABLE + gc_signal], rbx
  call extern "$rt.SignalStopGCLA"

  add  rsp, 30h

  mov  rbx, rdi

  mov  rsp, rbp 
  pop  rcx
  pop  rdx
  pop  rbp
  pop  r11
  pop  r10

  ret

end
