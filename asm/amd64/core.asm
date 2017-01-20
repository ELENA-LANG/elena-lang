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

// ; validate

inline % 03Ah

  cmp dword ptr [rax], eax

end

// xcallrm (rdx contains message, __arg1 contains vmtentry)
inline % 0FEh

   call code : __arg1

end
