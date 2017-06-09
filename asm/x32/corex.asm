// --- Predefined References  --
define GC_ALLOC	            10001h
define HOOK                 10010h
define INIT_RND             10012h
define INIT                 10013h
define NEWFRAME             10014h
define INIT_ET              10015h
define ENDFRAME             10016h
define RESTORE_ET           10017h
define OPENFRAME            10019h
define CLOSEFRAME           1001Ah
define NEWTHREAD            1001Bh
define CLOSETHREAD          1001Ch
define EXIT                 1001Dh
define CALC_SIZE            1001Fh
define GET_COUNT            10020h
define THREAD_WAIT          10021h
define NEW_HEAP             10025h
define BREAK                10026h
define PREPARE              10027h
define EXPAND_HEAP          10028h
define NEW_EVENT            10101h

define CORE_EXCEPTION_TABLE 20001h
define CORE_GC_TABLE        20002h
define CORE_GC_SIZE         20003h
define CORE_STAT_COUNT      20004h
define CORE_STATICROOT      20005h
define CORE_TLS_INDEX       20007h
define THREAD_TABLE         20008h
define CORE_OS_TABLE        20009h

// CORE GC SIZE OFFSETS
define gcs_MGSize	0000h
define gcs_YGSize	0004h
define gcs_TTSize       0008h

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
define gc_mg_wbar            002Ch
define gc_lock               0030h
define gc_signal             0034h
define tt_ptr                0038h
define tt_lock               003Ch

// GCXT TLS TABLE
define tls_stack_frame       0000h
define tls_stack_bottom      0004h
define tls_catch_addr        0008h
define tls_catch_level       000Ch
define tls_catch_frame       0010h
define tls_sync_event        0014h
define tls_flags             0018h

// Page Size
define page_size               10h
define page_size_order          4h
define page_size_order_minus2   2h
define page_mask        0FFFFFFF0h
define page_ceil               17h
define struct_mask         800000h

// Object header fields
define elObjectOffset        0008h
define elSyncOffset          0008h
define elSizeOffset          0008h
define elVMTOffset           0004h 
define elVMTFlagOffset       0008h
define elVMTSizeOffset       000Ch
define elPackageOffset       0010h

define subj_mask         80FFFFF0h
define page_align_mask   000FFFF0h

// --- System Core Preloaded Routines --

structure % CORE_EXCEPTION_TABLE

  dd 0 // ; dummy

end

structure %CORE_GC_TABLE

  dd 0 // ; gc_header             : +00h
  dd 0 // ; gc_start              : +04h
  dd 0 // ; gc_yg_start           : +08h
  dd 0 // ; gc_yg_current         : +0Ch
  dd 0 // ; gc_yg_end             : +10h
  dd 0 // ; gc_shadow             : +14h
  dd 0 // ; gc_shadow_end         : +18h
  dd 0 // ; gc_mg_start           : +1Ch
  dd 0 // ; gc_mg_current         : +20h
  dd 0 // ; gc_end                : +24h
  dd 0 // ; reserved              : +28h
  dd 0 // ; gc_mg_wbar            : +2Ch
  dd 0 // ; gc_lock               : +30h
  dd 0 // ; gc_signal             : +34h
  dd 0 // ; tt_ptr                : +38h
  dd 0 // ; tt_lock               : +3Ch

end

// --- GC_ALLOC ---
// in: ecx - counter ; ebx - size ; ecx - actual size ; out: eax - created object ; edi contains the object or zero
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
  mov  [eax], ebx
  mov  [data : %CORE_GC_TABLE + gc_yg_current], ecx
  
  // ; GCXT: clear sync field
  mov  edx, 0FFFFFFFFh
  lea  eax, [eax + elObjectOffset]
  
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
  push edi
  mov  eax, [edx+eax*4]
  push ebp
  
  // ; GCXT: lock stack frame
  // ; get current thread event
  mov  esi, [eax + tls_sync_event]         
  mov  [eax + tls_stack_frame], esp
  
  push ecx
  push ebx

  // ; === GCXT: safe point ===
  mov  edx, [data : %CORE_GC_TABLE + gc_signal]
  // ; if it is a collecting thread, starts the GC
  test edx, edx                       
  jz   short labConinue
  // ; otherwise eax contains the collecting thread event

  // ; signal the collecting thread that it is stopped
  push 0FFFFFFFFh // -1
  mov  edi, data : %CORE_GC_TABLE + gc_lock
  push edx
  push esi

  // ; signal the collecting thread that it is stopped
  call extern 'dlls'kernel32.SetEvent

  // ; free lock
  // ; could we use mov [esi], 0 instead?
  mov  ebx, 0FFFFFFFFh
  lock xadd [edi], ebx

  // ; stop until GC is ended
  call extern 'dlls'kernel32.WaitForSingleObject

  // ; restore registers and try again
  pop  ebx
  pop  ecx
  pop  ebp
  pop  edi

  jmp  labStart

labConinue:

  mov  [data : %CORE_GC_TABLE + gc_signal], esi // set the collecting thread signal
  mov  ebp, esp

  // ; === thread synchronization ===

  // ; create list of threads need to be stopped
  mov  eax, esi
  mov  edi, [data : %CORE_GC_TABLE + tt_ptr]
  // ; get tls entry address  
  mov  esi, data : %THREAD_TABLE
labNext:
  mov  edx, [esi]
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
  call extern 'dlls'kernel32.ResetEvent      

  lea  esi, [esi+4]
  mov  eax, [data : %CORE_GC_TABLE + gc_signal]
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
  push 0FFFFFFFFh // -1
  shr  ebx, 2
  push 0FFFFFFFFh // -1
  push ecx
  push ebx
  call extern 'dlls'kernel32.WaitForMultipleObjects

labSkipWait:
  // ; remove list
  mov  esp, ebp     

  // ==== GCXT end ==============
  
  // ; create set of roots
  mov  ebp, esp
  xor  ecx, ecx
  push ecx
  push ecx
  push ecx                                                              

  // ; save static roots
  mov  esi, data : %CORE_STATICROOT
  mov  ecx, [data : %CORE_STAT_COUNT]
  push esi
  push ecx

  // ; == GCXT: save frames ==
  mov  ebx, [data : %CORE_GC_TABLE + tt_ptr]

labYGNextThread:  
  sub  ebx, 1
  mov  eax, data : %THREAD_TABLE
  
  // ; get tls entry address
  mov  esi, [eax+ebx*4]            
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
  test ebx, ebx
  jnz  short labYGNextThread
  // ; == GCXT: end ==
  
  // === Minor collection ===
  mov [ebp-4], esp

  // ; save mg -> yg roots 
  mov  ebx, [data : %CORE_GC_TABLE + gc_mg_current]
  mov  edi, [data : %CORE_GC_TABLE + gc_mg_start]
  sub  ebx, edi                                        // ; we need to check only MG region
  jz   labWBEnd                                        // ; skip if it is zero
  mov  esi, [data : %CORE_GC_TABLE + gc_mg_wbar]
  shr  ebx, page_size_order
  // ; lea  edi, [edi + elObjectOffset]

labWBNext:
  cmp  [esi], 0
  lea  esi, [esi+4]
  jnz  short labWBMark
  sub  ebx, 4
  ja   short labWBNext
  nop
  nop
  jmp  short labWBEnd

labWBMark:
  lea  eax, [esi-4]
  sub  eax, [data : %CORE_GC_TABLE + gc_mg_wbar]
  mov  edx, [esi-4]
  shl  eax, page_size_order
  lea  eax, [edi + eax + elObjectOffset]
  
  test edx, 0FFh
  jz   short labWBMark2
  mov  ecx, [eax-elSizeOffset]
  push eax
  and  ecx, 0FFFFFh
  push ecx

labWBMark2:
  lea  eax, [eax + page_size]
  test edx, 0FF00h
  jz   short labWBMark3
  mov  ecx, [eax-elSizeOffset]
  push eax
  and  ecx, 0FFFFFh
  push ecx

labWBMark3:
  lea  eax, [eax + page_size]
  test edx, 0FF0000h
  jz   short labWBMark4
  mov  ecx, [eax-elSizeOffset]
  push eax
  and  ecx, 0FFFFFh
  push ecx

labWBMark4:
  lea  eax, [eax + page_size]
  test edx, 0FF000000h
  jz   short labWBNext
  mov  ecx, [eax-elSizeOffset]
  push eax
  and  ecx, 0FFFFFh
  push ecx
  jmp  short labWBNext

