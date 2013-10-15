// --- operators ---

// r1 == r2

inline core_api'equal (p1:ref, p2:ref)

  pop  ecx
  mov  eax, const : system'true
  pop  ebx
  cmp  ebx, ecx
  jz   short labEnd
  mov  eax, const : system'false

labEnd:

end

// ; n1 << n2

inline core_api'write (n1:out int, n1:int)

  pop  ebx
  pop  eax
  mov  ecx, [ebx]
  mov  [eax], ecx

end

// ; n1 << c2

inline core_api'write (n1:out int, n1:short)

  pop  ebx
  pop  eax
  mov  ecx, [ebx]
  mov  [eax], ecx

end

// ; n1 == n2
inline core_api'equal (n1:int,n2:int)

  pop  ecx
  mov  eax, const : system'true
  mov  ecx, [ecx]
  pop  ebx
  cmp  ecx, [ebx]
  jz   short labEnd
  mov  eax, const : system'false
labEnd:  

end

// ; n1 != n2
inline core_api'notequal (n1:int,n2:int)

  pop  ecx
  mov  eax, const : system'false
  mov  ecx, [ecx]
  pop  ebx
  cmp  ecx, [ebx]
  jz   short labEnd
  mov  eax, const : system'true
labEnd:  

end

// ; n1 < n2
inline core_api'less (n1:int,n2:int)

  pop  ebx
  mov  eax, const : system'true
  mov  ebx, [ebx]
  pop  ecx
  cmp  [ecx], ebx
  jl   short labEnd
  mov  eax, const : system'false
labEnd:  

end

// ; n1 > n2
inline core_api'greater (n1:int,n2:int)

  pop  ebx
  mov  eax, const : system'true
  mov  ebx, [ebx]
  pop  ecx
  cmp  [ecx], ebx
  jg   short labEnd
  mov  eax, const : system'false
labEnd:  

end

// ; n1 >= n2
inline core_api'notless (n1:int,n2:int)

  pop  ebx
  mov  eax, const : system'true
  mov  ebx, [ebx]
  pop  ecx
  cmp  [ecx], ebx
  jge   short labEnd
  mov  eax, const : system'false
labEnd:  

end

// ; n1 <= n2
inline core_api'notgreater (n1:int,n2:int)

  pop  ebx
  mov  eax, const : system'true
  mov  ebx, [ebx]
  pop  ecx
  cmp  [ecx], ebx
  jle   short labEnd
  mov  eax, const : system'false
labEnd:  

end

// ; n1 + n2 => n3
inline core_api'add (n1:int, n2:int, n3:out int)

  pop  eax
  pop  ebx
  mov  ebx, [ebx]
  pop  ecx
  add  ebx, [ecx]
  mov  [eax], ebx

end

// ; n1 + n2 => w3
inline core_api'add (n1:int, n2:int, n3:out short)

  pop  eax
  pop  ebx
  mov  ebx, [ebx]
  pop  ecx
  add  ebx, [ecx]
  mov  [eax], ebx

end

// ; n1 - n2 => n3
inline core_api'subtract (n1:int, n2:int, n3:out int)

  pop  eax
  pop  ecx
  mov  ecx, [ecx]
  pop  ebx
  mov  ebx, [ebx]
  sub  ebx, ecx
  mov  [eax], ebx

end

// ; n2 * n1 => n3
inline core_api'multiply (n1:int, n2:int, n3:out int)

  pop  eax      
  pop  ecx
  mov  esi, eax
  pop  ebx
  mov  eax, [ebx]
  imul [ecx]
  mov  [esi], eax
  mov  eax, esi

end

// ; n1 / n2 => n3
inline core_api'divide (n1:int, n2:int, n3:out int)

  pop  eax
  pop  ebx
  mov  esi, eax
  pop  ecx
  mov  eax, [ecx]
  mov  ebx, [ebx]
  cdq
  idiv ebx
  mov  [esi], eax
  mov  eax, esi

end

// ; n2 && n1 => n3
inline core_api'and (n1:int, n2:int, n3:out int)

  pop  eax
  pop  ecx
  pop  ebx
  mov  ebx, [ebx]
  and  ebx, [ecx]
  mov  [eax], ebx

end

// ; n2 || n1 => n3
inline core_api'or (n2:int, n1:int, n3:out int)

  pop  eax
  pop  ecx
  pop  ebx
  mov  ebx, [ebx]
  or   ebx, [ecx]
  mov  [eax], ebx
  
end

// ; n2 ~~ n1 => n3
inline core_api'xor (n1:int, n2:int, n3:out int)

  pop  eax
  pop  ecx
  pop  ebx
  mov  ebx, [ebx]
  xor  ebx, [ecx]
  mov  [eax], ebx

end

// l1 << l2
inline core_api'write (l1:out long, l2:long)

  pop  ebx
  pop  eax
  mov  ecx, [ebx]
  mov  [eax], ecx
  mov  edx, [ebx+4]
  mov  [eax+4], edx

end

// n1 += n2
inline core_api'append (n1:out int, n2:int)

  pop  ebx  
  pop  eax
  mov  ebx, [ebx]
  add  [eax], ebx

end

// n1 -= n2
inline core_api'reduce (n1:out int, n2:int)

  pop  ecx
  pop  eax
  mov  ecx, [ecx]
  sub  [eax], ecx

end

// n1 *= n2
inline core_api'multiplyBy (n1:out int, n2:int)
        
  pop  ecx  
  pop  esi
  mov  eax, [esi]
  imul [ecx]
  mov  [esi], eax
  mov  eax, esi

end

// n1 /= n2
inline core_api'divideInto (n1:out int, n2: int)

  pop  ebx
  pop  esi
  mov  eax, [esi]
  mov  ecx, [ebx]
  cdq
  idiv ecx
  mov  [esi], eax
  mov  eax, esi

end

// l1 << n2
inline core_api'write (l1:out long, n2:int)
         
  pop  ebx
  pop  eax
  mov  ecx, [ebx]
  xor  edx, edx
  mov  [eax], ecx
  mov  [eax+4], edx

end

// l1 == l2
inline core_api'equal (l1:long,l2:long)

  pop ecx
  mov eax, const : system'false
  pop edx
  mov ebx, [ecx]
  mov ecx, [ecx+4]
  cmp [edx], ebx
  jnz short Lab1
  cmp [edx+4], ecx
  jnz short Lab1
  mov eax, const : system'true
Lab1:

end

// l1 < l2
inline core_api'less (l1:long,l2:long)

  pop ecx
  mov eax, const : system'true
  pop edx
  mov ebx, [ecx]
  mov ecx, [ecx+4]
  cmp [edx+4], ecx
  jl  short Lab1
  nop
  jnz  short Lab2
  cmp  [edx], ebx
  jl   short Lab1
Lab2:
  mov eax, const : system'false
Lab1:

end

// l1 >= l2
inline core_api'notless (l1:long,l2:long)

  pop ecx
  mov eax, const : system'true
  pop edx
  mov ebx, [ecx]    // ; l2
  mov ecx, [ecx+4]
  cmp [edx+4], ecx
  jl  short Lab1
  nop
  jnz  short Lab2
  cmp  [edx], ebx
  jge   short Lab1
Lab2:
  mov eax, const : system'false
Lab1:

end

// l2 + l1
inline core_api'add (l1:long, l2:long, l3:out long)

  pop eax
  pop ecx 
  pop esi

  mov ebx, [ecx]
  mov edx, [esi]

  mov ecx, [ecx+4]
  mov esi, [esi+4]

  add ebx, edx
  adc ecx, esi

  mov [eax], ebx
  mov [eax+4], ecx

end

// l1 - l2
inline core_api'subtract (l1:long, l2:long, l3:out long)

  pop eax
  pop ecx
  pop esi

  mov ebx, [ecx]           // sour lo
  mov edx, [esi]           // dest lo

  mov ecx, [ecx+4]         // sour hi
  mov esi, [esi+4]         // dest hi

  sub edx, ebx             // dest lo
  sbb esi, ecx             // dest hi

  mov [eax], edx
  mov [eax+4], esi

