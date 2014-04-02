// --- System Core Data  --
define CORE_EXCEPTION_TABLE 01h
define CORE_GC_TABLE        02h
define CORE_GC_SIZE         03h
define CORE_STAT_COUNT      04h
define CORE_STATICROOT      05h
define CORE_VM_TABLE        06h

// GC TABLE
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
define gc_stack_frame        002Ch
define gc_mg_wbar            0030h
define gc_stack_bottom       0034h

// CORE VM TABLE
define vm_Instance      0000h
define vm_loadSymbol    0004h
define vm_loadName      0008h
define vm_interprete    000Ch

// Object header fields
define elObjectOffset    000Ch
define elSizeOffset      000Ch
define elCountOffset     0008h
define elVMTOffset       0004h 
define elVMTSizeOffset   000Ch

// Page Size
define page_size               10h
define page_size_order          4h
define page_size_order_minus2   2h
define page_mask        0FFFFFFF0h
define page_ceil               1Bh

define subj_mask         80FFFFF0h

// --- System Core API  --
define GC_ALLOC	         10001h
define HOOK              10010h
define GETCLASSNAME      10011h
define INIT_RND          10012h
define EVALSCRIPT        10013h

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
  dd 0 // ; gc_stack_frame        : +2Ch 
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
  mov  [eax], ebx
  mov  [data : %CORE_GC_TABLE + gc_yg_current], esi
  lea  eax, [eax + elObjectOffset]
  ret

labYGCollect:

  // ; save registers
  push edi                             

  // ; lock frame
  mov  edx, [data : %CORE_GC_TABLE + gc_stack_frame]
  mov  [edx], esp

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

  // ; collect frames
  mov  esi, [data : %CORE_GC_TABLE + gc_stack_frame]
  
labYGNextFrame:
  mov  eax, [esi+4]
  mov  ecx, [esi]
  sub  esi, ecx
  push ecx
  push esi
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

// ; ==== Command Set ==

// ; throw
inline % 7

  jmp  [data : %CORE_EXCEPTION_TABLE]

end

// bsredirect

inline % 0Eh // (eax - object, edx - message)

  mov  esi, [eax-4]
  xor  ebx, ebx
  mov  ecx, [esi - elVMTSizeOffset]

labSplit:
  test ecx, ecx
  jz   short labEnd

labStart:
  shr  ecx, 1
  setnc bl
  cmp  edx, [esi+ecx*8]
  jb   short labSplit
  nop
  nop
  jz   short labFound
  lea  esi, [esi+ecx*8+8]
  sub  ecx, ebx
  jnz  short labStart
  nop
  nop
  jmp  labEnd
  nop
  nop
labFound:
  jmp  [esi+ecx*8+4]
  nop
  nop

labEnd:
                                                                
end

// ; unbox (esi - size)

inline % 0Fh

   mov  ecx, esi
labNext:
   sub  ecx, 1
   lea  ebx, [eax+ecx*4]
   push [ebx]
   jnz  short labNext

end

// bsgredirect

inline % 10h // (eax - object, edx - message)

  push 0
  mov  ecx, [eax-8]
  push 0

labScanStart:
  mov  [esp+4], eax
  mov  eax, [eax]
  mov  [esp], ecx

  mov  esi, [eax-4]
  xor  ebx, ebx
  mov  ecx, [esi - elVMTSizeOffset]

labSplit:
  test ecx, ecx
  jz   short labNext

labStart:
  shr  ecx, 1
  setnc bl
  cmp  edx, [esi+ecx*8]
  jb   short labSplit
  nop
  nop
  jz   short labFound
  lea  esi, [esi+ecx*8+8]
  sub  ecx, ebx
  jnz  short labStart
  nop
  nop
  jmp  labNext
labFound:
  lea  esp, [esp+8]
  jmp  [esi+ecx*8+4]
  nop

labNext:
  mov  eax, [esp+4]
  mov  ecx, [esp]
  lea  eax, [eax+4]
  sub  ecx, 4
  jnz  labScanStart
  lea  esp, [esp+8]
                                                                
end

// getlen

inline % 11h

  mov  esi, [eax-8]
  cmp  esi, 0
  jl   short labEnd
  shr  esi, 2

labEnd:

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

// get

inline % 18h
   mov  ebx, esi
   shl  ebx, 2
   mov  eax, [edi + ebx]

end

// set

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

// mquit
inline % 1Bh

  mov  ecx, edx
  and  ecx, 0Fh
  pop  esi
  lea  esp, [esp + ecx * 4 + 4]
  jmp  esi
  nop
  nop
 
end

// unhook
inline % 1Dh

  mov  esp, [data : %CORE_EXCEPTION_TABLE + 4]
  mov  edx, [data : %CORE_EXCEPTION_TABLE + 8]
  pop  ebx
  mov  [data : %CORE_GC_TABLE + gc_stack_frame], edx
  mov [data : %CORE_EXCEPTION_TABLE + 8], ebx
  pop  ebx
  mov  [data : %CORE_EXCEPTION_TABLE + 4], ebx
  pop  edx
  mov  [data : %CORE_EXCEPTION_TABLE], edx
  pop  ebp

end

// ; exclude
inline % 1Eh

  mov  ebx, [data : %CORE_GC_TABLE + gc_stack_frame]
  // ; lock managed stack frame          
  mov  [ebx], esp     

end

// ; include
inline % 1Fh

  // ; save previous pointer 
  push [data : %CORE_GC_TABLE + gc_stack_frame]                  
  // ; size field
  push 0                                
  mov  [data : %CORE_GC_TABLE + gc_stack_frame], esp

end

// ; pushai
inline % 24h

  push [eax+__arg1]

end

// msaveparams
inline %29h

  mov  ecx, edx
  and  ecx, 0Fh
  lea  esi, [ebp + __arg1]
  jz   short labEnd
  lea  esi, [esi + ecx * 4]
labNext:
  push [esi]
  sub  ecx, 1
  lea  esi, [esi+4]
  jnz  short labNext
labEnd:

end

// ; pushf
inline % 2Dh

  lea  ebx, [ebp + __arg1]
  push ebx

end

// ; popbi
inline % 31h
  pop  eax

  // ; calculate write-barrier address
  mov  esi, edi
  sub  esi, [data : %CORE_GC_TABLE + gc_start]
  mov  edx, [data : %CORE_GC_TABLE + gc_header]
  shr  esi, page_size_order
  mov  byte ptr [esi + edx], 1  
  mov  [edi + __arg1], eax

end

// ; xpopai
inline % 33h

  pop  ebx
  mov  [eax+__arg1], ebx

end

// popai
inline % 035h
  // ; calculate write-barrier address
  mov  esi, eax
  sub  esi, [data : %CORE_GC_TABLE + gc_start]
  mov  edx, [data : %CORE_GC_TABLE + gc_header]
  shr  esi, page_size_order
  mov  byte ptr [esi + edx], 1  

  pop  edx
  mov [eax + __arg1], edx

end

// ; callextr
inline % 040h

  call extern __arg1

end

// ; acallvi (ecx - offset to VMT entry)
inline % 42h

  mov  esi, [eax - 4]
  call [esi + __arg1]

end

// ; mloadai (__arg1 - index)
inline % 47h

  mov  edx, [eax + __arg1]

end

// ; mloadsi

inline % 048h

  mov  edx, [esp + __arg1]

end

// ; mloadfi

inline % 049h

  mov edx, [ebp + __arg1]
  
end

// ; msaveai (__arg1 - index)
inline % 4Ah

  mov  [eax + __arg1], edx

end

// ; msetverb

inline % 4Bh

  and edx, subj_mask
  or  edx, __arg1

end


// ; maddai

inline % 4Dh

  or  edx, [eax + __arg1]

end

// ; dloadsi

inline % 50h

  mov  esi, [esp + __arg1]

end

// ; dsavesi

inline % 51h

  mov  [esp + __arg1], esi

end

// ; aloadfi
inline % 54h

  mov  eax, [ebp+__arg1]

end

// ; dloadfi

inline % 52h

  mov  esi, [ebp + __arg1]

end

// ; aloadsi
inline % 55h

  mov  eax, [esp+__arg1]

end

