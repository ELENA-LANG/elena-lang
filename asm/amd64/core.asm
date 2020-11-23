// ; ==== Command Set ==

// ; open
inline % 98h

  push rbp
  mov  rbp, rsp

end

// ; callextr
inline % 0A5h

  call extern __arg1
  mov  rdx, rax

end

// ; pushs
inline % 0BEh

  lea  rax, [rsp + __arg1]
  push rax

end

// ; reserve
inline % 0BFh

  sub  rsp, __arg1
  push rbp
  push 0
  mov  rbp, rsp

end


