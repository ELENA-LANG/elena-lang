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

// GC TABLE OFFSETS
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
define gc_et_current         002Ch 
define gc_stack_frame        0030h 
define gc_lock               0034h 
define gc_signal             0038h 
define tt_ptr                003Ch 
define tt_lock               0040h 
define gc_rootcount          004Ch
define gc_perm_start         0050h 
define gc_perm_end           0054h 
define gc_perm_current       0058h 

// SYSTEM_ENV OFFSETS
define se_mgsize	     0014h
define se_ygsize	     001Ch

// GCXT TLS TABLE
define tls_et_current        0000h
define tls_stack_frame       0004h
define tls_sync_event        0008h
define tls_flags             000Ch

// Page Size
define page_size               10h
define page_size_order          4h
define page_size_order_minus2   2h
define page_mask        0FFFFFFF0h
define page_ceil               17h
define struct_mask         800000h
define struct_mask_inv     7FFFFFh

// Object header fields
define elSizeOffset          0004h
define elSyncOffset          0004h
define elVMTOffset           0008h 
define elObjectOffset        0008h

define elPageSizeOffset     0004h
define elPageVMTOffset      0000h

// VMT header fields
define elVMTSizeOffset       0004h
define elVMTFlagOffset       000Ch
define elPackageOffset       0010h

define page_align_mask   000FFFF0h

define ACTION_ORDER              9
define ARG_ACTION_MASK        1DFh
define ACTION_MASK            1E0h
define ARG_MASK               01Fh
define ARG_MASK_INV     0FFFFFFE0h

// --- GC_ALLOC ---
// in: ecx - size ; out: ebx - created object
procedure %GC_ALLOC
  
  // ; GCXT: set lock
labStart:
  mov  esi, data : %CORE_GC_TABLE + gc_lock
labWait:
  mov edx, 1
  xor eax, eax
  lock cmpxchg dword ptr[esi], edx
  jnz  short labWait

  mov  eax, [data : %CORE_GC_TABLE + gc_yg_current]
  mov  edx, [data : %CORE_GC_TABLE + gc_yg_end]
  add  ecx, eax
  cmp  ecx, edx
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
  // ; restore ecx
  sub  ecx, eax

  // ; GCXT: find the current thread entry
  mov  edx, fs:[2Ch]
  mov  eax, [data : %CORE_TLS_INDEX]

  // ; GCXT: save registers
  mov  eax, [edx+eax*4]
  push ebp
  
  // ; GCXT: lock stack frame
  // ; get current thread event
  mov  esi, [eax + tls_sync_event]         
  mov  [eax + tls_stack_frame], esp
  
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
  call extern 'rt_dlls.GCSignalStop
  add  esp, 4

  // ; free lock
  // ; could we use mov [esi], 0 instead?
  mov  edi, data : %CORE_GC_TABLE + gc_lock
  mov  ebx, 0FFFFFFFFh
  lock xadd [edi], ebx

  // ; stop until GC is ended
  call extern 'rt_dlls.GCWaitForSignal
  add  esp, 4

  // ; restore registers and try again
  pop  ecx
  pop  ebp

  jmp  labStart

labConinue:

  mov  [data : %CORE_GC_TABLE + gc_signal], esi // set the collecting thread signal
  mov  ebp, esp

  // ; === thread synchronization ===

  // ; create list of threads need to be stopped
  mov  eax, esi
  // ; get tls entry address  
  mov  esi, data : %THREAD_TABLE
  mov  edi, [esi - 4]
labNext:
  mov  edx, [esi]
  test edx, edx
  jz   short labSkipTT
  cmp  eax, [edx + tls_sync_event]
  setz cl
  or  ecx, [edx + tls_flags]
  test ecx, 1
  // ; skip current thread signal / thread in safe region from wait list
  jnz  short labSkipSave
  push [edx + tls_sync_event]
labSkipSave:

  // ; reset all signal events
  push [edx + tls_sync_event]
  call extern 'rt_dlls.GCSignalClear
  add  esp, 4

  lea  esi, [esi+4]
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
  call extern 'rt_dlls.GCWaitForSignals

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

  // ; save static roots
  mov  esi, data : %CORE_STATICROOT
  mov  ecx, [data : %CORE_GC_TABLE + gc_rootcount]
  push esi
  push ecx

  // ; == GCXT: save frames ==
  mov  eax, data : %THREAD_TABLE
  mov  ebx, [eax - 4]

