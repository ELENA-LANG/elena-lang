// ; --- Predefined References  --
define GC_ALLOC	            10002h
define VEH_HANDLER          10003h
define GC_COLLECT	    10004h
define PREPARE	            10006h

define SYSTEM_ENV           20002h
define CORE_GC_TABLE        20003h
define CORE_SINGLE_CONTENT  2000Bh

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
define gc_perm_start         0058h 
define gc_perm_end           0060h 
define gc_perm_current       0068h 

define et_current            0008h
define tt_stack_frame        0010h
define tt_stack_root         0028h

// ; --- Object header fields ---
define elSizeOffset          0004h
define elVMTOffset           0010h 
define elObjectOffset        0010h

// ; ==== System commands ===

// VEH_HANDLER() 
procedure % VEH_HANDLER

  mov  r10, rdx
  mov  rdx, rax   // ; set exception code
  mov  rax, [data : % CORE_SINGLE_CONTENT]
  jmp  rax

end

// ; --- GC_COLLECT ---
// ; in: ecx - size, edx - 1 - full collect, 0 - normal one
inline % GC_COLLECT

  push r10
  push r11
  push rbp

  // ; lock frame
  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_frame], rsp

  push rdx
  push rcx

  // ; create set of roots
  mov  rbp, rsp
  xor  ecx, ecx
  push rcx        // ; reserve place 
  push rcx        
  push rcx
  push rcx

  // ;   save static roots
  mov  rax, rdata : %SYSTEM_ENV
  mov  rsi, stat : %0
  mov  ecx, dword ptr [rax]
  shl  ecx, 3
  push rsi
  push rcx

  // ; save perm roots
  mov  rsi, [data : %CORE_GC_TABLE + gc_perm_start]
  mov  rcx, [data : %CORE_GC_TABLE + gc_perm_current]
  sub  rcx, rsi
  push rsi
  push rcx

  // ;   collect frames
  mov  rax, [data : %CORE_SINGLE_CONTENT + tt_stack_frame]  
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

  mov  rdx, [rbp+8]
  mov  rsi, [rbp]
  mov  rdi, rsp

  // ; restore frame to correctly display a call stack
  mov  rax, rbp
  mov  rbp, [rax+16]

  // ; call GC routine
  sub  rsp, 30h
  mov  [rsp+28h], rax
  call extern "$rt.CollectGCLA"

  mov  rbp, [rsp+28h] 
  add  rsp, 30h
  mov  rbx, rax

  mov  rsp, rbp 
  pop  rcx
  pop  rdx
  pop  rbp
  pop  r11
  pop  r10

  ret

end

procedure %PREPARE

  mov  rdi, rax
  call extern "$rt.PrepareLA"
  ret

end

// ; ==== Overridden Command Set ==

// ; system startup
inline %4CFh

  finit
  ffree st(0)
  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_root], rsp

  mov  rax, rsp
  call %PREPARE

  xor  rbp, rbp
  push rbp                 // ; note an extra push to simulate the function entry

end

// ; extclosen
inline %0CAh

  add  rbp, __n_1
  mov  rsp, rbp
  pop  rbp

  add  rsp, 16
  pop  rbx
  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_frame], rbx

  pop  rbp
  pop  r15
  pop  r14
  pop  r13
  pop  r12
  pop  rbx
  add  rsp, 40

end

// ; extclosen 0
inline %1CAh

  mov  rsp, rbp
  pop  rbp

  add  rsp, 16
  pop  rbx
  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_frame], rbx

  pop  rbp
  pop  r15
  pop  r14
  pop  r13
  pop  r12
  pop  rbx
  add  rsp, 40

end

