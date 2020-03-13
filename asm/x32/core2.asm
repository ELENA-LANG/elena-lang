// --- Predefined References  --
define GC_ALLOC	            10001h
define BREAK                10026h
define EXPAND_HEAP          10028h

define CORE_GC_TABLE        20002h
define CORE_STATICROOT      20005h

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
  mov  ebx, [ebp-4]
  push ebx
  mov  eax, esp
  push eax
  call extern 'rt_dlls.GCCollect

  mov  ebx, eax

  mov  esp, ebp 
  pop  ecx 

  ret

end
