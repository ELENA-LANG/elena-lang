// !! NOTE : R15 register must be preserved

// ; --- Predefined References  --
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
define elSyncOffset          0008h
define elSyncForwardOffset   0008h
define elSizeOffset          0004h
define elVMTOffset           0010h 
define elObjectOffset        0010h

// ; --- VMT header fields ---
define elVMTSizeOffset       0008h
define elVMTFlagOffset       0018h
define elPackageOffset       0020h

// ; --- sysenv offsets ---
define env_tls_size          0008h

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
define gc_lock               0070h 
define gc_signal             0078h 

// ; THREAD CONTENT
define et_current            0008h
define tt_stack_frame        0010h
define tt_sync_event         0018h
define tt_flags              0020h
define tt_stack_root         0028h

define tt_size               0030h

define es_prev_struct        0000h
define es_catch_addr         0008h
define es_catch_level        0010h
define es_catch_frame        0018h

// ; THREAD TABLE
define tt_slots             00008h

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
  dq 0
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
  xor  edx, edx    // ; edx is empty here, so we can use it in both branches
  mov  r12, [data : %CORE_GC_TABLE + gc_yg_end]
  add  rcx, rax
  cmp  rcx, r12
  jae  short labYGCollect
  mov  [data : %CORE_GC_TABLE + gc_yg_current], rcx

  // ; GCXT: clear the lock flag. Nothing else writes it, so the page comes back carrying
  // ; whatever the dead object left there and trylock spins on a byte that never reads
  // ; zero. Only shows up once a collection has recycled the YG, a fresh one is zero
  // ; filled by the OS
  mov  dword ptr [rax + elSyncForwardOffset], edx

  mov  edx, 0FFFFFFFFh
  lea  rbx, [rax + elObjectOffset]

  // ; GCXT: free lock
  // ; could we use mov [esi], 0 instead?
  lock xadd [rdi], edx

  ret

labYGCollect:
  // ; save registers
  sub  rcx, rax

  // ; the entry through GC_ALLOC carries one return address more than the one through
  // ; system 1 and system 2, so pad here to make both arrive with the same alignment.
  // ; it has to be nil, this range is scanned for roots
  xor  eax, eax
  push rax

  call %GC_COLLECT

  pop  rax
  ret

end

// ; --- GC_COLLECT ---
// ; in: ecx - fullmode (0, 1)
inline % GC_COLLECT

labStart:
  // ; GCXT: find the current thread entry
#if _WIN
  mov  rdi, gs:[58h]
#elif (_LNX || _FREEBSD)
  mov  rdi, fs:[0]
#endif

  push r10
  push r11

  // ; GCXT: find the current thread entry
#if _WIN
  mov  rax, [rdi]
#elif (_LNX || _FREEBSD)
  lea  rax, [rdi-tt_size]
#endif

  push rbp

  // ; GCXT: lock frame
  // ; get current thread event
  mov  rsi, [rax + tt_sync_event]
  mov  [rax + tt_stack_frame], rsp

  // ; the table walk uses r13 as its cursor and rbx as its counter, because rsi and rdi
  // ; are argument registers on System V and do not survive a call. r12 and r13 are not
  // ; saved : this is never entered from external code, and a stacked copy inside the
  // ; range the root scan walks would be taken for an object reference
  push rdx
  push rcx

  // ; === GCXT: safe point ===
  mov  rdx, [data : %CORE_GC_TABLE + gc_signal]
  // ; if it is a collecting thread, starts the GC
  test rdx, rdx                       
  jz   short labConinue
  // ; otherwise eax contains the collecting thread event

  // ; a thread waiting on the barrier is still listed by the scan of the next collection,
  // ; which then waits for a signal it cannot give until that collection ends. Mark the
  // ; safe region so those scans skip it, the frame stays frozen while it sleeps and the
  // ; root scan walks it as usual. r13 is scratch here, see the note in GC_COLLECT
  mov  ecx, dword ptr [rax + tt_flags]
  mov  r13, rcx
  mov  dword ptr [rax + tt_flags], 1

  sub  rsp, 30h

  // ; signal the collecting thread that it is stopped
#if _WIN
  mov  rcx, rsi
#elif (_LNX || _FREEBSD)
  mov  rdi, rsi
#endif
  call extern "$rt.SignalStopGCLA"

  // ; free lock
  // ; could we use mov [esi], 0 instead?
  mov  rdi, data : %CORE_GC_TABLE + gc_lock
  mov  ebx, 0FFFFFFFFh
  lock xadd [rdi], ebx

  // ; stop until this collection ends. The barrier is the collection state, not the
  // ; event of the thread that happens to be collecting : the table scan of every
  // ; collection resets every event, that one included
  call extern "$rt.WaitForCollectionGCLA"
  add  rsp, 30h
  // ; restore registers and try again

  // ; leave the safe region, the calls above clobbered rax so find the entry again
#if _WIN
  mov  rcx, gs:[58h]
  mov  rax, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rax, [rcx-tt_size]
#endif
  mov  rcx, r13
  mov  dword ptr [rax + tt_flags], ecx

  pop  rcx
  pop  rdx
  pop  rbp
  pop  r11
  pop  r10

  test rcx, rcx
  jnz  short labRepeatAlloc

  // ; the lock was released before the wait above, but labStart runs under it :
  // ; labConinue publishes gc_signal and walks the thread table, then releases a lock
  // ; this thread no longer owns. Only system 1 and system 2 come through here, they
  // ; are the ones that call with a zero size
  mov  rdi, data : %CORE_GC_TABLE + gc_lock
