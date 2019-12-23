// --- Predefined References  --
define GC_ALLOC	            10001h
define HOOK                 10010h
define INIT_RND             10012h
define ENDFRAME             10016h
define RESTORE_ET           10017h
define CALC_SIZE            1001Fh
define GET_COUNT            10020h
define THREAD_WAIT          10021h
define BREAK                10026h
define EXPAND_HEAP          10028h

define CORE_GC_TABLE        20002h
define CORE_STATICROOT      20005h
define CORE_TLS_INDEX       20007h
define THREAD_TABLE         20008h
define CORE_OS_TABLE        20009h
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

// SYSTEM_ENV OFFSETS
define se_mgsize	     0014h
define se_ygsize	     001Ch

// Page Size
define page_size               10h
define page_size_order          4h
define page_size_order_minus2   2h
define page_mask        0FFFFFFF0h
define page_ceil               17h
define struct_mask         800000h
define struct_mask_inv     7FFFFFh

// Object header fields
define elObjectOffset        0008h
define elSizeOffset          0008h
define elVMTOffset           0004h 
define elVMTFlagOffset       0008h
define elVMTSizeOffset       000Ch
define elPackageOffset       0010h

define page_align_mask   000FFFF0h

define ACTION_ORDER              9
define PARAM_MASK             01Fh
define PARAM_MASK_INV   0FFFFFFE0h

// --- System Core Preloaded Routines --

structure % CORE_ET_TABLE

  dd 0 // ; critical_exception    ; +x00   - pointer to critical exception handler

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
  dd 0 // ; gc_mg_wbar            : +28h
  dd 0 // ; gc_et_current         : +2Ch 
  dd 0 // ; gc_stack_frame        : +30h 
  dd 0 // ; gc_lock               : +34h 
  dd 0 // ; gc_signal             : +38h 
  dd 0 // ; tt_ptr                : +3Ch 
  dd 0 // ; tt_lock               : +40h 
  dd 0 // ; dbg_ptr               : +44h 
  dd 0 // ; gc_roots              : +48h 
  dd 0 // ; gc_rootcount          : +4Ch 

end

// ; NOTE : the table is tailed with GCMGSize,GCYGSize and MaxThread fields
rstructure %SYSTEM_ENV

  dd 0
  dd data : %CORE_STATICROOT
  dd data : %CORE_GC_TABLE
  dd data : %CORE_TLS_INDEX
  dd data : %THREAD_TABLE

end

rstructure %VOID

  dd 0
  dd 0
  dd 0
  dd 0
  dd 0

end

rstructure %VOIDPTR

  dd rdata : %VOID
  dd 0

end

// --- GC_ALLOC ---
// in: ecx - size ; out: ebx - created object
procedure %GC_ALLOC

  mov  eax, [data : %CORE_GC_TABLE + gc_yg_current]
  mov  edx, [data : %CORE_GC_TABLE + gc_yg_end]
  add  ecx, eax
  cmp  ecx, edx
  jae  short labYGCollect
  mov  [data : %CORE_GC_TABLE + gc_yg_current], ecx
  lea  ebx, [eax + elObjectOffset]
  ret

labYGCollect:
  // ; restore ecx
  sub  ecx, eax

  // ; save registers
  push ebp

  // ; lock frame
  mov  [data : %CORE_GC_TABLE + gc_stack_frame], esp

  push ecx
  
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

  // ; collect frames
  mov  eax, [data : %CORE_GC_TABLE + gc_stack_frame]  
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

  // === Minor collection ===
  mov [ebp-4], esp      // ; save position for roots

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
  pop  ecx
  pop  ebp

  // ; try to allocate once again
  mov  eax, [data : %CORE_GC_TABLE + gc_yg_current]
  add  ecx, eax
  lea  ebx, [eax + elObjectOffset]
  mov  [data : %CORE_GC_TABLE + gc_yg_current], ecx
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
  shr  ecx, page_size_order
  rep  stos

  // ; free root set
  mov  esp, [esp]
  pop  edx
  pop  ebp

  // ; allocate
  mov  eax, [data : %CORE_GC_TABLE + gc_yg_current]
  mov  ecx, [data : %CORE_GC_TABLE + gc_yg_end]
  add  edx, eax
  cmp  edx, ecx
  jae  labBigAlloc
  lea  ebx, [eax + elObjectOffset]
  mov  [data : %CORE_GC_TABLE + gc_yg_current], edx
  ret