// daddsi
inline % 5Bh

  add esi, [esp+__arg1]

end

// dsubsi
inline % 5Ch

  sub esi, [esp+__arg1]

end

// ; dsavefi

inline % 5Dh

  mov  [ebp + __arg1], esi

end

// ; asavebi
inline %61h

  mov  esi, edi                     
  // calculate write-barrier address
  sub  esi, [data : %CORE_GC_TABLE + gc_start]
  mov  edx, [data : %CORE_GC_TABLE + gc_header]
  shr  esi, page_size_order
  mov  byte ptr [esi + edx], 1  

  mov [edi + __arg1], eax

end

// ; asavesi
inline % 63h

  mov  [esp+__arg1], eax

end

// ; asavefi
inline % 64h

  mov  [ebp+__arg1], eax

end

// ; swapsi
inline % 6Ch

  mov ebx, [esp]
  mov ecx, [esp+__arg1]
  mov [esp], ecx
  mov [esp+__arg1], ebx
  
end

// ; aswapsi
inline % 6Dh

  mov ebx, eax
  mov ecx, [esp+__arg1]
  mov [esp+__arg1], ebx
  mov eax, ecx
  
end

// ; axcopyr

inline % 6Eh

   // ; mov  ebx, esi
   // ; shl  ebx, 2
   // ; mov  [eax+ebx], __arg1
   mov  [eax+esi*4], __arg1

end

// iaxloadb

inline % 6Fh

   mov  [eax+__arg1], edi

end
                                 
// ; restore

inline % 82h

  lea  esp, [esp+4]
  pop  ebx
  lea  esp, [esp + __arg1]
  mov  [data : %CORE_GC_TABLE + gc_stack_frame], ebx

end

// ; open
inline % 88h

  push ebp
  mov  ebp, esp

end

// ; ajumpvi

inline % 0A1h

  mov  esi, [eax - 4]
  jmp  [esi + __arg1]
  nop
  nop
  nop
  nop

end

// ; hook label (ecx - offset)

inline % 0A6h

  call code : %HOOK
  push ebp
  push [data : %CORE_EXCEPTION_TABLE]
  mov  ebx, [data : %CORE_GC_TABLE + gc_stack_frame]
  push [data : %CORE_EXCEPTION_TABLE + 4]
  push [data : %CORE_EXCEPTION_TABLE + 8]
  mov  [data : %CORE_EXCEPTION_TABLE], ecx
  mov  [data : %CORE_EXCEPTION_TABLE + 4], esp
  mov  [data : %CORE_EXCEPTION_TABLE + 8], ebx
  
end

// ; nbox (esi - size, __arg1 - vmt)

inline % 0B0h

  mov  ebx, esi
  cmp  eax, [data : %CORE_GC_TABLE + gc_stack_bottom]
  ja   short labSkip                      
  cmp  eax, esp
  jb   short labSkip

  push eax
  mov  ecx, esi
  add  ebx, page_ceil
  neg  ecx
  and  ebx, page_mask  
  call code : %GC_ALLOC
  mov  [eax-8], ecx
  mov  [eax-4], __arg1
  pop  esi
  mov  ebx, eax
labCopy:
  mov  edx, [esi]
  mov  [ebx], edx
  lea  esi, [esi+4]
  lea  ebx, [ebx+4]
  add  ecx, 4
  jnz  short labCopy

labSkip:

end

// ; box (esi - size, __arg1 - vmt)

inline % 0B1h

  mov  ebx, esi
  cmp  eax, [data : %CORE_GC_TABLE + gc_stack_bottom]  
  ja   short labSkip                      
  shl  ebx, 2
  cmp  eax, esp
  jb   short labSkip

  mov  ecx, ebx
  push eax
  add  ebx, page_ceil
  and  ebx, page_mask  
  call code : %GC_ALLOC
  mov  [eax-8], ecx
  mov  [eax-4], __arg1
  pop  esi
  mov  ebx, eax
labCopy:
  mov  edx, [esi]
  mov  [ebx], edx
  lea  esi, [esi+4]
  lea  ebx, [ebx+4]
  sub  ecx, 4
  jnz  short labCopy

labSkip:

end

// ; aloadbi (__arg1 : index)

inline % 0CEh

  mov  eax, [edi + __arg1]

end

// ; test
inline % 0DEh

  mov  ebx, esi
  mov  ecx, [eax-8]
  shl  ebx, 2
  cmp  ecx, ebx

end

// ; wstest
inline % 0DFh

  mov  ecx, [eax-8]
  mov  ebx, esi
  neg  ecx
  shl  ebx, 1
  cmp  ecx, ebx

end
                                 
// ; bstest
inline % 0DDh

  mov  ecx, [eax-8]
  mov  ebx, esi
  neg  ecx
  cmp  ecx, ebx

end

// ; create (ebx - size, __arg1 - length)

inline % 0F0h
	
  mov  ecx, __arg1
  call code : %GC_ALLOC

  mov  [eax-8], ecx

end

// ; createn (ebx - size, __arg1 - length)

inline % 0F1h

  mov  ecx, __arg1
  call code : %GC_ALLOC

  mov  [eax-8], ecx

end

// scallvi (__arg1 : vmt index , ebx - the target)

inline % 0FCh

  mov  esi, [ebx - 4]
  call [esi + __arg1]
  
end

// xcallrm (edx contains message, __arg1 contains vmtentry)
inline % 0FEh

   call code : __arg1

end

// ; === extensions ===

// ncopy (src, tgt)
inline % 101h

  mov  ebx, [eax]
  mov  [edi], ebx
    
end

// ; ncopystr
inline % 102h
  mov  ebx, esi                     // ; radix
  mov  esi, eax                     // ; get str
  mov  ecx, [esi-8]
  xor  edx, edx                     // ; clear flag
  cmp  byte ptr [esi], 2Dh
  lea  ecx, [ecx+2]                 // ; to skip zero
  jnz  short Lab4
  lodsw
  mov  edx, 1                        // ; set flag
  lea  ecx, [ecx+2]                 //  ; to skip minus
Lab4:
  push edx
  xor  eax, eax
Lab1:
  mov  edx, ebx
  mul  edx
  mov  edx, eax
  xor  eax, eax
  lodsw
  cmp  eax, 3Ah
  jb   short lab11
  sub  al, 7
lab11:
  sub  al, 30h
  jb   short Lab2
  cmp  ax, bx
  ja   short Lab2
  add  eax, edx
  add  ecx, 2
  jnz  short Lab1
  nop
  pop  ebx
  test ebx, ebx                                
  jz   short Lab5
  neg  eax
Lab5:
  mov  [edi], eax
  mov  esi, 1
  jmp  short Lab3
Lab2:
  xor  esi, esi
Lab3:
  mov  eax, edi

end

// ; nload
inline % 103h

  mov  ebx, [esp]
  mov  ecx, [ebx]
  mov  [eax], ecx

end

// nequal

inline % 104h

  mov  ecx, [esp]
  mov  ebx, [eax]
  cmp  ebx, [ecx]
  mov  esi, 1
  jz   short labEnd
  mov  esi, 0  
labEnd:

end

// nless
inline % 105h

  mov  ecx, [esp]
  mov  ebx, [eax]
  cmp  ebx, [ecx]
  mov  esi, 1
  jl   short labEnd
  mov  esi, 0  
labEnd:

end

// nnotgreate
inline % 106h

  mov  ecx, [esp]
  mov  ebx, [eax]
  cmp  ebx, [ecx]
  mov  esi, 1
  jle   short labEnd
  mov  esi, 0  
labEnd:

end

// ; nadd
inline % 107h

  mov  ebx, [esp]
  mov  ecx, [ebx]
  add  [eax], ecx

end

// ; nsub
inline % 108h

  mov  ebx, [esp]
  mov  ecx, [ebx]
  sub  [eax], ecx

end

// ; nmul
inline % 0109h

  mov  esi, eax
  mov  ebx, [esp]
  mov  eax, [eax]
  imul [ebx]
  mov  [esi], eax
  mov  eax, esi

end

// ; ndiv
inline % 10Ah
                                                   
  mov  esi, eax
  mov  ebx, [esp]
  mov  eax, [eax]
  cdq
  idiv [ebx]
  mov  [esi], eax
  mov  eax, esi

