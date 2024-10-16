define INVOKER           10001h

define CORE_SINGLE_CONTENT  2000Bh

define tt_stack_frame        0010h
define tt_stack_root         0028h

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

  // ; check if it is a nested call
  mov  rsi, [data : %CORE_SINGLE_CONTENT + tt_stack_root]
  test rsi, rsi
  jnz  labNested

  // ; declare new frame
  xor  rdi, rdi 
  mov  rax, rcx
  push 0              // ; FrameHeader.previousFrame
  push 0              // ; FrameHeader.reserved
  mov  rbp, rsp       // ; FrameHeader

  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_root], rsp
  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_frame], rsp

  // ; allocate shadow stack
  sub  rsp, 20h
  mov  rcx, rdx
  mov  rdx, rdi
  mov  r8, rdi
  mov  r9, rdi

  call rax
  add  rsp, 30h        // ; clear FrameHeader+args
  mov  rax, rbx

  // ; clear root
  xor  ecx, ecx
  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_root], rcx
  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_frame], rcx

  jmp  labEnd

labNested:
  mov  rax, rcx

  // ; allocate shadow stack
  sub  rsp, 20h
  mov  rcx, rdx
  xor  rdx, rdx
  mov  r8, rdx
  mov  r9, rdx

  call rax
  add  rsp, 20h        // ; clear args
  mov  rax, rbx

labEnd:

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

// ; system startup
inline %4CFh

  mov  [data : %CORE_SINGLE_CONTENT + tt_stack_root], rsp

end
