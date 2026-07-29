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
define gc_signal             003Ch 

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
  mov  eax, gs:[0]
#endif
  push esi

#if _WIN
  // ; GCXT: find the current thread entry
  mov  eax, [eax]
#elif _LNX
  lea  eax, [eax-tt_size]
#endif

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
  push esi
  call extern "$rt.SignalStopGCLA"
  add  esp, 4

  // ; free lock
  // ; could we use mov [esi], 0 instead?
  mov  edi, data : %CORE_GC_TABLE + gc_lock
  mov  ebx, 0FFFFFFFFh
  lock xadd [edi], ebx

  // ; stop until GC is ended. The barrier is gc_signal itself, not the event of the
  // ; thread that happens to be collecting : that event is reset by the table scan of
  // ; the next collection, and its owner may never collect again
#if (_LNX || _FREEBSD)
  mov  esi, esp
  and  esp, 0FFFFFFF0h
#endif

  call extern "$rt.WaitForCollectionGCLA"

#if (_LNX || _FREEBSD)
  mov  esp, esi
#endif

  // ; restore registers and try again
  pop  ecx
  pop  edx
  pop  ebp
  pop  esi

  test ecx, ecx
  jnz  short labRepeatAlloc

  // ; the lock was released before the wait above, but labStart runs under it :
  // ; labConinue publishes gc_signal and walks the thread table, then releases a lock
  // ; this thread no longer owns. Only system 1 and system 2 come through here, they
  // ; are the ones that call with a zero size
  mov  edi, data : %CORE_GC_TABLE + gc_lock
labRetakeLock:
  mov  edx, 1
  xor  eax, eax
  lock cmpxchg dword ptr[edi], edx
  jnz  short labRetakeLock

  jmp  labStart

labRepeatAlloc:
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

  // ; advance on both paths, a null slot used to make the loop re-scan the same entry
  // ; and leave every higher index thread unstopped
  lea  esi, [esi + 8]

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

  // ; take gc_lock again for the root scan and the collection. Every thread that had
  // ; to stop has signalled by now, so nobody needs the lock to make progress, while a
  // ; thread in teardown does : without this it can null its slot and let the OS free
  // ; the TLS holding its ThreadContent while the scan below is walking it
  mov  edi, data : %CORE_GC_TABLE + gc_lock
labRootLock:
  mov  edx, 1
  xor  eax, eax
  lock cmpxchg dword ptr[edi], edx
  jnz  short labRootLock

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

  // ; a thread that has registered its slot but not yet reached a safe point or an
  // ; allocation still has a null frame chain, and walking it from zero faults right
  // ; here, while gc_lock is held : the handler then allocates and spins on that lock
  test eax, eax
  jz   short labYGNextThreadSkip

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
#if (_LNX || _FREEBSD)
  // ; System V wants esp % 16 == 0 here. The caller cannot help : x86 keeps no
  // ; 16-byte invariant, and the pushes above vary with the thread and frame count.
  and  esp, 0FFFFFFF0h
#endif

  push ecx
  push edx
  push ebx
  push eax
  call extern "$rt.CollectGCLA"

  mov  edi, eax
  mov  ebp, [esp+12] 

  // ; GCXT: release every thread stopped by this collection. Clearing gc_signal is what
  // ; the barrier waits on, so it has to come first
  xor  ecx, ecx
  mov  [data : %CORE_GC_TABLE + gc_signal], ecx

#if (_LNX || _FREEBSD)
  // ; the four CollectGCLA arguments are still on the stack. esp is restored from ebp
  // ; further down, so aligning here costs nothing
  and  esp, 0FFFFFFF0h
#endif

  call extern "$rt.SignalCollectionEndGCLA"

  mov  ebx, edi

  // ; release the lock taken for the root scan
  mov  edi, data : %CORE_GC_TABLE + gc_lock
  mov  edx, 0FFFFFFFFh
  lock xadd [edi], edx

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
  mov  eax, gs:[0]