labError:
  // ; restore stack
  mov  esp, [esp]
  pop  ecx
  pop  ebp

labError2:
  mov  ebx, 17h
  call code : % BREAK
  ret  

// ; bad luck, we have to expand GC
labBigAlloc2:
  push ecx

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
  mov  [data : %CORE_GC_TABLE + gc_mg_current], ecx
  lea  ebx, [eax + elObjectOffset]

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
  mov  edi, eax
  cmp  edi, ebx
  setb al
  cmp  edx, edi
  setb ah
  test eax, 0FFFFh
  mov  eax, edi
  jnz  labYGNext

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
  mov  edi, eax
  cmp  edi, ebx
  setb al
  cmp  edx, edi
  setb ah
  test eax, 0FFFFh
  mov  eax, edi
  jnz  labMGNext

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
  mov  edi, eax
  cmp  edi, ebx
  setb al
  cmp  edx, edi
  setb ah
  test eax, 0FFFFh
  mov  eax, edi
  jnz  labFixNext

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

  mov  eax, [esp]       
  lea  ecx, [eax + ecx - 5]               
  // ; add  ecx, [esp]
  // ; sub  ecx, 5             // ; call command size should be excluded
  ret

end

// --- System Core Functions --

procedure % ENDFRAME

  // ; save return pointer
  pop  ecx  
  
  xor  edx, edx
  lea  esp, [esp+8]
  pop  ebp

  pop  eax
  mov  fs:[0], eax
  lea  esp, [esp+4]

  // ; restore return pointer
  push ecx   
  ret

end

procedure % THREAD_WAIT

  ret
  
end

procedure % CALC_SIZE

  mov  ecx, edx
  add  ecx, page_ceil
  and  ecx, page_mask

  ret

end

procedure % GET_COUNT

  mov  edx, [ebx - elSizeOffset]
  test edx, 0800000h
  jnz  short labErr
  and  edx, 0FFFFFFh
  shr  edx, 2
  ret

labErr:
  xor  edx, edx
  ret 

end

// ; ==== Command Set ==

// ; snop
inline % 4

  nop

end

// ; throw
inline % 7

  mov  eax, [data : %CORE_GC_TABLE + gc_et_current]
  jmp  [eax]

end

// ; bsredirect
inline % 0Eh // (ebx - object, edx - message)

  mov  edi, [ebx-4]
  mov  esi, [edi - elVMTSizeOffset]
  xor  ecx, ecx

labSplit:
  test esi, esi
  jz   short labEnd

labStart:
  shr   esi, 1
  setnc cl
  cmp   edx, [edi+esi*8]
  je    short labFound
  lea   eax, [edi+esi*8]
  jb    short labSplit
  lea   edi, [eax+8]
  sub   esi, ecx
  jmp   labSplit
labFound:
  jmp   [edi+esi*8+4]

labEnd:
                                                                
end

// ; close
inline % 15h

  mov  esp, ebp
  pop  ebp
  
end

// ; get
inline % 18h

   mov  ebx, [ebx + edx * 4]

end

// ; unhook
inline % 1Dh

  mov  esi, [data : %CORE_GC_TABLE + gc_et_current]
  mov  esp, [esi + 4]
  mov  ebp, [esi + 8]
  pop  ecx
  mov  [data : %CORE_GC_TABLE + gc_et_current], ecx
  
end

// ; include
inline % 25h

  add  esp, 4                                                       

end

// ; exclude
inline % 26h
                                                       
  push ebp     
  mov  [data : %CORE_GC_TABLE + gc_stack_frame], esp

end

// ; trylock
inline % 27h

  xor  edx, edx

end

// ; freelock
inline % 28h

  nop

end

// ; loadenv
inline % 2Ah

  mov  edx, rdata : %SYSTEM_ENV

