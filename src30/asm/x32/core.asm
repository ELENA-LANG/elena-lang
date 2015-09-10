// --- Predefined References  --
define GC_ALLOC	            10001h
define HOOK                 10010h
define LOAD_SYMBOL          10011h
define INIT_RND             10012h
define INIT                 10013h
define NEWFRAME             10014h
define INIT_ET              10015h
define ENDFRAME             10016h
define RESTORE_ET           10017h
define LOAD_CLASSNAME       10018h
define OPENFRAME            10019h
define CLOSEFRAME           1001Ah
define NEWTHREAD            1001Bh
define CLOSETHREAD          1001Ch
define EXIT                 1001Dh
define CALC_SIZE            1001Eh
define SET_COUNT            1001Fh
define GET_COUNT            10020h
define THREAD_WAIT          10021h
define LOAD_ADDRESSINFO     10023h
define LOAD_CALLSTACK       10024h
define NEW_HEAP             10025h
define BREAK                10026h
define PREPARE              10027h
define LOAD_SUBJECT         10028h
define LOAD_SUBJECTNAME     10029h

define CORE_EXCEPTION_TABLE 20001h
define CORE_GC_TABLE        20002h
define CORE_GC_SIZE         20003h
define CORE_STAT_COUNT      20004h
define CORE_STATICROOT      20005h
define CORE_RT_TABLE        20006h
define CORE_OS_TABLE        20009h

// CORE RT TABLE OFFSETS
define rt_Instance      0000h
define rt_loadSymbol    0004h
define rt_loadName      0008h
define rt_interprete    000Ch
define rt_lasterr       0010h
define rt_loadaddrinfo  0014h
define rt_loadSubject   0018h
define rt_loadSubjName  001Ch

// CORE GC SIZE OFFSETS
define gcs_MGSize	0000h
define gcs_YGSize	0004h

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
define gc_promotion          0028h
define gc_ext_stack_frame    002Ch
define gc_mg_wbar            0030h
define gc_stack_bottom       0034h

// Page Size
define page_size               10h
define page_size_order          4h
define page_size_order_minus2   2h
define page_mask        0FFFFFFF0h
define page_ceil               1Fh

// Object header fields
define elObjectOffset        0010h
define elSizeOffset          000Ch
define elCountOffset         0008h
define elVMTOffset           0004h 
define elVMTFlagOffset       0008h
define elVMTSizeOffset       000Ch

define elPageSizeOffset      0004h //  ; = elObjectOffset - elSizeOffset

define subj_mask         80FFFFF0h

// --- System Core Preloaded Routines --

structure % CORE_EXCEPTION_TABLE

  dd 0 // ; core_catch_addr       : +x00   - exception point of return
  dd 0 // ; core_catch_level      : +x04   - stack level
  dd 0 // ; core_catch_frame      : +x08   - stack frame pointer

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
  dd 0 // ; gc_promotion          : +28h
  dd 0 // ; gc_ext_stack_frame    : +2Ch 
  dd 0 // ; gc_mg_wbar            : +30h
  dd 0 // ; gc_stack_bottom       : +34h

end

// --- GC_ALLOC ---
// in: ecx - counter ; ebx - size ; out: eax - created object ; edi contains the object or zero ; ecx is not used but its value saved
procedure %GC_ALLOC

  mov  eax, [data : %CORE_GC_TABLE + gc_yg_current]
  mov  esi, ebx
  mov  edx, [data : %CORE_GC_TABLE + gc_yg_end]
  add  esi, eax
  cmp  esi, edx
  jae  short labYGCollect
  mov  [eax + elPageSizeOffset], ebx
  mov  [data : %CORE_GC_TABLE + gc_yg_current], esi
  lea  eax, [eax + elObjectOffset]
  ret