labYGNextThread:  
  sub  ebx, 1
  mov  eax, data : %THREAD_TABLE
  
  // ; get tls entry address
  mov  esi, [eax+ebx*4]            
  test esi, esi
  jz   short labYGNextThreadSkip

  // ; get the top frame pointer
  mov  eax, [esi + tls_stack_frame]
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
  
  // === Minor collection ===
  mov [ebp-4], esp      // ; save position for roots

  mov  ebx, [ebp]
  mov  eax, esp

  // ; restore rbp to correctly display a call stack
  mov  edx, ebp
  mov  ebp, [edx+4]

  push edx
  push ebx
  push eax
  call extern 'rt_dlls.GCCollect

  mov  edi, eax
  add  esp, 8
  pop  ebp

  // ; GCXT: signal the collecting thread that GC is ended
  // ; should it be placed into critical section?
  xor  ecx, ecx
  mov  esi, [data : %CORE_GC_TABLE + gc_signal]
  // ; clear thread signal var
  mov  [data : %CORE_GC_TABLE + gc_signal], ecx
  push esi
  call extern 'rt_dlls.GCSignalStop

  mov  ebx, edi

  mov  esp, ebp 
  pop  ecx 
  pop  ebp

  ret

end

// --- GC_ALLOCPERM ---
// in: ecx - size ; out: ebx - created object
procedure %GC_ALLOCPERM
  
  // ; GCXT: set lock
labStart:
  mov  esi, data : %CORE_GC_TABLE + gc_lock
labWait:
  mov edx, 1
  xor eax, eax
  lock cmpxchg dword ptr[esi], edx
  jnz  short labWait

  mov  eax, [data : %CORE_GC_TABLE + gc_perm_current]
  mov  edx, [data : %CORE_GC_TABLE + gc_perm_end]
  add  ecx, eax
  cmp  ecx, edx
  jae  short labPERMCollect
  mov  [data : %CORE_GC_TABLE + gc_perm_current], ecx

  // ; GCXT: clear sync field
  mov  edx, 0FFFFFFFFh
  lea  ebx, [eax + elObjectOffset]
  
  // ; GCXT: free lock
  // ; could we use mov [esi], 0 instead?
  lock xadd [esi], edx

  ret

labPERMCollect:
  // ; restore ecx
  sub  ecx, eax

  // ; GCXT: find the current thread entry
  mov  edx, fs:[2Ch]
  mov  eax, [data : %CORE_TLS_INDEX]

  // ; GCXT: save registers
  mov  eax, [edx+eax*4]
  
  // ; GCXT: lock stack frame
  // ; get current thread event
  mov  esi, [eax + tls_sync_event]         
  mov  [eax + tls_stack_frame], esp
  
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
  call extern 'rt_dlls.GCSignalStop
  add  esp, 4

  // ; free lock
  // ; could we use mov [esi], 0 instead?
  mov  edi, data : %CORE_GC_TABLE + gc_lock
  mov  ebx, 0FFFFFFFFh
  lock xadd [edi], ebx

  // ; stop until GC is ended
  call extern 'rt_dlls.GCWaitForSignal
  add  esp, 4

  // ; restore registers and try again
  pop  ecx

  jmp  labStart

labConinue:

  push ebp
  mov  [data : %CORE_GC_TABLE + gc_signal], esi // set the collecting thread signal
  mov  ebp, esp

  // ; === thread synchronization ===

  // ; create list of threads need to be stopped
  mov  eax, esi
  // ; get tls entry address  
  mov  esi, data : %THREAD_TABLE
  mov  edi, [esi - 4]
labNext:
  mov  edx, [esi]
  test edx, edx
  jz   short labSkipTT
  cmp  eax, [edx + tls_sync_event]
  setz cl
  or  ecx, [edx + tls_flags]
  test ecx, 1
  // ; skip current thread signal / thread in safe region from wait list
  jnz  short labSkipSave
  push [edx + tls_sync_event]
labSkipSave:

  // ; reset all signal events
  push [edx + tls_sync_event]
  call extern 'rt_dlls.GCSignalClear
  add  esp, 4

  lea  esi, [esi+4]
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
  call extern 'rt_dlls.GCWaitForSignals

