// ; --- Predefined References  --
define INVOKER              10001h
define GC_ALLOC	            10002h
define VEH_HANDLER          10003h

define SYSTEM_ENV           20002h
define CORE_GC_TABLE        20003h
define CORE_THREAD_TABLE    2000Bh
define CORE_ET_TABLE        2000Bh

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

define et_current            0008h
define tt_stack_frame        0010h

// ; --- Object header fields ---
define elSizeOffset          0004h
define elVMTOffset           0010h 
define elObjectOffset        0010h

// ; ==== System commands ===

// INVOKER(function, arg)
procedure % INVOKER

  // ; RDI - function
  // ; RSI - arg
                                            
  // ; save registers
  push 0
  push rsi
  push rdi
  push rbx
  push rbp
  push r12
  push r13
  push r14
  push r15

  // ; declare new frame
  mov  rax, rdi
  xor  rdi, rdi
  push 0              // ; FrameHeader.previousFrame
  push 0              // ; FrameHeader.reserved
  mov  rbp, rsp       // ; FrameHeader
  push rdi
  push rsi            // ; arg

  call rax
  add  rsp, 32        // ; clear FrameHeader+arg
  xor  eax, eax

  // ; restore registers
  pop  r15
  pop  r14
  pop  r13
  pop  r12
  pop  rbp
  pop  rbx
  pop  rdi
  pop  rsi
  add  rsp, 8
  ret

end

// VEH_HANDLER() 
procedure % VEH_HANDLER

  mov  r10, rdx
  mov  rdx, rax   // ; set exception code
  mov  rax, [data : % CORE_ET_TABLE]
  jmp  rax

end

// ; --- GC_ALLOC ---
// ; in: rcx - size ; out: ebx - created object
// ; note for linux - there is a separate copy
inline % GC_ALLOC

  mov  rax, [data : %CORE_GC_TABLE + gc_yg_current]
  mov  r12, [data : %CORE_GC_TABLE + gc_yg_end]
  add  rcx, rax
  cmp  rcx, r12
  jae  short labYGCollect
  mov  [data : %CORE_GC_TABLE + gc_yg_current], rcx
  lea  rbx, [rax + elObjectOffset]
  ret

labYGCollect:
  // ; save registers
  sub  rcx, rax
  push r10
  push r11
  push rbp

  // ; lock frame
  mov  [data : %CORE_THREAD_TABLE + tt_stack_frame], rsp

  push rcx

  // ; create set of roots
  mov  rbp, rsp
  xor  ecx, ecx
  push rcx        // ; reserve place 
  push rcx
  push rcx

  // ;   save static roots
  mov  rax, rdata : %SYSTEM_ENV
  mov  rsi, stat : %0
  mov  ecx, dword ptr [rax]
  shl  ecx, 3
  push rsi
  push rcx

  // ;   collect frames
  mov  rax, [data : %CORE_THREAD_TABLE + tt_stack_frame]  
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

  mov [rbp-8], rsp      // ; save position for roots

  mov  rsi, [rbp]
  mov  rdi, rsp

  // ; restore frame to correctly display a call stack
  mov  rax, rbp
  mov  rbp, [rax+8]

  // ; call GC routine
  sub  rsp, 30h
  mov  [rsp+28h], rax
  call extern "$rt.CollectGCLA"

  mov  rbp, [rsp+28h] 
  add  rsp, 30h
  mov  rbx, rax

  mov  rsp, rbp 
  pop  rcx
  pop  rbp
  pop  r11
  pop  r10

  ret

end

// ; ==== Overridden Command Set ==

// ; openheaderin
inline %0F2h

  xor  rbp, rbp
  push rbp                 // ; note an extra push to simulate the function entry

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  sub  rsp, __n_2          // ; note __n_2 should be aligned to 16
  push rbp
  push rax
  mov  rcx, __n_1          // ; note __n_1 should be aligned to 16
  mov  rbp, rsp
  mov  rdi, rsp
  rep  stos
  sub  rsp, __arg32_1

end 

// ; openheaderin 0, 0
inline %1F2h

  xor  rbp, rbp
  push rbp                 // ; note an extra push to simulate the function entry

  push rbp
  mov  rbp, rsp

end 

// ; openheaderin 1, 0
inline %2F2h

  xor  rbp, rbp
  push rbp                 // ; note an extra push to simulate the function entry

  push rbp
  mov  rbp, rsp
  push 0
  push 0

end 

// ; openheaderin 2, 0
inline %3F2h

  xor  rbp, rbp
  push rbp                 // ; note an extra push to simulate the function entry

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  push rax
  push rax

end 

// ; openheaderin 3, 0
inline %4F2h

  xor  rbp, rbp
  push rbp                 // ; note an extra push to simulate the function entry

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  push rax
  push rax
  push rax
  push rax

end 

// ; openheaderin 0, n
inline %5F2h

  xor  rbp, rbp
  push rbp                 // ; note an extra push to simulate the function entry

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  sub  rsp, __n_2          // ; note __n_2 should be aligned to 16
  push rbp
  push rax
  mov  rbp, rsp

end 

// ; openheaderin i, 0
inline %6F2h

  xor  rbp, rbp
  push rbp                 // ; note an extra push to simulate the function entry

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  mov  rcx, __n_1          // ; note __n_1 should be aligned to 16
  mov  rbp, rsp
  mov  rdi, rsp
  rep  stos
  sub  rsp, __arg32_1

end 

// ; callext
inline %0FEh

  mov  r9, [rsp+40]
  mov  r8,  [rsp+32]
  mov  rcx, [rsp+24]
  mov  rdx, [rsp+16]
  mov  rsi, r11
  mov  rdi, r10
  call extern __relptr32_1
  mov  rdx, rax

end

// ; callext
inline %1FEh

  call extern __relptr32_1
  mov  rdx, rax

end

// ; callext
inline %2FEh

  mov  rdi, r10
  call extern __relptr32_1
  mov  rdx, rax

end

// ; callext
inline %3FEh

  mov  rsi, r11
  mov  rdi, r10
  call extern __relptr32_1
  mov  rdx, rax

end

// ; callext
inline %4FEh

  mov  rdx, [rsp+16]
  mov  rsi, r11
  mov  rdi, r10
  call extern __relptr32_1
  mov  rdx, rax

end

// ; callext
inline %5FEh

  mov  rcx, [rsp+24]
  mov  rdx, [rsp+16]
  mov  rsi, r11
  mov  rdi, r10
  call extern __relptr32_1
  mov  rdx, rax

end