end

// ; read
inline % 2Dh

  mov  edx, [ebx + edx]

end

// ; len
inline % 31h

  mov  edx, 0FFFFFh
  mov  ecx, [ebx-8]
  and  edx, ecx

end

// ; flag
inline % 33h

  mov  eax, [ebx - 4]
  mov  edx, [eax - elVMTFlagOffset]
  
end

// ; class
inline % 36h

  mov ebx, [ebx - elVMTOffset]

end

// ; nequal
inline % 40h

  mov  eax, [esp]
  xor  edx, edx
  mov  ecx, [ebx]
  cmp  ecx, [eax]
  setz dl

end

// ; nless
inline % 41h

  mov  eax, [esp]
  xor  edx, edx
  mov  ecx, [ebx]
  cmp  ecx, [eax]
  setl dl

end

// ; save
inline % 47h

  mov [ebx], edx

end

// ; load
inline % 48h

  mov edx, [ebx]

end

// ; div
inline %05Bh

  mov  eax, edx
  mov  ecx, __arg1
  xor  edx, edx
  idiv ecx
  mov  edx, eax

end

// ; write
inline % 5Ch

  mov  esi, [esp]
  mov  ecx, __arg1
  lea  edi, [ebx+edx]
  rep  movsb

end

// ; write
inline % 15Ch

  mov  ecx, [esp]
  lea  edi, [ebx+edx]
  mov  eax, [ecx]
  mov  byte ptr [edi], al

end

// ; write
inline % 25Ch

  mov  ecx, [esp]
  lea  edi, [ebx+edx]
  mov  eax, [ecx]
  mov  word ptr [edi], ax

end

// ; write
inline % 35Ch

  mov  ecx, [esp]
  lea  edi, [ebx+edx]
  mov  eax, [ecx]
  mov  dword ptr [edi], eax

end

// ; write
inline % 45Ch

  mov  ecx, [esp]
  lea  edi, [ebx+edx]
  mov  esi, [ecx+4]
  mov  eax, [ecx]
  mov  dword ptr [edi], eax
  mov  dword ptr [edi+4], esi

end

// ; copyto
inline % 5Dh

  lea edi, [ebx+edx]
  mov  ecx, __arg1
  mov esi, [esp]
  rep movsd

end

// ; shlf
inline % 5Eh

  mov eax, [ebp+__arg1]
  mov ecx, [ebx]
  shl eax, cl
  mov [ebp+__arg1], eax

end

// ; shrf
inline % 5Fh

  mov eax, [ebp+__arg1]
  mov ecx, [ebx]
  shr eax, cl
  mov [ebp+__arg1], eax

end

// ; geti
inline % 91h

  mov  ebx, [ebx+__arg1]
  
end

// ; restore
inline % 92h

  add  ebp, __arg1
  
end

// ; peekfi
inline % 94h

  mov  ebx, [ebp+__arg1]

end

// ; peeksi
inline % 95h

  mov  ebx, [esp+__arg1]

end

// ; xseti
inline %97h

  mov  eax, [esp]                   
  mov [ebx + __arg1], eax

end

// ; open
inline % 98h

  push ebp
  mov  ebp, esp

end

// ; create
inline % 9Ah

  mov  eax, [esp]
  mov  ecx, page_ceil
  mov  edx, [eax]
  lea  ecx, [ecx + edx*4]
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  mov   eax, [esp]
  xor   edx, edx
  mov   [ebx-4], __arg1
  mov   ecx, [eax]
  mov   esi, 800000h
  test  ecx, ecx
  cmovz edx, esi
  shl   ecx, 2
  or    ecx, edx
  mov   [ebx-8], ecx

end

// ; fillr (__arg1 - r)
inline % 09Bh
  mov  esi, [esp]	
  mov  eax, __arg1	
  mov  edi, ebx
  mov  ecx, [esi]
  rep  stosd

end

// ; ajumpvi
inline % 0A1h

  mov  eax, [ebx - 4]
  jmp  [eax + __arg1]

end