#endif

  push esi

  // ; GCXT: find the current thread entry
#if _WIN
  mov  eax, [eax]
#elif _LNX
  lea  eax, [eax-tt_size]
#endif

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
  push esi
  call extern "$rt.SignalStopGCLA"
  add  esp, 4

  // ; free lock
  // ; could we use mov [esi], 0 instead?
  mov  edi, data : %CORE_GC_TABLE + gc_lock
  mov  ebx, 0FFFFFFFFh
  lock xadd [edi], ebx

  // ; stop until GC is ended, on gc_signal itself, see the note in GC_COLLECT
#if (_LNX || _FREEBSD)
  mov  esi, esp
  and  esp, 0FFFFFFF0h
#endif

  call extern "$rt.WaitForCollectionGCLA"

#if (_LNX || _FREEBSD)
  mov  esp, esi
#endif

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

  // ; advance on both paths, a null slot used to make the loop re-scan the same entry
  // ; and leave every higher index thread unstopped
  lea  esi, [esi + 8]

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

#if (_LNX || _FREEBSD)
  // ; no caller can align this one, the thread list above pushes 4 bytes per stopped
  // ; thread and the count is only known at run time. size is still at [ebp]
  mov  ecx, [ebp]
  and  esp, 0FFFFFFF0h
  sub  esp, 12                        // ; 12 + 4 (argument) = 16
  push ecx
#endif

  call extern "$rt.CollectPermGCLA"

  mov  edi, eax

  // ; GCXT: release every thread stopped by this collection, see the note in GC_COLLECT
  xor  ecx, ecx
  mov  [data : %CORE_GC_TABLE + gc_signal], ecx

#if (_LNX || _FREEBSD)
  and  esp, 0FFFFFFF0h
#endif

  call extern "$rt.SignalCollectionEndGCLA"

  mov  ebx, edi

#if (_LNX || _FREEBSD)
  // ; absolute, the and above moved esp by an amount not known here
  mov  esp, ebp     
  add  esp, 4
#elif _WIN
  add  esp, 4
#endif

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

  // ; esi is the cached arg0 of the x86 runtime, the mirror of sp:0, and eax and edx
  // ; are the halves of the long accumulator. snop is the only caller and it saves
  // ; nothing, while being emitted at the top of every loop, so whatever is clobbered
  // ; here comes back as corruption in the caller. In STA this routine is empty, and in
  // ; amd64 the cached argument lives in r10 and r11, which is why the translation
  // ; missed it. Pushed below the frame marker above, so the chain the collector walks
  // ; keeps its shape
  push esi
  push eax
  push edx

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
  mov  eax, [eax]
#elif _LNX
  mov  eax, gs:[0]
  lea  eax, [eax-tt_size]
#endif

  mov  esi, [eax+tt_sync_event]   // ; get current thread event
  mov  [eax+tt_stack_frame], edi  // ; lock stack frame

  // ; snop read gc_signal without the lock, so that collection may have ended by now,
  // ; or another one may have started. Re-read it here, where the lock is held
  mov  edx, [data : %CORE_GC_TABLE + gc_signal]
  test edx, edx                       
  jz   short labNoCollect

  // ; signal the collecting thread that it is stopped
  push esi
  mov  edi, data : %CORE_GC_TABLE + gc_lock

  call extern "$rt.SignalStopGCLA"
  add  esp, 4

  // ; free lock
  // ; could we use mov [esi], 0 instead?
  mov  ebx, 0FFFFFFFFh
  lock xadd [edi], ebx

  // ; stop until GC is ended, on gc_signal itself, see the note in GC_COLLECT
#if (_LNX || _FREEBSD)
  mov  esi, esp
  and  esp, 0FFFFFFF0h
#endif

  call extern "$rt.WaitForCollectionGCLA"

#if (_LNX || _FREEBSD)
  mov  esp, esi