end

// ; nand                                          
inline % 10Bh
  mov  ebx, [esp]
  mov  ecx, [ebx]
  and  [eax], ecx

end

// ; nor
inline % 10Ch

  mov  ebx, [esp]
  mov  ecx, [ebx]
  or   [eax], ecx

end

// ; nxor
inline % 10Dh

  mov  ebx, [esp]
  mov  ecx, [ebx]
  xor  [eax], ecx

end

// ; nshift
inline % 10Eh

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
  mov [eax], ebx

end

// ; nnot
inline % 10Fh

  mov  ebx, [eax]  
  not  ebx
  mov  [eax], ebx

end

// ; ninc
inline % 110h

  add  [eax], 1

end

// ; lcopy
inline % 111h

  mov  ecx, [eax]
  mov  ebx, [eax+4]
  mov  [edi], ecx
  mov  [edi+4], ebx
    
end

// ; lcopyint
inline % 112h

  mov  ebx, [eax]
  xor  edx, edx
  mov  [edi], ebx
  mov  [edi+4], edx
    
end

// ; lcopystr
inline % 113h

  push edi
  push esi
  mov  esi, eax
  mov  ecx, [esi-8]
  xor  edx, edx
  neg  ecx

  cmp  byte ptr [esi], 2Dh
  lea  ecx, [ecx-2]
  jnz  short labStart

  lea  esi, [esi+2]
  lea  ecx, [ecx-2]
  mov  edx, 1        // set flag in ebx

labStart:
  push edx           // save sign flag
  xor  edi, edi      // edi   - DHI
  xor  ebx, ebx      // ebx   - DLO

labConvert:
  mov  edx, [esp+4]
  mov  eax, edi
  mul  edx           // DHI * 10
  mov  edi, eax

  mov  eax, ebx
  mov  edx, [esp+4]
  mul  edx           // DLO * 10
  add  edi, edx
  mov  ebx, eax

  xor  eax, eax
  lodsw
  sub  al, 30h
  jb   short labErr
  cmp  al, 9
  ja   short labErr

  add ebx, eax       // DLO + EAX
  adc edi, 0         // DHI + CF

  sub  ecx, 2
  jnz  short labConvert

  pop  eax           // restore flag
  test eax, eax
  jz   short labSave

  not  edi           // invert number
  neg  ebx

labSave:

  mov  edx, edi
  pop  esi
  pop  edi

  mov  [edi], ebx
  mov  [edi+4], edx
  mov  esi, 1
  jmp  short labEnd

labErr:
  xor  esi, esi
  pop  edi

labEnd:
  mov  eax, edi

end

// ; lload
inline % 114h

  mov  ebx, [esp]
  mov  ecx, [ebx]
  mov  [eax], ecx
  mov  edx, [ebx+4]
  mov  [eax+4], edx

end

// ; lequal

inline % 115h

  mov  ecx, [esp]
  mov  ebx, [eax]
  mov  edx, [eax+4]  
  cmp  ebx, [ecx]
  mov  esi, 0
  jnz  short labEnd
  cmp  edx, [ecx+4]
  jnz  short labEnd
  mov  esi, 1

labEnd:

end

// ; lless(lo, ro, tr, fr)
inline % 116h

  mov  ecx, [esp]
  mov  ebx, [eax]
  mov  edx, [eax+4]  
  cmp  edx, [ecx+4]
  mov  esi, 1
  jl   short Lab1
  nop
  jnz  short Lab2
  cmp  ebx, [ecx]
  jl   short Lab1
Lab2:
  mov  esi, 0
Lab1:

end

// ; lnotgreate(lo, ro, tr, fr)
inline % 117h

  mov  ecx, [esp]
  mov  ebx, [eax]                                                        
  mov  edx, [eax+4]  
  cmp  edx, [ecx+4]
  mov  esi, 1
  jl   short Lab1
  nop
  jnz  short Lab2
  cmp  ebx, [ecx]
  jle  short Lab1
Lab2:
  mov  esi, 0
Lab1:

end

// ; ladd
inline % 118h

  mov  ebx, [esp]
  mov  edx, [ebx+4]
  mov  ecx, [ebx]
  add [eax], ecx
  adc [eax+4], edx

end

// ; lsub
inline % 119h

  mov  ebx, [esp]
  mov  edx, [eax+4]
  mov  ecx, [eax]
  sub  ecx, [ebx]
  sbb  edx, [ebx+4]
  mov  [eax], ecx
  mov  [eax+4], edx

end

// ; lmul
inline % 11Ah                       
  mov  esi, [esp]        // sour
  mov  edx, eax          // dest

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
  pop  ebx
  mov  [ebx], eax
  mov  [ebx+4], edx
  mov  eax, ebx 

end

// ; ldiv
inline % 11Bh
               
  mov  ebx, [esp]    // ; DVSR
  mov  esi, eax      // ; DVND

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
  pop  ebx

  mov  [ebx], eax
  mov  [ebx+4], edx
  mov  eax, ebx
                                    
end

// ; land                                          
inline % 11Ch
  mov esi, [esp]
  
  mov ebx, [eax]
  mov edx, [esi]

  mov ecx, [eax+4]
  mov esi, [esi+4]

  and ebx, edx
  and ecx, esi

  mov [eax], ebx
  mov [eax+4], ecx
end

// ; lor
inline % 11Dh
  mov esi, [esp]
  
  mov ebx, [eax]
  mov edx, [esi]

  mov ecx, [eax+4]
  mov esi, [esi+4]

  or  ebx, edx
  or  ecx, esi

  mov [eax], ebx
  mov [eax+4], ecx
end

// ; lxor
inline % 11Eh

  mov esi, [esp]
  
  mov ebx, [eax]
  mov edx, [esi]

  mov ecx, [eax+4]
  mov esi, [esi+4]

  xor ebx, edx
  xor ecx, esi

  mov [eax], ebx
  mov [eax+4], ecx

end

// ; lnot
inline % 11Fh

  mov ebx, [eax]
  mov ecx, [eax+4]
                                                                        
  not ebx
  not ecx

  mov [eax], ebx
  mov [eax+4], ecx

end

// ; lshift
inline % 120h

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
  mov  [eax], edx
  mov  [eax+4], ebx

lEnd2:

end

// ; rcopy (src, tgt)
inline % 121h

  mov  ecx, [eax]
  mov  ebx, [eax+4]
  mov  [edi], ecx
  mov  [edi+4], ebx

end

// ; rcopyint (src, tgt)
inline % 122h

  fild dword ptr [eax]
  fstp qword ptr [edi]

end

// ; rcopylong (src, tgt)
inline % 123h

  fild qword ptr [eax]
  fstp qword ptr [edi]

end


// ; rcopystr
inline % 124h

  mov   esi, eax
  push  edi
  sub   esp, 12
  xor   edx, edx
  xor   eax, eax
  xor   ebx, ebx
  mov   edi, esp
  stosd
  stosd
  mov   word ptr [edi], ax
  mov   ecx, 19

atof1:
  lodsw
  cmp   eax, 32                  // " "
  jz    short atof1
  or    eax, eax
  jnz   short atof2

atoflerr:
  add   esp, 12
  pop   edi
  xor   esi, esi
  jmp   atoflend

  //----------------------
  // check for leading sign
  //----------------------

atof2:

  cmp   eax, 43                  // +
  jz    short atof3
  cmp   eax,45                   // -
  jnz   short integer
  mov   dh,80h
atof3:
  mov   byte ptr [edi+1], dh    // put sign byte in bcd string
  xor   edx,edx
  lodsw

  //------------------------------------
  // convert the digits to packed decimal
  //------------------------------------
integer:

  cmp   eax, 46                  // .
  jnz   short atof4
  test  bh, 1
  jnz   short atoflerr           // only one decimal point allowed
  or    bh, 1
  lodsw
atof4:
  cmp   eax, 101                 // "e"
  jnz   short atof5 
  cmp   cl, 19
  jnz   short atof41
  test  bh, 4
  jz    short atoflerr
atof41:  
  jmp   scient
