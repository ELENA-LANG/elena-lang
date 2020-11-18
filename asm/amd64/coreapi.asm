// ; --- API ---

procedure coreapi'initProgramHeader

  lea  rdx, [rsp+10h]
  mov  rax, [rsp+8]
  mov  [rax+8], rdx
  mov  [rax+10h], rbp
  ret

end
