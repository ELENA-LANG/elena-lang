// ; --- Predefined References  --
define INVOKER           10001h
define VEH_HANDLER       10003h

define CORE_ET_TABLE     2000Bh

// ; ==== System commands ===

// INVOKER(function, arg)
procedure % INVOKER

  // ; RCX - function
  // ; RDX - arg
                                            
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
  xor  rdi, rdi 
  mov  rax, rcx
  push 0              // ; FrameHeader.previousFrame
  push 0              // ; FrameHeader.reserved
  mov  rbp, rsp       // ; FrameHeader
  push 0
  push r8             // ; arg

  call rax
  add  rsp, 32        // ; clear FrameHeader+arg
  mov  rax, rbx

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

// ; callext
inline %0FEh

  mov  r9, [rsp+24]
  mov  r8, [rsp+16]
  mov  rdx, r11
  mov  rcx, r10
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

  mov  rcx, r10
  call extern __relptr32_1
  mov  rdx, rax

end

// ; callext
inline %3FEh

  mov  rdx, r11
  mov  rcx, r10
  call extern __relptr32_1
  mov  rdx, rax

end

// ; callext
inline %4FEh

  mov  r8, [rsp+16]
  mov  rdx, r11
  mov  rcx, r10
  call extern __relptr32_1
  mov  rdx, rax

end

// ; callext
inline %5FEh

  mov  r9, [rsp+24]
  mov  r8, [rsp+16]
  mov  rdx, r11
  mov  rcx, r10
  call extern __relptr32_1
  mov  rdx, rax

end
