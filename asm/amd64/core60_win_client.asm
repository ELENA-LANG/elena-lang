define INVOKER           10001h

define CORE_SINGLE_CONTENT  2000Bh

define tt_stack_root         0028h

// INVOKER(function, arg)
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

  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_root], rsp

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