labWBEnd:
  push ebp                      // save the stack restore-point

  // ; init registers
  mov  ebx, [data : %CORE_GC_TABLE + gc_yg_start]
  mov  edx, [data : %CORE_GC_TABLE + gc_yg_end]
  mov  ebp, [data : %CORE_GC_TABLE + gc_shadow]

  // ; collect roots
  lea  eax, [esp+4]
  mov  ecx, [eax]
  mov  esi, [esp+8]
  mov  [data : %CORE_GC_TABLE + gc_yg_current], ebp

labCollectFrame:
  push eax
  call labCollectYG
  pop  eax
  lea  eax, [eax+8]
  mov  esi, [eax+4]
  test esi, esi
  mov  ecx, [eax]
  jnz short labCollectFrame 
  
  // ; save gc_yg_current to mark survived objects
  mov  [data : %CORE_GC_TABLE + gc_yg_current], ebp
  
  // ; switch main YG heap with a shadow one
  mov  eax, [data : %CORE_GC_TABLE + gc_yg_start]
  mov  ebx, [data : %CORE_GC_TABLE + gc_shadow]
  mov  ecx, [data : %CORE_GC_TABLE + gc_yg_end]
  mov  edx, [data : %CORE_GC_TABLE + gc_shadow_end]

  mov  [data : %CORE_GC_TABLE + gc_yg_start], ebx
  mov  [data : %CORE_GC_TABLE + gc_yg_end], edx
  mov  ebx, [esp]
  mov  [data : %CORE_GC_TABLE + gc_shadow], eax  
  mov  ebx, [ebx+4]                           // ; restore object size  
  mov  [data : %CORE_GC_TABLE + gc_shadow_end], ecx

  sub  edx, ebp
  
  pop  ebp
  mov  esp, [ebp-4]  // ; remove wb-roots

  // ; check if it is enough place
  cmp  ebx, edx
  jae  short labFullCollect

  // ; free root set
  mov  esp, ebp

  // ; restore registers
  pop  ebx

  // ; try to allocate once again
  mov  eax, [data : %CORE_GC_TABLE + gc_yg_current]
  mov  ecx, [esp]
  mov  [eax], ebx
  add  ecx, eax
  mov  [data : %CORE_GC_TABLE + gc_yg_current], ecx
  lea  edi, [eax + elObjectOffset]

  // ; GCXT: signal the collecting thread that GC is ended
  // ; should it be placed into critical section?
  xor  ecx, ecx
  mov  esi, [data : %CORE_GC_TABLE + gc_signal]
  // ; clear thread signal var
  mov  [data : %CORE_GC_TABLE + gc_signal], ecx
  push esi
  call extern 'dlls'kernel32.SetEvent 

  mov  eax, edi
  pop  ecx
  pop  ebp
  pop  edi  

  ret

labFullCollect:
  // ; ====== Major Collection ====
  // ; save the stack restore-point
  push ebp                     
	
  // ; expand MG if required
  mov  ecx, [data : %CORE_GC_TABLE + gc_end]
  sub  ecx, [data : %CORE_GC_TABLE + gc_mg_current]
  mov  edx, [data : %CORE_GC_TABLE + gc_yg_current]
  sub  edx, [data : %CORE_GC_TABLE + gc_yg_start]
  cmp  ecx, edx
  ja   labSkipExpand

  mov  eax, [data : %CORE_GC_TABLE + gc_end]
  mov  ecx, 2A000h
  call code : % EXPAND_HEAP

  mov  eax, [data : %CORE_GC_TABLE + gc_header]
  mov  ecx, [data : %CORE_GC_TABLE + gc_end]
  sub  ecx, [data : %CORE_GC_TABLE + gc_start]
  shr  ecx, page_size_order_minus2
  add  eax, ecx
  mov  ecx, 0A800h
  call code : % EXPAND_HEAP

  mov  ecx, [data : %CORE_GC_TABLE + gc_end]
  add  ecx, 15000h
  mov  [data : %CORE_GC_TABLE + gc_end], ecx

labSkipExpand:

  // ; mark both yg and mg objects
  mov  ebx, [data : %CORE_GC_TABLE + gc_yg_start]
  mov  edx, [data : %CORE_GC_TABLE + gc_mg_current]

  // ; collect roots
  lea  eax, [esp+4]
  mov  ecx, [eax]
  mov  esi, [esp+8]

labMGCollectFrame:
  push eax
  call labCollectMG
  pop  eax
  lea  eax, [eax+8]
  mov  esi, [eax+4]
  test esi, esi
  mov  ecx, [eax]
  jnz short labMGCollectFrame 

  // ; compact mg
  mov  esi, [data : %CORE_GC_TABLE + gc_mg_start]
  mov  edi, esi
  sub  edi, [data : %CORE_GC_TABLE + gc_start]
  shr  edi, page_size_order_minus2
  add  edi, [data : %CORE_GC_TABLE + gc_header]

  // ; skip the permanent part
labMGSkipNext:
  mov  ecx, [esi]
  test ecx, ecx
  jns  short labMGSkipEnd
  add  ecx, page_ceil
  mov  eax, esi
  and  ecx, page_align_mask
  lea  eax, [eax + elObjectOffset]
  add  esi, ecx
  mov  [edi], eax
  shr  ecx, page_size_order_minus2
  add  edi, ecx
  cmp  esi, edx
  jb   short labMGSkipNext
  // ; !! undefined behaviour
  xor  ecx, ecx

labMGSkipEnd:
  mov  ebp, esi
  
  // ; compact
labMGCompactNext:
  add  ecx, page_ceil
  and  ecx, page_align_mask  
  add  esi, ecx  
  
  shr  ecx, page_size_order_minus2
  add  edi, ecx
  cmp  esi, edx
  jae  short labMGCompactEnd

labMGCompactNext2:
  mov  ecx, [esi]
  test ecx, ecx
  jns  short labMGCompactNext
  add  ecx, page_ceil
  mov  eax, ebp
  and  ecx, page_align_mask
  lea  eax, [eax + elObjectOffset]
  mov  [edi], eax
  mov  eax, ecx
  shr  eax, page_size_order_minus2
  add  edi, eax

labMGCopy:
  mov  eax, [esi]
  mov  [ebp], eax
  sub  ecx, 4
  lea  esi, [esi+4]
  lea  ebp, [ebp+4]
  jnz  short labMGCopy
  cmp  esi, edx
  jb   short labMGCompactNext2
labMGCompactEnd:

  // ; promote yg
  mov  ebx, [data : %CORE_GC_TABLE + gc_end]
  mov  esi, [data : %CORE_GC_TABLE + gc_yg_start]
  sub  ebx, ebp
  mov  edi, esi
  sub  edi, [data : %CORE_GC_TABLE + gc_start]
  shr  edi, page_size_order_minus2
  mov  edx, [data : %CORE_GC_TABLE + gc_yg_current]
  add  edi, [data : %CORE_GC_TABLE + gc_header]
  jmp  short labYGPromNext2

labYGPromNext:
  add  ecx, page_ceil
  and  ecx, page_align_mask
  add  esi, ecx
  shr  ecx, page_size_order_minus2
  add  edi, ecx
  cmp  esi, edx
  jae  short labYGPromEnd
labYGPromNext2:
  mov  ecx, [esi]
  test ecx, ecx
  jns  short labYGPromNext
  add  ecx, page_ceil
  mov  eax, ebp
  and  ecx, page_align_mask
  // ; raise an exception if it is not enough memory to promote object
  lea  eax, [eax + elObjectOffset]
  sub  ebx, ecx
  js   short labError
  mov  [edi], eax
  mov  eax, ecx
  shr  eax, page_size_order_minus2
  add  edi, eax
labYGProm:
  mov  eax, [esi]
  sub  ecx, 4
  mov  [ebp], eax
  lea  esi, [esi+4]
  lea  ebp, [ebp+4]
  jnz  short labYGProm
  cmp  esi, edx
  jb   short labYGPromNext2
labYGPromEnd:

  // ; get previous heap end
  mov  edx, [data : %CORE_GC_TABLE + gc_mg_current]

  // ; set mg_current, clear yg and survive
  mov  [data : %CORE_GC_TABLE + gc_mg_current], ebp
  mov  eax, [data : %CORE_GC_TABLE + gc_yg_start]
  mov  [data : %CORE_GC_TABLE + gc_yg_current], eax
  
  // ; fix roots
  lea  eax, [esp+4]
  mov  ecx, [eax]
  mov  esi, [esp+8]

  mov  ebx, [data : %CORE_GC_TABLE + gc_yg_start]
  mov  ebp, [data : %CORE_GC_TABLE + gc_start]

