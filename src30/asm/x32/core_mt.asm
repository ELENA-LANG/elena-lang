// --- System Core Data  --
define CORE_GC_TABLE        02h
define CORE_GC_SIZE         03h
define CORE_STAT_COUNT      04h
define CORE_STATICROOT      05h
define CORE_VM_TABLE        06h
define CORE_TLS_INDEX       07h
define THREAD_TABLE         08h

// --- System Core API  --
define GC_ALLOC	         10001h
define HOOK              10010h
define GETCLASSNAME      10011h
define INIT_RND          10012h
define EVALSCRIPT        10013h

// GCXT TABLE
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
define gc_promotion          0028h
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

// Object header fields
define elObjectOffset        0010h
define elSyncOffset          0010h
define elSizeOffset          000Ch
define elCountOffset         0008h
define elVMTOffset           0004h 

// Page Size
define page_size               10h
define page_size_order          4h
define page_size_order_minus2   2h
define page_mask        0FFFFFFF0h

// CORE GC SIZE
define gcs_HeapSize	0000h
define gcs_YGRatio	0004h

// CORE VM TABLE
define vm_Instance      0000h
define vm_loadSymbol    0004h
define vm_loadName      0008h
define vm_interprete    000Ch

define GC_HEAP_ATTRIBUTE 00Dh

// verbs
define EXEC_MESSAGE_ID  85000000h

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
  dd 0 // ; gc_promotion          : +28h
  dd 0 // ; gc_mg_wbar            : +2Ch

end

// --- System Core Functions --

// --- GC_ALLOC ---
// in: ecx - counter ; ebx - size ; out: eax - created object ; edi contains the object or zero ; ecx is not used but its value saved
procedure %GC_ALLOC

  mov  eax, [data : %CORE_GC_TABLE + gc_yg_current]
  mov  esi, ebx
  mov  edx, [data : %CORE_GC_TABLE + gc_yg_end]
  add  esi, eax
  cmp  esi, edx
  jae  short labYGCollect
  mov  [eax], ebx
  mov  [data : %CORE_GC_TABLE + gc_yg_current], esi
  lea  eax, [eax + elObjectOffset]
  ret

labYGCollect:

  // ; save registers
  push edi                             

  // ; lock frame
  // !! mov  eax, [data : %CORE_GC_TABLE + gc_stack_frame]
  mov  edx, eax
  sub  edx, esp
  mov  [eax], edx

  push ecx
  push ebx
  push ebp
  
  // ; create set of roots
  mov  ebp, esp
  xor  ecx, ecx
  push ecx
  push ecx

  // ; save static roots
  mov  esi, data : %CORE_STATICROOT
  mov  ecx, [data : %CORE_STAT_COUNT]
  push esi
  push ecx

  // ; save frames
  // !! mov  esi, [data : %CORE_GC_TABLE + gc_stack_frame]
  
labYGNextFrame:
  mov  eax, [esi+4]
  mov  ecx, [esi]
  sub  esi, ecx
  push esi
  push ecx
  mov  esi, eax
  test esi, esi
  jnz  short labYGNextFrame

  // ; check if major collection should be performed
  mov  edx, [data : %CORE_GC_TABLE + gc_end]
  mov  ebx, [data : %CORE_GC_TABLE + gc_yg_end]
  sub  edx, [data : %CORE_GC_TABLE + gc_mg_current] // ; mg free space
  sub  ebx, [data : %CORE_GC_TABLE + gc_yg_start]   // ; size to promote 
  cmp  ebx, edx                                     // ; currently it is presumed that all objects
  jae  short labFullCollect                         // ; can be promoted

  // === Minor collection ===

  // ; save mg -> yg roots 
  mov  ebx, [data : %CORE_GC_TABLE + gc_mg_current]
  mov  edi, [data : %CORE_GC_TABLE + gc_mg_start]
  sub  ebx, edi                                        // ; we need to check only MG region
  jz   labWBEnd                                        // ; skip if it is zero
  mov  esi, [data : %CORE_GC_TABLE + gc_mg_wbar]
  shr  ebx, page_size_order
  lea  edi, [edi + elObjectOffset]

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
  lea  eax, [edi + eax]
  
  test edx, 0FFh
  jz   short labWBMark2
  push eax
  mov  ecx, [eax-elCountOffset]
  push ecx

labWBMark2:
  lea  eax, [eax + page_size]
  test edx, 0FF00h
  jz   short labWBMark3
  push eax
  mov  ecx, [eax-elCountOffset]
  push ecx