end

// l2 * l1
inline core_api'multiply (l1:long, l2:long, l3:out long)

  pop  eax
  pop  esi            // sour
  pop  edx            // dest

  push eax

  mov  ecx, [edx+4]   // DHI
  mov  eax, [esi+4]   // SHI
  or   eax, ecx
  mov  ecx, [edx]     // DLO
  jnz  short lLong
  mov  eax, [esi]
  mul  ecx
  jmp  short lEnd

lLong:
  mov  eax, [esi+4]
  push edi
  mov  edi, edx
  mul  ecx               // SHI * DLO
  mov  ebx, eax
  mov  eax, dword ptr [esi]
  mul  dword ptr [edi+4]  // SLO * DHI
  add  ebx, eax     
  mov  eax, dword ptr [esi] // SLO * DLO
  mul  ecx
  add  edx, ebx 
  pop  edi

lEnd:
  pop  ebx
  mov  [ebx], eax
  mov  [ebx+4], edx
  mov  eax, ebx

end

// l1 / l2
// esp    - lo DVSR
// esp+4  - hi DVSR
// esp+8  - lo DVND
// esp+0C - hi DVND

inline core_api'divide (l1:long, l2:long, l3:out long)

  pop  eax
  pop  ebx        // DVSR
  pop  esi        // DVND

  push eax
  push edi

  push [esi+4]    // DVND hi dword
  push [esi]      // DVND lo dword
  push [ebx+4]    // DVSR hi dword
  push [ebx]      // DVSR lo dword

  xor  edi, edi

  mov  eax, [esp+0Ch]    // hi DVND
  or   eax, eax
  jge  short L1
  add  edi, 1
  mov  edx, [esp+8]      // lo DVND
  neg  eax
  neg  edx
  sbb  eax, 0
  mov  [esp+0Ch], eax    // hi DVND
  mov  [esp+8], edx      // lo DVND

L1:
  mov  eax, [esp+4]      // hi DVSR
  or   eax, eax
  jge  short L2
  add  edi, 1
  mov  edx, [esp]        // lo DVSR
  neg  eax
  neg  edx
  sbb  eax, 0
  mov  [esp+4], eax      // hi DVSR
  mov  [esp], edx        // lo DVSR

L2:
  or   eax, eax
  jnz  short L3
  mov  ecx, [esp]        // lo DVSR
  mov  eax, [esp+0Ch]    // hi DVND
  xor  edx, edx
  div  ecx
  mov  ebx, eax 
  mov  eax, [esp+8]      // lo DVND
  div  ecx

  mov  esi, eax          // result
  jmp  short L4

L3:
  mov  ebx, eax 
  mov  ecx, [esp]        // lo DVSR
  mov  edx, [esp+0Ch]    // hi DVND
  mov  eax, [esp+8]      // lo DVDN
L5:
  shr  ebx, 1 
  rcr  ecx, 1
  shr  edx, 1 
  rcr  eax, 1
  or   ebx, ebx 
  jnz  short L5
  div  ecx
  mov  esi, eax          // result

  // check the result with the original
  mul  [esp+4]           // hi DVSR
  mov  ecx, eax 
  mov  eax, [esp]        // lo DVSR
  mul  esi
  add  edx, ecx

  // carry means Quotient is off by 1
  jb   short L6

  cmp  edx, [esp+0Ch]    // hi DVND
  ja   short L6
  jb   short L7
  cmp  eax, [esp+8]      // lo DVND
  jbe  short L7

L6:
  sub  esi, 1

L7:
  xor  ebx, ebx

L4:
  mov  edx, ebx
  mov  eax, esi

  sub  edi, 1
  jnz  short L8
  neg  edx
  neg  eax
  sbb  edx, 0

L8:
  lea  esp, [esp+10h]
  pop  edi
  pop  ebx

  mov  [ebx], eax
  mov  [ebx+4], edx
  mov  eax, ebx

end

// l1 += l2
inline core_api'append (l1:out long, l2:long)

  pop ecx 
  pop eax

  mov ebx, [ecx]
  mov edx, [eax]

  mov ecx, [ecx+4]
  mov esi, [eax+4]

  add ebx, edx
  adc ecx, esi

  mov [eax], ebx
  mov [eax+4], ecx

end

// l1 -= l2
inline core_api'reduce (p1:out long, p2:long)

  pop ecx
  pop eax

  mov ebx, [ecx]           // sour lo
  mov edx, [eax]           // dest lo

  mov ecx, [ecx+4]         // sour hi
  mov esi, [eax+4]         // dest hi

  sub edx, ebx             // dest lo
  sbb esi, ecx             // dest hi

  mov [eax], edx
  mov [eax+4], esi

end

// l1 *= l2
inline core_api'multiplyBy (l1:out long, l2:long)

  pop  esi            // sour
  mov  edx, [esp]     // dest

  mov  ecx, [edx+4]   // DHI
  mov  eax, [esi+4]   // SHI
  or   eax, ecx
  mov  ecx, [edx]     // DLO
  jnz  short lLong
  mov  eax, [esi]
  mul  ecx
  jmp  short lEnd

lLong:
  mov  eax, [esi+4]
  push edi
  mov  edi, edx
  mul  ecx               // SHI * DLO
  mov  ebx, eax
  mov  eax, dword ptr [esi]
  mul  dword ptr [edi+4]  // SLO * DHI
  add  ebx, eax     
  mov  eax, dword ptr [esi] // SLO * DLO
  mul  ecx
  add  edx, ebx 
  pop  edi

lEnd:
  pop  ebx
  mov  [ebx], eax
  mov  [ebx+4], edx
  mov  eax, ebx

end

// l1 /= l2
// esp    - lo DVSR
// esp+4  - hi DVSR
// esp+8  - lo DVND
// esp+0C - hi DVND

inline core_api'divideInto (l1:out long, l2:long)

  pop  ebx        // DVSR
  mov  esi, [esp] // DVND

  push edi

  push [esi+4]    // DVND hi dword
  push [esi]      // DVND lo dword
  push [ebx+4]    // DVSR hi dword
  push [ebx]      // DVSR lo dword

  xor  edi, edi

  mov  eax, [esp+0Ch]    // hi DVND
  or   eax, eax
  jge  short L1
  add  edi, 1
  mov  edx, [esp+8]      // lo DVND
  neg  eax
  neg  edx
  sbb  eax, 0
  mov  [esp+0Ch], eax    // hi DVND
  mov  [esp+8], edx      // lo DVND

L1:
  mov  eax, [esp+4]      // hi DVSR
  or   eax, eax
  jge  short L2
  add  edi, 1
  mov  edx, [esp]        // lo DVSR
  neg  eax
  neg  edx
  sbb  eax, 0
  mov  [esp+4], eax      // hi DVSR
  mov  [esp], edx        // lo DVSR

L2:
  or   eax, eax
  jnz  short L3
  mov  ecx, [esp]        // lo DVSR
  mov  eax, [esp+0Ch]    // hi DVND
  xor  edx, edx
  div  ecx
  mov  ebx, eax 
  mov  eax, [esp+8]      // lo DVND
  div  ecx

  mov  esi, eax          // result
  jmp  short L4

L3:
  mov  ebx, eax 
  mov  ecx, [esp]        // lo DVSR
  mov  edx, [esp+0Ch]    // hi DVND
  mov  eax, [esp+8]      // lo DVDN
L5:
  shr  ebx, 1 
  rcr  ecx, 1
  shr  edx, 1 
  rcr  eax, 1
  or   ebx, ebx 
  jnz  short L5
  div  ecx
  mov  esi, eax          // result

  // ; check the result with the original
  mul  [esp+4]           // hi DVSR
  mov  ecx, eax 
  mov  eax, [esp]        // lo DVSR
  mul  esi
  add  edx, ecx

  // carry means Quotient is off by 1
  jb   short L6

  cmp  edx, [esp+0Ch]    // hi DVND
  ja   short L6
  jb   short L7
  cmp  eax, [esp+8]      // lo DVND
  jbe  short L7