labFixRoot:
  push eax
  call labFixObject
  pop  eax
  lea  eax, [eax+8]
  mov  esi, [eax+4]
  test esi, esi
  mov  ecx, [eax]
  jnz  short labFixRoot 

  // ; clear WBar
  mov  ecx, [data : %CORE_GC_TABLE + gc_end ] 
  mov  edi, [data : %CORE_GC_TABLE + gc_mg_wbar]
  sub  ecx, [data : %CORE_GC_TABLE + gc_mg_start]
  xor  eax, eax
  shr  ecx, page_size_order // !! pay attention
  rep  stos 
	
  // ; free root set
  mov  esp, [esp]
  pop  ebx

  // ; allocate
  mov  eax, [data : %CORE_GC_TABLE + gc_yg_current]
  mov  ecx, [esp]
  mov  edx, [data : %CORE_GC_TABLE + gc_yg_end]
  add  ecx, eax
  cmp  ecx, edx
  jae  labBigAlloc
  mov  [eax], ebx
  mov  [data : %CORE_GC_TABLE + gc_yg_current], ecx
  lea  edi, [eax + elObjectOffset]

  // ; GCXT: signal the collecting thread that GC is ended
  // ; should it be placed into critical section?
  xor  ecx, ecx
  mov  esi, [data : %CORE_GC_TABLE + gc_signal]
  // ; clear thread signal var
  mov  [data : %CORE_GC_TABLE + gc_signal], ecx
  push esi
  call extern 'dlls'kernel32.SetEvent 

  mov  eax, edi
  pop  ecx
  pop  ebp
  pop  edi  
  ret

labError:
  // ; restore stack
  mov  esp, [esp]
  pop  ebx
  pop  ecx
  pop  ebp
  pop  edi 

labError2:
  mov  ebx, 0C0000017h
  call code : % BREAK
  ret  

// ; bad luck, we have to expand GC
labBigAlloc2:
  push ecx
  push ebx

  mov  eax, [data : %CORE_GC_TABLE + gc_end]
  mov  ecx, 2A000h
  call code : % EXPAND_HEAP

  mov  eax, [data : %CORE_GC_TABLE + gc_header]
  mov  ecx, [data : %CORE_GC_TABLE + gc_end]
  sub  ecx, [data : %CORE_GC_TABLE + gc_start]
  shr  ecx, page_size_order_minus2
  add  eax, ecx
  mov  ecx, 0A800h
  call code : % EXPAND_HEAP

  mov  ecx, [data : %CORE_GC_TABLE + gc_end]
  add  ecx, 15000h
  mov  [data : %CORE_GC_TABLE + gc_end], ecx

  pop  ebx
  pop  ecx

labBigAlloc:
  // ; try to allocate in the mg
  sub  ecx, eax
  cmp  ecx, 800000h
  jae  labError2

  mov  eax, [data : %CORE_GC_TABLE + gc_mg_current]
  mov  edx, [data : %CORE_GC_TABLE + gc_end]
  add  ecx, eax
  cmp  ecx, edx
  jae  labBigAlloc2
  mov  [eax], ebx
  mov  [data : %CORE_GC_TABLE + gc_mg_current], ecx
  lea  eax, [eax + elObjectOffset]

  // ; mark it as root in WB
  cmp  ebx, 0800000h
  jae  short labSkipBigAlloc

  mov  ecx, eax
  mov  esi, [data : %CORE_GC_TABLE + gc_header]
  sub  ecx, [data : %CORE_GC_TABLE + gc_start]
  shr  ecx, page_size_order
  mov  byte ptr [ecx + esi], 1  

labSkipBigAlloc:
  mov  edi, eax
  // ; GCXT: signal the collecting thread that GC is ended
  // ; should it be placed into critical section?
  xor  ecx, ecx
  mov  esi, [data : %CORE_GC_TABLE + gc_signal]
  // ; clear thread signal var
  mov  [data : %CORE_GC_TABLE + gc_signal], ecx
  push esi
  call extern 'dlls'kernel32.SetEvent 

  mov  eax, edi
  pop  ecx
  pop  ebp
  pop  edi  
  ret  

  // ; start collecting: esi => ebp, [ebx, edx] ; ecx - count
labCollectYG:
  push 0

  lea  ecx, [ecx+4]
  lea  esi, [esi-4]

labYGNext:
  lea  esi, [esi+4]
  sub  ecx, 4
  jz   labYGResume

labYGCheck:
  mov  eax, [esi]

  // ; check if it valid reference
  cmp  eax, ebx
  jl   short labYGNext  
  nop
  cmp  edx, eax
  jl   short labYGNext

  // ; check if it was collected
  mov  edi, [eax-elSizeOffset]
  test edi, edi
  js   labYGContinue

  // ; save previous ecx field
  push ecx

  // ; copy object size
  mov  [ebp], edi
  
  // ; copy object vmt
  mov  ecx, [eax - elVMTOffset]
  mov  [ebp + 04h], ecx
  
  // ; mark as collected
  or   [eax - elSizeOffset], 80000000h

  // ; reserve shadow YG
  mov  ecx, edi
  add  ecx, page_ceil
  lea  edi, [ebp + elObjectOffset]
  and  ecx, page_align_mask  
  mov  [esi], edi          // ; update reference 
  add  ebp, ecx

  // ; get object size
  mov  ecx, [eax-elSizeOffset]
  and  ecx, 8FFFFFh

  // ; save ESI
  push esi
  mov  esi, eax

  // ; save new reference
  mov  [eax - elVMTOffset], edi

  // ; check if the object has fields
  cmp  ecx, 800000h

  // ; save original reference
  push eax

  // ; collect object fields if it has them
  jb   labYGCheck

  lea  esp, [esp+4]
  jz   short labYGSkipCopyData

  // ; copy meta data object to shadow YG
  add  ecx, 3
  and  ecx, 0FFFFCh

labYGCopyData:
  mov  eax, [esi]
  sub  ecx, 4
  mov  [edi], eax
  lea  esi, [esi+4]
  lea  edi, [edi+4]
  jnz  short labYGCopyData

labYGSkipCopyData:
  pop  esi
  pop  ecx
  jmp  labYGNext

labYGResume:
  // ; copy object to shadow YG
  pop  edi
  test edi, edi
  jz   short labYGEnd

  mov  ecx, [edi-elSizeOffset]
  mov  esi, [edi - elVMTOffset]
  and  ecx, 0FFFFFh

labYGCopy:
  mov  eax, [edi]
  sub  ecx, 4
  mov  [esi], eax
  lea  esi, [esi+4]
  lea  edi, [edi+4]
  jnz  short labYGCopy

  pop  esi
  pop  ecx
  jmp  labYGNext

  nop
labYGEnd:
  ret

labYGContinue:
  // ; update reference
  mov  edi, [eax - elVMTOffset]
  mov  [esi], edi
  jmp  labYGNext

  // ---- start collecting: esi => ebp, [ebx, edx] ; ecx - count ---
labCollectMG:

  lea  ecx, [ecx+4]
  push 0
  lea  esi, [esi-4]
  push 0

labMGNext:
  sub  ecx, 4
  lea  esi, [esi+4]
  jz   short labMGResume

labMGCheck:
  mov  eax, [esi]

  // ; check if it valid reference
  cmp  eax, ebx
  jl   short labMGNext  
  nop
  cmp  edx, eax
  jl   short labMGNext

  // ; check if it was collected
  mov  edi, [eax-elSizeOffset]
  test edi, edi
  js   short labMGNext

  // ; mark as collected
  or  [eax - elSizeOffset], 080000000h

  cmp  edi, 0800000h
  jae  short labMGNext

  // ; save previous ecx field
  push ecx

  // ; get object size
  mov  ecx, [eax - elSizeOffset]

  // ; save ESI
  push esi
  and  ecx, 0FFFFFh

  mov  esi, eax

  // ; collect object fields if it has them
  jmp   short labMGCheck

labMGResume:
  pop  esi
  pop  ecx
  test esi, esi
  jnz  short labMGNext

  nop
labMGEnd:
  ret

labFixObject:

  lea  ecx, [ecx+4]
  push 0
  lea  esi, [esi-4]
  push 0

labFixNext:
  sub  ecx, 4
  lea  esi, [esi+4]
  jz   short labFixResume

