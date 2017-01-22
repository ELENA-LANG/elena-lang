// --- Predefined References  --
define INIT                 10013h
define NEWFRAME             10014h
define EXIT                 1001Dh
define PREPARE              10027h

// --- System Core Functions --

procedure % INIT

  ret

end

procedure % NEWFRAME

  ret

end

// ; ==== Command Set ==

// ; close

inline % 15h

  mov  rsp, rbp
  pop  rbp
  
end

// ; validate

inline % 03Ah

  cmp dword ptr [rax], eax

end

// ; nsave
inline % 47h

  mov dword ptr [rdi], ebx

end

// ; restore

inline % 92h

  add  rbp, __arg1
  
end

// ; aloadsi
inline % 95h

  mov  rax, qword ptr [rsp+__arg1]

end

// ; open
inline % 98h

  push rbp
  mov  rbp, rsp

end

// ; reserve
inline % 0BFh

  sub  rsp, __arg1
  push rbp
  push 0
  mov  rbp, rsp

end

// xcallrm (rdx contains message, __arg1 contains vmtentry)
inline % 0FEh

   call code : __arg1

end