// ; extopenin
inline %0F2h

  push 0 
  push rcx
  push rdx
  push rsi  
  push rdi
  push rbx
  push r12
  push r13
  push r14
  push r15

  push rbp     
  mov  rax, [data : %CORE_SINGLE_CONTENT + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  sub  rsp, __n_2
  push rbp
  push rax
  mov  rbp, rsp
  mov  rcx, __n_1
  sub  rsp, __arg32_1
  mov  rdi, rsp
  rep  stos
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 0, n
inline %1F2h

  push 0 
  push rcx
  push rdx
  push rsi  
  push rdi
  push rbx
  push r12
  push r13
  push r14
  push r15

  push rbp     
  mov  rax, [data : %CORE_SINGLE_CONTENT + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  sub  rsp, __n_2
  push rbp
  push rax
  mov  rbp, rsp
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 1, n
inline %2F2h

  push 0 
  push rcx
  push rdx
  push rsi  
  push rdi
  push rbx
  push r12
  push r13
  push r14
  push r15

  push rbp     
  mov  rax, [data : %CORE_SINGLE_CONTENT + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  sub  rsp, __n_2
  push rbp
  push rax
  mov  rbp, rsp
  push rax
  push rax
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 2, n
inline %3F2h

  push 0 
  push rcx
  push rdx
  push rsi  
  push rdi
  push rbx
  push r12
  push r13
  push r14
  push r15

  push rbp     
  mov  rax, [data : %CORE_SINGLE_CONTENT + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  sub  rsp, __n_2
  push rbp
  push rax
  mov  rbp, rsp
  push rax
  push rax
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 3, n
inline %4F2h

  push 0 
  push rcx
  push rdx
  push rsi  
  push rdi
  push rbx
  push r12
  push r13
  push r14
  push r15

  push rbp     
  mov  rax, [data : %CORE_SINGLE_CONTENT + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  sub  rsp, __n_2
  push rbp
  push rax
  mov  rbp, rsp
  push rax
  push rax
  push rax
  push rax
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 4, n
inline %5F2h

  push 0 
  push rcx
  push rdx
  push rsi  
  push rdi
  push rbx
  push r12
  push r13
  push r14
  push r15

  push rbp     
  mov  rax, [data : %CORE_SINGLE_CONTENT + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  sub  rsp, __n_2
  push rbp
  push rax
  mov  rbp, rsp
  push rax
  push rax
  push rax
  push rax
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin i, 0
inline %6F2h

  push 0 
  push rcx
  push rdx
  push rsi  
  push rdi
  push rbx
  push r12
  push r13
  push r14
  push r15

  push rbp     
  mov  rax, [data : %CORE_SINGLE_CONTENT + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  mov  rcx, __n_1
  sub  rsp, __arg32_1
  mov  rdi, rsp
  rep  stos
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 0, 0
inline %7F2h

  push 0 
  push rcx
  push rdx
  push rsi  
  push rdi
  push rbx
  push r12
  push r13
  push r14
  push r15

  push rbp     
  mov  rax, [data : %CORE_SINGLE_CONTENT + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  mov  rbp, rsp
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 1, 0
inline %8F2h

  push 0 
  push rcx
  push rdx
  push rsi  
  push rdi
  push rbx
  push r12
  push r13
  push r14
  push r15

  push rbp     
  mov  rax, [data : %CORE_SINGLE_CONTENT + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  mov  rbp, rsp
  push 0
  push 0
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 2, 0
inline %9F2h

  push 0 
  push rcx
  push rdx
  push rsi  
  push rdi
  push rbx
  push r12
  push r13
  push r14
  push r15

  push rbp     
  mov  rax, [data : %CORE_SINGLE_CONTENT + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  push rax
  push rax
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 3, 0
inline %0AF2h

  push 0 
  push rcx
  push rdx
  push rsi  
  push rdi
  push rbx
  push r12
  push r13
  push r14
  push r15

  push rbp     
  mov  rax, [data : %CORE_SINGLE_CONTENT + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  push rax
  push rax
  push rax
  push rax
  mov  r10, rax
  mov  r11, rax

end 

// ; extopenin 4, 0
inline %0BF2h

  push 0 
  push rcx
  push rdx
  push rsi  
  push rdi
  push rbx
  push r12
  push r13
  push r14
  push r15

  push rbp     
  mov  rax, [data : %CORE_SINGLE_CONTENT + tt_stack_frame]
  push rax 

  mov  rbp, rax
  xor  eax, eax
  push rbp
  push rax
  mov  rbp, rsp

  push rbp
  xor  rax, rax
  mov  rbp, rsp
  push rax
  push rax
  push rax
  push rax
  mov  r10, rax
  mov  r11, rax

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