labRetakeLock:
  mov  edx, 1
  xor  eax, eax
  lock cmpxchg dword ptr[rdi], edx
  jnz  short labRetakeLock

  jmp  labStart

labRepeatAlloc:

  // ; repeat the alloc operation if required. The pad restores the parity of a generated
  // ; call site : without it a nested collection runs with every call misaligned.
  // ; it has to be nil, this range is scanned for roots
  xor  eax, eax
  push rax
  call %GC_ALLOC
  pop  rax
  ret

labConinue:
  mov  [data : %CORE_GC_TABLE + gc_signal], rsi // set the collecting thread signal
  mov  rbp, rsp

  // ; === thread synchronization ===

  // ; create list of threads need to be stopped
  mov  rax, rsi
  // ; get tls entry address  
  mov  r13, data : %CORE_THREAD_TABLE + tt_slots
  xor  ecx, ecx
  mov  rbx, [r13 - 8]
labNext:
  mov  rdx, [r13]

  // ; advance on both paths, a null slot used to make the loop re-scan the same entry
  // ; and leave every higher index thread unstopped
  lea  r13, [r13 + 16]

  test rdx, rdx
  jz   short labSkipTT
  cmp  rax, [rdx + tt_sync_event]
  setz cl
  or   ecx, dword ptr [rdx + tt_flags]
  test ecx, 1
  // ; skip current thread signal / thread in safe region from wait list
  jnz  short labSkipSave
  push [rdx + tt_sync_event]
labSkipSave:

  // ; reset all signal events. The handle pushes above leave rsp on either parity, so
  // ; align it explicitly : the callee is free to spill xmm registers on its stack.
  // ; r12 is dead here and callee saved, it carries the unaligned rsp across the call
  mov  r12, rsp
  sub  rsp, 30h
  and  rsp, 0FFFFFFF0h
#if _WIN
  mov  rcx, [rdx + tt_sync_event]
#elif (_LNX || _FREEBSD)
  mov  rdi, [rdx + tt_sync_event]
#endif
  call extern "$rt.SignalClearGCLA"
  mov  rsp, r12

  // ; the own event comes from the thread entry, not from gc_signal : reloading gc_signal
  // ; here makes the scan fail to recognise the collecting thread whenever that value is
  // ; not its own event, and it pushes its own handle on the wait list, then waits for a
  // ; signal only it could give
#if _WIN
  mov  rax, gs:[58h]
  mov  rax, [rax]
#elif (_LNX || _FREEBSD)
  mov  rax, fs:[0]
  lea  rax, [rax-tt_size]
#endif
  mov  rax, [rax + tt_sync_event]
labSkipTT:
  sub  rbx, 1
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

  // ; wait until they all stopped. One handle was pushed per stopped thread, so rsp can
  // ; be on either parity here, align it for the call, labSkipWait restores it from rbp
  shr  ebx, 3
  sub  rsp, 30h
  and  rsp, 0FFFFFFF0h
#if _WIN
  mov  ecx, ebx
#elif (_LNX || _FREEBSD)
  // ; rdx still holds the base of the wait list, set before the frame was reserved
  mov  edi, ebx
  mov  rsi, rdx
#endif
  call extern "$rt.WaitForSignalsGCLA"

labSkipWait:
  // ; remove list
  mov  rsp, rbp

  // ; take gc_lock again for the root scan and the collection. Every thread that had
  // ; to stop has signalled by now, so nobody needs the lock to make progress, while a
  // ; thread in teardown does : without this it can null its slot and let the OS free
  // ; the TLS holding its ThreadContent while the scan below is walking it
  mov  rdi, data : %CORE_GC_TABLE + gc_lock
labRootLock:
  mov  edx, 1
  xor  eax, eax
  lock cmpxchg dword ptr[rdi], edx
  jnz  short labRootLock

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

  // ; get the thread local roots. The variables sit in front of the thread content on the
  // ; System V targets, because the block ends at the thread pointer and the content is
  // ; found as tp - tt_size : anything past it belongs to the C library
  mov  rax, rdata : %SYSTEM_ENV
  mov  rcx, [rax + env_tls_size]
  lea  rax, [rsi + tt_size]
  push rax
  push rcx

  // ; get the top frame pointer
  mov  rax, [rsi + tt_stack_frame]

  // ; a thread that has registered its slot but not yet reached a safe point or an
  // ; allocation still has a null frame chain, and walking it from zero faults right
  // ; here, while gc_lock is held : the handler then allocates and spins on that lock
  test rax, rax
  jz   short labYGNextThreadSkip

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

#if _WIN
  mov  r8,  [rbp+8]
  mov  rdx, [rbp]
  mov  rcx, rsp
#elif (_LNX || _FREEBSD)
  mov  rdx, [rbp+8]
  mov  rsi, [rbp]
  mov  rdi, rsp
#endif

  // ; restore frame to correctly display a call stack
  mov  rax, rbp
  mov  rbp, [rax+16]

  // ; call GC routine
  // ; rsp is aligned : both entry paths arrive at rsp % 16 == 8, the entry pushes
  // ; cancel it out, and the thread list was dropped by the mov rsp, rbp above
  sub  rsp, 30h
  mov  [rsp+28h], rax
  call extern "$rt.CollectGCLA"

  mov  r12, rax

  mov  rbp, [rsp+28h]

  // ; GCXT: release every thread stopped by this collection. Clearing gc_signal is what
  // ; the barrier waits on, so it has to come first
  xor  ebx, ebx
  mov  [data : %CORE_GC_TABLE + gc_signal], rbx
  call extern "$rt.SignalCollectionEndGCLA"

  add  rsp, 30h

  mov  rbx, r12

  // ; release the lock taken for the root scan
  mov  rdi, data : %CORE_GC_TABLE + gc_lock
  mov  edx, 0FFFFFFFFh
  lock xadd [rdi], edx

  mov  rsp, rbp
  pop  rcx
  pop  rdx
  pop  rbp
  pop  r11
  pop  r10

  ret

