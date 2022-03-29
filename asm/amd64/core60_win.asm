// ; --- Predefined References  --
define INVOKER         10001h

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