labYGCollect:

  // ; save registers
  push edi                             
  push ebp

  // ; lock frame
  mov  [data : %CORE_GC_TABLE + gc_ext_stack_frame], esp

  push ecx
  push ebx                        
  
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

  // ; collect frames
  mov  eax, [data : %CORE_GC_TABLE + gc_ext_stack_frame]  
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
  lea  esi, [esi + 1]
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
  // ; save the stack restore-point
  push ebp

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
  mov  ebx, [ebx]                           // ; restore object size
  mov  [data : %CORE_GC_TABLE + gc_shadow_end], ecx

  sub  edx, ebp

  // ; check if it is enough place
  cmp  ebx, edx
  pop  ebp                   
  jae  short labFullCollect

  // ; free root set
  mov  esp, ebp             
  
  // ; restore registers
  pop  ebx
  pop  ecx
  pop  ebp
  pop  edi

  // ; try to allocate once again
  mov  eax, [data : %CORE_GC_TABLE + gc_yg_current]
  mov  [eax + elPageSizeOffset], ebx
  add  ebx, eax
  mov  [data : %CORE_GC_TABLE + gc_yg_current], ebx
  lea  eax, [eax + elObjectOffset]
  ret

labFullCollect:
  // ; ====== Major Collection ====
  // ; save the stack restore-point
  push ebp                                     

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
  mov  ecx, [esi + elPageSizeOffset]
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
  mov  ecx, [esi + elPageSizeOffset]
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
  mov  ecx, [esi + elPageSizeOffset]
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
  pop  ebx
  pop  ecx
  pop  ebp
  pop  edi 

  // ; allocate
  mov  eax, [data : %CORE_GC_TABLE + gc_yg_current]
  mov  [eax + elPageSizeOffset], ebx
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
  pop  ebx
  pop  ecx
  pop  ebp
  pop  edi 

labError2:

  mov  ebx, 0C0000017h
  call code : % BREAK
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
  mov  [ebp+4], edi

  // ; copy object vmt
  mov  ecx, [eax - elVMTOffset]
  mov  [ebp + 0Ch], ecx
  
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
  mov  [ebp + elPageSizeOffset], edi

  // ; copy object vmt
  mov  ecx, [eax - elVMTOffset]
  mov  [ebp + 0Ch], ecx
  
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

  // ; initialize
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
  // ;calculate total heap size
  // ; mg size
  mov  edi, [data : %CORE_GC_SIZE]             
  and  edi, 0FFFFFF80h     // align to 128
  mov  eax, edi
  
  // ; yg size
  mov  esi, [data : %CORE_GC_SIZE + gcs_YGSize]
  and  esi, 0FFFFFF80h    // align to 128
  add  eax, esi
  add  eax, esi
  
  // ; add header
  mov  ebx, eax
  shl  eax, page_size_order   
  shl  ebx, 2
  push ebx
  add  eax, ebx

  // ; create heap
  call code : %NEW_HEAP

  shl  esi, page_size_order
  shl  edi, page_size_order

  // ; initialize gc table
  pop  ecx
  mov  [data : %CORE_GC_TABLE + gc_header], eax

  // ; skip header
  add  eax, ecx           

  // ; initialize yg
  mov  [data : %CORE_GC_TABLE + gc_start], eax
  mov  [data : %CORE_GC_TABLE + gc_yg_start], eax
  mov  [data : %CORE_GC_TABLE + gc_yg_current], eax
  mov  [data : %CORE_GC_TABLE + gc_promotion], eax

  // ; initialize gc end
  mov  ecx, eax
  add  ecx, esi
  add  ecx, esi
  add  ecx, edi
  mov  [data : %CORE_GC_TABLE + gc_end], ecx
  
  add  eax, esi
  mov  [data : %CORE_GC_TABLE + gc_yg_end], eax
  mov  [data : %CORE_GC_TABLE + gc_shadow], eax

  add  eax, esi
  mov  [data : %CORE_GC_TABLE + gc_shadow_end], eax
  mov  [data : %CORE_GC_TABLE + gc_mg_start], eax
  mov  [data : %CORE_GC_TABLE + gc_mg_current], eax
  
  // ; initialize wbar start
  mov  edx, [data : %CORE_GC_TABLE + gc_mg_start]
  sub  edx, [data : %CORE_GC_TABLE + gc_start]
  shr  edx, page_size_order
  add  edx, [data : %CORE_GC_TABLE + gc_header]
  mov  [data : %CORE_GC_TABLE + gc_mg_wbar], edx

  ret