labWBMark3:
  lea  eax, [eax + page_size]
  test edx, 0FF0000h
  jz   short labWBMark4
  push eax
  mov  ecx, [eax-elCountOffset]
  push ecx

labWBMark4:
  lea  eax, [eax + page_size]
  test edx, 0FF000000h
  jz   short labWBNext
  push eax
  mov  ecx, [eax-elCountOffset]
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
  mov  [data : %CORE_GC_TABLE + gc_promotion], ebp
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

  // ; check if it is enough place
  cmp  ebx, edx
  jae  short labFullCollect

  // ; free root set
  mov  esp, [esp]

  // ; restore registers
  pop  ebp
  pop  ebx
  pop  ecx
  pop  edi

  // ; try to allocate once again
  mov  eax, [data : %CORE_GC_TABLE + gc_yg_current]
  mov  [eax], ebx
  add  ebx, eax
  mov  [data : %CORE_GC_TABLE + gc_yg_current], ebx
  lea  eax, [eax + elObjectOffset]
  ret

labFullCollect:
  // ====== Major Collection ====
  push ebp                      // ; save the stack restore-point

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
  mov  eax, esi
  neg  ecx
  lea  eax, [eax + elObjectOffset]
  add  esi, ecx
  mov  [edi], eax
  shr  ecx, page_size_order_minus2
  add  edi, ecx
  cmp  esi, edx
  jb   short labMGSkipNext

labMGSkipEnd:
  mov  ebp, esi
  
  // ; compact
labMGCompactNext:
  add  esi, ecx
  shr  ecx, page_size_order_minus2
  add  edi, ecx
  cmp  esi, edx
  jae  short labMGCompactEnd

labMGCompactNext2:
  mov  ecx, [esi]
  test ecx, ecx
  jns  short labMGCompactNext
  mov  eax, ebp
  neg  ecx
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
  add  esi, ecx
  shr  ecx, page_size_order_minus2
  add  edi, ecx
  cmp  esi, edx
  jae  short labYGPromEnd
labYGPromNext2:
  mov  ecx, [esi]
  test ecx, ecx
  jns  short labYGPromNext
  mov  eax, ebp
  neg  ecx
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
  mov  [data : %CORE_GC_TABLE + gc_promotion], eax
  
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
  mov  esi, [data : %CORE_GC_TABLE + gc_mg_wbar]
  mov  ecx, [data : %CORE_GC_TABLE + gc_end]
  xor  eax, eax
  sub  ecx, [data : %CORE_GC_TABLE + gc_mg_start]
  shr  ecx, page_size_order

labClearWBar:
  mov  [esi], eax
  sub  ecx, 4
  lea  esi, [esi+4]
  ja   short labClearWBar
	
  // ; free root set
  mov  esp, [esp]
  pop  ebp
  pop  ebx
  pop  ecx
  pop  edi 

  // ; allocate
  mov  eax, [data : %CORE_GC_TABLE + gc_yg_current]
  mov  [eax], ebx
  mov  edx, [data : %CORE_GC_TABLE + gc_yg_end]
  add  ebx, eax
  cmp  ebx, edx
  jae  short labError2
  mov  [data : %CORE_GC_TABLE + gc_yg_current], ebx
  lea  eax, [eax + elObjectOffset]
  ret

labError:
  // ; restore stack
  mov  esp, [esp]
  pop  ebp
  pop  ebx
  pop  ecx
  pop  edi 

labError2:
  push 0
  push 0
  push 1
  push 0C0000017h
  call extern 'dlls'KERNEL32.RaiseException
  ret  

  // start collecting: esi => ebp, [ebx, edx] ; ecx - count
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

  // ; check if the object should be promoted
  cmp  eax, [data : %CORE_GC_TABLE + gc_promotion]
  jb   labYGPromMin

  // ; save previous ecx field
  push ecx

  // ; copy object size
  mov  [ebp], edi

  // ; copy object vmt
  mov  ecx, [eax - elVMTOffset]
  mov  [ebp + 8], ecx
  
  // ; mark as collected
  or   [eax - elSizeOffset], 80000000h

  // ; reserve shadow YG
  mov  ecx, edi
  lea  edi, [ebp + elObjectOffset]
  add  ebp, ecx

  // ; update reference
  mov  [esi], edi

  // ; get object size
  mov  ecx, [eax - elCountOffset]

  // ; save ESI
  push esi
  mov  esi, eax

  // ; copy object size
  mov  [edi - elCountOffset], ecx

  // ; save new reference
  mov  [eax - elVMTOffset], edi

  // ; check if the object has fields
  cmp  ecx, 0 // probaly will be enough just test ecx, ecx and analize c flag

  // ; save original reference
  push eax

  // ; collect object fields if it has them
  jg   labYGCheck

  lea  esp, [esp+4]
  jz   short labYGSkipCopyData

  // ; copy meta data object to shadow YG
  neg  ecx  
  add  ecx, 3
  and  ecx, 0FFFFFFFCh

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

  mov  ecx, [edi-elCountOffset]
  mov  esi, [edi - elVMTOffset]

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

  // ; ---- minor promotion to mg ---