L6:
  sub  esi, 1

L7:
  xor  ebx, ebx

L4:
  mov  edx, ebx
  mov  eax, esi

  sub  edi, 1
  jnz  short L8
  neg  edx
  neg  eax
  sbb  edx, 0

L8:
  lea  esp, [esp+10h]
  pop  edi
  pop  ebx

  mov  [ebx], eax
  mov  [ebx+4], edx
  mov  eax, ebx

end

// ; l2 && l1
inline core_api'and (l1:long, l2:long, l3:out long)

  pop eax
  pop ecx 
  pop esi

  mov ebx, [ecx]
  mov edx, [esi]

  mov ecx, [ecx+4]
  mov esi, [esi+4]

  and ebx, edx
  and ecx, esi

  mov [eax], ebx
  mov [eax+4], ecx

end

// ; l2 || l1
inline core_api'or (l1:long, l2:long, l3:out long)

  pop eax
  pop ecx 
  pop esi

  mov ebx, [ecx]
  mov edx, [esi]

  mov ecx, [ecx+4]
  mov esi, [esi+4]

  or  ebx, edx
  or  ecx, esi
  
  mov [eax], ebx
  mov [eax+4], ecx

end

// ; l2 ~~ l1
inline core_api'xor (l1:long, l2:long, l3:out long)

  pop eax
  pop ecx 
  pop esi

  mov ebx, [ecx]
  mov edx, [esi]

  mov ecx, [ecx+4]
  mov esi, [esi+4]

  xor ebx, edx
  xor ecx, esi
  
  mov [eax], ebx
  mov [eax+4], ecx

end

// ; r1 << r2
inline core_api'write (r1:out real, r2:real)

  pop  ebx
  pop  eax
  mov  ecx, [ebx]
  mov  [eax], ecx
  mov  edx, [ebx+4]
  mov  [eax+4], edx

end

// ; r1 << n2
inline core_api'write (r1:out real, n2:int)

  pop  ebx
  pop  eax

  fild dword ptr [ebx]
  fstp qword ptr [eax]

end

// ; r1 << l2
inline core_api'write (r1:out real, l2:long)

  pop  ebx
  pop  eax

  fild qword ptr [ebx]
  fstp qword ptr [eax]

end

// r1 == r2
inline core_api'equal (r1:real,r2:real)

  pop    ecx
  fld    qword ptr [ecx]
  pop    ebx
  fld    qword ptr [ebx]
  fcomip st, st(1)
  mov    eax, const : system'true
  je     short lab1
  mov    eax, const : system'false
lab1:
  fstp  st(0)

end

// r1 < r2
inline core_api'less (r1:real,r2:real)

  pop    ebx
  fld    qword ptr [ebx]
  pop    ecx
  fld    qword ptr [ecx]
  fcomip st, st(1)
  mov    eax, const : system'true
  jb     short lab1
  mov    eax, const : system'false
lab1:
  fstp  st(0)

end

// r1 >= r2
inline core_api'notless (r1:real,r2:real)

  pop    ebx
  fld    qword ptr [ebx]
  pop    ecx
  fld    qword ptr [ecx]
  fcomip st, st(1)
  mov    eax, const : system'true
  jae     short lab1
  mov    eax, const : system'false
lab1:
  fstp  st(0)

end

// r1 + r2
inline core_api'add (r1:real, r2:real, r3:out real)

  pop  eax
  pop  ecx
  pop  ebx

  fld  qword ptr [ecx]
  fadd qword ptr [ebx] 
  fstp qword ptr [eax]

end

// r1 - r2
inline core_api'subtract (r1:real, r2:real, r3:out real)

  pop  eax
  pop  ebx
  pop  ecx

  fld  qword ptr [ecx]
  fsub qword ptr [ebx] 
  fstp qword ptr [eax]

end

// r1 * r2
inline core_api'multiply (r1:real, r2:real, r3:out real)

  pop  eax
  pop  ecx
  pop  ebx

  fld  qword ptr [ecx]
  fmul qword ptr [ebx] 
  fstp qword ptr [eax]

end

// r1 / r2
inline core_api'divide (r1:real, r2:real, r3:out real)

  pop  eax
  pop  ebx
  pop  ecx
  
  fld  qword ptr [ecx]
  fdiv qword ptr [ebx] 
  fstp qword ptr [eax]

end

// r1 += r2 
inline core_api'append (r1:out real, r2:real)

  pop  ecx
  pop  eax

  fld  qword ptr [ecx]
  fadd qword ptr [eax] 
  fstp qword ptr [eax]

end

// r1 += n2
inline core_api'append (r1:out real, n2:int)

  pop  ecx
  pop  eax

  fld  qword ptr [eax]
  fild dword ptr [ecx]
  faddp
  fstp qword ptr [eax]

end

// r1 -= r2
inline core_api'reduce (r1:out real, r2:real)

  pop  ebx
  pop  eax

  fld  qword ptr [eax]
  fsub qword ptr [ebx] 
  fstp qword ptr [eax]

end

// r1 -= n2
inline core_api'reduce (r1:out real, n2:int)

  pop  ecx
  pop  eax

  fld  qword ptr [eax]
  fild dword ptr [ecx]
  fsubp
  fstp qword ptr [eax]

end

// r1 *= r2
inline core_api'multiplyBy (r1:out real, r2:real)

  pop  ecx
  pop  eax

  fld  qword ptr [eax]
  fmul qword ptr [ecx] 
  fstp qword ptr [eax]

end

// r1 *= n2
inline core_api'multiplyBy (r1:out real, n2:int)

  pop  ecx
  pop  eax

  fld  qword ptr [eax]
  fild dword ptr [ecx]
  fmulp
  fstp qword ptr [eax]

end

// r1 /= r2 
inline core_api'divideInto (r1:out real, r2:real)

  pop  ebx
  pop  eax

  fld  qword ptr [eax]
  fdiv qword ptr [ebx] 
  fstp qword ptr [eax]

end

// r1 /= n2
inline core_api'divideInto (r1:out real, n2:int)

  pop  ecx
  pop  eax

  fld  qword ptr [eax]
  fild dword ptr [ecx]
  fdivp
  fstp qword ptr [eax]

end

// s1 == s2
                                  
inline core_api'equal (s1:wstr, s2:wstr)

  pop  esi                   // s2
  mov  eax, const : system'false
  pop  edx                   // s1
  mov  ecx, [edx-8]          // s1.length
  cmp  ecx, [esi-8]          // compare with s2.length
  jnz  short Lab1
  add  ecx, 2
Lab2:
  mov  ebx, [esi]
  cmp  ebx,  [edx]
  jnz  short Lab1
  lea  esi, [esi+4]
  lea  edx, [edx+4]
  add  ecx, 4
  js   short Lab2
  mov  eax, const : system'true
Lab1:

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
                                  
// s1 < s2

inline core_api'less (s1:wstr, s2:wstr)

  pop  esi                     // s2
  mov  eax, const : system'false
  pop  edx                     // s1
  mov  ecx, [edx-8]            // s1 length

  cmp  ecx, [esi-8]
  jbe  short Lab3
  mov  ecx, [esi-8]
Lab3:
  neg  ecx
Lab2:
  mov  ebx, [edx]              // s1[i] 
  cmp  bx, word ptr [esi]      // compare s2[i] with 
  jb   short Lab1
  ja   short LabEnd
  lea  esi, [esi+2]
  lea  edx, [edx+2]
  sub  ecx, 2
  jnz  short Lab2

Lab1:
  mov  eax, const : system'true

LabEnd:

end
                                  
// s1 > s2

inline core_api'greater (s1:wstr, s2:wstr)

  pop  esi                     // s2
  mov  eax, const : system'false
  pop  edx                     // s1
  mov  ecx, [edx-8]            // s1 length

  cmp  ecx, [esi-8]
  jae  short Lab3
  mov  ecx, [esi-8]
Lab3:
  neg  ecx