end

// --- GC_ALLOCPERM ---
// in: ecx - size ; out: ebx - created object
procedure %GC_ALLOCPERM

  // ; GCXT: set lock
labStart:
  mov  rdi, data : %CORE_GC_TABLE + gc_lock

labWait:
  mov edx, 1
  xor eax, eax
  lock cmpxchg dword ptr[rdi], edx
  jnz  short labWait

  mov  rax, [data : %CORE_GC_TABLE + gc_perm_current]
  xor  edx, edx    // ; edx is empty here, so we can use it in both branches
  mov  r12, [data : %CORE_GC_TABLE + gc_perm_end]
  add  rcx, rax
  cmp  rcx, r12
  jae  short labPERMCollect
  mov  [data : %CORE_GC_TABLE + gc_perm_current], rcx

  // ; GCXT: clear the lock flag, see the note in GC_ALLOC
  mov  dword ptr [rax + elSyncForwardOffset], edx

  mov  edx, 0FFFFFFFFh
  lea  rbx, [rax + elObjectOffset]

  // ; GCXT: free lock
  // ; could we use mov [esi], 0 instead?
  lock xadd [rdi], edx

  ret

labPERMCollect:
  sub  rcx, rax

  // ; GCXT: find the current thread entry
#if _WIN
  mov  rdi, gs:[58h]
#elif (_LNX || _FREEBSD)
  mov  rdi, fs:[0]
#endif

  push r10
  push r11

  // ; GCXT: find the current thread entry
#if _WIN
  mov  rax, [rdi]
#elif (_LNX || _FREEBSD)
  lea  rax, [rdi-tt_size]
#endif

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

  // ; a thread waiting on the barrier is still listed by the scan of the next collection,
  // ; which then waits for a signal it cannot give until that collection ends. Mark the
  // ; safe region so those scans skip it, the frame stays frozen while it sleeps and the
  // ; root scan walks it as usual. r13 is scratch here, see the note in GC_COLLECT
  mov  ecx, dword ptr [rax + tt_flags]
  mov  r13, rcx
  mov  dword ptr [rax + tt_flags], 1

  sub  rsp, 30h

  // ; signal the collecting thread that it is stopped
#if _WIN
  mov  rcx, rsi
#elif (_LNX || _FREEBSD)
  mov  rdi, rsi
#endif
  call extern "$rt.SignalStopGCLA"

  // ; free lock
  // ; could we use mov [esi], 0 instead?
  mov  rdi, data : %CORE_GC_TABLE + gc_lock
  mov  ebx, 0FFFFFFFFh
  lock xadd [rdi], ebx

  // ; stop until this collection ends, see the note in GC_COLLECT
  call extern "$rt.WaitForCollectionGCLA"
  add  rsp, 30h
  // ; restore registers and try again

  // ; leave the safe region, the calls above clobbered rax so find the entry again
#if _WIN
  mov  rcx, gs:[58h]
  mov  rax, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rax, [rcx-tt_size]
#endif
  mov  rcx, r13
  mov  dword ptr [rax + tt_flags], ecx

  pop  rcx
  pop  rdx
  pop  rbp
  pop  r11
  pop  r10

  // ; retry in the same generation, labStart takes the lock again. This used to fall
  // ; into %GC_ALLOC, which satisfies a permanent allocation from the young generation
  jmp  labStart

labConinue:
  mov  [data : %CORE_GC_TABLE + gc_signal], rsi // set the collecting thread signal
  mov  rbp, rsp

  // ; === thread synchronization ===

  // ; create list of threads need to be stopped
  mov  rax, rsi
  // ; get tls entry address
  mov  r13, data : %CORE_THREAD_TABLE + tt_slots
  xor  ecx, ecx
  mov  rbx, [r13 - 8]
labNext:
  mov  rdx, [r13]

  // ; advance on both paths, a null slot used to make the loop re-scan the same entry
  // ; and leave every higher index thread unstopped
  lea  r13, [r13 + 16]

  test rdx, rdx
  jz   short labSkipTT
  cmp  rax, [rdx + tt_sync_event]
  setz cl
  or   ecx, dword ptr [rdx + tt_flags]
  test ecx, 1
  // ; skip current thread signal / thread in safe region from wait list
  jnz  short labSkipSave
  push [rdx + tt_sync_event]
labSkipSave:

  // ; reset all signal events. The handle pushes above leave rsp on either parity, so
  // ; align it explicitly : the callee is free to spill xmm registers on its stack.
  // ; r12 is dead here and callee saved, it carries the unaligned rsp across the call
  mov  r12, rsp
  sub  rsp, 30h
  and  rsp, 0FFFFFFF0h
#if _WIN
  mov  rcx, [rdx + tt_sync_event]
#elif (_LNX || _FREEBSD)
  mov  rdi, [rdx + tt_sync_event]
#endif
  call extern "$rt.SignalClearGCLA"
  mov  rsp, r12

  // ; the own event comes from the thread entry, not from gc_signal : reloading gc_signal
  // ; here makes the scan fail to recognise the collecting thread whenever that value is
  // ; not its own event, and it pushes its own handle on the wait list, then waits for a
  // ; signal only it could give
#if _WIN
  mov  rax, gs:[58h]
  mov  rax, [rax]