labYGPromMin:

  // ; save yg pointer
  push ecx
  mov  [data : %CORE_GC_TABLE + gc_yg_current], ebp
  push 0
  mov  ebp, [data : %CORE_GC_TABLE + gc_mg_current]
  mov  ecx, 4
  jmp  short labYGPromMinBegin

labYGPromMinNext:
  lea  esi, [esi+4]
  sub  ecx, 4
  jz   labYGPromMinResume

labYGPromMinCheck:
  mov  eax, [esi]

  // ; check if it valid reference
  cmp  eax, ebx
  jl   short labYGPromMinNext 
  nop
  cmp  edx, eax
  jl   short labYGPromMinNext

  // ; check if it was collected
  mov  edi, [eax-elSizeOffset]
  test edi, edi
  js   labYGPromMinContinue

labYGPromMinBegin:
  // ; save previous ecx field
  push ecx

  // ; copy object size
  mov  [ebp], edi

  // ; copy object vmt
  mov  ecx, [eax - elVMTOffset]
  mov  [ebp + 8], ecx
  
  // ; mark as collected
  or   [eax - elSizeOffset], 80000000h

  // ; reserve MG
  mov  ecx, edi
  lea  edi, [ebp + elObjectOffset]
  add  ebp, ecx

  // ; update reference
  mov  [esi], edi

  // ; get object size
  mov  ecx, [eax - elCountOffset]

  // ; save ESI
  push esi
  mov  esi, eax

  // ; copy object size
  mov  [edi - elCountOffset], ecx

  // ; save new reference
  mov  [eax - elVMTOffset], edi

  // ; check if the object has fields
  cmp  ecx, 0

  // ; save original reference
  push eax

  // ; collect object fields if it has them
  jg   labYGPromMinCheck

  lea  esp, [esp+4]
  jz   short labYGPromMinSkipCopyData

  // ; copy meta data object to MG
  neg  ecx  
  add  ecx, 3
  and  ecx, 0FFFFFFFCh

labYGPromMinCopyData:
  mov  eax, [esi]
  sub  ecx, 4
  mov  [edi], eax
  lea  esi, [esi+4]
  lea  edi, [edi+4]
  jnz  short labYGPromMinCopyData

labYGPromMinSkipCopyData:
  pop  esi
  pop  ecx
  jmp  labYGPromMinNext

labYGPromMinResume:
  // ; copy object to shadow MG
  pop  edi
  test edi, edi
  jnz   short labYGPromMinResume2

  lea  esi, [esi-4]
  mov  [data : %CORE_GC_TABLE + gc_mg_current], ebp
  pop  ecx
  mov  ebp, [data : %CORE_GC_TABLE + gc_yg_current]
  jmp  labYGNext

labYGPromMinResume2:
  mov  ecx, [edi-elCountOffset]
  mov  esi, [edi - elVMTOffset]

labYGPromMinCopy:
  mov  eax, [edi]
  sub  ecx, 4
  mov  [esi], eax
  lea  esi, [esi+4]
  lea  edi, [edi+4]
  jnz  short labYGPromMinCopy

  pop  esi
  pop  ecx
  jmp  labYGPromMinNext
  
labYGPromMinContinue:
  // ; bad luck, the referred object cannot be promoted
  // ; we have to mark in WB card
  push ecx
  mov  ecx, [esp+8]
  // ; get the promoted object (the referree object) address
  mov  ecx, [ecx]
  sub  ecx, [data : %CORE_GC_TABLE + gc_start]
  shr  ecx, page_size_order
  add  ecx, [data : %CORE_GC_TABLE + gc_header]
  mov  byte ptr [ecx], 1  
  pop  ecx

  // ; update reference
  mov  edi, [eax - elVMTOffset]
  mov  [esi], edi
  jmp  labYGPromMinNext

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
  neg  edi
  cmp  [eax - elCountOffset], 0
  mov  [eax - elSizeOffset], edi

  jle  short labMGNext

  // ; save previous ecx field
  push ecx

  // ; get object size
  mov  ecx, [eax - elCountOffset]

  // ; save ESI
  push esi
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

  neg  edi
  mov  [eax - elSizeOffset], edi

  cmp  [eax - elCountOffset], 0
  jle  short labFixNext

  // ; save previous ecx field
  push ecx

  // ; get object size
  mov  ecx, [eax - elCountOffset]

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