Lab2:
  mov  ebx, [edx]              // s1[i] 
  cmp  bx, word ptr [esi]      // compare s2[i] with 
  ja   short Lab1
  jb   short LabEnd
  lea  esi, [esi+2]
  lea  edx, [edx+2]
  sub  ecx, 2
  jnz  short Lab2

Lab1:
  mov  eax, const : system'true

LabEnd:

end
                                  
// s1 >= s2

inline core_api'notless (s1:wstr, s2:wstr)

  pop  esi                     // s2
  mov  eax, const : system'false
  pop  edx                     // s1
  mov  ecx, [edx-8]            // s1 length

  cmp  ecx, [esi-8]
  ja   short Lab3
  mov  ecx, [esi-8]
Lab3:
  neg  ecx
Lab2:
  mov  ebx, [edx]              // s1[i] 
  cmp  bx, word ptr [esi]      // compare s2[i] with 
  jb   short Lab1
  ja   short LabEnd
  lea  esi, [esi+2]
  lea  edx, [edx+2]
  sub  ecx, 2
  jnz  short Lab2

Lab1:
  mov  eax, const : system'true

LabEnd:

end

// s1 << s2

inline core_api'write (s1:out wstr, s2:wstr)

  pop  esi                     // s2
  pop  eax                     // s1

  mov  ecx, [esi-8]
  mov  edx, eax
  mov  [eax-8], ecx
  
labNext1:
  mov  ebx, [esi]
  mov  [edx], ebx
  lea  esi, [esi+4]
  lea  edx, [edx+4]
  add  ecx, 4
  ja   short labNext1

end

// s1 += s2

inline core_api'append (s1:out wstr, s2:wstr)

  mov  ecx, 2
  pop  esi                     // s2
  mov  edx, 2
  pop  eax                     // s1
  
  add  ecx, [esi-8]
  add  edx, [eax-8]
  neg  edx
  add  [eax-8], ecx
  add  edx, eax
  lea  ecx, [ecx-2]
  
labNext2:
  mov  ebx, [esi]
  mov  word ptr [edx], ebx
  lea  esi, [esi+2]
  lea  edx, [edx+2]
  add  ecx, 2
  jnz  short labNext2

end

// s1 @ i2
inline core_api'getAt (s1:wstr, n2:int, ch:out short)

  pop  eax
  pop  edx
  pop  esi
  mov  ebx, [edx]
  shl  ebx, 1
  mov  edx, [esi+ebx]
  and  edx, 0FFFFh
  mov  [eax], edx

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

// ; n2 => s1
inline core_api'convert (s1:out wstr, n2:int)

   pop  eax
   push ebp
   push [eax]
   mov  ebp, esp
   mov  eax, [eax]     // get dword
   xor  ecx, ecx
   cmp  eax, 0
   jns  short Lab6
   neg  eax
Lab6:
   mov  ebx, 0Ah
   cmp  ebx, eax
   jnbe short Lab5
Lab1:
   xor  edx, edx
   idiv ebx
   push edx
   add  ecx, 1
   cmp  eax, 9                  
   ja   short Lab1
Lab5:   
   push eax
   add  ecx, 1
   mov  eax, [ebp]
   cmp  eax, 0
   jns  short Lab7
   push 0FDh      // to get "-" after adding 0x30
   add  ecx, 1
Lab7:
   mov  edx, ecx
   add  ecx, 1     // include trailing zero
   shl  ecx, 1
   mov  esi, [ebp+8]
   neg  ecx
   mov  [esi-8], ecx
   mov  ebx, 0FFh
   mov  ecx, edx
Lab2:
   pop  eax
   add  eax, 30h
   and  eax, ebx
   mov  word ptr [esi], ax
   add  esi, 2
   sub  ecx, 1
   jnz  short Lab2
   xor  eax, eax
   mov  word ptr [esi], ax
   pop  ebx
   pop  ebp
   pop  eax

end

inline core_api'convert (s1:out wstr, n2:int, n3: int)

   pop  ebx
   mov  ebx, [ebx]
   pop  eax
   push ebp
   push [eax]
   mov  ebp, esp
   mov  eax, [eax]     // get dword
   xor  ecx, ecx
   cmp  eax, ebx
   jb   short Lab5
Lab1:
   xor  edx, edx
   idiv ebx
   push edx
   add  ecx, 2
   cmp  eax, ebx
   jae  short Lab1
Lab5:   
   add  ecx, 4
   push eax
   neg  ecx
   mov  esi, [ebp+8]
   mov  [esi-8], ecx
   mov  edx, 0FFh
   add  ecx, 2             // to skip zero
Lab2:
   pop  eax
   cmp  eax, 0Ah
   jb   short Lab8
   add  eax, 7
Lab8:
   add  eax, 30h
   and  eax, edx
   mov  word ptr [esi], ax
   add  esi, 2
   add  ecx, 2
   jnz  short Lab2
   xor  eax, eax
   mov  word ptr [esi], ax
   pop  ebx
   pop  ebp
   pop  eax

end

// ; l2 => s1

inline core_api'convert (s1:out wstr, l2:long)

   pop  eax
   push edi
   push ebp
   push [eax+4]
   mov  ebp, esp
   mov  edx, [eax]     // NLO
   mov  eax, [eax+4]   // NHI
   xor  ecx, ecx
   push ecx 
   or   eax, eax
   jge  short Lab6

   neg  eax 
   neg  edx 
   sbb  eax, 0

Lab6:                 // convert 
   mov  esi, edx      // NLO
   mov  edi, eax      // NHI
   mov  ecx, 10       // LO

Lab1:
   test edi, edi
   jnz  short labConvert
   cmp  esi, 9
   jbe  short Lab5

labConvert:
   mov  eax, edi      // NHI
   xor  edx, edx
   div  ecx
   mov  ebx, eax
   mov  eax, esi      // NLO
   div  ecx
   mov  edi,ebx 
   mov  esi,eax

   push edx
   add  [ebp-4], 1
   jmp  short Lab1

Lab5:   
   push esi

   mov  ecx, [ebp-4]
   add  ecx, 1

   mov  eax, [ebp]
   cmp  eax, 0
   jns  short Lab7
   push 0FDh      // to get "-" after adding 0x30
   add  ecx, 1
Lab7:
   mov  edx, ecx
   add  ecx, 1    // ;  including trailing zero
   shl  ecx, 1
   mov  esi, [ebp+0Ch]
   neg  ecx
   mov  [esi-8], ecx
   mov  ebx, 0FFh
   mov  ecx, edx
Lab2:
   pop  eax
   add  eax, 30h
   and  eax, ebx
   mov  word ptr [esi], ax
   add  esi, 2
   sub  ecx, 1
   jnz  short Lab2
   xor  eax, eax
   mov  word ptr [esi], ax
   lea  esp, [esp+8]
   pop  ebp
   pop  edi
   pop  eax
   
end

// r2 >> s1
inline core_api'convert (s1:out wstr, r2:real, n3:int)

   pop   ebx
   mov   ebx, [ebx]
   pop   ecx
   mov   eax, [esp]   
   push  ebp
   mov   ebp, esp

   sub   esp, 52  

   push  edi

   mov   edi, eax	
   
   lea   ebx, [ebx-3]         // get the number of decimal digits (minus 2 for sign and dot)
   cmp   ebx, 13
   jbe   short ftoa1   
   mov   ebx, 13
ftoa1:
   xor   edx, edx

   //-------------------------------------------
   //first examine the value on FPU for validity
   //-------------------------------------------

   fld   qword ptr [ecx]
   fxam                       // examine value on FPU
   fstsw ax                   // get result

   sahf                       // transfer to CPU flags
   jz    short maybezero
   jpo   srcerr               // C3=0 and C2=0 would be NAN or unsupported
   jnc   short getnumsize      // continue if normal finite number

   //--------------------------------
   //value to be converted = INFINITY
   //--------------------------------

   mov   al,43                // "+"
   test  ah,2                 // C1 field = sign
   jz    short ftoa2
   mov   al, 45               // "-"