labFixCheck:
  mov  eax, [esi]

  // ; check if it valid reference
  cmp  eax, ebx
  jl   short labFixNext
  nop
  cmp  edx, eax
  jl   short labFixNext

  lea  edi, [eax-elObjectOffset]

  sub  edi, ebp
  shr  edi, page_size_order_minus2
  add  edi, [data : %CORE_GC_TABLE + gc_header]

  mov  eax, [edi]
  mov  [esi], eax

  // ; make sure the object was not already fixed
  mov  edi, [eax - elSizeOffset]
  test edi, edi
  jns  short labFixNext

  and  edi, 7FFFFFFFh
  mov  [eax - elSizeOffset], edi

  cmp  edi, 0800000h
  jae  short labFixNext

  // ; save previous ecx field
  push ecx

  // ; get object size
  mov  ecx, [eax - elSizeOffset]
  and  ecx, 0FFFFFh

  // ; save ESI
  push esi
  mov  esi, eax

  // ; collect object fields if it has them
  jmp   short labFixCheck

labFixResume:
  pop  esi
  pop  ecx
  test esi, esi
  jnz  short labFixNext
  nop

  ret

end

// ; --- HOOK ---
// ; in: ecx - catch offset
procedure %HOOK
         
  mov  edx, [esp]       
  lea  ecx, [edx + ecx - 5]               
  // ; add  ecx, [esp]
  // ; sub  ecx, 5             // ; call command size should be excluded
  ret

end

// --- System Core Functions --

procedure % INIT
  // ; initialize fpu
  finit

  // GCXT: initialize signal
  xor  ebx, ebx
  mov  [data : %CORE_GC_TABLE + gc_signal], ebx

  // ; initialize
  mov  ecx, [data : %CORE_STAT_COUNT]
  mov  edi, data : %CORE_STATICROOT
  shr  ecx, 2
  xor  eax, eax
  rep  stos

labNext:
  mov  ecx, 10000000h
  mov  ebx, [data : %CORE_GC_SIZE]
  and  ebx, 0FFFFFF80h     // ; align to 128
  shr  ebx, page_size_order_minus2
  call code : %NEW_HEAP
  mov  [data : %CORE_GC_TABLE + gc_header], eax

  mov  ecx, 40000000h
  mov  ebx, [data : %CORE_GC_SIZE]
  and  ebx, 0FFFFFF80h     // align to 128
  call code : %NEW_HEAP
  mov  [data : %CORE_GC_TABLE + gc_start], eax

  // ; initialize yg
  mov  [data : %CORE_GC_TABLE + gc_start], eax
  mov  [data : %CORE_GC_TABLE + gc_yg_start], eax
  mov  [data : %CORE_GC_TABLE + gc_yg_current], eax

  // ; initialize gc end
  mov  ecx, [data : %CORE_GC_SIZE]
  and  ecx, 0FFFFFF80h     // ; align to 128
  add  ecx, eax
  mov  [data : %CORE_GC_TABLE + gc_end], ecx

  // ; initialize gc shadow
  mov  ecx, [data : %CORE_GC_SIZE + gcs_YGSize]
  and  ecx, page_mask
  add  eax, ecx
  mov  [data : %CORE_GC_TABLE + gc_yg_end], eax
  mov  [data : %CORE_GC_TABLE + gc_shadow], eax

  // ; initialize gc mg
  add  eax, ecx
  mov  [data : %CORE_GC_TABLE + gc_shadow_end], eax
  mov  [data : %CORE_GC_TABLE + gc_mg_start], eax
  mov  [data : %CORE_GC_TABLE + gc_mg_current], eax
  
  // ; initialize wbar start
  mov  edx, eax
  sub  edx, [data : %CORE_GC_TABLE + gc_start]
  shr  edx, page_size_order
  add  edx, [data : %CORE_GC_TABLE + gc_header]
  mov  [data : %CORE_GC_TABLE + gc_mg_wbar], edx
  
  // ; GCXT: assign tls entry
  mov  ebx, [data : %CORE_TLS_INDEX]
  mov  ecx, fs:[2Ch]
  mov  esi, [ecx + ebx*4]

  // ; init thread flags  
  mov  [esi + tls_flags], 0       

  call code : %NEW_EVENT
  mov  [esi + tls_sync_event], eax     

  mov  eax, data : %THREAD_TABLE
  mov  [eax], esi       // ; save tls reference 

  mov  ecx, [data : %CORE_GC_SIZE + gcs_TTSize]
  mov  [eax-4], ecx
  
  ret

end

procedure % NEWFRAME

  // ; put frame end and move procedure returning address
  pop  edx           

  // ; GCXT                                                               
  // ; get thread table entry from tls
  mov  ecx, [data : %CORE_TLS_INDEX]
  mov  esi, fs:[2Ch]
  mov  esi, [esi+ecx*4]

  xor  ebx, ebx

  push ebp
  push ebx
  push ebx

  // ; GCXT
  // ; set stack frame pointer / bottom stack pointer
  mov  ebp, esp 
  mov  [esi + tls_stack_bottom], esp
  push edx
  mov  [esi + tls_stack_frame], ebx
  
  // ; GCXT
  // ; set thread table length
  mov  ebx, 1
  mov  [data : %CORE_GC_TABLE + tt_ptr], ebx   

  mov  ebx, code : "$native'coreapi'default_handler"
  call code : % INIT_ET
  
  ret

end

procedure % ENDFRAME

  // ; save return pointer
  pop  ecx  
  
  xor  edx, edx
  lea  esp, [esp+8]
  pop  ebp

  // ; restore return pointer
  push ecx   
  ret

end

// ; init exception table, ebx contains default handler address
procedure % INIT_ET

  pop esi

  mov edx, ebx

  // ; GCXT: get current thread frame
  mov  ebx, [data : %CORE_TLS_INDEX]
  mov  ecx, fs:[2Ch]
  mov  ebx, [ecx+ebx*4]

  // ; set default exception handler
  mov  [ebx + tls_catch_addr], edx
  mov  [ebx + tls_catch_level], esp
  mov  [ebx + tls_catch_frame], ebp

  push esi
  ret

end 

procedure % RESTORE_ET

  // ; GCXT: get current thread frame
  mov  ebx, [data : %CORE_TLS_INDEX]
  mov  ecx, fs:[2Ch]
  mov  ebx, [ecx+ebx*4]

  pop  edx
  mov  esp, [ebx + tls_catch_level]
  push edx

  ret

end 

// ; NOTE : some functions (e.g. system'core_routines'win_WndProc) assumes the function reserves 12 bytes
// ; does not affect eax
procedure % OPENFRAME

  // ; GCXT: get thread table entry from tls
  mov  ebx, [data : %CORE_TLS_INDEX]
  mov  esi, fs:[2Ch]
  xor  edi, edi
  mov  esi, [esi+ebx*4]
                                        
  // ; save return pointer
  pop  ecx  

  // ; GCXT: get thread table entry from tls
  // ; save previous pointer / size field
  mov  esi, [esi + tls_stack_frame]
  push ebp
  push esi                                
  push edi                              
  mov  ebp, esp
  
  // ; restore return pointer
  push ecx   
  ret

end

// ; does not affect eax
procedure % CLOSEFRAME

  // ; GCXT: get thread table entry from tls
  mov  ebx, [data : %CORE_TLS_INDEX]
  mov  esi, fs:[2Ch]
  xor  edi, edi
  mov  esi, [esi+ebx*4]
                                        
  // ; save return pointer
  pop  ecx  

  // ; GCXT
  lea  esp, [esp+4]
  pop  edx
  mov  [esi + tls_stack_frame], edx
  pop  ebp
  
  // ; restore return pointer
  push ecx   
  ret

end

procedure % NEWTHREAD

  push eax
  
  // ; GCXT
  mov  edx, data : %THREAD_TABLE
  mov  esi, data : %CORE_GC_TABLE + tt_lock
  mov  ecx, [edx - 4]

labWait:
  // ; set lock
  xor  eax, eax
  mov  edx, 1
  lock cmpxchg dword ptr[esi], edx
  jnz  short labWait

  mov  ebx, [data : %CORE_GC_TABLE + tt_ptr]
  sub  ecx, ebx
  jz   short labSkipSave

  add  ebx, 1
  mov  [data : %CORE_GC_TABLE + tt_ptr], ebx