// ; callvi (ecx - offset to VMT entry)
inline % 0A2h

  mov  eax, [ebx - 4]
  call [eax + __arg1]

end

// ; callextr
inline % 0A5h

  call extern __arg1
  mov  edx, eax

end

// ; hook label (ecx - offset)
// ; NOTE : hook calling should be the first opcode
inline % 0A6h

  call code : %HOOK

  push [data : %CORE_GC_TABLE + gc_et_current]

  mov  edx, esp 
  push ebp
  push edx
  push ecx

  mov  [data : %CORE_GC_TABLE + gc_et_current], esp
  
end

// ; movn
inline % 0B1h

  mov  edx, __arg1

end

// ; pushai
inline % 0B4h

  push [ebx+__arg1]

end

// ; loadfi
inline % 0B7h

  mov  edx, [ebp + __arg1]

end

// ; savef
inline % 0B9h

  mov  [ebp + __arg1], edx

end

// ; savesi
inline % 0BBh

  mov  [esp + __arg1], edx

end

// ; pushf
inline % 0BDh

  lea  eax, [ebp + __arg1]
  push eax

end

// ; reserve
inline % 0BFh

  sub  esp, __arg1
  push ebp
  push 0
  mov  ebp, esp

end

// ; pushs
inline % 0BEh

  lea  eax, [esp + __arg1]
  push eax

end

// ; seti
inline %0C0h

  mov  esi, ebx
  mov  eax, [esp]                   
  // calculate write-barrier address
  sub  esi, [data : %CORE_GC_TABLE + gc_start]
  mov  ecx, [data : %CORE_GC_TABLE + gc_header]
  shr  esi, page_size_order
  mov  byte ptr [esi + ecx], 1  

  mov [ebx + __arg1], eax

end

// ; storesi
inline % 0C3h

  mov  [esp+__arg1], ebx

end

// ; storefi
inline % 0C4h

  mov  [ebp+__arg1], ebx

end

// ; addf
inline % 0C5h

  mov  ecx, [ebx]
  add  [ebp+__arg1], ecx

end

// ; mulf
inline % 0C6h

  mov  eax, [ebp+__arg1]
  imul dword ptr [ebx]
  mov  [ebp+__arg1], eax

end

// ; subf
inline % 0C8h

  mov  ecx, dword ptr [ebx]
  sub  [ebp+__arg1], ecx

end

// ; divf
inline % 0C9h

  mov  eax, [ebp+__arg1]
  cdq
  idiv dword ptr [ebx]
  mov  [ebp+__arg1], eax

end

// ; clonef
inline % 0CEh

  mov  ecx, [ebx - elSizeOffset]
  and  ecx, struct_mask_inv
  lea  esi, [ebp+__arg1]
  shr  ecx, 2
  mov  edi, ebx
  rep  movsd

end

// ; alloci
inline %0D1h

  // ; generated in jit : sub  esp, __arg1*4
  mov  ecx, __arg1
  xor  eax, eax
  mov  edi, esp
  rep  stos

end

// ; inc
inline %0D6h

  add  edx, __arg1

end

// ; mtredirect (__arg3 - number of parameters, eax - points to the stack arg list)
inline % 0E8h

  mov  esi, __arg1
  push ebx
  xor  edx, edx
  mov  ebx, [esi] // ; message from overload list

labNextOverloadlist:
  shr  ebx, ACTION_ORDER
  mov  edi, rdata : % CORE_MESSAGE_TABLE
  mov  ebx, [edi + ebx * 8 + 4]
  mov  ecx, __arg3
  lea  ebx, [edi + ebx - 4]

labNextParam:
  sub  ecx, 1
  jnz  short labMatching

  mov  esi, __arg1
  pop  ebx
  mov  eax, [esi + edx * 8 + 4]
  mov  ecx, [ebx - 4]
  mov  edx, [esi + edx * 8]
  jmp  [ecx + eax * 8 + 4]

labMatching:
  mov  edi, [eax + ecx * 4]

  //; check nil
  mov   esi, rdata : %VOIDPTR + 4
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi - 4]
  mov  esi, [ebx + ecx * 4]