atof5:
  cmp   eax,69                  // "E" 
  jnz   short atof6
  cmp   cl, 19
  jnz   short atof51
  test  bh, 4
  jz    short atoflerr
atof51:  
  jmp   scient
atof6:
  or    eax,eax
  jnz   short atof7
  cmp   cl, 19
  jnz   atof61
  test  bh, 4
  jz    short atoflerr
atof61:
  jmp   laststep1
atof7:
  sub   eax,48                 // "0"
  jc    short atoflerr          // unacceptable character
  cmp   eax,9
  ja    short atoflerr          // unacceptable character
  or    bh,4                   // at least 1 numerical character
  test  bh,1
  jz    short atof8
  add   bl,1                   // bl holds number of decimal digits
  jc    atoflerr               // more than 255 decimal digits
atof8:
  test  eax, eax
  jnz   short atof9
  test  bh,2
  jnz   short atof9
  lodsw
  jmp   short integer
atof9:
  or    bh,2                   // at least 1 non-zero numerical character
  sub   ecx, 1
  jnz   short atof10
  test  bh,1                   // check if decimal point
  jz    atoflerr               // error if more than 18 integer digits in number
  test  eax, eax
  jnz   short atof91            // if trailing decimal 0
  add   ecx, 1
  sub   bl, 1
  lodsw
  jmp   integer
atof91:
  jmp   atoflerr
atof10:
  mov   dh,al
  
integer1:
  lodsw
  cmp   eax, 46                 // "."
  jnz   short atof20
  test  bh,1
  jnz   atoflerr               // only one decimal point allowed
  or    bh, 1                  // use bh bit0 as the decimal point flag
  lodsw
atof20:
  cmp   eax, 101                // "e"
  jnz   short atof30
  mov   ah, dh
  mov   al,0
  rol   al,4
  ror   ax,4
  mov   byte ptr [edi],al
  mov   dh, ah
  jmp   scient
atof30:
  cmp   eax, 69                 // "E"
  jnz   short atof40
  mov   ah, dh
  mov   al,0
  rol   al,4
  ror   ax,4
  mov   byte ptr [edi],al
  mov   dh, ah
  jmp   scient
atof40:  
  or    eax,eax
  jnz   short atof50
  mov   ah, dh
  rol   al,4
  ror   ax,4
  mov   byte ptr [edi],al
  mov   dh, ah
  jmp   short laststep1
atof50:
  sub   eax, 48               // "0"
  jc    atoflerr             // unacceptable character
  cmp   eax,9
  ja    atoflerr             // unacceptable character
  test  bh,1            
  jz    short atof60
  add   bl, 1                // processing decimal digits
atof60:
  sub   ecx, 1
  jnz   short atof70
  test  bh,1                // check if decimal point
  jz    atoflerr            // error if more than 18 integer digits in number
  test  eax, eax
  jnz   short atof602
  add   ecx, 1
  sub   bl, 1
  jmp   integer1
atof602:
  jmp   atoflerr
atof70:
  mov   ah, dh
  rol   al,4
  ror   ax,4
  mov   byte ptr [edi],al
  mov   dh, ah
  sub   edi, 1
  lodsw
  jmp   integer

laststep1:
  cmp   cl,19
  jnz   short laststep
  fldz
  jmp   short laststep2

laststep:
  mov   ah, dh
  xor   edx, edx
  fbld  [esp]
  sub   cl, 1
  add   bl,cl
  movzx eax,bl
  sub   edx,eax

  push  edx
  fild  dword ptr [esp]     // load the exponent
  fldl2t                    // load log2(10)
  fmulp                     // ->log2(10)*exponent
  pop   edx

  // at this point, only the log base 2 of the 10^exponent is on the FPU
  // the FPU can compute the antilog only with the mantissa
  // the characteristic of the logarithm must thus be removed
     
  fld   st(0)             // copy the logarithm
  frndint                 // keep only the characteristic
  fsub  st(1),st(0)       // keeps only the mantissa
  fxch st(1)              // get the mantissa on top

  f2xm1                   // ->2^(mantissa)-1
  fld1
  faddp                   // add 1 back

  // the number must now be readjusted for the characteristic of the logarithm

  fscale                  // scale it with the characteristic
      
  // the characteristic is still on the FPU and must be removed

  fstp  st(1)             // clean-up the register

  fmulp
  fstsw ax                // retrieve exception flags from FPU
  shr   al,1              // test for invalid operation
  jc    atoflerr          // clean-up and return error

laststep2:

  add   esp, 12
  pop   edi
  fstp  qword ptr[edi]    // store result at specified address
  jmp   short atoflend

scient:
  cmp   cl,19
  jnz   short atof80
  fldz
  jmp   short laststep2
  xor   edx, edx

atof80:
  xor   eax,eax
  lodsw
  cmp   ax, 43            // "+"
  jz    atof90
  cmp   ax, 45            // "-"
  jnz   short scient1
  stc
  rcr   eax,1             // keep sign of exponent in most significant bit of EAX
     
atof90:

  lodsw                   // get next digit after sign

scient1:
  push  eax
  and   eax,0ffh
  jnz   short atof100      // continue if 1st byte of exponent is not terminating 0

scienterr:
  pop   edi
  xor   esi, esi
  jmp   atoflerr         // no exponent

atof100:
  sub   eax,30h
  jc    short scienterr    // unacceptable character
  cmp   eax,9
  ja    short scienterr    // unacceptable character
  imul  edx,10
  add   edx,eax
  cmp   edx,4931h
  ja    short scienterr    // exponent too large
  lodsw
  or    eax,eax
  jnz   short atof100
  pop   eax               // retrieve exponent sign flag
  rcl   eax,1             // is most significant bit set?
  jnc   short atof200
  neg   edx

atof200:
  jmp   laststep  

atoflend:
   mov  eax, esi

end

// ; requal

inline % 125h

  mov    ebx, [esp]
  fld    qword ptr [ebx]
  fld    qword ptr [eax]
  fcomip st, st(1)
  mov    esi, 1
  je     short lab1
  mov    esi, 0
lab1:
  fstp  st(0)

end

// ; rless(lo, ro, tr, fr)
inline % 126h

  mov    ebx, [esp]
  fld    qword ptr [ebx]
  fld    qword ptr [eax]
  fcomip st, st(1)
  mov    esi, 1
  jb     short lab1
  mov    esi, 0
lab1:
  fstp  st(0)

end

// ; nnotgreate(lo, ro, tr, fr)
inline % 127h

  mov    ebx, [esp]
  fld    qword ptr [ebx]
  fld    qword ptr [eax]
  fcomip st, st(1)
  mov    esi, 1
  jbe    short lab1
  mov    esi, 0
lab1:
  fstp  st(0)

end

// ; radd
inline % 128h

  mov  ebx, [esp]
  fld  qword ptr [eax]
  fadd qword ptr [ebx] 
  fstp qword ptr [eax]

end

// ; rsub
inline % 129h

  mov  ebx, [esp]
  fld  qword ptr [eax]
  fsub qword ptr [ebx] 
  fstp qword ptr [eax]

end

// ; rmul
inline % 12Ah

  mov  ebx, [esp]
  fld  qword ptr [eax]
  fmul qword ptr [ebx] 
  fstp qword ptr [eax]

end

// ; rdiv
inline % 12Bh
                                                   
  mov  ebx, [esp]
  fld  qword ptr [eax]
  fdiv qword ptr [ebx] 
  fstp qword ptr [eax]

end


// ; raddint
inline % 12Ch

  mov  ebx, [esp]
  fld  qword ptr [eax]
  fild dword ptr [ebx]
  faddp
  fstp qword ptr [eax]

end

// ; rsubint
inline % 12Dh

  mov  ebx, [esp]
  fld  qword ptr [eax]
  fild dword ptr [ebx]
  fsubp
  fstp qword ptr [eax]

end

// ; rmulint
inline % 12Eh

  mov  ebx, [esp]
  fld  qword ptr [eax]
  fild dword ptr [ebx]
  fmulp
  fstp qword ptr [eax]

end

// ; rdivint
inline % 12Fh
                                                   
  mov  ebx, [esp]
  fld  qword ptr [eax]
  fild dword ptr [ebx]
  fdivp
  fstp qword ptr [eax]

end