// --- HOOK ---
// in: ecx - catch offset
procedure %HOOK
                          
  add  ecx, [esp]
  sub  ecx, 5             // ; call command size should be excluded
  ret

end

// --- Core API --

procedure core'default_handler

/*                                                       
  mov  esp, [data : %CORE_EXCEPTION_TABLE + 4]
  mov  eax, 1                         // exit error code
  push eax
  call extern 'dlls'KERNEL32.ExitProcess     // exit
*/

end

procedure core'init
  // GCXT: initialize signal
  xor  ebx, ebx
  mov  [data : %CORE_GC_TABLE + gc_signal], ebx

  // initialize
  mov  ecx, [data : %CORE_STAT_COUNT]
  mov  edi, data : %CORE_STATICROOT
  test ecx, ecx
  jz   short labNext
  xor  eax, eax

clear:
  mov  [edi], eax     
  sub  ecx, 4
  lea  edi, [edi+4]
  jnz  short clear

labNext:

  // calculate total heap size
  mov  ebx, [data : %CORE_GC_SIZE]
  and  ebx, 0FFFFFF80h     // align to 128

  mov  ecx, ebx
  mov  eax, ebx
  shl  ebx, page_size_order   
  shl  eax, 2              
  push ebx                // save size 
  mov  edx, ebx            
  add  ebx, eax           // add heap header  

  xor  edx, edx
  mov  ecx, 100           // calculate yg heap size
  div  ecx
  mov  ecx, [data : %CORE_GC_SIZE + gcs_YGRatio]
  mul  ecx

  and  eax, 0FFFFFF80h    // align to 128
  push eax                // save yg size

  push ebx                // create heap
  push GC_HEAP_ATTRIBUTE
  call extern 'dlls'KERNEL32.GetProcessHeap
  push eax 
  call extern 'dlls'KERNEL32.HeapAlloc

  mov  [data : %CORE_GC_TABLE + gc_header], eax
  mov  ecx, [data : %CORE_GC_SIZE]
  shl  ecx, 2              
  add  eax, ecx           // skip header

  mov  [data : %CORE_GC_TABLE + gc_start], eax
  mov  [data : %CORE_GC_TABLE + gc_yg_start], eax
  mov  [data : %CORE_GC_TABLE + gc_yg_current], eax
  mov  [data : %CORE_GC_TABLE + gc_promotion], eax

  pop  edx                // get yg size 
  pop  ebx                // get total size 
  add  ebx, eax
  mov  [data : %CORE_GC_TABLE + gc_end], ebx
  
  shr  edx, 1             // halve yg size
  and  edx, page_mask     

  add  eax, edx
  mov  [data : %CORE_GC_TABLE + gc_yg_end], eax
  mov  [data : %CORE_GC_TABLE + gc_shadow], eax

  add  eax, edx
  mov  [data : %CORE_GC_TABLE + gc_shadow_end], eax
  mov  [data : %CORE_GC_TABLE + gc_mg_start], eax
  mov  [data : %CORE_GC_TABLE + gc_mg_current], eax

  // ; initialize wbar start
  mov  edx, [data : %CORE_GC_TABLE + gc_mg_start]
  sub  edx, [data : %CORE_GC_TABLE + gc_start]
  shr  edx, page_size_order
  add  edx, [data : %CORE_GC_TABLE + gc_header]
  mov  [data : %CORE_GC_TABLE + gc_mg_wbar], edx
  
  // ; GCXT: assign tls entry
  mov  ebx, [data : %CORE_TLS_INDEX]
  mov  ecx, fs:[2Ch]
  mov  esi, [ecx + ebx*4]

  // ; init thread flags  
  // mov  [esi + tls_flags], 0       
  
  // ; set thread event handle
  push 0
  push 0
  push 0FFFFFFFFh // -1
  push 0
  call extern 'dlls'KERNEL32.CreateEventW  
  mov  [esi + tls_sync_event], eax     

  mov  eax, data : %THREAD_TABLE
  mov  [eax], esi       // ; save tls reference 

  ret

end