end

procedure % NEWFRAME

  // ; put frame end and move procedure returning address
  pop  edx           

  xor  ebx, ebx

  push ebp
  push ebx                      
  push ebx

  // ; set stack frame pointer / bottom stack pointer
  mov  ebp, esp 
  mov  [data : %CORE_GC_TABLE + gc_stack_bottom], esp
  
  push edx
  mov  [data : %CORE_GC_TABLE + gc_ext_stack_frame], ebx

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

  // ; set default exception handler
  pop  ecx
  mov  [data : %CORE_EXCEPTION_TABLE + 4], esp
  mov  [data : %CORE_EXCEPTION_TABLE], ebx
  mov  [data : %CORE_EXCEPTION_TABLE + 8], ebp
  push ecx

  ret

end 

procedure % RESTORE_ET

  pop  edx
  mov  esp, [data : %CORE_EXCEPTION_TABLE + 4]
  push edx

  ret

end 

// ; get class name
// ; in:  esi - max length, eax - PSTR, edi - object
// ; out: eax - PSTR, esi - length
procedure % LOAD_CLASSNAME

  push esi
  push eax
  mov  edx, [edi-elVMTOffset]
  push edx

  mov  esi, data : %CORE_RT_TABLE
  mov  eax, [esi]
  // ; if vm instance is zero, the operation is not possible
  test eax, eax
  jz   short labEnd

  // ; call LoadClassName (instance, object,out buffer, maxlength)
  push eax
  mov  edx, [esi + rt_loadName] 
  call edx
  lea  esp, [esp+4]  

labEnd:
  lea  esp, [esp+0Ch]  
  ret

end

// ; get symbol ref
// ; in:  eax - PSTR
// ; out: eax - address
procedure % LOAD_SYMBOL

  push eax

  mov  esi, data : %CORE_RT_TABLE
  mov  eax, [esi]
  // ; if vm instance is zero, the operation is not possible
  test eax, eax
  jz   short labEnd

  // ; call LoadClassName (instance, object,out buffer, maxlength)
  push eax
  mov  edx, [esi + rt_loadSymbol] 
  call edx
  lea  esp, [esp+4]  

labEnd:
  lea  esp, [esp+4h]  
  ret

end

// ; NOTE : some functions (e.g. system'core_routines'win_WndProc) assumes the function reserves 12 bytes
// ; does not affect eax
procedure % OPENFRAME

  // ; save return pointer
  pop  ecx  

  xor  edi, edi

  mov  esi, [data : %CORE_GC_TABLE + gc_ext_stack_frame]
  // ; save previous pointer / size field
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

  // ; save return pointer
  pop  ecx  
  
  lea  esp, [esp+4]
  pop  edx
  mov  [data : %CORE_GC_TABLE + gc_ext_stack_frame], edx
  pop  ebp
  
  // ; restore return pointer
  push ecx   
  ret

end

procedure % NEWTHREAD

  xor eax, eax
  ret

end

procedure % CLOSETHREAD

  xor eax, eax
  ret
  
end

procedure % THREAD_WAIT

  ret
  
end

// ; ebx - a new length
procedure % CALC_SIZE

  shl  ebx, 2
  add  ebx, page_ceil
  and  ebx, page_mask  
  ret

end

procedure % SET_COUNT

  mov  ebx, [eax - elSizeOffset]
  mov  edx, esi
  lea  ebx, [ebx - elObjectOffset]

  shl  edx, 2
  cmp  ebx, edx
  jl   short labErr

  mov  [eax - elCountOffset], edx
  ret

labErr:
  xor  esi, esi
  ret

end

procedure % GET_COUNT

  mov  esi, [eax - elCountOffset]
  shr  esi, 2
  ret

end