#endif

  pop  edx
  pop  eax
  pop  esi
  add  esp, 4                     // ; the ebp of the frame marker
  pop  ebx

  ret

labNoCollect:
  // ; the collection ended before we got the lock, nothing to wait for
  mov  edi, data : %CORE_GC_TABLE + gc_lock
  mov  ebx, 0FFFFFFFFh
  lock xadd [edi], ebx

  pop  edx
  pop  eax
  pop  esi
  add  esp, 4                     // ; the ebp of the frame marker
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
  mov  ecx, [eax]
#elif _LNX
  mov  eax, gs:[0]
  lea  ecx, [eax-tt_size]
#endif

  mov  edi, [ecx + et_current]
  jmp  [edi + es_catch_addr]

end

// ; unhook
inline %0Bh

  // ; GCXT: get current thread frame
#if _WIN
  mov  eax, fs:[2Ch]
  mov  ecx, [eax]
#elif _LNX
  mov  eax, gs:[0]
  lea  ecx, [eax-tt_size]
#endif

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
  mov  edi, [eax]
#elif _LNX
  mov  eax, gs:[0]
  lea  edi, [eax-tt_size]
#endif

  // ; the store below has to be ordered against the gc_signal load that follows. On a
  // ; plain mov the two can be reordered, and then the collector reads tt_flags as zero,
  // ; puts this thread on its wait list, while the thread reads gc_signal as zero and
  // ; goes on to block inside the foreign call, where no safe point follows. cmpxchg is
  // ; the only locked store this assembler emits, and it leaves a nested exclude alone
  mov  edx, 1
  xor  eax, eax
  lock cmpxchg dword ptr [edi + tt_flags], edx

  mov  eax, [data : %CORE_GC_TABLE + gc_signal]
  test eax, eax
  jz   short labNoCollect

  // ; a collection is already counting on this thread to stop. Do it here, where a safe
  // ; point still exists, rather than inside the call it is about to make
  call %THREAD_WAIT

#if _WIN
  mov  eax, fs:[2Ch]
  mov  edi, [eax]
#elif _LNX
  mov  eax, gs:[0]
  lea  edi, [eax-tt_size]
#endif

labNoCollect:
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
  mov  edi, [eax]
#elif _LNX
  mov  eax, gs:[0]
  lea  edi, [eax-tt_size]
#endif

  mov  [edi + tt_flags], 0
  pop  eax
  mov  [edi + tt_stack_frame], eax

end

// ; tststck
inline %17h

  // ; COREX
#if _WIN
  mov  eax, fs:[2Ch]
  mov  edi, [eax]
#elif _LNX
  mov  eax, gs:[0]
  lea  edi, [eax-tt_size]
#endif

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
  mov  eax, [eax]
  lea  edi, [eax + __arg32_1]
#elif _LNX
  mov  eax, gs:[0]
  lea  edi, [eax - __arg32_1]
  lea  edi, [edi-4]
#endif

  mov  ebx, [edi]

end

// ; storetls
inline %0BCh

#if _WIN
  mov  eax, fs:[2Ch]
  mov  eax, [eax]
  lea  edi, [eax + __arg32_1]
#elif _LNX
  mov  eax, gs:[0]
  lea  edi, [eax - __arg32_1]
  lea  edi, [edi-4]
#endif

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
  mov  edi, [ecx]
#elif _LNX
  mov  ecx, gs:[0]
  lea  edi, [ecx-tt_size]
#endif

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
  mov  edi, [ecx]
#elif _LNX
  mov  ecx, gs:[0]
  lea  edi, [ecx-tt_size]
#endif

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
  mov  eax, [eax]
#elif _LNX
  mov  eax, gs:[0]
  lea  eax, [eax-tt_size]
#endif

  mov  edi, data : %CORE_THREAD_TABLE + tt_slots
  mov  [edi + edx * 8], eax

  mov  [eax + tt_stack_root], esp

end

// ; system startup
inline %4CFh

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