#elif (_LNX || _FREEBSD)
  mov  rax, fs:[0]
  lea  rax, [rax-tt_size]
#endif
  mov  rax, [rax + tt_sync_event]
labSkipTT:
  sub  rbx, 1
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

  // ; wait until they all stopped. One handle was pushed per stopped thread, so rsp can
  // ; be on either parity here, align it for the call, labSkipWait restores it from rbp
  shr  ebx, 3
  sub  rsp, 30h
  and  rsp, 0FFFFFFF0h
#if _WIN
  mov  ecx, ebx
#elif (_LNX || _FREEBSD)
  // ; rdx still holds the base of the wait list, set before the frame was reserved
  mov  edi, ebx
  mov  rsi, rdx
#endif
  call extern "$rt.WaitForSignalsGCLA"

labSkipWait:
  // ; remove list
  mov  rsp, rbp     

  // ==== GCXT end ==============

  // ; rsp is aligned : the generated code calls us aligned, the entry pushes cancel
  // ; the return address, and the thread list was dropped by the mov rsp, rbp above
  sub  rsp, 30h

#if _WIN
  mov  rcx, [rbp]
#elif (_LNX || _FREEBSD)
  mov  rdi, [rbp]
#endif
  call extern "$rt.CollectPermGCLA"

  mov  r12, rax

  // ; GCXT: release every thread stopped by this collection, see the note in GC_COLLECT
  xor  ebx, ebx
  mov  [data : %CORE_GC_TABLE + gc_signal], rbx
  call extern "$rt.SignalCollectionEndGCLA"

  mov  rbx, r12

  // ; 40h drops the shadow space plus the rcx and rdx pushed on entry
  add  rsp, 40h

  pop  rbp
  pop  r11
  pop  r10
  ret

end

// --- THREAD_WAIT ---
// GCXT: it is presumed that gc lock is off, it is taken here

procedure % THREAD_WAIT

  // ; everything that can hold an object goes ABOVE the frame marker, inside the range the
  // ; root scan walks, so it comes back relocated instead of stale : rbx is the
  // ; accumulator and r10 / r11 are the cached arguments. GC_COLLECT already saves the
  // ; same three that way. Below the marker only scratch may live, a raw pointer there
  // ; would be read as an object reference
  push rbx
  push r10
  push r11
  push rbp
  mov  rdi, rsp

  // ; snop is the only caller and it saves nothing, and the two calls below are free to
  // ; clobber all of these. Measured : dropping rax and rsi here costs runs in the churn
  // ; tests, so they stay saved together with rdx
  push rdx
  push rsi
  push rax

  // ; set lock
  mov  rbx, data : %CORE_GC_TABLE + gc_lock
labWait:
  mov edx, 1
  xor eax, eax
  lock cmpxchg dword ptr[rbx], edx
  jnz  short labWait

  // ; find the current thread entry
#if _WIN
  mov  rdx, gs:[58h]
  mov  rax, [rdx]
#elif (_LNX || _FREEBSD)
  mov  rdx, fs:[0]
  lea  rax, [rdx-tt_size]
#endif

  mov  rsi, [rax+tt_sync_event]   // ; get current thread event

  // ; the head published here describes the frame of this procedure, and that frame dies
  // ; on the ret. Every caller is inside a snop, so the head has to go back to what it was
  // ; or the next collection walks a chain that lives on dead stack : the slot then holds
  // ; whatever the thread pushed since, and a heap word there is read as a frame link
  push [rax+tt_stack_frame]
  mov  [rax+tt_stack_frame], rdi  // ; lock stack frame

  // ; snop read gc_signal without the lock, so that collection may have ended by now,
  // ; or another one may have started. Re-read it here, where the lock is held
  mov  rdx, [data : %CORE_GC_TABLE + gc_signal]
  test rdx, rdx
  jz   short labNoCollect

  // ; mark the safe region before parking, see the note in GC_COLLECT. No callee saved
  // ; register is free here, so the old value goes to a stack slot : through exclude this
  // ; is entered with tt_flags already one and the restore has to put that back
  push [rax + tt_flags]
  mov  dword ptr [rax + tt_flags], 1

  // ; signal the collecting thread that it is stopped
  // ; nine pushes leave rsp on the call boundary, hence 30h
  sub  rsp, 30h
#if _WIN
  mov  rcx, rsi
#elif (_LNX || _FREEBSD)
  mov  rdi, rsi
#endif

  call extern "$rt.SignalStopGCLA"

  // ; free lock. The address cannot be loaded before the call the way the x32 core does
  // ; it : there edi is callee saved, here rdi is an argument register and comes back
  // ; from the call holding whatever the callee left in it
  mov  rdi, data : %CORE_GC_TABLE + gc_lock
  mov  ebx, 0FFFFFFFFh
  lock xadd [rdi], ebx

  // ; stop until this collection ends, see the note in GC_COLLECT. The
  // ; shadow space above is still reserved, the callee is free to spill into it
  call extern "$rt.WaitForCollectionGCLA"
  add  rsp, 30h

  // ; leave the safe region, see the note in GC_COLLECT
  pop  rdx
#if _WIN
  mov  rcx, gs:[58h]
  mov  rax, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rax, [rcx-tt_size]
#endif
  mov  dword ptr [rax + tt_flags], edx

  // ; give the caller its head back, see the note above
  pop  rdx
  mov  [rax+tt_stack_frame], rdx

  pop  rax
  pop  rsi
  pop  rdx
  add  rsp, 8                     // ; the rbp of the frame marker
  pop  r11
  pop  r10
  pop  rbx

  ret