labSkipSave:

  // ; free lock
  // ; could we use mov [esi], 0 instead?
  mov  edx, -1
  lock xadd [esi], edx

  xor  eax, eax

  // check if there is a place in thread table
  test ecx, ecx
  jz   short lErr
  
  sub  ebx, 1
  mov  esi, data : %THREAD_TABLE
  lea  esi, [esi+ebx*4]

  // ; assign tls entry
  mov  ebx, [data : %CORE_TLS_INDEX]
  push 0

  mov  eax, fs:[2Ch]
  push 0

  mov  eax, [eax + ebx*4] 
  push 0FFFFFFFFh //-1

  mov  [esi], eax               // ; save tls entry
  push 0
  mov  esi, eax

  call extern 'dlls'kernel32.CreateEventW

  // ; initialize thread entry
  mov  [esi + tls_sync_event], eax     // ; set thread event handle

  mov  [esi+tls_flags], 0              // ; init thread flags  

  pop  eax
  pop  edx                             // ; put frame end and move procedure returning address

  xor  ebx, ebx
  push ebp
  push ebx                      
  push ebx

  // ; set stack frame pointer  
  mov  ebp, esp 
  mov  [esi + tls_stack_frame], ebx
  mov  [esi + tls_stack_bottom], esp

  // ; restore return pointer
  push edx              

  mov  ebx, code : "$native'coreapi'core_thread_handler"
  call code : % INIT_ET

  ret

lErr:
  add esp, 4 
  ret

end

procedure % CLOSETHREAD

  // ; GCXT
  mov  ebx, [data : %CORE_TLS_INDEX]
  mov  edi, fs:[2Ch]

  mov  esi, data : %CORE_GC_TABLE + tt_lock

labWait:
  // ; set lock
  xor  eax, eax  
  mov  edx, 1
  lock cmpxchg dword ptr[esi], edx
  jnz  short labWait

  // ; tls reference
  mov  eax, [edi + ebx*4]           
  mov  ecx, [data : %CORE_GC_TABLE + tt_ptr]
  mov  edi, data : %THREAD_TABLE

  // ; find the current thread entry
labSearch:
  test ecx, ecx
  jz   short labNotFound

  cmp  [edi], eax
  lea  edi, [edi+4]
  lea  ecx, [ecx-1]
  jnz   short labSearch

  // ; delete the entry
  test ecx, ecx
  jz   short labSkipDelete

labDelete:
  mov  ebx, [edi]
  mov  [edi-4], ebx
  lea  edi, [edi+4]
  sub  ecx, 1
  jnz   short labDelete
  
labSkipDelete:
  mov  ecx, [data : %CORE_GC_TABLE + tt_ptr]
  mov  edi, eax
  sub  ecx, 1
  mov  ebx, [edi + tls_sync_event]
  mov  [data : %CORE_GC_TABLE + tt_ptr], ecx

  // ; close handle

  push ebx
  call extern 'dlls'kernel32.CloseHandle

labNotFound:

  // ; free lock
  // ; could we use mov [esi], 0 instead?
  mov  edx, -1
  lock xadd [esi], edx

  pop  edx
  lea  esp, [esp+0Ch]
  push edx

lErr:
  ret
  
end

// --- THREAD_WAIT ---
// GCXT: it is presumed that gc lock is on, edx - contains the collecting thread event handle

procedure % THREAD_WAIT

  push eax
  push edi
  push ebp
  mov  edi, esp

  push 0FFFFFFFFh // -1     // WaitForSingleObject::dwMilliseconds
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
  mov  [eax+tls_stack_frame], edi                  // ; lock stack frame

  // ; signal the collecting thread that it is stopped
  push esi
  mov  edi, data : %CORE_GC_TABLE + gc_lock

  // ; signal the collecting thread that it is stopped
  call extern 'dlls'kernel32.SetEvent

  // ; free lock
  // ; could we use mov [esi], 0 instead?
  mov  ebx, 0FFFFFFFFh
  lock xadd [edi], ebx

  // ; stop until GC is ended
  call extern 'dlls'kernel32.WaitForSingleObject

  add  esp, 4
  pop  edi
  pop  eax

  ret

end

procedure % CALC_SIZE

  mov  ecx, ebx
  add  ecx, page_ceil
  and  ecx, page_mask

  ret

end

procedure % GET_COUNT

  mov  ebx, [eax - elSizeOffset]
  test ebx, 0800000h
  jnz  short labErr
  and  ebx, 0FFFFFFh
  shr  ebx, 2
  ret

labErr:
  xor  ebx, ebx
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
  mov  edx, fs:[2Ch]
  mov  esi, [edx+esi*4]
  jmp  [esi + tls_catch_addr]

end

// ; bsredirect

inline % 0Eh // (eax - object, ecx - message)

  mov  edi, [eax-4]
  xor  edx, edx
  mov  esi, [edi - elVMTSizeOffset]

labSplit:
  test esi, esi
  jz   short labEnd

labStart:
  shr  esi, 1
  setnc dl
  cmp  ecx, [edi+esi*8]
  jb   short labSplit
  nop
  nop
  jz   short labFound
  lea  edi, [edi+esi*8+8]
  sub  esi, edx
  jnz  short labStart
  nop
  nop
  jmp  labEnd
  nop
  nop
labFound:
  jmp  [edi+esi*8+4]
  nop
  nop

labEnd:
                                                                
end

// ; len

inline % 11h

  mov  edx, 0FFFFFh
  mov  ebx, [eax-elSizeOffset]
  and  ebx, edx
  shr  ebx, 2

end

// ; bcopya
inline % 12h

  mov  edi, eax

end

// ; close

inline % 15h

  mov  esp, ebp
  pop  ebp
  
end

// ; get

inline % 18h

   mov  eax, [edi + ebx * 4]

end

// ; set

inline % 19h
                                
   // ; calculate write-barrier address
   mov  ecx, edi
   mov  esi, [data : %CORE_GC_TABLE + gc_header]
   sub  ecx, [data : %CORE_GC_TABLE + gc_start]
   shr  ecx, page_size_order
   mov  byte ptr [ecx + esi], 1  
   mov  [edi + ebx*4], eax

end

// ; equit
inline % 1Bh

  mov  edx, ecx
  pop  esi
  and  edx, 000000Fh
  lea  esp, [esp + edx * 4 + 4]
  jmp  esi
  nop
  nop
 
end

// ; count
inline % 1Ch

  mov  edx, 0FFFFFh
  mov  ecx, [edi-elSizeOffset]
  and  ecx, edx
  shr  ecx, 2

end

// ; unhook
inline % 1Dh

  // ; GCXT: get current thread frame
  mov  edx, [data : %CORE_TLS_INDEX]
  mov  esi, fs:[2Ch]
  mov  edx, [esi+edx*4]

  mov  esp, [edx + tls_catch_level]  
  mov  ebp, [edx + tls_catch_frame]
  mov  esi, [esp]
  mov  [edx + tls_catch_level], esi
  mov  esi, [esp+4]
  mov  [edx + tls_catch_frame], esi
  mov  esi, [esp+8]
  mov  [edx + tls_catch_addr], esi
  add  esp, 12
  
end

// ; create
inline % 1Fh

  xor  edx, edx
  shl  ebx, 2  
  setz dl
  push eax  
  mov  ecx, ebx
  shl  edx, 23
  add  ecx, page_ceil
  or   ebx, edx
  and  ecx, page_mask  
  call code : %GC_ALLOC
  pop  edx
  mov  [eax-4], edx
  
end

// ; include
inline % 25h
       
  add  esp, 4
  mov  edx, fs:[2Ch]
  mov  eax, [data : %CORE_TLS_INDEX]
  mov  esi, [edx+eax*4]
  mov  [esi + tls_flags], 0

end

// ; exclude
inline % 26h
                                                       
  mov  edx, fs:[2Ch]
  mov  eax, [data : %CORE_TLS_INDEX]
  mov  esi, [edx+eax*4]
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
  mov  ebx, eax
  mov  eax, esi

end

// ; freelock
inline % 28h

  mov  edx, -1

  // ; free lock
  lock xadd byte ptr [eax - elSyncOffset], dl

end

// ; eswap
inline % 2Ch

  mov  edx, ecx
  mov  esi, ebx
  mov  ebx, edx
  mov  ecx, esi
  
end

// ; bswap
inline % 2Dh

  mov  edx, eax
  mov  esi, edi
  mov  edi, edx
  mov  eax, esi

end

// ; copy

inline % 2Eh

  mov  ecx, [edi-8]
  mov  esi, eax
  add  ecx, 3
  mov  ebx, edi
  and  ecx, 0FFFFCh
labCopy:
  mov  edx, [esi]
  mov  [ebx], edx
  lea  esi, [esi+4]
  lea  ebx, [ebx+4]
  sub  ecx, 4
  js   short labCopy

end

// ; xset

inline % 2Fh
                                
   mov  [edi + ebx * 4], eax

end

// ; xlen

inline % 30h

  mov  edx, 0FFFFFh
  mov  ecx, [eax-8]
  and  ecx, edx
  shr  ecx, 2

end