// ; load address info
// ; in:  esi - max length, eax - PSTR, ecx - address
// ; out: eax - PSTR, esi - length
procedure % LOAD_ADDRESSINFO

  push esi
  push eax
  push ecx

  mov  esi, data : %CORE_RT_TABLE
  mov  eax, [esi]
  // ; if vm instance is zero, the operation is not possible
  test eax, eax
  jz   short labEnd

  // ; call LoadAddressInfo (instance, ret point,out buffer, maxlength)
  push eax
  mov  edx, [esi + rt_loadaddrinfo] 
  call edx
  lea  esp, [esp+4]  

labEnd:
  lea  esp, [esp+0Ch]  
  ret
  
end

// ; eax - buffer, ecx - max length ; esi - actual length
procedure % LOAD_CALLSTACK

  mov  edx, [esp]
  xor  esi, esi                                                                             
  mov  ebx, ebp

labNext:
  mov  edx, [ebx + 4]
  cmp  [ebx], 0
  jnz  short labSave
  test edx, edx
  jz   short labEnd
  mov  ebx, edx
  jmp  short labNext                              

labSave:
  mov  [eax + esi * 4], edx
  add  esi, 1
  cmp  esi, ecx
  jge  short labEnd
  mov  ebx, [ebx]
  jmp  short labNext                              

labEnd:
  ret  

end

// ; get subject name
// ; in:  esi - max length, eax - PSTR, ecx - message
// ; out: eax - PSTR, esi - length
procedure % LOAD_SUBJECTNAME

  push esi
  push eax
  push ecx

  mov  esi, data : %CORE_RT_TABLE
  mov  eax, [esi]
  // ; if vm instance is zero, the operation is not possible
  test eax, eax
  jz   short labEnd

  // ; call LoadClassName (instance, object,out buffer, maxlength)
  push eax
  mov  edx, [esi + rt_loadSubjName] 
  call edx
  lea  esp, [esp+4]  

labEnd:
  lea  esp, [esp+0Ch]  
  ret

end

// ; get class name
// ; in:  eax - PSTR subject name
// ; out: ecx - subject id
procedure % LOAD_SUBJECT

  push eax

  mov  esi, data : %CORE_RT_TABLE
  mov  eax, [esi]
  // ; if vm instance is zero, the operation is not possible
  test eax, eax
  jz   short labEnd

  // ; call LoadSubject (instance, name)
  push eax
  mov  edx, [esi + rt_loadSubject] 
  call edx
  lea  esp, [esp+4]  
  mov  ecx, eax

labEnd:
  lea  esp, [esp+04]  
  ret

end

// ; ==== Command Set ==

// ; snop
inline % 4

  nop

end

// ; throw
inline % 7

  jmp  [data : %CORE_EXCEPTION_TABLE]

end

// ; bsredirect

inline % 0Eh // (eax - object, ecx - message)

  mov  edi, [eax-4]
  xor  ebx, ebx
  mov  edx, [edi - elVMTSizeOffset]

labSplit:
  test edx, edx
  jz   short labEnd

labStart:
  shr  edx, 1
  setnc bl
  cmp  ecx, [edi+edx*8]
  jb   short labSplit
  nop
  nop
  jz   short labFound
  lea  edi, [edi+edx*8+8]
  sub  edx, ebx
  jnz  short labStart
  nop
  nop
  jmp  labEnd
  nop
  nop
labFound:
  jmp  [edi+edx*8+4]
  nop
  nop

labEnd:
                                                                
end

// ; len

inline % 11h

  mov  ecx, [edi-8]
  shr  ecx, 2

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

   mov  eax, [edi + esi * 4]

end

// ; set

inline % 19h
                                
   mov  ebx, esi
   shl  ebx, 2
    
   // ; calculate write-barrier address
   mov  ecx, edi
   sub  ecx, [data : %CORE_GC_TABLE + gc_start]
   mov  edx, [data : %CORE_GC_TABLE + gc_header]
   shr  ecx, page_size_order
   mov  byte ptr [ecx + edx], 1  
   mov  [edi + ebx], eax
lEnd:

end

// ; equit
inline % 1Bh

  mov  edx, ecx
  pop  ebx
  and  edx, 000000Fh
  lea  esp, [esp + edx * 4 + 4]
  jmp  ebx
  nop
  nop
 
