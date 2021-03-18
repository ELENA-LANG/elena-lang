// !! NOTE : ELENA 64 will not maintain 16 byte stack alignment due to the current call conventions 
//           and some language features (like a command tape). 
//           The alignment will be maintained only for external operations

// !! NOTE : R15 register must be preserved

// --- Predefined References  --
define GC_ALLOC	            10001h
define HOOK                 10010h
define INVOKER              10011h
define INIT_RND             10012h
define INIT_ET              10015h
define ENDFRAME             10016h
define RESTORE_ET           10017h
define OPENFRAME            10019h
define CLOSEFRAME           1001Ah
define LOCK                 10021h
define UNLOCK               10022h
define LOAD_CALLSTACK       10024h

//R12, R13, R14, and R15

// INVOKER(prevFrame, function, arg)
procedure % INVOKER

  // ; RCX - prevFrame
  // ; RDX - function
  // ; R8  - arg
                                            
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
  push rcx            // ; FrameHeader.previousFrame
  mov  rax, rdx
  push 0              // ; FrameHeader.reserved
  mov  rbp, rsp       // ; FrameHeader
  push r8             // ; arg

  call rax
  add  rsp, 24        // ; clear FrameHeader+arg

  // ; restore registers
  pop  r15
  pop  r14
  pop  r13
  pop  r12
  pop  rbp
  pop  rbx
  pop  rdi
  pop  rsi
  add  rsp, 4
  ret

end

procedure % INIT_RND

  sub  rsp, 8h
  mov  rax, rsp
  sub  rsp, 10h
  lea  rbx, [rsp]
  push rax 
  push rbx
  push rbx
  mov  rcx, [rsp]
  sub  rsp, 18h
  call extern 'dlls'KERNEL32.GetSystemTime
  add  rsp, 20h

  mov  rcx, [rsp]
  mov  rdx, [rsp+8]
  sub  rsp, 10h
  call extern 'dlls'KERNEL32.SystemTimeToFileTime
  add  rsp, 30h

  pop  rax
  ret
  
end