labNoCollect:
  // ; the collection ended before we got the lock, nothing to wait for
  mov  rdi, data : %CORE_GC_TABLE + gc_lock
  mov  ebx, 0FFFFFFFFh
  lock xadd [rdi], ebx

  // ; give the caller its head back, see the note above. rax still holds the entry here,
  // ; this path made no call
  pop  rdx
  mov  [rax+tt_stack_frame], rdx

  pop  rax
  pop  rsi
  pop  rdx
  add  rsp, 8
  pop  r11
  pop  r10
  pop  rbx

  ret

end

// ; --- System Core Preloaded Routines --

// ; ==== Command Set ==

// ; snop
inline % 2

  // ; safe point
  mov  rdx, [data : %CORE_GC_TABLE + gc_signal]
  test rdx, rdx                       // ; if it is a collecting thread, waits
  jz   short labConinue               // ; otherwise goes on

  nop
  nop
  call %THREAD_WAIT                   // ; waits until the GC is stopped

labConinue:

end

// ; throw
inline %0Ah

#if _WIN
  mov  rcx, gs:[58h]
  mov  rcx, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rcx, [rcx-tt_size]
#endif
  mov  rdi, [rcx + et_current]
  jmp  [rdi + es_catch_addr]

end

// ; unhook
inline %0Bh

  // ; GCXT: get current thread frame
#if _WIN
  mov  rcx, gs:[58h]
  mov  rcx, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rcx, [rcx-tt_size]
#endif
  mov  rdi, [rcx + et_current]

  mov  rax, [rdi + es_prev_struct]
  mov  rbp, [rdi + es_catch_frame]
  mov  rsp, [rdi + es_catch_level]

  mov  [rcx + et_current], rax

end

// ; exclude
inline % 10h

#if _WIN
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rdi, [rcx-tt_size]
#endif

  // ; the store below has to be ordered against the gc_signal load that follows. On a
  // ; plain mov the two can be reordered, and then the collector reads tt_flags as zero,
  // ; puts this thread on its wait list, while the thread reads gc_signal as zero and
  // ; goes on to block inside the foreign call, where no safe point follows. cmpxchg is
  // ; the only locked store this assembler emits, and it leaves a nested exclude alone
  mov  edx, 1
  xor  eax, eax
  lock cmpxchg dword ptr [rdi + tt_flags], edx

  mov  rax, [data : %CORE_GC_TABLE + gc_signal]
  test rax, rax
  jz   short labNoCollect

  // ; a collection is already counting on this thread to stop. Do it here, where a safe
  // ; point still exists, rather than inside the call it is about to make
  call %THREAD_WAIT

#if _WIN
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rdi, [rcx-tt_size]
#endif

labNoCollect:
  mov  rax, [rdi + tt_stack_frame]
  push rax
  push rbp
  mov  [rdi + tt_stack_frame], rsp

end

// ; include
inline % 11h

  add  rsp, 8

  // ; leaving the safe region has to be serialised against the root scan. The collector
  // ; keeps a thread in a safe region out of its wait list, but it still walks the frame
  // ; chain published at exclude, and that chain dies the moment this thread restores it
  // ; and runs on. gc_lock is held for the whole scan, so taking it here waits the scan
  // ; out. Same registers exclude already clobbers
  mov  rcx, data : %CORE_GC_TABLE + gc_lock
labWait:
  mov  edx, 1
  xor  eax, eax
  lock cmpxchg dword ptr[rcx], edx
  jnz  short labWait

#if _WIN
  mov  rdi, gs:[58h]
  mov  rdi, [rdi]
#elif (_LNX || _FREEBSD)
  mov  rdi, fs:[0]
  lea  rdi, [rdi-tt_size]
#endif
  mov  dword ptr [rdi + tt_flags], 0
  pop  rax
  mov  [rdi + tt_stack_frame], rax

  mov  edx, 0FFFFFFFFh
  lock xadd [rcx], edx

end

// ; tststck
inline %17h

  // ; COREX
#if _WIN
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rdi, [rcx-tt_size]
#endif
  mov  rax, [rdi + tt_stack_root]

  xor  ecx, ecx
  cmp  rbx, rsp
  setl cl
  cmp  rbx, rax
  setg ch
  test ecx, ecx

end

// ; trylock
inline %02Bh

  // ; GCXT: try to lock
  xor  eax, eax
  mov  ecx, 1
  lock cmpxchg byte ptr[rbx - elSyncOffset], cl
  test eax, eax 

end

// ; freelock
inline %02Ch

  mov  ecx, -1

  // ; free lock
  lock xadd byte ptr [rbx - elSyncOffset], cl

end

// ; peektls
inline %0BBh

#if _WIN
  mov  rcx, gs:[58h]
  mov  rax, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rax, [rcx-tt_size]
#endif
  lea  rdi, [rax + __arg32_1]
  mov  rbx, [rdi]

end

// ; storetls
inline %0BCh

#if _WIN
  mov  rcx, gs:[58h]
  mov  rax, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rax, [rcx-tt_size]
#endif
  lea  rdi, [rax + __arg32_1]
  mov  [rdi], rbx

end

// ; extclosen
inline %0CAh

  add  rbp, __n_1
  mov  rsp, rbp
  pop  rbp

  add  rsp, 16
  pop  rbx

#if _WIN
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rdi, [rcx-tt_size]
#endif
  mov  [rdi + tt_stack_frame], rbx

  pop  rbp
  pop  r15
  pop  r14
  pop  r13
  pop  r12
  pop  rbx

#if _WIN

  pop  rdi
  pop  rsi
  add  rsp, 8

#elif (_LNX || _FREEBSD)

  // ; rdi, rsi, rdx, rcx and the pushed zero, see extopen
  add  rsp, 40

