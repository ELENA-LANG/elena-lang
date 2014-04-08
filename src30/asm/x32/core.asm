// --- System Core Data  --
define CORE_EXCEPTION_TABLE 01h
define CORE_GC_TABLE        02h
define CORE_GC_SIZE         03h
define CORE_STAT_COUNT      04h
define CORE_STATICROOT      05h
define CORE_VM_TABLE        06h

// CORE GC SIZE
define gcs_MGSize	0000h
define gcs_YGSize	0004h

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

// Page Size
define page_size               10h
define page_size_order          4h
define page_size_order_minus2   2h
define page_mask        0FFFFFFF0h

define GC_HEAP_ATTRIBUTE 00Dh

// --- System Core Preloaded Routines --


// --- System Core Functions --

procedure core'init
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
  push eax                
  push GC_HEAP_ATTRIBUTE
  call extern 'dlls'KERNEL32.GetProcessHeap
  push eax 
  call extern 'dlls'KERNEL32.HeapAlloc

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

procedure core'newframe

  // ; put frame end and move procedure returning address
  pop  edx           

  xor  ebx, ebx
  push ebx                      
  push ebx

  // ; set stack frame pointer / bottom stack pointer
  mov  [data : %CORE_GC_TABLE + gc_stack_frame], esp 
  mov  [data : %CORE_GC_TABLE + gc_stack_bottom], esp
  
  push edx

  ret

end

procedure core'init_ex_tbl

  // ; set default exception handler
  mov  [data : %CORE_EXCEPTION_TABLE + 4], esp
  mov  ebx, code : "$package'core'default_handler"
  mov  [data : %CORE_EXCEPTION_TABLE], ebx

  ret

end

procedure core'default_handler
                                                       
  mov  esp, [data : %CORE_EXCEPTION_TABLE + 4]
  mov  eax, 1                         // exit error code
  push eax
  call extern 'dlls'KERNEL32.ExitProcess     // exit  

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

procedure core'endframe

  // ; save return pointer
  pop  ecx  
  
  xor  edx, edx
  lea  esp, [esp+8]
  mov  [data : %CORE_GC_TABLE + gc_stack_frame], edx

  // ; restore return pointer
  push ecx   
  ret

end

procedure core'openframe

  // ; save return pointer
  pop  ecx  

  xor  edi, edi

  mov  esi, [data : %CORE_GC_TABLE + gc_stack_frame]
  // ; save previous pointer / size field
  push esi                              
  push edi                              
  mov  [data : %CORE_GC_TABLE + gc_stack_frame], esp
  
  // ; restore return pointer
  push ecx   
  ret

end

procedure core'closeframe

  // ; save return pointer
  pop  ecx  

  lea  esp, [esp+4]
  pop  edx
  mov  [data : %CORE_GC_TABLE + gc_stack_frame], edx
  
  // ; restore return pointer
  push ecx   
  ret

end