ftoa2:
   and   eax, 0FFh
   stosw
   mov   eax,4E0049h        // "NI"
   stosd
   mov   eax,490046h        // "IF"
   stosd
   mov   eax,49004Eh        // "IN"
   stosd
   mov   eax,590054h        // "YT"
   stosd
   jmp   finish      

   //-------------------------
   //value to be converted = 0
   //-------------------------
         
maybezero:
   jpe   short getnumsize      // would be denormalized number
   fstp  st(0)                // flush that 0 value off the FPU
   mov   eax,2E0030h          // ".0" szstring
   stosd                      // write it
   mov   eax,30h              // "0" szstring
   stosd                      // write it
   jmp   finish

   //---------------------------
   // get the size of the number
   //---------------------------

getnumsize:
   fldlg2                     // log10(2)
   fld   st(1)                // copy Src
   fabs                       // insures a positive value
   fyl2x                      // ->[log2(Src)]*[log10(2)] = log10(Src)
      
   fstcw word ptr [ebp-4]     // get current control word
   mov   ax, word ptr [ebp-4]
   or    ax,0C00h             // code it for truncating
   mov   word ptr [ebp-8],ax
   fldcw word ptr [ebp-8]     // insure rounding code of FPU to truncating
      
   fist  [ebp-12]             // store characteristic of logarithm
   fldcw word ptr [ebp-4]     // load back the former control word

   ftst                       // test logarithm for its sign
   fstsw ax                   // get result
   sahf                       // transfer to CPU flags
   sbb   [ebp-12],0           // decrement esize if log is negative
   fstp  st(0)                // get rid of the logarithm

   //-----------------------------------------------------------------------
   // get the power of 10 required to generate an integer with the specified
   // number of significant digits
   //-----------------------------------------------------------------------
   
   mov   eax, [ebp-12]
   lea   eax, [eax+1]  // one digit is required
   or    eax, eax
   js    short ftoa21
   cmp   eax, 13
   jbe   short ftoa20
   mov   edx, -1
   mov   ebx, 13
   mov   ecx, ebx
   sub   ecx, eax
   mov   [ebp-16], ecx
   jmp   short ftoa22

ftoa20:
   add   eax, ebx
   cmp   eax, 13
   jbe   short ftoa21
   sub   eax, 13
   sub   ebx, eax      

ftoa21:
   mov   [ebp-16], ebx

ftoa22:

   //----------------------------------------------------------------------------------------
   // multiply the number by the power of 10 to generate required integer and store it as BCD
   //----------------------------------------------------------------------------------------

   fild  dword ptr [ebp-16]
   fldl2t
   fmulp                      // ->log2(10)*exponent
   fld   st(0)
   frndint                    // get the characteristic of the log
   fxch st(1)
   fsub  st(0),st(1)          // get only the fractional part but keep the characteristic
   f2xm1                      // ->2^(fractional part)-1
   fld1
   faddp                      // add 1 back
   fscale                     // re-adjust the exponent part of the REAL number
   fstp  st(1)                // get rid of the characteristic of the log
   fmulp                      // ->16-digit integer

   fbstp tbyte ptr[ebp-28]    // ->TBYTE containing the packed digits
   fstsw ax                   // retrieve exception flags from FPU
   shr   eax,1                // test for invalid operation
   jc    srcerr               // clean-up and return error

   //------------------------------------------------------------------------------
   // unpack BCD, the 10 bytes returned by the FPU being in the little-endian style
   //------------------------------------------------------------------------------

   lea   esi, [ebp-19]        // go to the most significant byte (sign byte)
   push  edi
   lea   edi,[ebp-52]
   mov   eax,3020h
   movzx  ecx,byte ptr[esi]     // sign byte
   cmp   ecx, 00000080h
   jnz   short ftoa5
   mov   al, 45               // insert sign if negative number
ftoa5:

   stosw
   mov   ecx,9
ftoa6:
   sub   esi, 1
   movzx eax,byte ptr[esi]
   ror   ax,4
   ror   ah,4
   add   eax,3030h
   stosw
   sub   ecx, 1
   jnz   short ftoa6

   pop   edi
   lea   esi,[ebp-52]
   
   cmp   edx, 0
   jnz   short scientific

   //************************
   // REGULAR STRING NOTATION
   //************************

   movsb                      // insert sign
   xor   eax, eax
   stosb

   cmp   byte ptr[esi-1], 20h // test if we insert space
   jnz   short ftoa60
   lea   edi, [edi-2]         // erase it

ftoa60:
   mov   ecx,1                // at least 1 integer digit
   mov   eax, [ebp-12]
   or    eax, eax             // is size negative (i.e. number smaller than 1)
   js    short ftoa61
   add   ecx, eax

ftoa61:
   mov   eax, ebx
   add   eax, ecx             // ->total number of digits to be displayed
   sub   eax, 19
   sub   esi, eax             // address of 1st digit to be displayed
   cmp   byte ptr[esi-1], 49  // "1"
   jnz   ftoa8 
   sub   esi, 1
   add   ecx, 1 
ftoa8:
   test  ecx, ecx
   jz    short ftoa8End
ftoa8Next:                    // copy required integer digits
   movzx  eax, byte ptr [esi]
   mov   word ptr [edi], ax
   lea   esi, [esi+1]
   lea   edi, [edi+2]
   sub   ecx, 1
   jnz   short ftoa8Next
ftoa8End:
   mov   ecx,ebx
   or    ecx,ecx
   jz    short ftoa9
   mov   eax,46               // "."
   stosw

ftoa9Next:                    // copy required decimal digits
   movzx  eax, byte ptr [esi]
   mov   word ptr [edi], ax
   lea   esi, [esi+1]
   lea   edi, [edi+2]
   sub   ecx, 1
   jnz   short ftoa9Next
ftoa9:
   jmp   finish

scientific:
   movsb                      // insert sign
   xor   eax, eax
   stosb

   cmp   byte ptr[esi-1], 20h // test if we insert space
   jnz   short ftoa90
   lea   edi, [edi-2]         // erase it

ftoa90:
   mov   ecx, ebx
   mov   eax, 18
   sub   eax, ecx
   add   esi, eax
   cmp   byte ptr[esi-1],49   // "1"
   pushfd                     // save flags for extra "1"
   jnz   short ftoa10
   sub   esi, 1
ftoa10:
   movsb                      // copy the integer
   xor   eax, eax
   stosb

   mov   eax,46               // "."
   stosw

ftoa10Next:                    // copy the decimal digits
   movzx  eax, byte ptr [esi]
   mov   word ptr [edi], ax
   lea   esi, [esi+1]
   lea   edi, [edi+2]
   sub   ecx, 1
   jnz   short ftoa10Next

   mov   eax,69                // "E"
   stosw
   mov   eax,43                // "+"
   mov   ecx,[ebp-12]
   popfd                      // retrieve flags for extra "1"
   jnz   short ftoa11          // no extra "1"
   add   ecx, 1               // adjust exponent
ftoa11:
   or    ecx,ecx
   jns   short ftoa12
   mov   eax,45                // "-"
   neg   ecx                  // make number positive
ftoa12:
   stosw                      // insert proper sign

// Note: the absolute value of the size could not exceed 4931
   
   xor   ebx, ebx   
   mov   eax,ecx
   mov   cl,100
   div   cl                   // ->thousands & hundreds in al, tens & units in AH
   push  eax
   and   eax,0ffh             // keep only the thousands & hundreds
   mov   cl,10
   div   cl                   // ->thousands in al, hundreds in AH
   add   eax,3030h            // convert to characters
   mov   bl, al               // insert them 
   mov   word ptr [edi], bx
   lea   edi, [edi+2]
   shr   eax, 8
   mov   bl, al
   mov   word ptr [edi], bx
   lea   edi, [edi+2]
   pop   eax
   shr   eax,8                // get the tens & units in al
   div   cl                   // tens in al, units in AH
   add   eax,3030h            // convert to characters

   mov   bl, al               // insert them 
   mov   word ptr [edi], bx
   lea   edi, [edi+2]
   shr   eax, 8
   mov   bl, al
   mov   word ptr [edi], bx
   lea   edi, [edi+2]