end

// ; unhook
inline % 1Dh

  mov  esp, [data : %CORE_EXCEPTION_TABLE + 4]
  mov  ebp, [data : %CORE_EXCEPTION_TABLE + 8]
  pop  ebx
  mov  [data : %CORE_EXCEPTION_TABLE + 8], ebx
  pop  edx
  mov  [data : %CORE_EXCEPTION_TABLE + 4], edx
  pop  ebx
  mov  [data : %CORE_EXCEPTION_TABLE], ebx
  
end

// ; create
inline % 1Fh

  mov  ebx, esi
  shl  ebx, 2  
  push eax  
  mov  ecx, ebx
  add  ebx, page_ceil
  and  ebx, page_mask  
  call code : %GC_ALLOC
  mov  [eax-8], ecx
  pop  ebx
  mov  [eax-4], ebx

end

// ; dreserve
inline % 24h

  mov  ebx, ecx
  shl  ebx, 2
  sub  esp, ebx
  push ebp
  push 0
  mov  ebp, esp

end

// ; drestore
inline % 25h
                                                              
  mov  ebp, [ebp + 4]

end

// ; exclude
inline % 26h
                                                       
  push ebp     
  mov  [data : %CORE_GC_TABLE + gc_ext_stack_frame], esp

end

// ; trylock
inline % 27h

  xor  eax, eax

end

// ; freelock
inline % 28h

  nop

end

// ; eswap
inline % 2Ch

  mov  ebx, ecx
  mov  ecx, esi
  mov  esi, ebx
  
end

// ; bswap
inline % 2Dh

  mov  ebx, edi
  mov  edi, eax
  mov  eax, ebx

end

// ; copy

inline % 2Eh

  mov  ecx, [edi-8]
  mov  esi, eax
  mov  ebx, edi
labCopy:
  mov  edx, [esi]
  mov  [ebx], edx
  lea  esi, [esi+4]
  lea  ebx, [ebx+4]
  add  ecx, 4
  js   short labCopy

end

// ; xset

inline % 2Fh
                                
   mov  [edi + esi * 4], eax

end

// ; xlen

inline % 30h

  mov  ecx, [eax-8]
  shr  ecx, 2

end

// ; blen
inline % 31h

  mov  esi, [eax-8]
  neg  esi

end

// ; wlen
// ;in : eax - object, esi - size
inline % 32h

  mov  esi, [eax-8]
  neg  esi
  shr  esi, 1
  
end

// ; flag

inline % 33h

  mov  ebx, [eax - 4]
  mov  esi, [ebx - elVMTFlagOffset]
  
end

// ; nlen
// ;in : eax - object, esi - size
inline % 34h

  mov  esi, [eax-8]
  neg  esi
  shr  esi, 2
  
end

// ; class

inline % 36h

  mov eax, [edi - elVMTOffset]

end

// ; mindex
inline % 37h

  push edi
  mov  edi, [eax-4]
  xor  ebx, ebx
  mov  edx, [edi - elVMTSizeOffset]

labSplit:
  test edx, edx
  jz   short labEnd

labStart:
  shr  edx, 1
  setnc bl
  cmp  ecx, [edi+edx*8]
  jb   short labSplit
  nop
  nop
  jz   short labFound
  lea  edi, [edi+edx*8+8]
  sub  edx, ebx
  jnz  short labStart
  nop
  nop
labEnd:
  mov  esi, -1  

labFound:
  pop  edi  

end

// ; call

inline % 038h

  mov  ebx, [eax]
  call ebx

end

// ; acallvd (ecx - offset to VMT entry)
inline % 039h

  mov  edx, [eax - 4]
  call [edx + esi * 8 + 4]

end

// ; validate

inline % 03Ah

  cmp [eax], eax

end

// ; nequal

inline % 40h

  mov  ebx, [eax]
  cmp  ebx, [edi]
  mov  esi, 1
  jz   short labEnd
  mov  esi, 0  
labEnd:

end