// ; blen
inline % 31h

  mov  edx, 0FFFFFh
  mov  ebx, [eax-8]
  and  ebx, edx

end

// ; wlen
// ;in : eax - object, esi - size
inline % 32h

  mov  edx, 0FFFFFh
  mov  ebx, [eax-8]
  and  ebx, edx
  shr  ebx, 1
  
end

// ; flag

inline % 33h

  mov  esi, [eax - 4]
  mov  ebx, [esi - elVMTFlagOffset]
  
end

// ; nlen
// ;in : eax - object, esi - size
inline % 34h

  mov  edx, 0FFFFFh
  mov  ebx, [eax-8]
  and  ebx, edx
  shr  ebx, 2
  
end

// ; package

inline % 35h

  mov eax, [edi - elVMTOffset]
  mov eax, [eax - elPackageOffset]

end

// ; class

inline % 36h

  mov eax, [edi - elVMTOffset]

end

// ; mindex
inline % 37h

  push edi
  mov  edi, [eax-4]
  xor  edx, edx
  mov  ebx, [edi - elVMTSizeOffset]

labSplit:
  test ebx, ebx
  jz   short labEnd

labStart:
  shr  ebx, 1
  setnc dl
  cmp  ecx, [edi+ebx*8]
  jb   short labSplit
  nop
  nop
  jz   short labFound
  lea  edi, [edi+ebx*8+8]
  sub  ebx, edx
  jnz  short labStart
  nop
  nop
labEnd:
labFound:
  pop  edi  

end

// ; acallvd (ecx - offset to VMT entry)
inline % 039h

  mov  edx, [eax - 4]
  call [edx + ebx * 8 + 4]

end

// ; validate

inline % 03Ah

  cmp [eax], eax

end

// ; nequal

inline % 40h

  xor  ebx, ebx
  mov  edx, [eax]
  cmp  edx, [edi]
  setz bl

end

// ; nless
inline % 41h

  xor  ebx, ebx
  mov  edx, [eax]
  cmp  edx, [edi]
  setl bl

end

// ; ncopy (src, tgt)
inline % 42h

  mov  esi, [eax]
  mov  [edi], esi
    
end

// ; nadd
inline % 43h

  mov  edx, [eax]
  add  [edi], edx

end

// ; nsub
inline % 44h

  mov  edx, [eax]
  sub  [edi], edx

end

// ; nmul
inline % 45h

  mov  esi, eax
  mov  eax, [edi]
  imul [esi]
  mov  [edi], eax
  mov  eax, esi

end

// ; ndiv
inline % 46h
                                                   
  mov  esi, eax
  mov  ebx, [esp]
  mov  eax, [edi]
  cdq
  idiv [esi]
  mov  [edi], eax
  mov  eax, esi

end

// ; nsave
inline % 47h

  mov [edi], ebx

end

// ; nload
inline % 48h

  mov ebx, [eax]

end

// ; dcopyr
inline % 49h

  push 0
  mov  esi, esp
  fistp dword ptr [esi]
  pop ebx

end

// ; nand
inline % 4Ah

  mov  edx, [eax]
  and  [edi], edx

end

// ; nor
inline % 4Bh

  mov  edx, [eax]
  or   [edi], edx

end

// ; nxor
inline % 4Ch

  mov  edx, [eax]
  xor  [edi], edx

end

// ; nshift
inline % 4Dh

  mov edx, [edi]
  mov ecx, ebx
  and ecx, ecx
  jns short lab1
  neg ecx
  shl edx, cl
  jmp short lab2
lab1:
  shr edx, cl
lab2:
  mov [edi], edx

end

// ; nnot
inline % 4Eh

  mov  edx, [eax]  
  not  edx
  mov  [edi], edx

end

// ; ncreate
inline % 4Fh

  shl  ebx, 2
  push eax  
  mov  ecx, ebx
  add  ecx, page_ceil
  or   ebx, struct_mask
  and  ecx, page_mask  
  call code : %GC_ALLOC
  pop  edx
  mov  [eax-4], edx

end

// ; ncopyb (src, tgt)
inline % 50h

  mov  edx, [edi]
  mov  [eax], edx
    
end

// ; lcopyb
inline % 51h

  mov  edx, [edi]
  mov  esi, [edi+4]
  mov  [eax], edx
  mov  [eax+4], esi
    
end

// ; copyb

inline % 52h

  mov  ecx, [eax-8]
  mov  esi, edi
  add  ecx, 3
  mov  ebx, eax
  and  ecx, 0FFFFCh
labCopy:
  mov  edx, [esi]
  mov  [ebx], edx
  lea  esi, [esi+4]
  lea  ebx, [ebx+4]
  sub  ecx, 4
  jnz  short labCopy

end

// ; nloade
inline % 53h

  mov ecx, [eax]

end

// ; wread
inline % 59h

  mov edx, eax
  mov eax, [edx + ebx * 2]
  cwde
  mov ecx, eax
  mov eax, edx

end

// ; wwrite
inline % 5Ah

  mov [edi + ebx * 2], ecx
  
end

// ; nread
inline % 5Bh

  mov ecx, [eax + ebx * 4]

end

// ; nwrite
inline % 5Ch

  mov [edi + ebx * 4], ecx
  
end

// ; wcreate
inline % 5Fh

  shl  ebx, 1
  push eax  
  mov  ecx, ebx
  add  ecx, page_ceil
  or   ebx, struct_mask
  and  ecx, page_mask  
  call code : %GC_ALLOC
  pop  edx
  mov  [eax-4], edx

end

// ; breadw
inline % 60h

  mov edx, eax
  mov ax, word ptr [eax + ebx]
  cwde
  mov ecx, eax
  mov eax, edx

end

// ; bread
inline % 61h

  mov ecx, [eax + ebx]

end

// ; breadb
inline % 65h

  mov ecx, [eax + ebx]
  and ecx, 0FFh

end

// ; rsin

inline % 66h

  mov   edx, eax
  fld   qword ptr [eax]  
  fldpi
  fadd  st(0),st(0)       // ; ->2pi
  fxch

lReduce:
  fprem                   // ; reduce the angle
  fsin
  fstsw ax                // ; retrieve exception flags from FPU
  shr   al,1              // ; test for invalid operation
  // ; jc    short lErr        // ; clean-up and return error
  sahf                    // ; transfer to the CPU flags
  jpe   short lReduce     // ; reduce angle again if necessary
  fstp  st(1)             // ; get rid of the 2pi

  fstp  qword ptr [edi]    // ; store result 
  mov   eax, edx

end

// ; rcose

inline % 67h

  fld   qword ptr [eax]  
  fcos
  fstp  qword ptr [edi]    // store result 

end

// ; rarctan

inline % 68h

  fld   qword ptr [eax]  
  fld1
  fpatan                  // i.e. arctan(Src/1)
  fstp  qword ptr [edi]    // store result 

end

// ; bwrite
inline % 69h

  mov [edi + ebx], ecx
  
end

// ; bwriteb
inline % 6Ch

  mov byte ptr [edi + ebx], cl
  
end

// ; bwritew
inline % 6Dh

  mov word ptr [edi + ebx], cx
  
end

// ; bcreate
inline % 6Fh

  push eax  
  mov  ecx, ebx
  add  ecx, page_ceil
  or   ebx, struct_mask
  and  ecx, page_mask  
  call code : %GC_ALLOC
  pop  edx
  mov  [eax-4], edx

end

// ; lcopy
inline % 70h

  mov  edx, [eax]
  mov  esi, [eax+4]
  mov  [edi], edx
  mov  [edi+4], esi
    
end

// ; lequal

inline % 72h

  mov  edx, [eax]
  xor  ebx, ebx
  mov  esi, [eax+4]  
  cmp  edx, [edi]
  setz bl
  cmp  esi, [edi+4]
  setz dl
  and  bl, dl

end

// ; lless(lo, ro, tr, fr)
inline % 73h

  xor  ebx, ebx
  xor  edx, edx
  mov  esi, [eax]
  cmp  esi, [edi]
  setl dl
  mov  esi, [eax+4]  
  cmp  esi, [edi+4]
  setl bl
  cmovz ebx, edx

end

// ; ladd
inline % 74h

  mov  edx, [eax+4]
  mov  esi, [eax]
  add [edi], esi
  adc [edi+4], edx

end

// ; lsub
inline % 75h

  mov  esi, [edi]
  mov  edx, [edi+4]
  sub  esi, [eax]
  sbb  edx, [eax+4]
  mov  [edi], esi
  mov  [edi+4], edx

end