#endif

end

// ; extclosen 0
inline %1CAh

  mov  rsp, rbp
  pop  rbp

  add  rsp, 16
  pop  rbx

#if _WIN
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rdi, [rcx-tt_size]
#endif
  mov  [rdi + tt_stack_frame], rbx

  pop  rbp
  pop  r15
  pop  r14
  pop  r13
  pop  r12
  pop  rbx

#if _WIN

  pop  rdi
  pop  rsi
  add  rsp, 8

#elif (_LNX || _FREEBSD)

  // ; rdi, rsi, rdx, rcx and the pushed zero, see extopen
  add  rsp, 40

#endif
  
end


// ; system minor collect
inline %1CFh

  mov  rdi, data : %CORE_GC_TABLE + gc_lock

labWait:
  mov edx, 1
  xor eax, eax
  lock cmpxchg dword ptr[rdi], edx
  jnz  short labWait

  xor  rcx, rcx
  xor  rdx, rdx
  call %GC_COLLECT

end

// ; system full collect
inline %2CFh

  mov  rdi, data : %CORE_GC_TABLE + gc_lock

labWait:
  mov edx, 1
  xor eax, eax
  lock cmpxchg dword ptr[rdi], edx
  jnz  short labWait

  xor  rcx, rcx
  mov  edx, 1
  call %GC_COLLECT

end

// ; system 3 (thread startup)
inline %3CFh

#if _WIN
  mov  rcx, gs:[58h]
  mov  rax, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rax, [rcx-tt_size]
#endif
  mov  rdi, data : %CORE_THREAD_TABLE + tt_slots
  shl  rdx, 4 
  mov  [rdi + rdx], rax
  shr  rdx, 4 

  mov  [rax + tt_stack_root], rsp

end

// ; system startup
inline %4CFh

  finit

#if _FREEBSD

  push 0
  mov  rax, rdi

#elif (_LNX || _FREEBSD)

  mov  rax, rsp

#endif

#if _WIN

  mov  rax, rsp
  call %PREPARE

#elif (_LNX || _FREEBSD)

  call %PREPARE

  // ; the Linux branch was lost when this was translated from the STA core. Without the
  // ; extra push rsp stays odd for the whole program, and the first runtime call that
  // ; spills an SSE register faults. tt_stack_root is not set here as it is in the STA
  // ; core : under MTA it belongs to the thread and system 3 publishes it
  xor  rbp, rbp
  push rbp                 // ; note an extra push to simulate the function entry

#endif

end

// ; system : enter GC critical section
inline %6CFh

  mov  rdi, data : %CORE_GC_TABLE + gc_lock
  mov  ecx, 1
labWait:
  xor  eax, eax
  lock cmpxchg dword ptr[rdi], ecx
  jnz  short labWait

end

// ; system : leave GC critical section
inline %7CFh

  // ; GCXT: clear sync field
  mov  rdi, data : %CORE_GC_TABLE + gc_lock
  mov  ecx, 0FFFFFFFFh
  
  // ; GCXT: free lock
  // ; could we use mov [esi], 0 instead?
  lock xadd [rdi], ecx

end

// ; system : safe point for a thread that has just been registered
// ; The collector only walks the thread table under gc_lock, so a thread reaching
// ; system 3 after that walk is invisible to the collection already running and would
// ; read its argument out of the heap while the collector compacts. Stop here instead,
// ; right after the critical section is left.
// ; THREAD_WAIT publishes the stack as the head of the frame chain and the root scan
// ; walks it from there. This thread owns no roots yet, so build a chain that terminates
// ; at once and point rbp at it. Four slots keep rsp 16 byte aligned
inline %8CFh

  mov  rdx, [data : %CORE_GC_TABLE + gc_signal]
  test rdx, rdx                       
  jz   short labConinue

  push rbp
  xor  rax, rax
  push rax
  push rax
  push rax
  mov  rbp, rsp

  call %THREAD_WAIT

  // ; THREAD_WAIT leaves tt_stack_frame pointing at the stack it used, and that stack is
  // ; gone as soon as this returns. The thread still owns no roots, so clear it while
  // ; the region is still ours : the root scan skips a null frame, and the first safe
  // ; point inside the worker publishes a live one. Leaving the dead frame behind hangs
  // ; the collector, which walks it holding gc_lock
#if _WIN
  mov  rax, gs:[58h]
  mov  rdi, [rax]
#elif (_LNX || _FREEBSD)
  mov  rax, fs:[0]
  lea  rdi, [rax-tt_size]
#endif
  xor  rax, rax
  mov  [rdi + tt_stack_frame], rax

  add  rsp, 24
  pop  rbp

labConinue:

end

// ; xhookdpr
inline %0E6h

  // ; GCXT: get current thread frame
#if _WIN
  mov  rcx, gs:[58h]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
#endif
  lea  rdi, [rbp + __arg32_1]
#if _WIN
  mov  rax, [rcx]
#elif (_LNX || _FREEBSD)
  lea  rax, [rcx-tt_size]
#endif

  mov  rcx, [rax + et_current]
  mov  [rdi + es_catch_frame], rbp
  mov  [rdi + es_prev_struct], rcx
  mov  [rdi + es_catch_level], rsp
  mov  rcx, __ptr64_2
  mov  [rdi + es_catch_addr], rcx

  mov  [rax + et_current], rdi

end

// ; extopenin
inline %0F2h

#if _WIN

  mov  [rsp+8], rcx
  mov  [rsp+16], rdx
  mov  [rsp+24], r8
  mov  [rsp+32], r9

  push 0
  push rsi
  push rdi
  push rbx

