// --- operators ---


// ; n1 << c2

inline core_api'write (n1:out int, n1:short)

  pop  ebx
  pop  eax
  mov  ecx, [ebx]
  mov  [eax], ecx

end

// s1 == c2
                                  
inline core_api'equal (s1:wstr, c2:short)

  pop  esi                   // c2
  mov  eax, const : system'false
  pop  edx                   // s1
  cmp  [edx-8], 0FFFFFFFCh   // if it can be compared
  jnz  short Lab1
Lab2:
  mov  ebx, [esi]
  cmp  ebx,  [edx]
  jnz  short Lab1
  mov  eax, const : system'true
Lab1:

end

// a1 @ i2
inline core_api'getAt (a1:params, n2:int)

  pop  edx
  pop  esi
  mov  ebx, [edx]
  shl  ebx, 2
  mov  eax, [esi+ebx]

end

// --- conversion functions ---

// c2 >> s1
inline core_api'convert (s1: out wstr, c2:short)

  pop  ebx
  pop  eax
  mov  edx, [ebx]
  mov  [eax], edx

end

// --- memory operations

inline core_api'read (d1:dump, n:index, l:length, d2:out dump)

  mov  ecx, [esp+4]
  mov  edx, [esp+8]
  mov  ecx, [ecx]
  mov  esi, [esp+0Ch]
  mov  edx, [edx]

  // ; check if enough place
  mov  ebx, [esi-8]
  neg  ebx
  xor  eax, eax
  sub  ebx, edx
  sub  ebx, ecx
  js   short labEnd

  mov  eax, esi
  add  eax, edx
  pop  esi

  mov  ebx, eax
  and  ebx, 3h
  jz   short labNext
  sub  ecx, ebx

labNext1:
  mov  edx, [eax]
  mov  byte ptr [esi], dl
  lea  esi, [esi + 1]
  sub  ebx, 1
  lea  eax, [eax + 1]
  ja   short labNext1

  test ecx, ecx
  jz   short labSkip

labNext:
  mov  edx, [eax]
  mov  [esi], edx
  lea  esi, [esi + 4]
  sub  ecx, 4
  lea  eax, [eax + 4]
  ja   short labNext
  
labSkip:
  mov  eax, [esp+0Ch]

labEnd:
  lea  esp, [esp + 10h]

end

// --- win32 os routines ---

// ; verbs
define EXEC_MESSAGE_ID  801C0000h

inline core_api'read (p1:int, p2:out short)

  pop  eax
  pop  ebx
  mov  ecx, [ebx]
  and  ecx, 0FFFFh
  mov  [eax], ecx

end

// ; get the literal size
inline core_api'sizeOf (target:wstr,l2:out int)

  pop  eax
  pop  esi
  call code : "$package'core'getsize"
  shr  ecx, 1
  mov  [eax], ecx
  
end

// ; get the literal size
inline core_api'sizeOf (target:dump)

  call code : "system'NewIntNumber"
  
  pop  esi
  call code : "$package'core'getsize"
  mov  [eax], ecx
  
end



// fill array
inline core_api'fill (p1:pref, p2:ref)

   pop  ebx
   pop  eax
   mov  ecx, [eax-8]
   mov  esi, eax
labNext:
   mov  [esi], ebx
   sub  ecx, 4
   ja   short labNext

end

inline core_api'refresh (s:wstr)

   pop  eax
   mov  ecx, [eax-8]
   neg  ecx
   xor  ebx, ebx
   add  ecx, eax
   mov word ptr [ecx-2], bx

end

inline core_api'get (o1:ref, n2:index)

   pop  ebx
   pop  eax
   mov  ebx, [ebx]
   mov  eax, [ebx * 4]

end