// ; raddlong
inline % 130h

  mov  ebx, [esp]
  fld  qword ptr [eax]
  fild qword ptr [ebx]
  faddp
  fstp qword ptr [eax]

end

// ; rsublong
inline % 131h

  mov  ebx, [esp]
  fld  qword ptr [eax]
  fild qword ptr [ebx]
  fsubp
  fstp qword ptr [eax]

end

// ; rmullong
inline % 132h

  mov  ebx, [esp]
  fld  qword ptr [eax]
  fild qword ptr [ebx]
  fmulp
  fstp qword ptr [eax]

end

// ; rdivlong
inline % 133h
                                                   
  mov  ebx, [esp]
  fld  qword ptr [eax]
  fild qword ptr [ebx]
  fdivp
  fstp qword ptr [eax]

end

// ; wsgetlength
// ;in : eax - object, esi - size
inline % 134h

  mov  ecx, [eax-8]
  neg  ecx
  shr  ecx, 1
  sub  ecx, 1
  mov  esi, ecx

end

// ;wssetlength
// ;in : eax - object, esi - size
inline % 135h

  // ; lea  ecx, [esi * 2 + 2]                                             
  mov  ecx, esi
  shl  ecx, 1
  add  ecx, 2
  neg  ecx
  mov  [eax-8], ecx

end

// ; wscreate
inline % 136h

  //lea  ebx, [esi * 2 + 2]
  mov  ebx, esi
  shl  ebx, 1
  push eax  
  add  ebx, 2
  mov  ecx, ebx
  add  ebx, page_ceil
  neg  ecx
  and  ebx, page_mask  
  call code : %GC_ALLOC
  mov  [eax-8], ecx
  pop  ebx
  mov  [eax-4], ebx

end

// ; wscopy
inline % 137h

  mov  edx, edi
  mov  esi, eax

  mov  ecx, [esi-8]
  mov  [edi-8], ecx
  
labNext1:
  mov  ebx, [esi]
  mov  [edx], ebx
  lea  esi, [esi+4]
  lea  edx, [edx+4]
  add  ecx, 4
  ja   short labNext1

end

// ; wscopyint
inline % 138h

   mov  ebx, esi   
   push ebp
   mov  eax, [eax]
   mov  ebp, esp
   xor  ecx, ecx
   push eax
   cmp  eax, 0
   jns  short Lab6
   neg  eax
Lab6:
   cmp  eax, ebx
   jb   short Lab5
Lab1:
   xor  edx, edx
   idiv ebx
   push edx
   add  ecx, 2
   cmp  eax, ebx
   jae  short Lab1
Lab5:   
   add  ecx, 4
   push eax
   mov  eax, [ebp-4]
   cmp  eax, 0
   jns  short Lab7
   push 0F6h      // to get "-" after adding 0x30
   add  ecx, 2
Lab7:
   neg  ecx
   mov  esi, edi
   mov  [esi-8], ecx
   mov  edx, 0FFh
   add  ecx, 2             // to skip zero
Lab2:
   pop  eax
   cmp  eax, 0Ah
   jb   short Lab8
   add  eax, 7
Lab8:
   add  eax, 30h
   and  eax, edx
   mov  word ptr [esi], ax
   add  esi, 2
   add  ecx, 2
   jnz  short Lab2
   xor  eax, eax
   mov  word ptr [esi], ax
   lea  esp, [esp+4]
   pop  ebp

end


// ; wscopylong
inline % 139h

   mov  ecx, esi
   push edi
   push ebp
   push [eax+4]
   mov  ebp, esp
   mov  edx, [eax]     // NLO
   mov  eax, [eax+4]   // NHI
   push 0
   or   eax, eax
   jge  short Lab6

   neg  eax 
   neg  edx 
   sbb  eax, 0

Lab6:                 // convert 
   mov  esi, edx      // NLO
   mov  edi, eax      // NHI

Lab1:
   test edi, edi
   jnz  short labConvert
   cmp  esi, ecx
   jb   short Lab5

labConvert:
   mov  eax, edi      // NHI
   xor  edx, edx
   div  ecx
   mov  ebx, eax
   mov  eax, esi      // NLO
   div  ecx
   mov  edi,ebx 
   mov  esi,eax

   push edx
   add  [ebp-4], 1
   jmp  short Lab1

Lab5:   
   push esi

   mov  ecx, [ebp-4]
   add  ecx, 1

   mov  eax, [ebp]
   cmp  eax, 0
   jns  short Lab7
   push 0F6h      // to get "-" after adding 0x30
   add  ecx, 1                  
Lab7:
   mov  edx, ecx                                                                  
   add  ecx, 1    // ;  including trailing zero
   shl  ecx, 1
   mov  esi, [ebp+8]
   neg  ecx
   mov  [esi-8], ecx
   mov  ebx, 0FFh
   mov  ecx, edx
Lab2:
   pop  eax
   cmp  eax, 0Ah
   jb   short Lab8
   add  eax, 7
Lab8:
   add  eax, 30h
   and  eax, ebx
   mov  word ptr [esi], ax
   add  esi, 2
   sub  ecx, 1
   jnz  short Lab2
   xor  eax, eax
   mov  word ptr [esi], ax
   lea  esp, [esp+8]
   pop  ebp
   pop  edi
end


// ; wscopyreal
inline % 13Ah

   mov   ebx, esi                                 
   mov   ecx, eax
   push  ebp
   mov   ebp, esp

   sub   esp, 52  

   push  edi
   
   lea   ebx, [ebx-3]         // get the number of decimal digits (minus 2 for sign and dot)
   cmp   ebx, 13
   jbe   short ftoa1   
   mov   ebx, 13
ftoa1:
   xor   edx, edx

   //-------------------------------------------
   //first examine the value on FPU for validity
   //-------------------------------------------

   fld   qword ptr [ecx]
   fxam                       // examine value on FPU
   fstsw ax                   // get result

   sahf                       // transfer to CPU flags
   jz    short maybezero
   jpo   srcerr               // C3=0 and C2=0 would be NAN or unsupported
   jnc   short getnumsize      // continue if normal finite number

   //--------------------------------
   //value to be converted = INFINITY
   //--------------------------------

   mov   al,43                // "+"
   test  ah,2                 // C1 field = sign
   jz    short ftoa2
   mov   al, 45               // "-"
ftoa2:
   and   eax, 0FFh
   stosw
   mov   eax,4E0049h        // "NI"
   stosd
   mov   eax,490046h        // "IF"
   stosd
   mov   eax,49004Eh        // "IN"
   stosd
   mov   eax,590054h        // "YT"
   stosd
   jmp   finish      

   //-------------------------
   //value to be converted = 0
   //-------------------------
         
maybezero:
   jpe   short getnumsize      // would be denormalized number
   fstp  st(0)                // flush that 0 value off the FPU
   mov   eax,2E0030h          // ".0" szstring
   stosd                      // write it
   mov   eax,30h              // "0" szstring
   stosd                      // write it
   jmp   finish

   //---------------------------
   // get the size of the number
   //---------------------------

getnumsize:
   fldlg2                     // log10(2)
   fld   st(1)                // copy Src
   fabs                       // insures a positive value
   fyl2x                      // ->[log2(Src)]*[log10(2)] = log10(Src)
      
   fstcw word ptr [ebp-4]     // get current control word
   mov   ax, word ptr [ebp-4]
   or    ax,0C00h             // code it for truncating
   mov   word ptr [ebp-8],ax
   fldcw word ptr [ebp-8]     // insure rounding code of FPU to truncating
      
   fist  [ebp-12]             // store characteristic of logarithm
   fldcw word ptr [ebp-4]     // load back the former control word

   ftst                       // test logarithm for its sign
   fstsw ax                   // get result
   sahf                       // transfer to CPU flags
   sbb   [ebp-12],0           // decrement esize if log is negative
   fstp  st(0)                // get rid of the logarithm

   //-----------------------------------------------------------------------
   // get the power of 10 required to generate an integer with the specified
   // number of significant digits
   //-----------------------------------------------------------------------
   
   mov   eax, [ebp-12]
   lea   eax, [eax+1]  // one digit is required
   or    eax, eax
   js    short ftoa21
   cmp   eax, 13
   jbe   short ftoa20
   mov   edx, -1
   mov   ebx, 13
   mov   ecx, ebx
   sub   ecx, eax
   mov   [ebp-16], ecx
   jmp   short ftoa22

