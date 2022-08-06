// ; --- Predefined References  --
define INVOKER         10001h
define EXCEPTION_HANDLER    10003h

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

  // declare new frame
  mov  rax, rdi
  xor  rdi, rdi
  push rdi            // ; FrameHeader.previousFrame
  push rdi            // ; FrameHeader.reserved
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

// EXCEPTION_HANDLER() 
procedure % EXCEPTION_HANDLER

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
