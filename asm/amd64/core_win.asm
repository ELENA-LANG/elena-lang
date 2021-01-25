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
define BREAK                10026h
define EXPAND_HEAP          10028h

//R12, R13, R14, and R15

// INVOKER(prevFrame, function, arg)
procedure % INVOKER

  // ; RCX - prevFrame
  // ; RDX - function
  // ; R8  - arg
                                            
  // ; save registers
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
  ret

end

// ; ebx - exception code
procedure % BREAK

//  push 0
//  push 0
//  push 0
//  push ebx
//  call extern 'dlls'KERNEL32.RaiseException

end

// ; in - eax - heap, ebx - size
// ; out - eax - heap
procedure % EXPAND_HEAP

//  push 4
//  push 00001000h
//  push ecx
//  push eax
//  call extern 'dlls'KERNEL32.VirtualAlloc

  ret

end

procedure % INIT_RND

//  sub  esp, 8h
//  mov  eax, esp
//  sub  esp, 10h
//  lea  ebx, [esp]
//  push eax 
//  push ebx
//  push ebx
//  call extern 'dlls'KERNEL32.GetSystemTime
//  call extern 'dlls'KERNEL32.SystemTimeToFileTime
//  add  esp, 10h
//  pop  eax
//  pop  edx
  ret
  
end