ftoa20:
   add   eax, ebx
   cmp   eax, 13
   jbe   short ftoa21
   sub   eax, 13
   sub   ebx, eax      

ftoa21:
   mov   [ebp-16], ebx

ftoa22:

   //----------------------------------------------------------------------------------------
   // multiply the number by the power of 10 to generate required integer and store it as BCD
   //----------------------------------------------------------------------------------------

   fild  dword ptr [ebp-16]
   fldl2t
   fmulp                      // ->log2(10)*exponent
   fld   st(0)
   frndint                    // get the characteristic of the log
   fxch st(1)
   fsub  st(0),st(1)          // get only the fractional part but keep the characteristic
   f2xm1                      // ->2^(fractional part)-1
   fld1
   faddp                      // add 1 back
   fscale                     // re-adjust the exponent part of the REAL number
   fstp  st(1)                // get rid of the characteristic of the log
   fmulp                      // ->16-digit integer

   fbstp tbyte ptr[ebp-28]    // ->TBYTE containing the packed digits
   fstsw ax                   // retrieve exception flags from FPU
   shr   eax,1                // test for invalid operation
   jc    srcerr               // clean-up and return error

   //------------------------------------------------------------------------------
   // unpack BCD, the 10 bytes returned by the FPU being in the little-endian style
   //------------------------------------------------------------------------------

   lea   esi, [ebp-19]        // go to the most significant byte (sign byte)
   push  edi
   lea   edi,[ebp-52]
   mov   eax,3020h
   movzx  ecx,byte ptr[esi]     // sign byte
   cmp   ecx, 00000080h
   jnz   short ftoa5
   mov   al, 45               // insert sign if negative number
ftoa5:

   stosw
   mov   ecx,9
ftoa6:
   sub   esi, 1
   movzx eax,byte ptr[esi]
   ror   ax,4
   ror   ah,4
   add   eax,3030h
   stosw
   sub   ecx, 1
   jnz   short ftoa6

   pop   edi
   lea   esi,[ebp-52]
   
   cmp   edx, 0
   jnz   short scientific

   //************************
   // REGULAR STRING NOTATION
   //************************

   movsb                      // insert sign
   xor   eax, eax
   stosb

   cmp   byte ptr[esi-1], 20h // test if we insert space
   jnz   short ftoa60
   lea   edi, [edi-2]         // erase it

ftoa60:
   mov   ecx,1                // at least 1 integer digit
   mov   eax, [ebp-12]
   or    eax, eax             // is size negative (i.e. number smaller than 1)
   js    short ftoa61
   add   ecx, eax

ftoa61:
   mov   eax, ebx
   add   eax, ecx             // ->total number of digits to be displayed
   sub   eax, 19
   sub   esi, eax             // address of 1st digit to be displayed
   cmp   byte ptr[esi-1], 49  // "1"
   jnz   ftoa8 
   sub   esi, 1
   add   ecx, 1 
ftoa8:
   test  ecx, ecx
   jz    short ftoa8End
ftoa8Next:                    // copy required integer digits
   movzx  eax, byte ptr [esi]
   mov   word ptr [edi], ax
   lea   esi, [esi+1]
   lea   edi, [edi+2]
   sub   ecx, 1
   jnz   short ftoa8Next
ftoa8End:
   mov   ecx,ebx
   or    ecx,ecx
   jz    short ftoa9
   mov   eax,46               // "."
   stosw

ftoa9Next:                    // copy required decimal digits
   movzx  eax, byte ptr [esi]
   mov   word ptr [edi], ax
   lea   esi, [esi+1]
   lea   edi, [edi+2]
   sub   ecx, 1
   jnz   short ftoa9Next
ftoa9:
   jmp   finish

scientific:
   movsb                      // insert sign
   xor   eax, eax
   stosb

   cmp   byte ptr[esi-1], 20h // test if we insert space
   jnz   short ftoa90
   lea   edi, [edi-2]         // erase it

ftoa90:
   mov   ecx, ebx
   mov   eax, 18
   sub   eax, ecx
   add   esi, eax
   cmp   byte ptr[esi-1],49   // "1"
   pushfd                     // save flags for extra "1"
   jnz   short ftoa10
   sub   esi, 1
ftoa10:
   movsb                      // copy the integer
   xor   eax, eax
   stosb

   mov   eax,46               // "."
   stosw

ftoa10Next:                    // copy the decimal digits
   movzx  eax, byte ptr [esi]
   mov   word ptr [edi], ax
   lea   esi, [esi+1]
   lea   edi, [edi+2]
   sub   ecx, 1
   jnz   short ftoa10Next

   mov   eax,69                // "E"
   stosw
   mov   eax,43                // "+"
   mov   ecx,[ebp-12]
   popfd                      // retrieve flags for extra "1"
   jnz   short ftoa11          // no extra "1"
   add   ecx, 1               // adjust exponent
ftoa11:
   or    ecx,ecx
   jns   short ftoa12
   mov   eax,45                // "-"
   neg   ecx                  // make number positive
ftoa12:
   stosw                      // insert proper sign

// Note: the absolute value of the size could not exceed 4931
   
   xor   ebx, ebx   
   mov   eax,ecx
   mov   cl,100
   div   cl                   // ->thousands & hundreds in al, tens & units in AH
   push  eax
   and   eax,0ffh             // keep only the thousands & hundreds
   mov   cl,10
   div   cl                   // ->thousands in al, hundreds in AH
   add   eax,3030h            // convert to characters
   mov   bl, al               // insert them 
   mov   word ptr [edi], bx
   lea   edi, [edi+2]
   shr   eax, 8
   mov   bl, al
   mov   word ptr [edi], bx
   lea   edi, [edi+2]
   pop   eax
   shr   eax,8                // get the tens & units in al
   div   cl                   // tens in al, units in AH
   add   eax,3030h            // convert to characters

   mov   bl, al               // insert them 
   mov   word ptr [edi], bx
   lea   edi, [edi+2]
   shr   eax, 8
   mov   bl, al
   mov   word ptr [edi], bx
   lea   edi, [edi+2]

finish:
   cmp   word ptr [edi-2], 48 // '0'
   jnz   short finish1
   lea   edi, [edi-2]
   jmp   short finish

finish1:
   cmp   word ptr [edi-2], 46 // '.'
   jnz   short finish2
   lea   edi, [edi+2]

finish2:
   xor   ecx, ecx
   mov   word ptr [edi], cx
   lea   ebx, [edi+2]
   pop   edi
   add   esp, 52
   pop   ebp

   sub   ebx, edi
   neg   ebx
   mov   [edi-8], ebx

   jmp   short finish3

srcerr:
   pop   edi
   add   esp, 52
   pop   ebp
   xor   eax,eax
finish3:

/*
oldcw   :-4  (4)
truncw  :-8  (4)
esize   :-12 (4)
tempdw  :-16 (4)
bcdstr  :-28 (12)  // -20
unpacked:- (52)  // -32
*/

end

// ; wscopybuf
inline % 13Bh

  mov  ecx, esi            // ; length
  mov  edx, [esp]          // ; index
  mov  edx, [edx]
  mov  ebx, ecx
  shl  ebx, 1
  neg  ebx
  add  edx, eax            // ; src + index
  sub  ebx, 2
  mov  esi, edi            // ; dst  
  mov  [edi-8], ebx        // ; set length
  
labNext1:
  mov  ebx, [edx]
  and  ebx, 0FFh
  mov  word ptr [esi], bx
  lea  edx, [edx+1]
  lea  esi, [esi+2]
  sub  ecx, 1
  ja   short labNext1
  xor  ebx, ebx
  mov  eax, edi
  mov  word ptr [esi], bx
  
end

// ; wsequal
inline % 13Ch

  mov  esi, [esp]            // s2
  mov  edx, eax              // s1
  mov  ecx, [edx-8]          // s1.length
  mov  eax, 0
  cmp  ecx, [esi-8]          // compare with s2.length
  jnz  short Lab1
  add  ecx, 2
