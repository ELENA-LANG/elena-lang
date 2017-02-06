// --- Predefined References  --
define INIT                 10013h
define NEWFRAME             10014h
define EXIT                 1001Dh
define PREPARE              10027h
define CORE_OS_TABLE        20009h

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

// ; ncopy (src, tgt)
inline % 42h

  mov  esi, dword ptr [rax]
  mov  dword ptr [rdi], esi
    
end

// ; nadd
inline % 43h

  mov  edx, dword ptr [rax]
  add  dword ptr [rdi], edx

end

// ; nsave
inline % 47h

  mov dword ptr [rdi], ebx

end

// ; nload
inline % 48h

  mov ebx, dword ptr [rax]

end

// ; restore

inline % 92h

  add  rbp, __arg1
  
end

// ; aloadfi
inline % 94h

  mov  rax, qword ptr [rbp+__arg1]

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

// ; bloadsi

inline % 0C9h

  mov  rdi, qword ptr [rsp + __arg1]

end

// ; aloadbi (__arg1 : index)

inline % 0CEh

  mov  rax, qword ptr [rdi + __arg1]

end

// xcallrm (rdx contains message, __arg1 contains vmtentry)
inline % 0FEh

   call code : __arg1

end