procedure core'newframe

  // ; put frame end and move procedure returning address
  pop  edx           

  // ; GCXT
  // ; get thread table entry from tls
  mov  ecx, [data : %CORE_TLS_INDEX]
  mov  esi, fs:[2Ch]
  mov  esi, [esi+ecx*4]

  xor  ebx, ebx
  push ebx                      
  push ebx

  // ; GCXT
  // ; set stack frame pointer / bottom stack pointer
  mov  [esi + tls_stack_frame], esp 
  mov  [esi + tls_stack_bottom], esp
  
  // ; GCXT
  // ; set thread table length
  mov  ebx, 1
  mov  [data : %CORE_GC_TABLE + tt_ptr], ebx   
  
  push edx

  ret

end

procedure core'endframe

  // ; GCXT: get thread table entry from tls
  mov  ebx, [data : %CORE_TLS_INDEX]
  mov  esi, fs:[2Ch]
  mov  esi, [esi+ebx*4]

  // ; save return pointer
  pop  ecx  
  
  xor  edx, edx
  lea  esp, [esp+8]

  // ; GCXT
  mov  [esi + tls_stack_frame], edx

  // ; restore return pointer
  push ecx   
  ret

end

procedure core'openframe

  // ; GCXT: get thread table entry from tls
  mov  ebx, [data : %CORE_TLS_INDEX]
  mov  esi, fs:[2Ch]
  xor  edi, edi
  mov  esi, [esi+ebx*4]
                                        
  // ; save return pointer
  pop  ecx  

  // ; GCXT: get thread table entry from tls
  // ; save previous pointer / size field
  push [esi + tls_stack_frame]
  push edi                             
  mov  [esi + tls_stack_frame], esp
  
  // ; restore return pointer
  push ecx   
  ret

end

procedure core'closeframe

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
  
  // ; restore return pointer
  push ecx   
  ret
  
end

// --- API ---

procedure core'console_entry

/*
  call code : "$package'core'init"
  call code : "$package'core'newframe"

  // set default exception handler
  mov  [data : %CORE_EXCEPTION_TABLE + 4], esp
  mov  ebx, code : "$package'core'default_handler"
  mov  [data : %CORE_EXCEPTION_TABLE], ebx

  // 'program start
  xor  edi, edi
  call code : "'program"
  push eax
  mov  edx, EXEC_MESSAGE_ID
  mov  esi, [eax - 4]
  call [esi + 4]

  mov  eax, 0                         // exit code
  push eax
  call extern 'dlls'KERNEL32.ExitProcess     // exit
*/

  ret

end

procedure core'vm_console_entry

  push ebx
  push ecx
  push edi
  push esi
  push ebp

  call code : "$package'core'init"

  pop  ebp
  pop  esi
  pop  edi
  pop  ecx
  pop  ebx
                                                           
  ret

end


// get total size
// in:  esi - object
// out: ecx - size
procedure core'getsize

  mov  ecx, [esi - elSizeOffset]
  lea  ecx, [ecx - elObjectOffset]  
  ret
                                                
end

// get length
// in:  esi - object
// out: ecx - size
procedure core'getcount

  mov  ecx, [esi - elCountOffset]
  cmp  ecx, 0
  jl   short labEnd
  shr  ecx, 2
labEnd:
  ret

end

// get class name
// in:  edx - VMT
// out: eax - PWSTR
procedure % GETCLASSNAME

  mov  esi, data : %CORE_VM_TABLE
  mov  eax, [esi]
  // ; if vm instance is zero, the operation is not possible
  test eax, eax
  jz   short labEnd

  // ; call LoadClassName (instance, object)
  push edx
  push eax
  mov  edx, [esi + vm_loadName] 
  call edx
  lea  esp, [esp+8]  

labEnd:
  ret

end

// in:  edi - script
// out: eax - result
procedure % EVALSCRIPT

  mov  esi, data : %CORE_VM_TABLE
  mov  eax, [esi]
  // ; if vm instance is zero, the operation is not possible
  test eax, eax
  jz   short labEnd

  // ; call interpreter (instance, tape)
  push edi
  push eax
  mov  edx, [esi + vm_interprete] 
  call edx
  lea  esp, [esp+8]  

labEnd:
  ret

end

procedure % INIT_RND

  sub  esp, 8h
  mov  eax, esp
  sub  esp, 10h
  lea  ebx, [esp]
  push eax 
  push ebx
  push ebx
  call extern 'dlls'KERNEL32.GetSystemTime
  call extern 'dlls'KERNEL32.SystemTimeToFileTime
  add  esp, 10h
  pop  eax
  pop  edx
  ret

end

// set wide literal size
// in : eax   - length
//      esi - literal  (removed by function)
procedure core'set_widestr_len

  shl  eax, 1
  // ; widestr length should include ending zero
  add  eax, 2
  neg  eax
  mov  [esi-8], eax

  mov  eax, esi  
  ret
  
end