Lab2:
  mov  ebx, [esi]
  cmp  ebx,  [edx]
  jnz  short Lab1
  lea  esi, [esi+4]
  lea  edx, [edx+4]
  add  ecx, 4
  js   short Lab2
  mov  eax, 1
Lab1:
  mov  esi, eax
  mov  eax, edi

end

// ; wsless
inline % 13Dh

  mov  esi, [esp]            // s2
  mov  edx, eax              // s1
  mov  ecx, [edx-8]          // s1 length

  cmp  ecx, [esi-8]
  mov  eax, 0
  jbe  short Lab3
  mov  ecx, [esi-8]
Lab3:
  neg  ecx
Lab2:
  mov  ebx, [edx]              // s1[i] 
  cmp  bx, word ptr [esi]      // compare s2[i] with 
  jb   short Lab1
  ja   short LabEnd
  lea  esi, [esi+2]
  lea  edx, [edx+2]
  sub  ecx, 2
  jnz  short Lab2

Lab1:
  mov  eax, 1

LabEnd:
  mov  esi, eax
  mov  eax, edi
  
end

// ; wsnotgreater
inline % 13Eh

  mov  esi, [esp]            // s2
  mov  edx, eax              // s1
  mov  ecx, [edx-8]          // s1 length

  cmp  ecx, [esi-8]
  mov  eax, [esp]
  ja   short Lab3
  mov  eax, 0
Lab3:
  neg  ecx
Lab2:
  mov  ebx, [edx]              // s1[i] 
  cmp  bx, word ptr [esi]      // compare s2[i] with 
  jb   short Lab1
  ja   short LabEnd
  lea  esi, [esi+2]
  lea  edx, [edx+2]
  sub  ecx, 2
  jnz  short Lab2

Lab1:
  mov  eax, 1

LabEnd:
  mov  esi, eax
  mov  eax, edi

end
                                                                                      
// ; wsadd
inline % 13Fh

  mov  ecx, 2
  mov  esi, eax
  mov  edx, 2
  
  add  ecx, [esi-8]     // ; src
  add  edx, [edi-8]     // ; dst
  neg  edx
  add  [edi-8], ecx
  add  edx, edi
  sub  ecx, 2
  
labNext2:
  mov  ebx, [esi]
  mov  word ptr [edx], ebx
  lea  esi, [esi+2]
  lea  edx, [edx+2]
  add  ecx, 2
  jnz  short labNext2
  
end

// ; wsgetat
inline % 140h

  //mov  ebx, [eax + esi*2]
  mov  ebx, esi  
  mov  edx, [esp]
  mov  ebx, [eax + ebx*2]
  and  ebx, 0FFFFh
  mov  [edx], ebx

end

// ; wssetat
inline % 141h

  //mov  ebx, [eax + esi*2]
  mov  ebx, esi  
  lea  ebx, [edi + ebx*2]
  mov  edx, [eax]
  mov  word ptr [ebx], dx

end

// ; wsindexofstr
inline % 142h

  push eax
  mov  edx, esi     // index
  mov  esi, [esp+4] // subs

  push edi
  mov  edi, eax     // s
  
  mov  ebx, [edi-8]   // get total length  

  neg  ebx
  shl  edx, 1
  sub  ebx, edx
  jbe  short labEnd

  add  ebx, 2
  sub  edx, 2

labNext:
  add  edx, 2
  mov  esi, [esp+8]
  mov  ecx, [esi-8]
  sub  ebx, 2
  lea  ecx, [ecx+2]
  jz   short labEnd
  neg  ecx
  cmp  ebx, ecx
  jb   short labEnd
  mov  edi, [esp+4]
  add  edi, edx

labCheck:
  mov  eax, [edi]
  cmp  ax, word ptr [esi]
  jnz  short labNext
  lea  edi, [edi+2]
  lea  esi, [esi+2]
  sub  ecx, 2
  jnz  short labCheck
  pop  edi
  pop  eax
  shr  edx, 1
  mov  esi, edx
  jmp  short labEnd2

labEnd:
  pop  edi
  pop  eax
  mov  esi, -1
labEnd2:

end

// ; wscopystr
inline % 143h

  mov  ecx, esi            // ; length
  mov  edx, [esp]          // ; index
  mov  edx, [edx]
  mov  ebx, ecx
  shl  edx, 1
  shl  ebx, 1
  neg  ebx
  add  edx, eax            // ; src + index
  sub  ebx, 2
  mov  esi, edi            // ; dst  
  mov  [edi-8], ebx        // ; set length
  
labNext1:
  mov  ebx, [edx]
  mov  word ptr [esi], bx
  lea  edx, [edx+2]
  lea  esi, [esi+2]
  sub  ecx, 1
  ja   short labNext1
  xor  ebx, ebx
  mov  eax, edi
  mov  word ptr [esi], bx
  
end

// ; wsaddstr
inline % 144h
                                                               
  mov  ecx, esi          // ; length
  mov  edx, [edi-8]
  mov  esi, [esp]        // ; index
  neg  edx
  mov  esi, [esi]
  sub  edx, 2  
  shl  ecx, 1
  shl  esi, 1
  sub  [edi-8], ecx
  add  esi, eax
  add  edx, edi
  
labNext2:
  mov  ebx, [esi]
  mov  word ptr [edx], ebx
  lea  esi, [esi+2]
  lea  edx, [edx+2]
  sub  ecx, 2
  ja   short labNext2
  xor  ebx, ebx
  mov  word ptr [edx], bx

end

// ; wsloadname
inline % 145h

  push esi
  
  mov  edx, [eax-elVMTOffset]
  call code : % GETCLASSNAME  

  pop  ecx
  xor  esi, esi

  test eax, eax
  jz   short labEnd

  mov  esi, eax
  mov  edx, edi

labCopy:
  mov  ebx, [esi]                                                                                           
  mov  word ptr [edx], bx
  test ebx, 0FFFFh
  jz   short labFixLen
  lea  esi, [esi+2]
  sub  ecx, 1
  lea  edx, [edx+2]
  jnz  short labCopy

labFixLen:
  mov  esi, edx
  sub  esi, edi
  mov  eax, edi
  
labEnd:

end

// ; bssetbuf
inline % 146h

  mov  ecx, esi            // ; length
  mov  esi, [esp]          // ; index
  mov  edx, [esi]
  mov  ebx, edx
  add  edx, edi            // ; dest + index

  mov  esi, eax            // ; src
  and  ebx, 3h

  jz   short labNext
  sub  ecx, ebx
  push eax
labNext1:
  mov  eax, [esi]
  mov  byte ptr [edx], al
  lea  esi, [esi+1]
  sub  ebx, 1
  lea  edx, [edx+1]
  ja   short labNext1
  pop  eax

labNext:
  test ecx, ecx
  jz   short labSkip

labNext2:
  mov  ebx, [esi]
  mov  [edx], ebx
  lea  esi, [esi+4]
  lea  edx, [edx+4]
  sub  ecx, 4
  ja   short labNext2

labSkip:
  
end

// ; bsgetbuf
inline % 147h

  mov  ecx, esi            // ; length
  mov  edx, [esp]          // ; index
  mov  esi, [edx]
  add  esi, edi            // ; src + index

  mov  edx, eax            // ; dst  

  test ecx, ecx
  jz   short labSkip

labNext2:
  mov  ebx, [esi]
  mov  [edx], ebx
  lea  edx, [edx+4]
  lea  esi, [esi+4]
  sub  ecx, 4
  ja   short labNext2

labSkip:
  
end

// ; bscopystr
inline % 148h

  mov  ecx, esi            // ; length
  mov  edx, [esp]          // ; index
  mov  ebx, ecx
  mov  edx, [edx]
  add  edx, eax            // ; src + index
  neg  ebx
  mov  esi, edi            // ; dst  
  mov  [edi-8], ebx        // ; set length
  
labNext1:
  mov  ebx, [edx]
  mov  word ptr [esi], bx
  lea  edx, [edx+2]
  lea  esi, [esi+2]
  sub  ecx, 2
  ja   short labNext1
  
end

