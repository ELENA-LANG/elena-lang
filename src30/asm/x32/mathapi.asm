procedure mathapi'matrixSum
  mov  eax, [esp + 20]  // vector 1
  mov  ecx, [eax]
  push ecx
  
  mov  eax, [esp + 20]  // vector 2
  mov  ecx, [eax]
  push ecx
  
  mov  ebx, [esp + 20]   // vector size
  mov  ebx, [ebx]
  push ebx
  
  mov  eax, [esp + 20]   // row size    
  mov  eax, [eax]  
  push eax
                   
  // Total reduction size
  imul eax, ebx 
  push eax 
  mov ebx, [esp+4]
  
  mov eax, [esp + 24]   // result vector
  mov ecx, [eax]
  push ecx
    
  push ebp    
mainLoop:
  add  [esp+24], 24
  add  [esp+20], 24
  add  [esp+4], 24
  mov  eax, 4
  mov  ecx, 0
addingLoop:
  mov edx, [esp+24]
  fld qword ptr [edx+ecx]
  fstp dword ptr [ebp]
  
  mov edx, [esp+20]
  fld qword ptr [edx+ecx]
  fstp dword ptr [ebp+24] 
  
  add ebp, 4
  sub ecx, 8
  sub [esp+8], ebx
  sub eax, 1
  jnz addingLoop
  
  sub ebp, 16
  movups xmm0, [ebp]
  movups xmm1, [ebp+24]
  addps xmm0, xmm1
  mov edx, [esp+4]
  movups [edx], xmm0
  
  mov edx, [esp+8]
  cmp edx, 0
  jnz mainLoop 
  pop eax
  pop eax
  pop eax
  pop eax
  pop eax
  pop eax 
  pop eax
  ret 28
end
