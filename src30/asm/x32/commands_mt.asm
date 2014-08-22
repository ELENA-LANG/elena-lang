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
  mov  edx, fs:[2Ch]
  mov  ebx, [edx+ebx*4]
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
  mov  edx, fs:[2Ch]
  mov  ebx, [edx+ebx*4]

  mov  esp, [ebx + tls_catch_level]  
  mov  ebp, [ebx + tls_catch_frame]
  pop  edx
  mov  [ebx + tls_catch_frame], edx
  pop  edx
  mov  [ebx + tls_catch_level], edx
  pop  edx
  mov  [ebx + tls_catch_addr], edx

end

// ; ifheap - part of the command

inline % 096h

  // ; GCXT: get current thread frame
  mov  ecx, tls_stack_bottom
  mov  ebx, [data : %CORE_TLS_INDEX]
  add  ecx, fs:[2Ch]
  mov  ebx, [ecx+ebx*4]

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