finish:
   cmp   word ptr [edi-2], 48 // '0'
   jnz   short finish1
   lea   edi, [edi-2]
   jmp   short finish

finish1:
   cmp   word ptr [edi-2], 46 // '.'
   jnz   short finish2
   lea   edi, [edi+2]

finish2:
   xor   ecx, ecx
   mov   word ptr [edi], cx
   lea   ebx, [edi+2]
   pop   edi
   add   esp, 52
   pop   ebp

   pop   eax
   sub   ebx, eax
   neg   ebx
   mov   [eax-8], ebx

   jmp   short finish3

srcerr:
   pop   edi
   add   esp, 52
   pop   ebp
   xor   eax,eax
finish3:

/*
oldcw   :-4  (4)
truncw  :-8  (4)
esize   :-12 (4)
tempdw  :-16 (4)
bcdstr  :-28 (12)  // -20
unpacked:- (52)  // -32
*/

end

// c2 >> s1
inline core_api'convert (s1: out wstr, c2:short)

  pop  ebx
  pop  eax
  mov  edx, [ebx]
  mov  [eax], edx

end

// literalToInt
inline core_api'toInt (s1:wstr, n2: out int)

  pop  eax
  pop  esi                          // get field
  mov  ecx, [esi-8]
  push eax
  xor  ebx, ebx  
  neg  ecx
  cmp  byte ptr [esi], 2Dh
  lea  ecx, [ecx-2]
  jnz  short Lab4
  lodsw
  lea  ecx, [ecx-2]
  mov  ebx, 1                        // set flag in ebx
Lab4:
  xor  eax, eax
Lab1:
  mov  edx, 10
  mul  edx
  mov  edx, eax
  xor  eax, eax
  lodsw
  sub  al, 30h
  jb   short Lab2
  cmp  al, 9
  ja   short Lab2
  add  eax, edx
  sub  ecx, 2
  jnz  short Lab1
  nop
  and  ebx, ebx
  jz   short Lab5
  neg  eax
Lab5:
  pop  ebx
  mov  [ebx], eax
  mov  eax, ebx
  jmp  short Lab3
Lab2:
  xor  eax, eax
Lab3:

end

// literalToIntRadix

inline core_api'toInt (s1:wstr, n2: int, n3: out int)

  pop  eax
  pop  ebx                          // get radix
  pop  esi                          // get str
  mov  ebx, [ebx]
  push eax
  mov  ecx, [esi-8]
  add  ecx, 2                       // to skip zero
Lab4:
  xor  eax, eax
Lab1:
  mov  edx, ebx
  mul  edx
  mov  edx, eax
  xor  eax, eax
  lodsw
  cmp  eax, 3Ah
  jb   short lab11
  sub  al, 7
lab11:
  sub  al, 30h
  jb   short Lab2
  cmp  ax, bx
  ja   short Lab2
  add  eax, edx
  add  ecx, 2
  jnz  short Lab1
Lab5:
  pop  ebx
  mov  [ebx], eax
  mov  eax, ebx
  jmp  short Lab3
Lab2:
  pop  ebx
  xor  eax, eax
Lab3:

end

// literalToReal

inline core_api'toReal (s1:wstr, r2: out real)

  pop   eax
  pop   esi
  push  eax
  push  edi
  sub   esp, 12
  xor   edx, edx
  xor   eax, eax
  xor   ebx, ebx
  mov   edi, esp
  stosd
  stosd
  mov   word ptr [edi], ax
  mov   ecx, 19

atof1:
  lodsw
  cmp   eax, 32                  // " "
  jz    short atof1
  or    eax, eax
  jnz   short atof2

atoflerr:
  add   esp, 12
  pop   edi
  xor   eax, eax
  jmp   atoflend

  //----------------------
  // check for leading sign
  //----------------------

atof2:

  cmp   eax, 43                  // +
  jz    short atof3
  cmp   eax,45                   // -
  jnz   short integer
  mov   dh,80h
atof3:
  mov   byte ptr [edi+1], dh    // put sign byte in bcd string
  xor   edx,edx
  lodsw

  //------------------------------------
  // convert the digits to packed decimal
  //------------------------------------
integer:

  cmp   eax, 46                  // .
  jnz   short atof4
  test  bh, 1
  jnz   short atoflerr           // only one decimal point allowed
  or    bh, 1
  lodsw
atof4:
  cmp   eax, 101                 // "e"
  jnz   short atof5 
  cmp   cl, 19
  jnz   short atof41
  test  bh, 4
  jz    short atoflerr
atof41:  
  jmp   scient
atof5:
  cmp   eax,69                  // "E" 
  jnz   short atof6
  cmp   cl, 19
  jnz   short atof51
  test  bh, 4
  jz    short atoflerr
atof51:  
  jmp   scient
atof6:
  or    eax,eax
  jnz   short atof7
  cmp   cl, 19
  jnz   atof61
  test  bh, 4
  jz    short atoflerr
atof61:
  jmp   laststep1
atof7:
  sub   eax,48                 // "0"
  jc    short atoflerr          // unacceptable character
  cmp   eax,9
  ja    short atoflerr          // unacceptable character
  or    bh,4                   // at least 1 numerical character
  test  bh,1
  jz    short atof8
  add   bl,1                   // bl holds number of decimal digits
  jc    atoflerr               // more than 255 decimal digits
atof8:
  test  eax, eax
  jnz   short atof9
  test  bh,2
  jnz   short atof9
  lodsw
  jmp   short integer
atof9:
  or    bh,2                   // at least 1 non-zero numerical character
  sub   ecx, 1
  jnz   short atof10
  test  bh,1                   // check if decimal point
  jz    atoflerr               // error if more than 18 integer digits in number
  test  eax, eax
  jnz   short atof91            // if trailing decimal 0
  add   ecx, 1
  sub   bl, 1
  lodsw
  jmp   integer
atof91:
  jmp   atoflerr
atof10:
  mov   dh,al
  
integer1:
  lodsw
  cmp   eax, 46                 // "."
  jnz   short atof20
  test  bh,1
  jnz   atoflerr               // only one decimal point allowed
  or    bh, 1                  // use bh bit0 as the decimal point flag
  lodsw
atof20:
  cmp   eax, 101                // "e"
  jnz   short atof30
  mov   ah, dh
  mov   al,0
  rol   al,4
  ror   ax,4
  mov   byte ptr [edi],al
  mov   dh, ah
  jmp   scient
atof30:
  cmp   eax, 69                 // "E"
  jnz   short atof40
  mov   ah, dh
  mov   al,0
  rol   al,4
  ror   ax,4
  mov   byte ptr [edi],al
  mov   dh, ah
  jmp   scient
atof40:  
  or    eax,eax
  jnz   short atof50
  mov   ah, dh
  rol   al,4
  ror   ax,4
  mov   byte ptr [edi],al
  mov   dh, ah
  jmp   short laststep1
atof50:
  sub   eax, 48               // "0"
  jc    atoflerr             // unacceptable character
  cmp   eax,9
  ja    atoflerr             // unacceptable character
  test  bh,1            
  jz    short atof60
  add   bl, 1                // processing decimal digits
atof60:
  sub   ecx, 1
  jnz   short atof70
  test  bh,1                // check if decimal point
  jz    atoflerr            // error if more than 18 integer digits in number
  test  eax, eax
  jnz   short atof602
  add   ecx, 1
  sub   bl, 1
  jmp   integer1
atof602:
  jmp   atoflerr
atof70:
  mov   ah, dh
  rol   al,4
  ror   ax,4
  mov   byte ptr [edi],al
  mov   dh, ah
  sub   edi, 1
  lodsw
  jmp   integer

laststep1:
  cmp   cl,19
  jnz   short laststep
  fldz
  jmp   short laststep2