// ; nless
inline % 41h

  mov  ebx, [eax]
  cmp  ebx, [edi]
  mov  esi, 1
  jl   short labEnd
  mov  esi, 0  
labEnd:

end

// ; ncopy (src, tgt)
inline % 42h

  mov  ebx, [eax]
  mov  [edi], ebx
    
end

// ; nadd
inline % 43h

  mov  ecx, [eax]
  add  [edi], ecx

end

// ; nsub
inline % 44h

  mov  ecx, [eax]
  sub  [edi], ecx

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

  mov [edi], esi

end

// ; nload
inline % 48h

  mov esi, [eax]

end

// ; dcopyr
inline % 49h

  push 0
  mov  esi, esp
  fistp dword ptr [esi]
  pop esi

end

// ; nand
inline % 4Ah

  mov  ecx, [eax]
  and  [edi], ecx

end

// ; nor
inline % 4Bh

  mov  ecx, [eax]
  or   [edi], ecx

end

// ; nxor
inline % 4Ch

  mov  ecx, [eax]
  xor  [edi], ecx

end

// ; nshift
inline % 4Dh

  mov ecx, esi
  mov ebx, [eax]
  and ecx, ecx
  jns short lab1
  neg ecx
  shl ebx, cl
  jmp short lab2
lab1:
  shr ebx, cl
lab2:
  mov [edi], ebx

end

// ; nnot
inline % 4Eh

  mov  ebx, [eax]  
  not  ebx
  mov  [edi], ebx

end

// ; ncreate
inline % 4Fh

  mov  ebx, esi
  shl  ebx, 2
  push eax  
  mov  ecx, ebx
  add  ebx, page_ceil
  neg  ecx
  and  ebx, page_mask  
  call code : %GC_ALLOC
  mov  [eax-8], ecx
  pop  ebx
  mov  [eax-4], ebx

end

// ; ncopyb (src, tgt)
inline % 50h

  mov  ebx, [edi]
  mov  [eax], ebx
    
end

// ; lcopyb
inline % 51h

  mov  ecx, [edi]
  mov  ebx, [edi+4]
  mov  [eax], ecx
  mov  [eax+4], ebx
    
end

// ; copyb

inline % 52h

  mov  ecx, [edi-8]
  mov  esi, edi
  mov  ebx, eax
labCopy:
  mov  edx, [esi]
  mov  [ebx], edx
  lea  esi, [esi+4]
  lea  ebx, [ebx+4]
  add  ecx, 4
  js   short labCopy

end

// ; wread
inline % 59h

  mov ecx, [eax + esi * 2]

end

// ; wwrite
inline % 5Ah

  mov [edi + esi * 2], ecx
  
end

// ; nread
inline % 5Bh

  mov ecx, [eax + esi * 4]

end

// ; nwrite
inline % 5Ch

  mov [edi + esi * 4], ecx
  
end

// ; wcreate
inline % 5Fh

  //lea  ebx, [esi * 2 + 2]
  mov  ebx, esi
  shl  ebx, 1
  push eax  
  mov  ecx, ebx
  add  ebx, page_ceil
  neg  ecx
  and  ebx, page_mask  
  call code : %GC_ALLOC
  mov  [eax-8], ecx
  pop  ebx
  mov  [eax-4], ebx

end

// ; breadw
inline % 60h

  mov ecx, [eax + esi]
  and ecx, 0FFFFh  

end

// ; bread
inline % 61h

  mov ecx, [eax + esi]

end

// ; breadb
inline % 65h

  mov ecx, [eax + esi]
  and ecx, 0FFh

end

// ; rsin

inline % 66h

//  fld   qword ptr [eax]  
//  fsin
//  fstp  qword ptr [edi]    // store result 

  mov   ebx, eax
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
  mov   eax, ebx

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

  mov [edi + esi], ecx
  
end

// ; bwriteb
inline % 6Ch

  mov byte ptr [edi + esi], cl
  
end

// ; bwritew
inline % 6Dh

  mov word ptr [edi + esi], cx
  
end