labSkipWait:
  // ; remove list
  mov  esp, ebp    
  pop  ebp

  // ==== GCXT end ==============
  
  call extern 'rt_dlls.GCCollectPerm

  mov  edi, eax
  add  esp, 4

  // ; GCXT: signal the collecting thread that GC is ended
  // ; should it be placed into critical section?
  xor  ecx, ecx
  mov  esi, [data : %CORE_GC_TABLE + gc_signal]
  // ; clear thread signal var
  mov  [data : %CORE_GC_TABLE + gc_signal], ecx
  push esi
  call extern 'rt_dlls.GCSignalStop

  mov  ebx, edi

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
  mov  edx, fs:[2Ch]
  mov  eax, [data : %CORE_TLS_INDEX]  
  mov  eax, [edx+eax*4]

  mov  esi, [eax+tls_sync_event]   // ; get current thread event
  mov  [eax+tls_stack_frame], edi  // ; lock stack frame

  // ; signal the collecting thread that it is stopped
  push esi
  mov  edi, data : %CORE_GC_TABLE + gc_lock

  // ; signal the collecting thread that it is stopped
  call extern 'rt_dlls.GCSignalStop
  add  esp, 4

  // ; free lock
  // ; could we use mov [esi], 0 instead?
  mov  ebx, 0FFFFFFFFh
  lock xadd [edi], ebx

  // ; stop until GC is ended
  call extern 'rt_dlls.GCWaitForSignal

  add  esp, 8
  pop  ebx

  ret

end

// ; ==== Command Set ==

// ; snop
inline % 4

  // ; safe point
  mov  edx, [data : %CORE_GC_TABLE + gc_signal]
  test edx, edx                       // ; if it is a collecting thread, waits
  jz   short labConinue               // ; otherwise goes on

  nop
  nop
  call code : %THREAD_WAIT            // ; waits until the GC is stopped

labConinue:

end

// ; throw
inline % 7

  // ; GCXT: get current thread frame
  mov  esi, [data : %CORE_TLS_INDEX]
  mov  eax, fs:[2Ch]
  mov  esi, [eax+esi*4]
  mov  eax, [esi + tls_et_current]

  jmp  [eax]

end

// ; unhook
inline % 1Dh

  // ; GCXT: get current thread frame
  mov  eax, [data : %CORE_TLS_INDEX]
  mov  esi, fs:[2Ch]
  mov  eax, [esi+eax*4]

  mov  esi, [eax + tls_et_current]

  mov  esp, [esi + 4]
  mov  ebp, [esi + 8]
  pop  esi
  mov  [eax + tls_et_current], esi
  
end

// ; include
inline % 25h
       
  add  esp, 4
  mov  ecx, fs:[2Ch]
  mov  eax, [data : %CORE_TLS_INDEX]
  mov  esi, [ecx+eax*4]
  mov  [esi + tls_flags], 0

end

// ; exclude
inline % 26h
                                                       
  mov  ecx, fs:[2Ch]
  mov  eax, [data : %CORE_TLS_INDEX]
  mov  esi, [ecx+eax*4]
  mov  [esi + tls_flags], 1
  push ebp     
  mov  [esi + tls_stack_frame], esp

end

// ; trylock
inline % 27h

  // ; GCXT: try to lock
  mov esi, [esp]
  xor eax, eax
  mov ebx, 1
  lock cmpxchg byte ptr[esi - elSyncOffset], bl
  mov  edx, eax
  mov  ebx, esi

end

// ; freelock
inline % 28h

  mov  ecx, -1

  // ; free lock
  lock xadd byte ptr [ebx - elSyncOffset], cl

end

// ; rethrow
inline % 29h

  // ; GCXT: get current thread frame
  mov  esi, [data : %CORE_TLS_INDEX]
  mov  ecx, fs:[2Ch]
  mov  ecx, [ecx+esi*4]

  mov  esi, [ecx + tls_et_current]
  mov  esi, [esi + 4]
  mov  esi, [esi]
  mov  [ecx + tls_et_current], esi

  jmp  [ecx + tls_et_current]

end

// ; hook label (ecx - offset)
// ; NOTE : hook calling should be the first opcode

inline % 0A6h

  call code : %HOOK

  // ; GCXT: get current thread frame
  mov  eax, [data : %CORE_TLS_INDEX]
  mov  edi, fs:[2Ch]
  mov  eax, [edi+eax*4]

  push [eax + tls_et_current]

  mov  esi, esp 
  push ebp
  push esi
  push ecx

  mov  [eax + tls_et_current], esp
  
end