laststep:
  mov   ah, dh
  xor   edx, edx
  fbld  [esp]
  sub   cl, 1
  add   bl,cl
  movzx eax,bl
  sub   edx,eax

  push  edx
  fild  dword ptr [esp]     // load the exponent
  fldl2t                    // load log2(10)
  fmulp                     // ->log2(10)*exponent
  pop   edx

  // at this point, only the log base 2 of the 10^exponent is on the FPU
  // the FPU can compute the antilog only with the mantissa
  // the characteristic of the logarithm must thus be removed
     
  fld   st(0)             // copy the logarithm
  frndint                 // keep only the characteristic
  fsub  st(1),st(0)       // keeps only the mantissa
  fxch st(1)              // get the mantissa on top

  f2xm1                   // ->2^(mantissa)-1
  fld1
  faddp                   // add 1 back

  // the number must now be readjusted for the characteristic of the logarithm

  fscale                  // scale it with the characteristic
      
  // the characteristic is still on the FPU and must be removed

  fstp  st(1)             // clean-up the register

  fmulp
  fstsw ax                // retrieve exception flags from FPU
  shr   al,1              // test for invalid operation
  jc    atoflerr          // clean-up and return error

laststep2:

  add   esp, 12
  pop   edi
  pop   eax
  fstp  qword ptr[eax]    // store result at specified address
  jmp   short atoflend

scient:
  cmp   cl,19
  jnz   short atof80
  fldz
  jmp   short laststep2
  xor   edx, edx

atof80:
  xor   eax,eax
  lodsw
  cmp   ax, 43            // "+"
  jz    atof90
  cmp   ax, 45            // "-"
  jnz   short scient1
  stc
  rcr   eax,1             // keep sign of exponent in most significant bit of EAX
     
atof90:

  lodsw                   // get next digit after sign

scient1:
  push  eax
  and   eax,0ffh
  jnz   short atof100      // continue if 1st byte of exponent is not terminating 0

scienterr:
  pop   eax
  jmp    atoflerr         // no exponent

atof100:
  sub   eax,30h
  jc    short scienterr    // unacceptable character
  cmp   eax,9
  ja    short scienterr    // unacceptable character
  imul  edx,10
  add   edx,eax
  cmp   edx,4931h
  ja    short scienterr    // exponent too large
  lodsw
  or    eax,eax
  jnz   short atof100
  pop   eax               // retrieve exponent sign flag
  rcl   eax,1             // is most significant bit set?
  jnc   short atof200
  neg   edx

atof200:
  jmp   laststep  

atoflend:

end
          
// literalToLong

inline core_api'toLong (s1: wstr, n2: out long)

  pop  eax
  pop  esi  
  mov  ecx, [esi-8]
  push eax
  push edi
  xor  ebx, ebx
  neg  ecx

  cmp  byte ptr [esi], 2Dh
  lea  ecx, [ecx-2]
  jnz  short labStart

  lea  esi, [esi+2]
  mov  ebx, 1        // set flag in ebx

labStart:
  push ebx           // save sign flag
  xor  edi, edi      // edi   - DHI
  xor  ebx, ebx      // ebx   - DLO

labConvert:
  mov  edx, 10
  mov  eax, edi
  mul  edx           // DHI * 10
  mov  edi, eax

  mov  eax, ebx
  mov  edx, 10
  mul  edx           // DLO * 10
  add  edi, edx
  mov  ebx, eax

  xor  eax, eax
  lodsw
  sub  al, 30h
  jb   short labErr
  cmp  al, 9
  ja   short labErr

  add ebx, eax       // DLO + EAX
  adc edi, 0         // DHI + CF

  sub  ecx, 2
  jnz  short labConvert

  pop  eax           // restore flag
  test eax, eax
  jz   short labSave

  not  edi           // invert number
  neg  ebx

labSave:

  mov  edx, edi
  pop  edi

  pop  eax
  mov  [eax], ebx
  mov  [eax+4], edx
  jmp  short labEnd

labErr:
  xor  eax, eax
  pop  ebx
  pop  edi

labEnd:

end

// ; d1 => s3

inline core_api'convert (d1:dump,l2:length,s3:out wstr)

  pop  eax
  mov  edx, eax
  pop  ebx
  mov  ecx, [ebx]
  pop  esi
labCopy:
  mov  ebx, [esi]
  mov  byte ptr [edx], bl
  lea  edx, [edx+1]
  xor  ebx, ebx
  mov  byte ptr [edx], bl
  lea  esi, [esi+1]
  lea  edx, [edx+1]
  sub  ecx, 1
  jnz  short labCopy
  mov  word ptr [edx], bx

end

// --- size management ---

// ; get the literal length
inline core_api'lengthOf (target:wstr, l2:out int)

  pop  eax  
  pop  ebx
  mov  ecx, [ebx-8] 
  neg  ecx
  shr  ecx, 1
  sub  ecx, 1
  mov  [eax], ecx

end

// ; get the object length
inline core_api'lengthOf (target:ref, l2:out int)

  pop  eax
  pop  esi
  call code : "$package'core'getcount"
  mov  [eax], ecx

end

// ; get the dump length
inline core_api'lengthOf (target:dump, l2:out int)

  pop  eax
  pop  ebx
  mov  ecx, [ebx-8] 
  neg  ecx
  mov  [eax], ecx

end

// --- memory operations

inline core_api'equal (p1:dump, p2:index, p3:int)

  mov  eax, const : system'false
  pop  edx
  mov  edx, [edx]
  pop  ebx
  mov  ebx, [ebx] 
  pop  esi
  add  esi, ebx
  cmp  [esi], edx
  jnz  short labEnd
  mov  eax, const : system'true

labEnd:

end

// write&dump&pref&wchar
inline core_api'read (p1:dump, p2:index, s3:out short)

  pop  edx
  pop  ebx
  mov  ebx, [ebx] 
  pop  esi
  mov  ecx, [esi+ebx]
  and  ecx, 0FFFFh
  mov  [edx], ecx

end

// write&dump&pref&wchar
inline core_api'write (p1:dump, p2:index, s3:short)

  pop  edx
  pop  ebx
  mov  ebx, [ebx] 
  pop  eax
  mov  edx, [edx]
  lea  esi, [eax+ebx]
  mov  word ptr [esi], dx

end

// write&dump&pref&int
inline core_api'read (p1:dump, p2:index, s3:out int)

  pop  eax
  pop  ebx
  mov  ebx, [ebx] 
  pop  esi
  mov  ecx, [esi+ebx]
  mov  [eax], ecx

end

// d2:[n3,n4] => d1
inline core_api'write (d1:out dump, n:index, l:length, d2:dump)

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
  mov  edx, [esi]
  mov  byte ptr [eax], dl
  lea  esi, [esi + 1]
  sub  ebx, 1
  lea  eax, [eax + 1]
  ja   short labNext1

  test ecx, ecx
  jz   short labSkip

labNext:
  mov  edx, [esi]
  mov  [eax], edx
  lea  esi, [esi + 4]
  sub  ecx, 4
  lea  eax, [eax + 4]
  ja   short labNext
  
labSkip:
  mov  eax, [esp+0Ch]

labEnd:
  lea  esp, [esp + 10h]

end

// d2:[n3,n4] => d1
inline core_api'write (d1:out dump, n:index, s:wstr)

  mov  ebx, [esp]
  mov  ecx, [ebx-8]
  mov  edx, [esp+4]
  neg  ecx
  mov  esi, [esp+08h]
  sub  ecx, 2
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

labNext:
  mov  edx, [esi]
  mov  word ptr [eax], dx
  lea  esi, [esi + 2]
  sub  ecx, 2
  lea  eax, [eax + 2]
  ja   short labNext
  
  mov  eax, [esp+08h]

labEnd:
  lea  esp, [esp + 0Ch]

end

// equal&dump&pref&short
inline core_api'equal (p1:dump, p2:index, p3:short)

  mov  eax, const : system'false
  pop  edx
  mov  edx, [edx]
  pop  ebx
  mov  ebx, [ebx] 
  pop  esi
  add  esi, ebx
  cmp  word ptr [esi], dx
  jnz  short labEnd
  mov  eax, const : system'true

