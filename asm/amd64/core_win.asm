// --- Predefined References  --
define INVOKER              10011h

// INVOKER(prevFrame, function, arg)
procedure % INVOKER

  // ; save registers
  mov  rax, [rsp+16]   // ; function
  push rsi
  mov  rsi, [rsp+16]   // ; prevFrame
  push rdi
  mov  rdi, [rsp+40]  // ; arg
  push rcx
  push rbx
  push rbp

  // declare new frame
  push rsi            // ; FrameHeader.previousFrame
  push 0              // ; FrameHeader.reserved
  mov  rbp, rsp       // ; FrameHeader
  push rdi            // ; arg

  call rax
  add  rsp, 24        // ; clear FrameHeader+arg

  // ; restore registers
  pop  rbp
  pop  rbx
  pop  rcx
  pop  rdi
  pop  rsi
  ret

end