labNextBaseClass:
  cmp  esi, edi
  jz   labNextParam
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass

  mov  esi, __arg1
  add  edx, 1
  mov  ebx, [esi + edx * 8] // ; message from overload list
  and  ebx, ebx
  jnz  labNextOverloadlist

  pop  ebx

end

// ; xmtredirect (__arg3 - number of parameters, eax - points to the stack arg list)
inline % 0E9h

  mov  esi, __arg1
  push ebx
  xor  edx, edx
  mov  ebx, [esi] // ; message from overload list

labNextOverloadlist:
  shr  ebx, ACTION_ORDER
  mov  edi, rdata : % CORE_MESSAGE_TABLE
  mov  ebx, [edi + ebx * 8 + 4]
  mov  ecx, __arg3
  lea  ebx, [edi + ebx - 4]

labNextParam:
  sub  ecx, 1
  jnz  short labMatching

  mov  esi, __arg1
  mov  ecx, edx
  pop  ebx
  mov  edx, [esi + ecx * 8]
  jmp  [esi + ecx * 8 + 4]

labMatching:
  mov  edi, [eax + ecx * 4]

  //; check nil
  mov   esi, rdata : %VOIDPTR + 4
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi - 4]
  mov  esi, [ebx + ecx * 4]

labNextBaseClass:
  cmp  esi, edi
  jz   labNextParam
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass

  mov  esi, __arg1
  add  edx, 1
  mov  ebx, [esi + edx * 8] // ; message from overload list
  and  ebx, ebx
  jnz  labNextOverloadlist

  pop  ebx

end

// ; mtredirect<1>
inline % 1E8h

  mov  ecx, __arg1
  xor  edx, edx
  mov  eax, [eax + 4]
  mov  ecx, [ecx] // ; message from overload list

  //; check nil
  mov   esi, rdata : %VOIDPTR + 4
  test  eax, eax
  cmovz eax, esi

  mov  eax, [eax - 4]

labNextOverloadlist:
  shr  ecx, ACTION_ORDER
  mov  edi, rdata : % CORE_MESSAGE_TABLE
  mov  ecx, [edi + ecx * 8 + 4]
  lea  ecx, [edi + ecx]

labMatching:
  mov  edi, eax
  mov  esi, [ecx]

labNextBaseClass:
  cmp  esi, edi
  jnz  short labContinue

  mov  esi, __arg1
  mov  eax, [esi + edx * 8 + 4]
  mov  ecx, [ebx - 4]
  mov  edx, [esi + edx * 8]
  jmp  [ecx + eax * 8 + 4]

labContinue:
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass

  mov  ecx, __arg1
  add  edx, 1
  mov  ecx, [ecx + edx * 8] // ; message from overload list
  and  ecx, ecx
  jnz  labNextOverloadlist

labEnd:

end

// ; xmtredirect<1>
inline % 1E9h

  mov  ecx, __arg1
  mov  eax, [eax + 4]
  xor  edx, edx
  mov  ecx, [ecx] // ; message from overload list

  //; check nil
  mov   esi, rdata : %VOIDPTR + 4
  test  eax, eax
  cmovz eax, esi

  mov  eax, [eax - 4]

labNextOverloadlist:
  shr  ecx, ACTION_ORDER
  mov  edi, rdata : % CORE_MESSAGE_TABLE
  mov  ecx, [edi + ecx * 8 + 4]
  lea  ecx, [edi + ecx]

labMatching:
  mov  edi, eax
  mov  esi, [ecx]

labNextBaseClass:
  cmp  esi, edi
  jnz  short labContinue

  mov  ecx, edx
  mov  esi, __arg1
  mov  edx, [esi + edx * 8]
  jmp  [esi + ecx * 8 + 4]

labContinue:
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass

  mov  ecx, __arg1
  add  edx, 1
  mov  ecx, [ecx + edx * 8] // ; message from overload list
  and  ecx, ecx
  jnz  labNextOverloadlist

labEnd:

end

// ; mtredirect<2> (eax - refer to the stack)
inline % 2E8h 

  mov  ecx, __arg1
  xor  edx, edx
  mov  ecx, [ecx] // ; message from overload list