// ; lmul
inline % 76h
  mov  esi, eax        // sour
  mov  edx, edi        // dest

  push eax
  
  mov  ecx, [edx+4]   // DHI
  mov  eax, [esi+4]   // SHI
  or   eax, ecx
  mov  ecx, [edx]     // DLO
  jnz  short lLong
  mov  eax, [esi]
  mul  ecx
  jmp  short lEnd

lLong:
  mov  eax, [esi+4]
  push edi
  mov  edi, edx
  mul  ecx               // SHI * DLO
  mov  ebx, eax
  mov  eax, dword ptr [esi]
  mul  dword ptr [edi+4]  // SLO * DHI
  add  ebx, eax     
  mov  eax, dword ptr [esi] // SLO * DLO
  mul  ecx
  add  edx, ebx 
  pop  edi

lEnd:
  mov  [edi], eax
  mov  [edi+4], edx
  pop  eax

end

// ; ldiv
inline % 77h
               
  mov  ebx, eax   // ; DVSR
  mov  esi, edi   // ; DVND

  push eax
  push edi

  push [esi+4]    // ; DVND hi dword
  push [esi]      // ; DVND lo dword
  push [ebx+4]    // ; DVSR hi dword
  push [ebx]      // ; DVSR lo dword

  xor  edi, edi

  mov  eax, [esp+0Ch]    // hi DVND
  or   eax, eax
  jge  short L1
  add  edi, 1
  mov  edx, [esp+8]      // lo DVND
  neg  eax
  neg  edx
  sbb  eax, 0
  mov  [esp+0Ch], eax    // hi DVND
  mov  [esp+8], edx      // lo DVND

L1:                                                               
  mov  eax, [esp+4]      // hi DVSR
  or   eax, eax
  jge  short L2
  add  edi, 1
  mov  edx, [esp]        // lo DVSR
  neg  eax
  neg  edx
  sbb  eax, 0
  mov  [esp+4], eax      // hi DVSR
  mov  [esp], edx        // lo DVSR

L2:
  or   eax, eax
  jnz  short L3
  mov  ecx, [esp]        // lo DVSR
  mov  eax, [esp+0Ch]    // hi DVND
  xor  edx, edx
  div  ecx
  mov  ebx, eax 
  mov  eax, [esp+8]      // lo DVND
  div  ecx

  mov  esi, eax          // result
  jmp  short L4

L3:
  mov  ebx, eax 
  mov  ecx, [esp]        // lo DVSR
  mov  edx, [esp+0Ch]    // hi DVND
  mov  eax, [esp+8]      // lo DVDN
L5:
  shr  ebx, 1 
  rcr  ecx, 1
  shr  edx, 1 
  rcr  eax, 1
  or   ebx, ebx 
  jnz  short L5
  div  ecx
  mov  esi, eax          // result

  // check the result with the original
  mul  [esp+4]           // hi DVSR
  mov  ecx, eax 
  mov  eax, [esp]        // lo DVSR
  mul  esi
  add  edx, ecx

  // carry means Quotient is off by 1
  jb   short L6

  cmp  edx, [esp+0Ch]    // hi DVND
  ja   short L6
  jb   short L7
  cmp  eax, [esp+8]      // lo DVND
  jbe  short L7

L6:
  sub  esi, 1

L7:
  xor  ebx, ebx

L4:
  mov  edx, ebx
  mov  eax, esi

  sub  edi, 1
  jnz  short L8
  neg  edx
  neg  eax
  sbb  edx, 0

L8:
  lea  esp, [esp+10h]
  pop  edi

  mov  [edi], eax
  mov  [edi+4], edx
  mov  eax, ebx
  pop  eax
                                    
end

// ; land
inline % 78h
  mov ebx, [edi]
  mov edx, [eax]

  mov ecx, [edi+4]
  mov esi, [eax+4]

  and ebx, edx
  and ecx, esi

  mov [edi], ebx
  mov [edi+4], ecx
end

// ; lor
inline % 79h
  mov ebx, [edi]
  mov edx, [eax]

  mov ecx, [edi+4]
  mov esi, [eax+4]

  or  ebx, edx
  or  ecx, esi

  mov [edi], ebx
  mov [edi+4], ecx
end

// ; lxor
inline % 7Ah
  mov ebx, [edi]
  mov edx, [eax]

  mov ecx, [edi+4]
  mov esi, [eax+4]

  xor ebx, edx
  xor ecx, esi

  mov [edi], ebx
  mov [edi+4], ecx
end

// ; lshift
inline % 7Bh

  mov  ecx, ebx
  mov  edx, [edi]
  mov  ebx, [edi+4]

  and  ecx, ecx
  jns  short LR
  neg  ecx

  cmp  cl, 40h 
  jae  short lErr
  cmp  cl, 20h
  jae  short LL32
  shld edx, ebx, cl
  shl  ebx, cl
  jmp  short lEnd

LL32:
  mov  ebx, edx
  xor  edx, edx
  sub  cl, 20h
  shl  ebx, cl 
  jmp  short lEnd

LR:

  cmp  cl, 64
  jae  short lErr

  cmp  cl, 32
  jae  short LR32
  shrd edx, ebx, cl
  sar  ebx, cl
  jmp  short lEnd

LR32:
  mov  edx, ebx
  xor  ebx, ebx
  sub  cl, 20h
  shr  edx, cl 
  jmp  short lEnd
  
lErr:
  xor  eax, eax
  jmp  short lEnd2

lEnd:
  mov  [edi], edx
  mov  [edi+4], ebx

lEnd2:

end

// ; lnot
inline % 7Ch

  mov esi, [eax]
  mov edx, [eax+4]
                                                                        
  not esi
  not edx

  mov [edi], esi
  mov [edi+4], edx

end

// ; rcopy (src, tgt)
inline % 80h

  push ebx
  fild dword ptr [esp]
  pop  ebx

end

// ; rsave
inline % 82h

  fstp qword ptr [edi]

end

// ; requal

inline % 83h

  fld    qword ptr [edi]
  fld    qword ptr [eax]
  xor    ebx, ebx
  fcomip st, st(1)
  sete   bl
  fstp  st(0)

end

// ; rless(lo, ro, tr, fr)
inline % 84h

  fld    qword ptr [edi]
  fld    qword ptr [eax]
  xor    ebx, ebx
  fcomip st, st(1)
  setb   bl
  fstp  st(0)

end

// ; radd
inline % 85h

  fld  qword ptr [eax]
  fadd qword ptr [edi] 
  fstp qword ptr [edi]

end

// ; rsub
inline % 86h

  fld  qword ptr [edi]
  fsub qword ptr [eax] 
  fstp qword ptr [edi]

end

// ; rmul
inline % 87h

  fld  qword ptr [edi]
  fmul qword ptr [eax] 
  fstp qword ptr [edi]

end

// ; rdiv
inline % 88h
                                                   
  fld  qword ptr [edi]
  fdiv qword ptr [eax] 
  fstp qword ptr [edi]

end

// ; rexp
inline % 8Ah

  mov   ebx, 0
  fld   qword ptr [eax]   // ; Src

  fldl2e                  // ; ->log2(e)
  fmulp                   // ; ->log2(e)*Src
                                                              
  // ; the FPU can compute the antilog only with the mantissa
  // ; the characteristic of the logarithm must thus be removed
      
  fld   st(0)             // ; copy the logarithm
  frndint                 // ; keep only the characteristic
  fsub  st(1),st(0)       // ; keeps only the mantissa
  fxch                    // ; get the mantissa on top

  f2xm1                   // ; ->2^(mantissa)-1
  fld1
  faddp                   // ; add 1 back

  //; the number must now be readjusted for the characteristic of the logarithm

  fscale                  // ;, scale it with the characteristic
      
  fstsw ax                // ; retrieve exception flags from FPU
  shr   al,1              // ; test for invalid operation
  jc    short lErr        // ; clean-up and return if error
      
  // ; the characteristic is still on the FPU and must be removed
  
  fstp  st(1)             // ; get rid of the characteristic

  fstp  qword ptr [edi]    // ; store result 
  mov   ebx, 1
  jmp   short labEnd
  
lErr:
  ffree st(1)
  
labEnd:

end

// ; rln
inline % 8Bh

  mov   ebx, 0
  fld   qword ptr [eax]  
  
  fldln2
  fxch
  fyl2x                   // ->[log2(Src)]*ln(2) = ln(Src)

  fstsw ax                // retrieve exception flags from FPU
  shr   al,1              // test for invalid operation
  jc    short lErr        // clean-up and return error

  fstp  qword ptr [edi]    // store result 
  mov   ebx, 1
  jmp   short labEnd