#elif (_LNX || _FREEBSD)

  // ; the arguments arrive in rdi and rsi here, and there is no home area to spill them
  // ; into. Pushing them builds the same frame the Windows branch gets from the spill,
  // ; and the matching extclosen drops five slots instead of three
  push 0
  push rcx
  push rdx
  push rsi
  push rdi
  push rbx

#endif

  push r12
  push r13
  push r14
  push r15

  push rbp     

#if _WIN
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rdi, [rcx-tt_size]
#endif
  mov  rax, [rdi + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  sub  rsp, __n_2
  push rbp
  push rax
  mov  rbp, rsp
  mov  rcx, __n_1
  sub  rsp, __arg32_1
  mov  rdi, rsp
  rep  stos
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 0, n
inline %1F2h

#if _WIN

  mov  [rsp+8], rcx
  mov  [rsp+16], rdx
  mov  [rsp+24], r8
  mov  [rsp+32], r9

  push 0
  push rsi
  push rdi
  push rbx

#elif (_LNX || _FREEBSD)

  // ; the arguments arrive in rdi and rsi here, and there is no home area to spill them
  // ; into. Pushing them builds the same frame the Windows branch gets from the spill,
  // ; and the matching extclosen drops five slots instead of three
  push 0
  push rcx
  push rdx
  push rsi
  push rdi
  push rbx

#endif

  push r12
  push r13
  push r14
  push r15

  push rbp     

#if _WIN
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rdi, [rcx-tt_size]
#endif
  mov  rax, [rdi + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  sub  rsp, __n_2
  push rbp
  push rax
  mov  rbp, rsp
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 1, n
inline %2F2h

#if _WIN

  mov  [rsp+8], rcx
  mov  [rsp+16], rdx
  mov  [rsp+24], r8
  mov  [rsp+32], r9

  push 0
  push rsi
  push rdi
  push rbx

#elif (_LNX || _FREEBSD)

  // ; the arguments arrive in rdi and rsi here, and there is no home area to spill them
  // ; into. Pushing them builds the same frame the Windows branch gets from the spill,
  // ; and the matching extclosen drops five slots instead of three
  push 0
  push rcx
  push rdx
  push rsi
  push rdi
  push rbx

#endif

  push r12
  push r13
  push r14
  push r15

  push rbp     

#if _WIN
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rdi, [rcx-tt_size]
#endif
  mov  rax, [rdi + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  sub  rsp, __n_2
  push rbp
  push rax
  mov  rbp, rsp
  push rax
  push rax
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 2, n
inline %3F2h

#if _WIN

  mov  [rsp+8], rcx
  mov  [rsp+16], rdx
  mov  [rsp+24], r8
  mov  [rsp+32], r9

  push 0
  push rsi
  push rdi
  push rbx

#elif (_LNX || _FREEBSD)

  // ; the arguments arrive in rdi and rsi here, and there is no home area to spill them
  // ; into. Pushing them builds the same frame the Windows branch gets from the spill,
  // ; and the matching extclosen drops five slots instead of three
  push 0
  push rcx
  push rdx
  push rsi
  push rdi
  push rbx

#endif

  push r12
  push r13
  push r14
  push r15

  push rbp     

#if _WIN
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rdi, [rcx-tt_size]
#endif
  mov  rax, [rdi + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  sub  rsp, __n_2
  push rbp
  push rax
  mov  rbp, rsp
  push rax
  push rax
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 3, n
inline %4F2h

#if _WIN

  mov  [rsp+8], rcx
  mov  [rsp+16], rdx
  mov  [rsp+24], r8
  mov  [rsp+32], r9

  push 0
  push rsi
  push rdi
  push rbx

#elif (_LNX || _FREEBSD)

  // ; the arguments arrive in rdi and rsi here, and there is no home area to spill them
  // ; into. Pushing them builds the same frame the Windows branch gets from the spill,
  // ; and the matching extclosen drops five slots instead of three
  push 0
  push rcx
  push rdx
  push rsi
  push rdi
  push rbx

#endif

  push r12
  push r13
  push r14
  push r15

  push rbp     

#if _WIN
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rdi, [rcx-tt_size]
#endif
  mov  rax, [rdi + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  sub  rsp, __n_2
  push rbp
  push rax
  mov  rbp, rsp
  push rax
  push rax
  push rax
  push rax
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 4, n
inline %5F2h

#if _WIN

  mov  [rsp+8], rcx
  mov  [rsp+16], rdx
  mov  [rsp+24], r8
  mov  [rsp+32], r9

  push 0
  push rsi
  push rdi
  push rbx

#elif (_LNX || _FREEBSD)

  // ; the arguments arrive in rdi and rsi here, and there is no home area to spill them
  // ; into. Pushing them builds the same frame the Windows branch gets from the spill,
  // ; and the matching extclosen drops five slots instead of three
  push 0
  push rcx
  push rdx
  push rsi
  push rdi
  push rbx

#endif

  push r12
  push r13
  push r14
  push r15

  push rbp     

#if _WIN
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rdi, [rcx-tt_size]
#endif
  mov  rax, [rdi + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  sub  rsp, __n_2
  push rbp
  push rax
  mov  rbp, rsp
  push rax
  push rax
  push rax
  push rax
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin i, 0
inline %6F2h

#if _WIN

  mov  [rsp+8], rcx
  mov  [rsp+16], rdx
  mov  [rsp+24], r8
  mov  [rsp+32], r9

  push 0
  push rsi
  push rdi
  push rbx

#elif (_LNX || _FREEBSD)

  // ; the arguments arrive in rdi and rsi here, and there is no home area to spill them
  // ; into. Pushing them builds the same frame the Windows branch gets from the spill,
  // ; and the matching extclosen drops five slots instead of three
  push 0
  push rcx
  push rdx
  push rsi
  push rdi
  push rbx

#endif

  push r12
  push r13
  push r14
  push r15

  push rbp     

#if _WIN
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rdi, [rcx-tt_size]
#endif
  mov  rax, [rdi + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  mov  rcx, __n_1
  sub  rsp, __arg32_1
  mov  rdi, rsp
  rep  stos
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 0, 0
inline %7F2h

#if _WIN

  mov  [rsp+8], rcx
  mov  [rsp+16], rdx
  mov  [rsp+24], r8
  mov  [rsp+32], r9

  push 0
  push rsi
  push rdi
  push rbx

#elif (_LNX || _FREEBSD)

  // ; the arguments arrive in rdi and rsi here, and there is no home area to spill them
  // ; into. Pushing them builds the same frame the Windows branch gets from the spill,
  // ; and the matching extclosen drops five slots instead of three
  push 0
  push rcx
  push rdx
  push rsi
  push rdi
  push rbx

#endif

  push r12
  push r13
  push r14
  push r15

  push rbp     

#if _WIN
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rdi, [rcx-tt_size]
#endif
  mov  rax, [rdi + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  mov  rbp, rsp
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 1, 0
inline %8F2h

#if _WIN

  mov  [rsp+8], rcx
  mov  [rsp+16], rdx
  mov  [rsp+24], r8
  mov  [rsp+32], r9

  push 0
  push rsi
  push rdi
  push rbx

#elif (_LNX || _FREEBSD)

  // ; the arguments arrive in rdi and rsi here, and there is no home area to spill them
  // ; into. Pushing them builds the same frame the Windows branch gets from the spill,
  // ; and the matching extclosen drops five slots instead of three
  push 0
  push rcx
  push rdx
  push rsi
  push rdi
  push rbx

#endif

  push r12
  push r13
  push r14
  push r15

  push rbp     

#if _WIN
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rdi, [rcx-tt_size]
#endif
  mov  rax, [rdi + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  mov  rbp, rsp
  push 0
  push 0
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 2, 0
inline %9F2h

#if _WIN

  mov  [rsp+8], rcx
  mov  [rsp+16], rdx
  mov  [rsp+24], r8
  mov  [rsp+32], r9

  push 0
  push rsi
  push rdi
  push rbx

#elif (_LNX || _FREEBSD)

  // ; the arguments arrive in rdi and rsi here, and there is no home area to spill them
  // ; into. Pushing them builds the same frame the Windows branch gets from the spill,
  // ; and the matching extclosen drops five slots instead of three
  push 0
  push rcx
  push rdx
  push rsi
  push rdi
  push rbx

#endif

  push r12
  push r13
  push r14
  push r15

  push rbp     

#if _WIN
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rdi, [rcx-tt_size]
#endif
  mov  rax, [rdi + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  push rax
  push rax
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 3, 0
inline %0AF2h

#if _WIN

  mov  [rsp+8], rcx
  mov  [rsp+16], rdx
  mov  [rsp+24], r8
  mov  [rsp+32], r9

  push 0
  push rsi
  push rdi
  push rbx

#elif (_LNX || _FREEBSD)

  // ; the arguments arrive in rdi and rsi here, and there is no home area to spill them
  // ; into. Pushing them builds the same frame the Windows branch gets from the spill,
  // ; and the matching extclosen drops five slots instead of three
  push 0
  push rcx
  push rdx
  push rsi
  push rdi
  push rbx

#endif

  push r12
  push r13
  push r14
  push r15

  push rbp     

#if _WIN
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rdi, [rcx-tt_size]
#endif
  mov  rax, [rdi + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  push rax
  push rax
  push rax
  push rax
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 4, 0
inline %0BF2h

#if _WIN

  mov  [rsp+8], rcx
  mov  [rsp+16], rdx
  mov  [rsp+24], r8
  mov  [rsp+32], r9

  push 0
  push rsi
  push rdi
  push rbx

#elif (_LNX || _FREEBSD)

  // ; the arguments arrive in rdi and rsi here, and there is no home area to spill them
  // ; into. Pushing them builds the same frame the Windows branch gets from the spill,
  // ; and the matching extclosen drops five slots instead of three
  push 0
  push rcx
  push rdx
  push rsi
  push rdi
  push rbx

#endif

  push r12
  push r13
  push r14
  push r15

  push rbp     

#if _WIN
  mov  rcx, gs:[58h]
  mov  rdi, [rcx]
#elif (_LNX || _FREEBSD)
  mov  rcx, fs:[0]
  lea  rdi, [rcx-tt_size]
#endif
  mov  rax, [rdi + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  push rax
  push rax
  push rax
  push rax
  mov  r10, rax
  mov  r11, rax

end 

// VEH_HANDLER() 
procedure % VEH_HANDLER

#if _WIN

  mov  esi, edx
  mov  edx, eax   // ; set exception code

  mov  rcx, gs:[58h]
  mov  rcx, [rcx]
  jmp  [rcx]

#elif (_LNX || _FREEBSD)

  // ; without this branch the procedure assembles empty, and an empty section here is
  // ; worse than none : corex60 is an overlay that wins over core60 in the resolution,
  // ; so it shadows the working handler instead of falling through to it
  mov  r10, rdx
  mov  rdx, rax   // ; set exception code

  mov  rcx, fs:[0]
  lea  rcx, [rcx-tt_size]

  jmp  [rcx]

#endif

end