// ; system : safe point for a thread that has just been registered
// ; The collector only walks the thread table under gc_lock, so a thread that reaches
// ; system 3 after that walk is invisible to the collection already running, and would
// ; go on to read its argument out of the heap while the collector compacts. Stop here
// ; instead, right after the critical section is left.
// ; THREAD_WAIT publishes the stack as the head of the frame chain, and the root scan
// ; walks it from there. This thread has run no ELENA code and owns no roots, so build
// ; a chain that terminates at once and point ebp at it, instead of letting the scan
// ; read whatever the stack happens to hold. Four slots keep esp 16 byte aligned
inline %8CFh

  mov  edx, [data : %CORE_GC_TABLE + gc_signal]
  test edx, edx                       
  jz   short labConinue

  push ebp
  xor  eax, eax
  push eax
  push eax
  push eax
  mov  ebp, esp

  call %THREAD_WAIT

  // ; THREAD_WAIT leaves tt_stack_frame pointing at the stack it used, and that stack is
  // ; gone as soon as this returns. The thread still owns no roots, so clear it while
  // ; the region is still ours : the root scan skips a null frame, and the first safe
  // ; point inside the worker publishes a live one. Leaving the dead frame behind hangs
  // ; the collector, which walks it holding gc_lock
#if _WIN
  mov  eax, fs:[2Ch]
  mov  edi, [eax]
#elif _LNX
  mov  eax, gs:[0]
  lea  edi, [eax-tt_size]
#endif
  xor  eax, eax
  mov  [edi + tt_stack_frame], eax

  add  esp, 12
  pop  ebp

labConinue:

end

// ; xhookdpr
inline %0E6h

  // ; GCXT: get current thread frame
#if _WIN
  mov  eax, fs:[2Ch]
#elif _LNX
  mov  eax, gs:[0]
#endif

  lea  edi, [ebp + __arg32_1]
#if _WIN
  mov  eax, [eax]
#elif _LNX
  lea  eax, [eax-tt_size]
#endif

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
  mov  edi, [eax]
#elif _LNX
  mov  eax, gs:[0]
  lea  edi, [eax-tt_size]
#endif

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
  mov  edi, [ecx]
#elif _LNX
  mov  ecx, gs:[0]
  lea  edi, [ecx-tt_size]
#endif

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
  mov  edi, [ecx]
#elif _LNX
  mov  ecx, gs:[0]
  lea  edi, [ecx-tt_size]
#endif
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
  mov  edi, [ecx]
#elif _LNX
  mov  ecx, gs:[0]
  lea  edi, [ecx-tt_size]
#endif
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
  mov  edi, [ecx]
#elif _LNX
  mov  ecx, gs:[0]
  lea  edi, [ecx-tt_size]
#endif
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
  mov  edi, [ecx]
#elif _LNX
  mov  ecx, gs:[0]
  lea  edi, [ecx-tt_size]
#endif
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
  mov  edi, [ecx]
#elif _LNX
  mov  ecx, gs:[0]
  lea  edi, [ecx-tt_size]
#endif
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
  mov  edi, [ecx]
#elif _LNX
  mov  ecx, gs:[0]
  lea  edi, [ecx-tt_size]
#endif
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
  mov  edi, [ecx]
#elif _LNX
  mov  ecx, gs:[0]
  lea  edi, [ecx-tt_size]
#endif
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
  mov  edi, [ecx]
#elif _LNX
  mov  ecx, gs:[0]
  lea  edi, [ecx-tt_size]
#endif
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
  mov  edi, [ecx]
#elif _LNX
  mov  ecx, gs:[0]
  lea  edi, [ecx-tt_size]
#endif
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
  mov  edi, [ecx]
#elif _LNX
  mov  ecx, gs:[0]
  lea  edi, [ecx-tt_size]
#endif
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

  mov  ecx, gs:[0]
  lea  ecx, [ecx-tt_size]

  jmp  [ecx]

#endif

end