labEnd:

end

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

// _trimLine (return true if trim was made or false)

inline core_api'_trimLine (p1: out wstr)

  mov  eax, const : system'false

  pop  esi
  mov  ecx, [esi-8]
  neg  ecx
  sub  ecx, 6
  mov  edx, [esi+ecx]
  cmp  edx, 0A000Dh
  jz   short labTrim
  add  ecx, 2
  mov  edx, [esi+ecx]
  cmp  dl, 10
  jnz  short labEnd
  mov  [esi+ecx], 0
  add  [esi-8], 2
  mov  eax, const : system'true
  jmp  short labEnd
labTrim:
  mov  [esi+ecx], 0
  add  [esi-8], 4
  mov  eax, const : system'true
labEnd:  

end

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

inline core_api'find (d1:dump,n2:index,l3:length,w4:byte,n5:out int)

  pop  eax
  pop  ebx
  mov  ebx, [ebx]
  pop  ecx
  mov  ecx, [ecx]
  pop  edx
  mov  esi, [esp]
  add  esi, [edx]
labNext:
  mov  edx, [esi]
  cmp  dl, bl
  jz   short labFound
  lea  esi, [esi+1]
  sub  ecx, 1
  ja   short labNext
  mov  esi, [esp]
  lea  esi, [esi-1]
labFound:
  pop  edx
  sub  esi, edx
  mov  [eax], esi
end

inline core_api'find (d1:dump,n2:index,l3:length,w4:short,n5:out int)

  pop  eax
  pop  ebx
  mov  ebx, [ebx]
  pop  ecx
  mov  ecx, [ecx]
  pop  edx
  mov  esi, [esp]
  add  esi, [edx]
labNext:
  mov  edx, [esi]
  cmp  dx, bx
  jz   short labFound
  lea  esi, [esi+2]
  sub  ecx, 2
  ja   short labNext
  mov  esi, [esp]
  lea  esi, [esi-1]
labFound:
  pop  edx
  sub  esi, edx
  mov  [eax], esi
end

inline core_api'find (s1:wstr,n2:index,s3:wstr,n4:out int)

  mov  esi, [esp+4]   // subs
  mov  ecx, [esp+8]   // offs

  push edi

  mov  edi, [esp+10h] // s
  push edi

  mov  ebx, [edi-8]   // get total length  
  push esi

  mov  edx, [ecx]
  neg  ebx
  shl  edx, 1
  sub  ebx, edx
  jbe  short labEnd

  add  ebx, 2
  sub  edx, 2

labNext:
  add  edx, 2
  mov  esi, [esp]
  mov  ecx, [esi-8]
  sub  ebx, 2
  lea  ecx, [ecx+2]
  jz   short labEnd
  neg  ecx
  cmp  ebx, ecx
  jb   short labEnd
  mov  edi, [esp+4]
  add  edi, edx

labCheck:
  mov  eax, [edi]
  cmp  ax, word ptr [esi]
  jnz  short labNext
  lea  edi, [edi+2]
  lea  esi, [esi+2]
  sub  ecx, 2
  jnz  short labCheck
  add  esp, 8
  pop  edi
  pop  eax
  lea  esp, [esp+12]
  shr  edx, 1
  mov  [eax], edx
  jmp  short labEnd2

labEnd:
  add  esp, 8
  pop  edi
  pop  eax
  lea  esp, [esp+12]
  mov  [eax], -1
labEnd2:

end

inline core_api'find (s1:wstr,n2:index,c3:short,n4:out int)

  pop  eax
  pop  ebx
  mov  ebx, [ebx]
  pop  edx
  mov  esi, [esp]
  mov  ecx, [esi-8]
  add  esi, [edx]
  neg  ecx
labNext:
  mov  edx, [esi]
  cmp  dx, bx
  jz   short labFound
  lea  esi, [esi+2]
  sub  ecx, 2
  ja   short labNext
  pop  edx
  mov  esi, -1
  jmp  short labSave
labFound:
  pop  edx
  sub  esi, edx
  shr  esi, 1
labSave:
  mov  [eax], esi
end

inline core_api'insert (s1:wstr,n2:index,s3:wstr,s4:out wstr)

  pop  edx              // retval
  pop  eax              // subs
  pop  ecx              // index
  pop  esi              // s
  mov  ecx, [ecx]       // index
  push edx
  push edi

  mov  edi, edx         // retval

  mov  edx, [esi-8]     // s
  neg  edx
  shl  ecx, 1
  lea  edx, [edx-2]
  cmp  edx, ecx
  jae  short lab1
  mov  ecx, edx
lab1:
  sub  edx, ecx

  test ecx, ecx
  lea  edx, [edx+2]    // to include zero

  jz   short lab2
labCopy:
  mov  ebx, [esi]
  mov  word ptr [edi], bx
  lea  esi, [esi+2]
  lea  edi, [edi+2]
  sub  ecx, 2
  jnz  short labCopy

lab2:
  mov  ecx, [eax-8]   // asubs
  lea  ecx, [ecx+2]
  neg  ecx
  test ecx, ecx
  jz   short lab3

labCopy2:
  mov  ebx, [eax]
  mov  word ptr [edi], bx
  lea  eax, [eax+2]
  lea  edi, [edi+2]
  sub  ecx, 2
  jnz  short labCopy2

lab3:
  test edx, edx
  jz   short lab4
  
labCopy3:
  mov  eax, [esi]
  mov  word ptr [edi], ax
  lea  esi, [esi+2]
  lea  edi, [edi+2]
  sub  edx, 2
  jnz  short labCopy3

lab4:
  pop  edi
  pop  eax

end

inline core_api'delete (s1:wstr,n2:index,n3:int,s4:out wstr)

  pop  ecx
  pop  eax              // length
  pop  ebx              // index
  pop  edx              // s

  push ecx
  push edi

  mov  edi, ecx         // retval

  mov  ecx, [ebx]       // get index
  shl  ecx, 1
  neg  ecx
  cmp  ecx, [edx-8]    // check if index withing string range
  jge  short lab1
  mov  ecx, [edx-8]    // set to the string end
  lea  ecx, [ecx+2]
lab1:
  neg  ecx
  mov  eax, [eax]    // length to delete
  shl  eax, 1
  mov  ebx, eax
  add  ebx, ecx      // check if deleting substring within string range
  neg  ebx
  cmp  ebx, [edx-8]
  jge  short lab2
  mov  eax, [edx-8]
  neg  eax
  sub  eax, ecx      // fix the length to delete

lab2:
  mov  ebx, [edx-8]  
  
  mov  esi, edx      // s
  neg  ebx
  
  mov  edx, eax      // ebx - total length, edx - length to delete, ecx - index
  sub  ebx, ecx      // length to move
  sub  ebx, edx

  test ecx, ecx
  jz   short lab3

labCopy1:
  mov  eax, [esi]
  mov  [edi], eax
  lea  esi, [esi+2]
  lea  edi, [edi+2]
  sub  ecx, 2
  jnz  short labCopy1

lab3:
  add  esi, edx
  test ebx, ebx
  jz   short lab4

labCopy2:
  mov  eax, [esi]
  mov  word ptr [edi], ax
  lea  esi, [esi+2]
  lea  edi, [edi+2]
  sub  ebx, 2
  jnz  short labCopy2

lab4:
  pop  edi
  pop  eax
  
end

inline core_api'substring (s1:wstr,n2:index,s3:out wstr)

  pop  esi
  pop  eax          // index
  pop  edx          // s  
  mov  ebx, [eax]
  push esi  
  shl  ebx, 1

  mov  ecx, [esi-8]
  add  edx, ebx
  test ecx, ecx
  jz   short lab2
  lea  ecx, [ecx+2]
lab1:
  mov  ebx, [edx]
  mov  word ptr[esi], bx
  lea  esi, [esi+2]
  lea  edx, [edx+2]
  add  ecx, 2
  jnz  short lab1
lab2:
  xor  ebx, ebx
  mov  word ptr [esi], bx
  pop  eax

end
