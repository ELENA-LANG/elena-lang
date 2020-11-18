// ; ==== Command Set ==

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