// ; bcreate
inline % 6Fh

  mov  ebx, esi
  push eax  
  mov  ecx, ebx
  add  ebx, page_ceil
  neg  ecx
  and  ebx, page_mask  
  call code : %GC_ALLOC
  mov  [eax-8], ecx
  pop  ebx
  mov  [eax-4], ebx

end

// ; lcopy
inline % 70h

  mov  ecx, [eax]
  mov  ebx, [eax+4]
  mov  [edi], ecx
  mov  [edi+4], ebx
    
end

// ; lequal

inline % 72h

  mov  ebx, [eax]
  mov  edx, [eax+4]  
  cmp  ebx, [edi]
  mov  esi, 0
  jnz  short labEnd
  cmp  edx, [edi+4]
  jnz  short labEnd
  mov  esi, 1

labEnd:

end

// ; lless(lo, ro, tr, fr)
inline % 73h

  mov  ebx, [eax]
  mov  edx, [eax+4]  
  cmp  edx, [edi+4]
  mov  esi, 1
  jl   short Lab1
  nop
  jnz  short Lab2
  cmp  ebx, [edi]
  jl   short Lab1
Lab2:
  mov  esi, 0
Lab1:

end

// ; ladd
inline % 74h

  mov  edx, [eax+4]
  mov  ecx, [eax]
  add [edi], ecx
  adc [edi+4], edx

end

// ; lsub
inline % 75h

  mov  edx, [edi+4]
  mov  ecx, [edi]
  sub  ecx, [eax]
  sbb  edx, [eax+4]
  mov  [edi], ecx
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

  mov  edx, [eax]
  mov  ecx, esi
  mov  ebx, [eax+4]

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
  mov  edx, ebx
  xor  ebx, ebx
  and  cl, 1Fh
  shl  edx, cl 
  jmp  short lEnd

LR:

  cmp  cl, 64
  jae  short lErr

  cmp  cl, 32
  jae  short LR32
  shrd ebx, edx, cl
  sar  edx, cl
  jmp  short lEnd

LR32:
  mov  ebx, edx
  sar  edx, 31
  and  cl, 31
  sar  ebx, cl
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

  mov ebx, [eax]
  mov ecx, [eax+4]
                                                                        
  not ebx
  not ecx

  mov [edi], ebx
  mov [edi+4], ecx

end

// ; rcopy (src, tgt)
inline % 80h

  push esi
  fild dword ptr [esp]
  pop  esi

end

// ; rsave
inline % 82h

  fstp qword ptr [edi]

end

// ; requal

inline % 83h

  fld    qword ptr [edi]
  fld    qword ptr [eax]
  fcomip st, st(1)
  mov    esi, 1
  je     short lab1
  mov    esi, 0
lab1:
  fstp  st(0)

end

// ; rless(lo, ro, tr, fr)
inline % 84h

  fld    qword ptr [edi]
  fld    qword ptr [eax]
  fcomip st, st(1)
  mov    esi, 1
  jb     short lab1
  mov    esi, 0
lab1:
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

  mov   esi, 0
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
  mov   esi, 1
  jmp   short labEnd
  
lErr:
  ffree st(1)
  
labEnd:

end

// ; rln
inline % 8Bh

  mov   esi, 0
  fld   qword ptr [eax]  
  
  fldln2
  fxch
  fyl2x                   // ->[log2(Src)]*ln(2) = ln(Src)

  fstsw ax                // retrieve exception flags from FPU
  shr   al,1              // test for invalid operation
  jc    short lErr        // clean-up and return error

  fstp  qword ptr [edi]    // store result 
  mov   esi, 1
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

  mov   esi, 0
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
  mov   esi, 1
  jmp   short labEnd
  
lErr:
  ffree st(0)

labEnd:
  
end

// ; rint