labNextOverloadlist:
  mov  edi, rdata : % CORE_MESSAGE_TABLE
  shr  ecx, ACTION_ORDER
  mov  ecx, [edi + ecx * 8 + 4]
  lea  ecx, [edi + ecx]

labMatching:
  mov  edi, [eax+4]

  //; check nil
  mov   esi, rdata : %VOIDPTR + 4
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi-4]
  mov  esi, [ecx]

labNextBaseClass:
  cmp  esi, edi
  jnz  labContinue

  mov  edi, [eax+8]

  //; check nil
  mov   esi, rdata : %VOIDPTR + 4
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi-4]
  mov  esi, [ecx + 4]

labNextBaseClass2:
  cmp  esi, edi
  jnz  short labContinue2

  mov  esi, __arg1
  mov  eax, [esi + edx * 8 + 4]
  mov  ecx, [ebx - 4]
  mov  edx, [esi + edx * 8]
  jmp  [ecx + eax * 8 + 4]

labContinue2:
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass2
  nop
  nop
  jmp short labNext

labContinue:
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass

labNext:
  mov  ecx, __arg1
  add  edx, 1
  mov  ecx, [ecx + edx * 8] // ; message from overload list
  and  ecx, ecx
  jnz  labNextOverloadlist

end

// ; xmtredirect<2>  (eax - refer to the stack)
inline % 2E9h

// ecx -> eax
// ebx -> ecx

  mov  ecx, __arg1
  xor  edx, edx
  mov  ecx, [ecx + edx * 8] // ; message from overload list

labNextOverloadlist:
  mov  edi, rdata : % CORE_MESSAGE_TABLE
  shr  ecx, ACTION_ORDER
  mov  ecx, [edi + ecx * 8 + 4]
  lea  ecx, [edi + ecx]

labMatching:
  mov  edi, [eax+4]

  //; check nil
  mov   esi, rdata : %VOIDPTR + 4
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi-4]
  mov  esi, [ecx]

labNextBaseClass:
  cmp  esi, edi
  jnz  labContinue

  mov  edi, [eax+8]

  //; check nil
  mov   esi, rdata : %VOIDPTR + 4
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi-4]
  mov  esi, [ecx + 4]

labNextBaseClass2:
  cmp  esi, edi
  jnz  short labContinue2

  mov  esi, __arg1
  mov  ecx, edx
  mov  edx, [esi + ecx * 8]
  jmp  [esi + ecx * 8 + 4]

labContinue2:
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass2
  nop
  nop
  jmp short labNext

labContinue:
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass

labNext:
  mov  ecx, __arg1
  add  edx, 1
  mov  ecx, [ecx + edx * 8] // ; message from overload list
  and  ecx, ecx
  jnz  labNextOverloadlist

end

// ; mtredirect<12> (__arg3 - number of parameters, eax - points to the stack arg list)

inline % 0CE8h

  push ebx
  xor  edx, edx
  mov  ebx, eax
  xor  ecx, ecx

labCountParam:
  lea  ebx, [ebx+4]
  cmp  [ebx], -1
  lea  ecx, [ecx+1]
  jnz  short labCountParam

  mov  esi, __arg1
  push ecx
  mov  ebx, [esi] // ; message from overload list

labNextOverloadlist:
  mov  edi, rdata : % CORE_MESSAGE_TABLE
  shr  ebx, ACTION_ORDER
  mov  ecx, [esp]              // ; param count
  mov  ebx, [edi + ebx * 8 + 4]
  lea  ebx, [edi + ebx - 4]

labNextParam:
  // ; check if signature contains the next class ptr
  lea  esi, [ebx + 4]
  cmp [esi], 0
  cmovnz ebx, esi

  sub  ecx, 1
  jnz  short labMatching

  mov  esi, __arg1
  lea  esp, [esp + 4]
  pop  ebx
  mov  eax, [esi + edx * 8 + 4]
  mov  ecx, [ebx - 4]
  mov  edx, [esi + edx * 8]
  jmp  [ecx + eax * 8 + 4]