// ; bssetword
inline % 149h 

  mov  edx, esi            // ; index
  mov  ebx, [eax]
  add  edx, edi            // ; dst + index
  
  mov  word ptr [edx], bx
  
end

// ; bsgetword
inline % 14Ah

  mov  edx, esi            // ; index
  add  edx, edi            // ; dst + index
  
  mov  ebx, [edx]
  and  ebx, 0FFFFh
  mov  [eax], ebx
  
end

// ; bsindexof
inline % 14Bh

  mov  ecx, [esp]
  mov  ecx, [ecx]
  add  esi, edi
  mov  ebx, [eax]
labNext:
  mov  edx, [esi]
  cmp  dl, bl
  jz   short labFound
  lea  esi, [esi+1]
  sub  ecx, 1
  ja   short labNext
  lea  esi, [edi-1]
labFound:                         
  sub  esi, edi

end
         
// ; bsindexofword
inline % 14Ch

  mov  ecx, [esp]
  mov  ecx, [ecx]
  add  esi, edi
  mov  ebx, [eax]
labNext:
  mov  edx, [esi]
  cmp  dx, bx
  jz   short labFound
  lea  esi, [esi+1]
  sub  ecx, 1
  ja   short labNext
  lea  esi, [edi-1]
labFound:                         
  sub  esi, edi

end

// ; bseval
inline % 14Dh

  call code : % EVALSCRIPT

end

// ; lrndnew
inline % 14Eh

  call code : % INIT_RND
  mov  [edi], eax
  
end

// ; lrndnext
inline % 14Fh

   mov  ecx, esi
   cmp  ecx, 0
   jz   short labEnd

   push eax
   push esi

   mov  ebx, [edi+4] // NUM.RE
   mov  esi, [edi]   // NUM.FR             
   mov  eax, ebx
   mov  ecx, 15Ah
   mov  ebx, 4E35h                              
   test eax, eax
   jz   short Lab1
   mul  ebx
Lab1: 
   xchg eax, ecx
   mul  esi
   add  eax, ecx
   xchg eax, esi
   mul  ebx
   add  edx, esi
   add  eax, 1
   adc  edx, 0
   mov  ebx, eax
   mov  esi, edx
   mov  ecx, edi
   mov  [ecx+4], ebx
   mov  eax, esi
   and  eax, 7FFFFFFFh
   mov  [ecx] , esi
   cdq
   pop  ecx
   idiv ecx
   pop  eax
   mov  [eax], edx
labEnd:

end

// ; rabs
inline %150h

  fld   qword ptr [eax]  
  fabs
  fstp  qword ptr [edi]    // ; store result 
  
end

// ; rround
inline %151h

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

// ; rexp
inline % 152h

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
inline % 153h

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

// ; rint

inline % 154h

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

// ; rcose

inline % 155h

  fld   qword ptr [eax]  
  fcos
  fstp  qword ptr [edi]    // store result 

end

// ; rsin

inline % 156h

  fld   qword ptr [eax]  
  fsin
  fstp  qword ptr [edi]    // store result 

end

// ; rarctan

inline % 157h

  fld   qword ptr [eax]  
  fld1
  fpatan                  // i.e. arctan(Src/1)
  fstp  qword ptr [edi]    // store result 

end

// ; rsqrt

inline % 158h

  fld   qword ptr [eax]  
  fsqrt
  fstp  qword ptr [edi]    // store result 

end

// ; rpi

inline % 159h

  fldpi
  fstp  qword ptr [edi]    // store result 

end

// ; rfgetlenz
inline % 15Ah

  mov  esi, eax
  lea  esi, [esi-4]
labNext:
  lea  esi, [esi+4]
  cmp  [esi], 0
  jnz  short labNext

  sub  esi, eax
  shr  esi, 2
  
end

// ; refcreate

inline % 15Bh

  push eax
// ; lea  ebx, [esi*4]
  mov  ebx, esi
  shl  ebx, 2  
  mov  ecx, ebx
  add  ebx, page_ceil
  and  ebx, page_mask  
  call code : %GC_ALLOC
  mov  [eax-8], ecx
  pop  ebx
  mov  esi, ecx
  mov  [eax-4], ebx
  shr  esi, 2

end

// ; bscreate
inline % 15Ch

  push eax
  mov  ebx, esi
  mov  ecx, esi
  add  ebx, page_ceil
  neg  ecx
  and  ebx, page_mask  
  call code : %GC_ALLOC
  mov  [eax-8], ecx
  pop  ebx
  mov  [eax-4], ebx

end

// ; nsave
inline % 15Dh

  mov  ebx, [esp]
  mov  ecx, [eax]
  mov  [ebx], ecx

end

// ; lsave
inline % 15Eh

  mov  ebx, [esp]
  mov  ecx, [eax]
  mov  [ebx], ecx
  mov  edx, [eax+4]
  mov  [ebx+4], edx

end

// ; wssave
// ; in : eax - literal
// ; out: eax - stack address
inline % 15Fh

  mov  ecx, [eax-8] 
  sub  ecx, 3
  and  ecx, 0FFFFFFF4h

  add  esp, ecx
  mov  esi, esp

labCopyIn:
  mov  ebx, [eax]
  mov  [esi], ebx
  lea  eax, [eax+4]
  lea  esi, [esi+4]
  add  ecx, 4
  js   short labCopyIn       

  mov  eax, esp
  
end

// ; wsreserve
// ; in : eax - literal
// ; out: eax - stack address
inline % 160h

  mov  ecx, [eax-8] 
  sub  ecx, 3
  and  ecx, 0FFFFFFF4h

  add  esp, ecx
  mov  eax, esp

end

// ; bssave
// ; in : eax - byte array
// ; out: eax - stack address
inline % 161h

  mov  ecx, [eax-8] 
  sub  ecx, 3
  and  ecx, 0FFFFFFF4h
  
  add  esp, ecx
  mov  esi, esp

labCopyIn:
  mov  ebx, [eax]
  mov  [esi], ebx
  lea  eax, [eax+4]
  lea  esi, [esi+4]
  add  ecx, 4
  js   short labCopyIn       

  mov  eax, esp
  
end

// ; bsreserve
// ; in : eax - byte array
// ; out: eax - stack address
inline % 162h

  mov  ecx, [eax-8] 
  sub  ecx, 3
  and  ecx, 0FFFFFFF4h
  
  add  esp, ecx
  mov  eax, esp

end

// ; bsgetlen
inline % 163h

  mov  ecx, [eax-8]
  neg  ecx
  mov  esi, ecx

end

// ; bssetlen
// ; in : eax - object, esi - size
inline % 164h

  mov ecx, esi
  neg ecx
  mov [eax-8], ecx
  
end

// ; wsload
inline % 165h

  pop esi
  mov ecx, [esi-8] 
  mov edx, esi

labCopyIn:
  mov  ebx, [eax]
  mov  [esi], ebx
  lea  eax, [eax+4]
  lea  esi, [esi+4]
  add  ecx, 4
  js   short labCopyIn

  mov  eax, edx  

end

// ; bsload
// in : eax - stack, [esp] - target
inline % 166h

  pop esi
  mov ecx, [esi-8] 
  mov edx, esi

labCopyIn:
  mov  ebx, [eax]
  mov  [esi], ebx
  lea  eax, [eax+4]
  lea  esi, [esi+4]
  add  ecx, 4
  js   short labCopyIn

  mov  eax, edx  

end


// ; bsgetint
inline % 167h

  mov  edx, esi            // ; index
  add  edx, edi            // ; dst + index
  
  mov  ebx, [edx]
  mov  [eax], ebx
  
end

// ; ncopyword (src, tgt)
inline % 168h

  mov  ebx, [eax]
  and  ebx, 0FFFFh
  mov  [edi], ebx
    
end

// ; loadclass
inline % 169h

  mov  eax, [edi - elVMTOffset]

end

// ; indexofmsg
inline % 16Ah

  mov  esi, [eax - elVMTOffset]
  xor  ebx, ebx
  mov  ecx, [esi - elVMTSizeOffset]
labNext:
  cmp  edx, [esi+ebx*8]
  jz   short labFound
  add  ebx, 1
  sub  ecx, 1
  jnz  short labNext
  mov  ebx, -1

labFound:
  mov   esi, ebx

end