inline % 8Eh

  mov   esi, 0
  fld   qword ptr [eax]

  push  ebx                // reserve space on stack
  fstcw word ptr [esp]     // get current control word
  mov   bx, [esp]
  or    bx,0c00h           // code it for truncating
  push  ebx
  fldcw word ptr [esp]    // change rounding code of FPU to truncate

  frndint                  // truncate the number
  pop   ebx                // remove modified CW from CPU stack
  fldcw word ptr [esp]     // load back the former control word
  pop   ebx                // clean CPU stack
      
  fstsw ax                 // retrieve exception flags from FPU
  shr   al,1               // test for invalid operation
  jc    short labErr       // clean-up and return error

labSave:
  fstp  qword ptr [edi]    // store result
  mov   esi, 1
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

  nop
  nop
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

  lea  ebx,[data : %CORE_GC_TABLE + gc_stack_bottom]

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
  mov  esi, eax

end

// ; hook label (ecx - offset)
// ; NOTE : hook calling should be the first opcode

inline % 0A6h

  call code : %HOOK

  push [data : %CORE_EXCEPTION_TABLE]
  push [data : %CORE_EXCEPTION_TABLE + 4]
  push [data : %CORE_EXCEPTION_TABLE + 8]
  mov  [data : %CORE_EXCEPTION_TABLE], ecx
  mov  [data : %CORE_EXCEPTION_TABLE + 4], esp
  mov  [data : %CORE_EXCEPTION_TABLE + 8], ebp
  
end

// ; address label (ecx - offset)

inline % 0A7h

  call code : %HOOK
  
end

// ; next
inline % 0AFh

  add  esi, 1
  cmp  esi, ecx

end

// ; eloadfi

inline % 0B1h

  mov  ecx, [ebp + __arg1]

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

  mov  esi, [ebp + __arg1]

end

// ; dloadsi

inline % 0B8h

  mov  esi, [esp + __arg1]

end

// ; dsavefi

inline % 0B9h

  mov  [ebp + __arg1], esi

end

// ; dsavesi

inline % 0BBh

  mov  [esp + __arg1], esi

end

// ; eloadsi

inline % 0BCh

  mov  ecx, [esp + __arg1]

end

// ; pushf
inline % 0BDh

  lea  ebx, [ebp + __arg1]
  push ebx

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

// ; aswapsi
inline % 0C2h

  mov ebx, eax
  mov ecx, [esp+__arg1]
  mov [esp+__arg1], ebx
  mov eax, ecx
  
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

  mov ebx, edi
  mov edx, [esp+__arg1]
  mov edi, edx
  mov [esp+__arg1], ebx
  
end

// ; eswapsi
inline % 0C6h

  mov ebx, ecx
  mov edx, [esp+__arg1]
  mov ecx, edx
  mov [esp+__arg1], ebx
  
end

// ; dswapsi
inline % 0C7h

  mov ebx, esi
  mov edx, [esp+__arg1]
  mov esi, edx
  mov [esp+__arg1], ebx
  
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

  mov  esi, [eax + __arg1]

end

// ; nsavei

inline % 0CBh

  mov  [edi + __arg1], esi

end

// ; aloadbi (__arg1 : index)

inline % 0CEh

  mov  eax, [edi + __arg1]

end

// ; axsavebi
inline %0CFh

  mov [edi + __arg1], eax

end

// ; new (ebx - size, __arg1 - length)

inline % 0F0h
	
  mov  ecx, __arg1
  call code : %GC_ALLOC

  mov  [eax-8], ecx

end

// ; newn (ebx - size, __arg1 - length)

inline % 0F1h

  mov  ecx, __arg1
  call code : %GC_ALLOC

  mov  [eax-8], ecx
  
end

// ; xselectr (ebx - r1, __arg1 - r2)

inline % 0F3h

  test   eax, eax
  mov    eax, __arg1
  cmovnz eax, ebx

end

// ; xindexrm
inline % 0F4h

  mov  esi, __arg1
  
end

// ; xjumprm
inline % 0F5h

  jmp __arg1

end

// ; selectr (ebx - r1, __arg1 - r2)

inline % 0F6h

  mov    ecx, __arg1
  test   esi, esi
  mov    eax, ebx
  cmovnz eax, ecx

end

// xcallrm (edx contains message, __arg1 contains vmtentry)
inline % 0FEh

   call code : __arg1

end
