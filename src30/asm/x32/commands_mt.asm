// --- System Core Data  --
define CORE_EXCEPTION_TABLE 01h
define CORE_GC_TABLE        02h
define CORE_TLS_INDEX       07h
define THREAD_TABLE         08h

// --- System Core API  --
define GC_ALLOC	         10001h
define HOOK              10010h
define GETCLASSNAME      10011h
define INIT_RND          10012h
define EVALSCRIPT        10013h

// GC TABLE
define gc_header             0000h
define gc_start              0004h

// GCXT TLS TABLE
define tls_stack_frame       0000h
define tls_stack_bottom      0004h
define tls_catch_addr        0008h
define tls_catch_level       000Ch
define tls_catch_frame       0010h

// Page Size
define page_mask        0FFFFFFF0h
define page_size_order          4h
define page_ceil               1Bh

// ; ==== Command Set ==

// ; throw
inline % 7

  // ; GCXT: get current thread frame
  mov  ebx, [data : %CORE_TLS_INDEX]
  mov  ecx, fs:[2Ch]
  mov  ebx, [ecx+ebx*4]
  jmp  [ebx + tls_catch_addr]

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

// ; unhook
inline % 1Dh

  // ; GCXT: get current thread frame
  mov  ebx, [data : %CORE_TLS_INDEX]
  mov  ecx, fs:[2Ch]
  mov  ebx, [ecx+ebx*4]

  mov  esp, [ebx + tls_catch_level]  
  mov  ebp, [ebx + tls_catch_frame]
  pop  ecx
  mov  [ebx + tls_catch_frame], ecx
  pop  ecx
  mov  [ebx + tls_catch_level], ecx
  pop  ecx
  mov  [ebx + tls_catch_addr], ecx

end

// ; exclude
inline % 1Eh

  // ; GCXT: get current thread frame
  mov  ebx, [data : %CORE_TLS_INDEX]
  mov  ecx, fs:[2Ch]
  mov  ebx, [ecx+ebx*4]
  // ; lock managed stack frame
  push  [ebx + tls_stack_frame]
  push ebp
  mov  [ebx + tls_stack_frame], esp

end

// ; include
inline % 1Fh

  // ; GCXT: get current thread frame
  mov  ebx, [data : %CORE_TLS_INDEX]
  mov  ecx, fs:[2Ch]
  mov  ebx, [ecx+ebx*4]
  // ; save previous pointer 
  mov  esp, [ebx + tls_stack_frame]
  pop  ebp
  pop  ecx
  // ; size field
  mov  [ebx + tls_stack_frame], ecx

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

// ; popai
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

// ; hook label (ecx - offset)

inline % 0A6h

  // ; GCXT: get current thread frame
  mov  ebx, [data : %CORE_TLS_INDEX]
  mov  edx, fs:[2Ch]
  mov  ebx, [edx+ebx*4]

  call code : %HOOK
  push ebp
  push [ebx + tls_catch_addr]
  push [ebx + tls_catch_frame]
  push [ebx + tls_catch_level]  
  mov  [ebx + tls_catch_addr], ecx
  mov  [ebx + tls_catch_level], esp
  mov  [ebx + tls_catch_frame], ebp

end

// ; nbox (esi - size, __arg1 - vmt)

inline % 0B0h

  // ; GCXT: get current thread frame
  mov  ebx, [data : %CORE_TLS_INDEX]
  mov  ecx, fs:[2Ch]
  mov  ebx, [ecx+ebx*4]

  cmp  eax, [ebx + tls_stack_bottom]
  ja   short labSkip                      
  mov  ebx, esi         
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

  // ; GCXT: get current thread frame
  mov  ebx, [data : %CORE_TLS_INDEX]
  mov  ecx, fs:[2Ch]
  mov  ebx, [ecx+ebx*4]

  cmp  eax, [ebx + tls_stack_bottom]
  ja   short labSkip                      
  mov  ebx, esi
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