labMatching:
  mov  esi, [esp]
  sub  esi, ecx
  mov  edi, [eax + esi * 4]

  //; check nil
  mov   esi, rdata : %VOIDPTR + 4
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi - 4]
  mov  esi, [ebx]

labNextBaseClass:
  cmp  esi, edi
  jz   labNextParam
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass

  mov  esi, __arg1
  add  edx, 1
  mov  ebx, [esi + edx * 8] // ; message from overload list
  and  ebx, ebx
  jnz  labNextOverloadlist

  lea  esp, [esp + 4]
  pop  eax

end

// ; xmtredirect<12>

inline % 0CE9h

  push ebx
  xor  edx, edx
  mov  ebx, eax
  xor  ecx, ecx

labCountParam:
  lea  ebx, [ebx+4]
  cmp  [ebx], -1
  lea  ecx, [ecx+1]
  jnz  short labCountParam

  mov  esi, __arg1
  push ecx
  mov  ebx, [esi] // ; message from overload list

labNextOverloadlist:
  mov  edi, rdata : % CORE_MESSAGE_TABLE
  shr  ebx, ACTION_ORDER
  mov  ecx, [esp]              // ; param count
  mov  ebx, [edi + ebx * 8 + 4]
  lea  ebx, [edi + ebx - 4]

labNextParam:
  // ; check if signature contains the next class ptr
  lea  esi, [ebx + 4]
  cmp [esi], 0
  cmovnz ebx, esi

  sub  ecx, 1
  jnz  short labMatching

  mov  esi, __arg1
  mov  ecx, edx
  lea  esp, [esp + 4]
  pop  ebx
  mov  edx, [esi + ecx * 8]
  jmp  [esi + ecx * 8 + 4]

labMatching:
  mov  esi, [esp]
  sub  esi, ecx
  mov  edi, [eax + esi * 4]

  //; check nil
  mov   esi, rdata : %VOIDPTR + 4
  test  edi, edi
  cmovz edi, esi

  mov  edi, [edi - 4]
  mov  esi, [ebx]

labNextBaseClass:
  cmp  esi, edi
  jz   labNextParam
  mov  edi, [edi - elPackageOffset]
  and  edi, edi
  jnz  short labNextBaseClass

  mov  esi, __arg1
  add  edx, 1
  mov  ebx, [esi + edx * 8] // ; message from overload list
  and  ebx, ebx
  jnz  labNextOverloadlist

  lea  esp, [esp + 4]
  pop  eax

end

// ; readtof (__arg1 - index, __arg2 - n)
inline % 0E0h

  mov  ecx, __arg2	
  lea  edi, [ebp + __arg1]
  lea  esi, [ebx+edx]
  rep  movsd

end

// ; readtof (__arg1 - index, __arg2 - n)
inline % 1E0h

  mov  ecx, [ebx+edx]
  lea  edi, [ebp + __arg1]
  mov  dword ptr [edi], ecx
end

// ; readtof (__arg1 - index, __arg2 - n)
inline % 2E0h

  mov  ecx, __arg2	
  lea  edi, [ebp + __arg1]
  lea  esi, [ebx+edx]
  rep  movsd

end

// ; readtof (__arg1 - index, __arg2 - n)
inline % 3E0h

  mov  ecx, __arg2	
  lea  edi, [ebp + __arg1]
  lea  esi, [ebx+edx]
  rep  movsd

end

// ; readtof (__arg1 - index, __arg2 - n)
inline % 4E0h

  mov  ecx, __arg2	
  lea  edi, [ebp + __arg1]
  lea  esi, [ebx+edx]
  rep  movsd

end

// ; readtof (__arg1 - index, __arg2 - n)
inline % 5E0h

  mov  ecx, __arg2	
  lea  edi, [ebp + __arg1]
  lea  esi, [ebx+edx]
  rep  movsd

end