lErr:
  ffree st(0)

labEnd:

end

// ; rabs
inline %8Ch

  fld   qword ptr [eax]  
  fabs
  fstp  qword ptr [edi]    // ; store result 
  
end

// ; rround
inline %8Dh

  mov   ebx, 0
  fld   qword ptr [eax]  

  push  eax               // ; reserve space on CPU stack

  fstcw word ptr [esp]    // ;get current control word
  mov   ax,[esp]
  and   ax,0F3FFh         // ; code it for rounding 
  push  eax
  fldcw word ptr [esp]    // ; change rounding code of FPU to round

  frndint                 // ; round the number
  pop   eax               // ; get rid of last push
  fldcw word ptr [esp]    // ; load back the former control word

  fstsw ax                // ; retrieve exception flags from FPU
  shr   al,1              // ; test for invalid operation
  pop   ebx               // ; clean CPU stack
  jc    short lErr        // ; clean-up and return error
  
  fstp  qword ptr [edi]   // ; store result 
  mov   ebx, 1
  jmp   short labEnd
  
lErr:
  ffree st(0)

labEnd:
  
end

// ; rint

inline % 8Eh

  mov   ebx, 0
  fld   qword ptr [eax]

  push  ebx                // reserve space on stack
  fstcw word ptr [esp]     // get current control word
  mov   dx, [esp]
  or    dx,0c00h           // code it for truncating
  push  edx
  fldcw word ptr [esp]    // change rounding code of FPU to truncate

  frndint                  // truncate the number
  pop   edx                // remove modified CW from CPU stack
  fldcw word ptr [esp]     // load back the former control word
  pop   edx                // clean CPU stack
      
  fstsw ax                 // retrieve exception flags from FPU
  shr   al,1               // test for invalid operation
  jc    short labErr       // clean-up and return error

labSave:
  fstp  qword ptr [edi]    // store result
  mov   ebx, 1
  jmp   short labEnd
  
lErr:
  ffree st(1)
  
labEnd:

end

// ; rload
inline %8Fh

  fld   qword ptr [eax]

end

// ; restore

inline % 92h

  add  ebp, __arg1
  
end

// ; aloadfi
inline % 94h

  mov  eax, [ebp+__arg1]

end

// ; aloadsi
inline % 95h

  mov  eax, [esp+__arg1]

end

// ; ifheap - part of the command

inline % 96h

  // ; GCXT: get current thread frame
  mov  ebx, [data : %CORE_TLS_INDEX]
  mov  ecx, fs:[2Ch]
  mov  ebx, [ecx+ebx*4]
  lea  ebx, [ebx + tls_stack_bottom]

end

// ; open
inline % 98h

  push ebp
  mov  ebp, esp

end

// ; ajumpvi

inline % 0A1h

  mov  edx, [eax - 4]
  jmp  [edx + __arg1]
  nop
  nop
  nop
  nop

end

// ; acallvi (ecx - offset to VMT entry)
inline % 0A2h

  mov  edx, [eax - 4]
  call [edx + __arg1]

end

// ; callextr
inline % 0A5h

  call extern __arg1
  mov  ebx, eax

end

// ; hook label (ecx - offset)
// ; NOTE : hook calling should be the first opcode

inline % 0A6h

  call code : %HOOK

  // ; GCXT: get current thread frame
  mov  ebx, [data : %CORE_TLS_INDEX]
  mov  edx, fs:[2Ch]
  mov  ebx, [edx+ebx*4]

  push [ebx + tls_catch_addr]
  push [ebx + tls_catch_frame]
  push [ebx + tls_catch_level]  
  mov  [ebx + tls_catch_addr], ecx
  mov  [ebx + tls_catch_level], esp
  mov  [ebx + tls_catch_frame], ebp
  
end

// ; address label (ecx - offset)

inline % 0A7h

  call code : %HOOK
  
end

// ; acalli

inline % 0A8h

  mov  esi, [eax + __arg1]
  call esi

end

// ; next
inline % 0AFh

  add  ebx, 1
  cmp  ebx, ecx

end

// ; eloadfi

inline % 0B1h

  mov  ecx, [ebp + __arg1]

end

// ; bsavesi
inline % 0B3h

  mov  [esp+__arg1], edi

end

// ; pushai
inline % 0B4h

  push [eax+__arg1]

end

// ; esavefi

inline % 0B5h

  mov  [ebp + __arg1], ecx

end

// ; dloadfi

inline % 0B7h

  mov  ebx, [ebp + __arg1]

end

// ; dloadsi

inline % 0B8h

  mov  ebx, [esp + __arg1]

end

// ; dsavefi

inline % 0B9h

  mov  [ebp + __arg1], ebx

end

// ; dsavesi

inline % 0BBh

  mov  [esp + __arg1], ebx

end

// ; eloadsi

inline % 0BCh

  mov  ecx, [esp + __arg1]

end

// ; pushf
inline % 0BDh

  lea  edx, [ebp + __arg1]
  push edx

end

// ; esavesi

inline % 0BEh

  mov  [esp + __arg1], ecx

end

// ; reserve
inline % 0BFh

  sub  esp, __arg1
  push ebp
  push 0
  mov  ebp, esp

end

// ; asavebi
inline %0C0h

  mov  esi, edi                     
  // calculate write-barrier address
  sub  esi, [data : %CORE_GC_TABLE + gc_start]
  mov  edx, [data : %CORE_GC_TABLE + gc_header]
  shr  esi, page_size_order
  mov  byte ptr [esi + edx], 1  

  mov [edi + __arg1], eax

end

// ; nwritei

inline % 0C1h

  mov  [edi + __arg1], ecx

end

// ; aswapsi
inline % 0C2h

  mov edx, eax
  mov esi, [esp+__arg1]
  mov [esp+__arg1], edx
  mov eax, esi
  
end

// ; asavesi
inline % 0C3h

  mov  [esp+__arg1], eax

end

// ; asavefi
inline % 0C4h

  mov  [ebp+__arg1], eax

end

// ; bswapsi
inline % 0C5h

  mov esi, edi
  mov edx, [esp+__arg1]
  mov edi, edx
  mov [esp+__arg1], esi
  
end

// ; eswapsi
inline % 0C6h

  mov esi, ecx
  mov edx, [esp+__arg1]
  mov ecx, edx
  mov [esp+__arg1], esi
  
end

// ; dswapsi
inline % 0C7h

  mov esi, ebx
  mov edx, [esp+__arg1]
  mov ebx, edx
  mov [esp+__arg1], esi
  
end

// ; bloadfi

inline % 0C8h

  mov  edi, [ebp + __arg1]

end

// ; bloadsi

inline % 0C9h

  mov  edi, [esp + __arg1]

end

// ; nloadi

inline % 0CAh

  mov  ebx, [eax + __arg1]

end

// ; nsavei

inline % 0CBh

  mov  [edi + __arg1], ebx

end

// ; aloadbi (__arg1 : index)

inline % 0CEh

  mov  eax, [edi + __arg1]

end

// ; axsavebi
inline %0CFh

  mov [edi + __arg1], eax

end

// ; nreadi

inline % 0D1h

  mov  ecx, [eax + __arg1]

end

// ; divn
inline %0DBh

  mov  esi, __arg1
  xor  edx, edx
  push eax
  mov  eax, ebx
  idiv esi
  mov  ebx, eax
  pop  eax

end

// ; init i
// ; __arg1 = i, __arg2 = i * 4 
inline %0DDh

  mov  ecx, __arg1
  sub  esp, __arg2
  xor  eax, eax
  mov  edi, esp
  rep  stos

end

// ; new (ebx - size, __arg1 - length)

inline % 0F0h
	
  mov  ebx, __arg1
  call code : %GC_ALLOC

end

// ; newn (ebx - size, __arg1 - length)

inline % 0F1h

  mov  ebx, __arg1
  call code : %GC_ALLOC
  
end

// ; xselectr (ebx - r1, __arg1 - r2)

inline % 0F3h

  test   eax, eax
  mov    eax, __arg1
  cmovnz eax, edx
  
end

// ; xindexrm
inline % 0F4h

  mov  ebx, __arg1
  
end

// ; xjumprm
inline % 0F5h

  cmp [eax], eax
  jmp __arg1

end

// ; selectr (ebx - r1, __arg1 - r2)

inline % 0F6h

  mov    ecx, __arg1
  test   ebx, ebx
  cmovnz eax, ecx

end

// xcallrm (edx contains message, __arg1 contains vmtentry)
inline % 0FEh

   call code : __arg1

end