// ; createn (__arg1 - item size)
inline % 0E1h

  mov  eax, [esp]
  mov  ecx, page_ceil
  mov  eax, [eax]
  mov  ebx, __arg1
  imul ebx
  add  ecx, eax
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  mov   eax, [esp]
  mov   ecx, 800000h
  mov   eax, [eax]
  mov   esi, __arg1
  imul  esi
  or    ecx, eax
  mov   [ebx-8], ecx
  
end

// ; createn (__arg1 = 1)
inline % 1E1h

  mov  eax, [esp]
  mov  ecx, page_ceil
  add  ecx, [eax]
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  mov   eax, [esp]
  mov   ecx, 800000h
  or    ecx, [eax]
  mov   [ebx-8], ecx
  
end

// ; createn (__arg1 = 2)
inline % 2E1h

  mov  eax, [esp]
  mov  ecx, page_ceil
  mov  eax, [eax]
  shl  eax, 1
  add  ecx, eax
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  mov   eax, [esp]
  mov   ecx, 800000h
  mov   eax, [eax]
  shl  eax, 1
  or    ecx, eax
  mov   [ebx-8], ecx
  
end

// ; createn (__arg1 = 4)
inline % 3E1h

  mov  eax, [esp]
  mov  ecx, page_ceil
  mov  eax, [eax]
  shl  eax, 2
  add  ecx, eax
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  mov   eax, [esp]
  mov   ecx, 800000h
  mov   eax, [eax]
  shl  eax, 2
  or    ecx, eax
  mov   [ebx-8], ecx
  
end

// ; createn (__arg1 = 8)
inline % 4E1h

  mov  eax, [esp]
  mov  ecx, page_ceil
  mov  eax, [eax]
  shl  eax, 3
  add  ecx, eax
  and  ecx, page_mask 
 
  call code : %GC_ALLOC

  mov   eax, [esp]
  mov   ecx, 800000h
  mov   eax, [eax]
  shl  eax, 3
  or    ecx, eax
  mov   [ebx-8], ecx
  
end

// ; xsetfi (__arg1 - index, __arg2 - index)
inline % 0E2h

  mov  eax, [ebp + __arg1]
  mov  [ebx + __arg2], eax

end

// ; copytoai (__arg1 - index, __arg2 - n)
inline % 0E3h

  mov  ecx, __arg2	
  lea  edi, [ebx + __arg1]
  mov  esi, [esp]
  rep  movsd

end

// ; copytofi (__arg1 - index, __arg2 - n)
inline % 0E4h

  mov  ecx, __arg2	
  mov  edi, [ebp + __arg1]
  mov  esi, ebx
  rep  movsd

end

// ; copytof (__arg1 - index, __arg2 - n)
inline % 0E5h

  mov  ecx, __arg2	
  lea  edi, [ebp + __arg1]
  mov  esi, ebx
  rep  movsd

end

// ; copyfi (__arg1 - index, __arg2 - n)
inline % 0E6h

  mov  ecx, __arg2	
  mov  esi, [ebp + __arg1]
  mov  edi, ebx
  rep  movsd

end

// ; copyf (__arg1 - index, __arg2 - n)
inline % 0E7h

  mov  ecx, __arg2	
  lea  esi, [ebp + __arg1]
  mov  edi, ebx
  rep  movsd

end

// ; xsavef (__arg1 - index, __arg2 - n)
inline % 0EFh

  mov dword ptr [ebp + __arg1], __arg2

end

// ; new (__arg1 - size)
inline % 0F0h
	
  mov  ecx, __arg1
  call code : %GC_ALLOC

end

// ; fillri (__arg1 - count)
inline % 0F2h
	
  mov  ecx, __arg1
  rep  stosd

end

// ; vcallrm
inline % 0F4h

  mov  ecx, __arg1
  mov  eax, [ebx - 4]
  call [eax + ecx * 8 + 4]
  
end

// ; jumprm
inline % 0F5h

  cmp [ebx], ebx
  jmp __arg1

end

// ; selectr (ebx - r1, __arg1 - r2)
inline % 0F6h

  mov    ecx, __arg1
  test   edx, edx
  cmovnz ebx, ecx

end

// callrm (edx contains message, __arg1 contains vmtentry)
inline % 0FEh

   call code : __arg1

end
