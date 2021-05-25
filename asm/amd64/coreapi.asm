// !! NOTE : R15 register must be preserved

define INIT_RND          10012h

// Object header fields
define elSizeOffset          0004h
define elVMTOffset           0010h 
define elObjectOffset        0010h

// ; --- API ---

// wsubcopyz(target,index,size,arr)
procedure coreapi'core_wsubcopyz

  mov  rax, [rsp+32]
  mov  rcx, [rsp+24]
  mov  rsi, [rsp+8]
  mov  rbx, [rsp+16]
  test ecx, ecx
  jz   short labEnd

labNext:
  mov  edx, dword ptr [rax + rbx*2]
  mov  word ptr [rsi], dx
  add  ebx, 1
  lea  rsi, [rsi + 2]
  sub  ecx, 1
  jnz  short labNext
  mov  word ptr [rsi], cx

labEnd:
  ret

end

// subcopyz(target,index,size,arr)
procedure coreapi'core_subcopyz

  mov  rax, [rsp+32]
  mov  rcx, [rsp+24]
  mov  rsi, [rsp+8]
  mov  rbx, [rsp+16]
  test ecx, ecx
  jz   short labEnd

labNext:
  mov  edx, dword ptr [rax + rbx]
  mov  byte ptr [rsi], dl
  add  rbx, 1
  add  rsi, 1
  sub  ecx, 1
  jnz  short labNext
  mov  byte ptr [rsi], cl

labEnd:
  ret

end

// ; insert(dest,sour,index,size)
procedure coreapi'core_insert

  mov  rcx, [rsp+32]
  mov  rdi, [rsp+8]
  mov  rbx, [rsp+24]
  mov  rsi, [rsp+16]
  test ecx, ecx
  jz   short labEnd

labNext:
  mov  edx, dword ptr [rsi]
  mov  byte ptr [rdi + rbx], dl
  add  ebx, 1
  lea  rsi, [rsi + 1]
  sub  ecx, 1
  jnz  short labNext

labEnd:
  ret

end

// winsert(target,source,index,len)
procedure coreapi'core_winsert

  mov  rbx, [rsp+24]
  mov  rcx, [rsp+32]
  mov  rdi, [rsp+8]
  mov  rsi, [rsp+16]
  test ecx, ecx
  jz   short labEnd

labNext:
  mov  edx, dword ptr [rsi]
  mov  word ptr [rdi + rbx*2], dx
  add  ebx, 1
  lea  rsi, [rsi + 2]
  sub  ecx, 1
  jnz  short labNext

labEnd:
  mov  edx, ecx
  mov  ebx, edi
  ret

end

// ; sadd(dest,sour,sindex,dindex)
procedure coreapi'core_sadd

  mov  rcx, [rsp+24]
  mov  rax, [rsp+16]
  mov  rdx, [rsp+32]
  mov  rdi, [rsp+8]

  mov  esi, ecx         // ; src index
  
  mov  ebx, dword ptr[rax-elSizeOffset]
  and  ebx, 0FFFFFh
  add  edx, edi
  sub  ecx, ebx
  add  esi, eax
  
labNext2:
  mov  ebx, dword ptr[rsi]
  mov  byte ptr [rdx], bl
  lea  rsi, [rsi+1]
  lea  rdx, [rdx+1]
  add  ecx, 1
  jnz  short labNext2

  ret
  
end

// ; wadd(dest,sour,sindex,dindex)
procedure coreapi'core_wadd

  mov  rcx, [rsp+24]              // 
  mov  rax, [rsp+16]              // ; src
  mov  rbx, [rsp+32]              // ; dindex
  mov  rdi, [rsp+8]               // ; dst
  
  shl  ebx, 1
  shl  ecx, 1

  mov  edx, ebx         // ; dst index
  mov  esi, ecx         // ; src index
  
  mov  ebx, dword ptr [rax-elSizeOffset]
  and  ebx, 0FFFFFh

  add  rdx, rdi
  sub  rcx, rbx
  add  rsi, rax
  
labNext2:
  mov  ebx, dword ptr [rsi]
  mov  word ptr [rdx], bx
  lea  rsi, [rsi+2]
  lea  rdx, [rdx+2]
  add  ecx, 2
  jnz  short labNext2

  ret
  
end

// sseek(s,subs,index,len)
procedure coreapi'core_sseek

  mov  rdi, [rsp+8] // s
  mov  rdx, [rsp+24]
  mov  rsi, [rsp+16] // subs
  
  mov  ebx, dword ptr[rdi-elSizeOffset]   // get total length  
  and  ebx, 0FFFFFh
  
  sub  ebx, edx
  jbe  short labEnd

  add  ebx, 1
  sub  edx, 1

labNext:
  add  edx, 1
  mov  ecx, dword ptr [rsp+32]
  sub  ebx, 1
  jz   short labEnd
  cmp  ebx, ecx
  jb   short labEnd
  mov  rdi, [rsp+8]
  add  rdi, rdx

labCheck:
  mov  eax, dword ptr [rdi]
  cmp  al, byte ptr [rsi]
  jnz  short labNext
  lea  rdi, [rdi+1]
  lea  rsi, [rsi+1]
  sub  ecx, 1
  jnz  short labCheck
  nop
  nop
  jmp  short labEnd2

labEnd:
  mov  edx, -1
labEnd2:
  ret

end

// wseek(s,subs,index,len)
procedure coreapi'core_wseek

  mov  rdi, [rsp+8] // s
  mov  rdx, [rsp+24]
  mov  rsi, [rsp+16] // subs
  
  mov  ebx, dword ptr [rdi-elSizeOffset]   // get total length  
  and  ebx, 0FFFFFh

  shl  edx, 1
  sub  ebx, edx
  jbe  short labEnd

  add  ebx, 2
  sub  edx, 2

labNext:
  add  edx, 2
  mov  ecx, dword ptr [rsp+32]
  sub  ebx, 2
  jz   short labEnd
  shl  ecx, 1
  cmp  ebx, ecx
  jb   short labEnd
  mov  rdi, [rsp+8]
  add  rdi, rdx

labCheck:
  mov  eax, dword ptr [rdi]
  cmp  ax, word ptr [rsi]
  jnz  short labNext
  lea  rdi, [rdi+2]
  lea  rsi, [rsi+2]
  sub  ecx, 2
  jnz  short labCheck
  shr  edx, 1
  jmp  short labEnd2

labEnd:
  mov  edx, -1
labEnd2:
  ret

end

// ; strtochar(index,str; ebx = 0 if err ; edx - out)
procedure coreapi'core_strtochar   

  mov  rbx, [rsp+8]
  mov  rax, [rsp+16]

  xor  edx, edx
  mov  dl, byte ptr [rax + rbx]
  cmp  edx, 00000080h
  jl   short lab1
  cmp  edx, 000000C0h
  jl   short err
  cmp  edx, 000000E0h
  jl   short lab2
  cmp  edx, 000000F0h
  jl   short lab3
  cmp  edx, 000000F8h
  jl   lab4

err:
  xor  ebx, ebx
  ret 

lab1:
  mov  ebx, eax
  ret

lab2:  
  mov  ecx, edx
  mov  dl, byte ptr [rax + rbx + 1]
  mov  esi, edx
  and  esi, 0C0h
  cmp  esi, 00000080h
  jnz  err
  shl  ecx, 6
  add  edx, ecx
  sub  edx, 3080h
  mov  rbx, rax
  ret
  
lab3:
  mov  ecx, edx
  mov  dl, byte ptr [rax + rbx + 1]
  mov  esi, edx
  and  esi, 0C0h
  cmp  esi, 00000080h
  jnz  err
  cmp  ecx, 000000E0h
  jnz  short lab3_1
  cmp  ebx, 000000A0h
  jl   short err

lab3_1:
  shl  ecx, 12
  shl  edx, 6
  add  ecx, edx
  xor  edx, edx
  mov  dl, byte ptr [rax + rbx + 2]
  mov  esi, edx
  and  esi, 0C0h
  cmp  esi, 00000080h
  jnz  err
  add  edx, ecx
  sub  edx, 0E2080h
  mov  ebx, eax
  ret
  
lab4:
  mov  ecx, edx
  mov  dl, byte ptr [rax + rbx + 1]
  mov  esi, edx
  and  esi, 0C0h
  cmp  esi, 00000080h
  jnz  err
  cmp  ecx, 000000F0h
  jnz  short lab4_1
  cmp  edx, 00000090h
  jl   short err

lab4_1:
  cmp  ecx, 000000F4h
  jnz  short lab4_2
  cmp  edx, 00000090h
  jae  short err

lab4_2:
  shl  ecx, 18
  shl  edx, 12
  add  ecx, edx

  xor  edx, edx
  mov  dl, byte ptr [rax + rbx + 2]
  mov  esi, edx
  and  esi, 000000C0h
  cmp  esi, 00000080h
  jnz  err

  shl  edx, 6
  add  ecx, edx
  
  xor  edx, edx
  mov  dl, byte ptr [rax + rbx + 3]
  mov  esi, edx
  and  esi, 000000C0h
  cmp  esi, 00000080h
  jnz  err

  add  edx, ecx
  sub  edx, 3C82080h
  mov  ebx, eax
  ret
  
end                                                       

// ; wstrtochar(index,str; ebx = 0 if err ; edx - out)
procedure coreapi'core_wstrtochar

  mov  rbx, [rsp+8]
  mov  rax, [rsp+16]

  mov  esi, dword ptr [rax + rbx * 2]
  and  esi, 0FFFFh
  cmp  esi, 0D800h
  jl   short lab1
  cmp  esi, 0DBFFh
  jg   short err

  mov  ecx, esi
  shl  ecx, 10
  mov  esi, dword ptr [rax + rbx * 2 + 2]
  and  esi, 0FFFFh
  cmp  esi, 0DC00h
  jl   short lab2
  cmp  esi, 0DFFFh
  jg   short err
  
lab2:
  mov  edx, ecx
  mov  rbx, [rsp+8]
  add  edx, ebx
  sub  edx, 35FDC00h
  ret  

lab1:
  mov  rbx, [rsp+16]
  mov  edx, esi
  ret

err:
  xor  ebx, ebx
  ret 

end

// ; slen_ch - ecx - len, eax - charr, esi - result 
procedure coreapi'core_slen_ch

   mov  rax, [rsp+24]
   mov  rcx, [rsp+16]
   mov  rdx, [rsp+8]
   mov  rdi, [rsp+32]
   lea  rax, [rax+rdx*4]
               
   xor  ebx, ebx
   test ecx, ecx
   jz   short labEnd

labNext:
   mov  edx, dword ptr[rax]
   cmp  edx, 00000080h
   jl   short lab1
   cmp  edx, 0800h
   jl   short lab2
   cmp  edx, 10000h
   jl   short lab3
   
   add  ebx, 4
   lea  rax, [rax + 4]
   sub  ecx, 1
   jnz  short labNext
labEnd:
   mov  dword ptr[rdi], ebx
   ret
   
lab1:
   add  ebx, 1
   lea  rax, [rax + 4]
   sub  ecx, 1
   jnz  short labNext
   mov  dword ptr [rdi], ebx
   ret

lab2:
   add  ebx, 2
   lea  rax, [rax + 4]
   sub  ecx, 1
   jnz  short labNext
   mov  dword ptr[rdi], ebx
   ret

lab3:
   add  ebx, 3
   lea  rax, [rax + 4]
   sub  ecx, 1
   jnz  short labNext
   mov  dword ptr[rdi], ebx
   ret

end

// ; wslen_ch - ecx - len, eax - charr, esi - result 
procedure coreapi'core_wslen_ch

   mov  rax, [rsp+24]
   mov  rcx, [rsp+16]
   mov  rdx, [rsp+8]
   mov  rdi, [rsp+32]
   lea  rax, [rax+rdx*4]
               
   xor  ebx, ebx
   test ecx, ecx
   jz   short labEnd

labNext:
   mov  edx, dword ptr [rax]
   cmp  edx, 010000h
   jl   short lab1

   add  ebx, 2
   lea  rax, [rax + 4]
   sub  ecx, 1
   jnz  short labNext
labEnd:
   mov  dword ptr[rdi], ebx
   ret   
   
lab1:
   add  ebx, 1
   lea  rax, [rax + 4]
   sub  ecx, 1
   jnz  short labNext
   mov  dword ptr[rdi], ebx
   ret   

end

procedure coreapi'core_scopychars

  mov  rax, [rsp+32]
  mov  rcx, [rsp+24]
  mov  rbx, [rsp+16]
  mov  rdi, [rsp+8]

  test ecx, ecx
  jz   labEnd

  lea  rsi, [rax + rbx * 4]

labNext:
  mov  ebx, dword ptr [rsi]
  
  cmp  ebx, 00000080h
  jl   short labs1
  cmp  ebx, 0800h
  jl   short labs2
  cmp  ebx, 10000h
  jl   short labs3
  
  mov  edx, ebx
  shr  edx, 18
  add  edx, 000000F0h 
  mov  byte ptr [rdi], dl
  add  rdi, 1
   
  mov  edx, ebx
  shr  edx, 12
  and  edx, 0000003Fh
  add  edx, 00000080h
  mov  byte ptr [rdi], dl
  add  rdi, 1
   
  mov  edx, ebx
  shr  edx, 6
  and  edx, 0000003Fh
  add  edx, 00000080h
  mov  byte ptr [rdi], dl
  add  rdi, 1
   
  mov  edx, ebx
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [rdi], dl
  add  rdi, 1
  jmp  labSave

labs2:
  mov  edx, ebx
  shr  edx, 6
  add  edx, 000000C0h
  mov  byte ptr [rdi], dl
  add  rdi, 1
  
  mov  edx, ebx
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [rdi], dl
  add  rdi, 1
  jmp  short labSave

labs3:
  mov  edx, ebx
  shr  edx, 12
  add  edx, 000000E0h
  mov  byte ptr [rdi], dl
  add  rdi, 1

  mov  edx, ebx
  shr  edx, 6
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [rdi], dl
  add  rdi, 1
  
  mov  edx, ebx
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [rdi], dl
  add  rdi, 1
  jmp  short labSave
  
labs1:
  mov  byte ptr [rdi], bl
  add  rdi, 1

labSave:
  lea  rsi, [rsi + 4]
  sub  ecx, 1
  jnz  labNext

labEnd:
  mov  rdx,  rdi
  mov  rdi, [rsp+18]
  sub  rdx, rdi

  ret

end

procedure coreapi'core_wscopychars

  mov  rax, [rsp+32]
  mov  rcx, [rsp+24]
  mov  rbx, [rsp+16]
  mov  rdi, [rsp+8]

  test ecx, ecx
  jz   labEnd

  lea  rsi, [rax + rbx * 4]

labNext:
  mov  ebx, dword ptr [rsi]
  cmp  ebx, 010000h
  jl   short lab1

  mov  edx, ebx
  shr  edx, 10
  add  edx, 0D7C0h
  mov  word ptr [rdi], dx
  add  edi, 2

  and  ebx, 03FFh
  add  ebx, 0DC00h
   
lab1:
  mov  word ptr [rdi], bx
  add  edi, 2
  lea  rsi, [rsi + 4]
  sub  ecx, 1
  jnz  short labNext

labEnd:
  mov  rbx, rdi
  pop  rdi
  sub  rbx, rdi
  shr  ebx, 1

  ret

end

// ; inttostr(s,b,t,out)
procedure coreapi'core_inttostr

   mov  rsi, [rsp+16]
   mov  rax, [rsp+8]
   mov  rdi, [rsp+24]

   push rbp
   mov  rbp, rsp
   xor  ecx, ecx
   push rax
   // ; take sign into account only for the decimal representation
   cmp  esi, 10        
   jnz  short Lab6
   cmp  eax, 0
   jns  short Lab6
   neg  eax
Lab6:
   cmp  eax, esi
   jb   short Lab5
Lab1:
   xor  edx, edx
   idiv esi
   push rdx
   add  ecx, 1
   cmp  eax, esi
   jae  short Lab1
Lab5:   
   add  ecx, 2
   push rax
   cmp  esi, 10        
   jnz  short Lab7
   mov  rax, [rbp-8]
   cmp  eax, 0
   jns  short Lab7
   push 0F6h      // to get "-" after adding 0x30
   add  ecx, 1
Lab7:
   sub  ecx, 1
   mov  esi, edi
   mov  edx, 0FFh
Lab2:
   pop  rax
   cmp  eax, 0Ah
   jb   short Lab8
   add  eax, 7
Lab8:
   add  eax, 30h
   and  eax, edx
   mov  byte ptr [rsi], al
   add  esi, 1
   sub  ecx, 1
   jnz  short Lab2
   mov  edx, esi
   sub  edx, edi
   lea  rsp, [rsp+8]
   pop  rbp
   mov  rdi, [rsp+32]
   mov  dword ptr [rdi], edx

   ret

end

procedure coreapi'core_inttowstr

   mov  rsi, [rsp+16]
   mov  rax, [rsp+8]
   mov  rdi, [rsp+24]

   push rbp
   mov  rbp, rsp
   xor  ecx, ecx
   push rax
   cmp  eax, 0
   jns  short Lab6
   neg  eax
Lab6:
   cmp  eax, ebx
   jb   short Lab5
Lab1:
   xor  edx, edx
   idiv ebx
   push rdx
   add  ecx, 2
   cmp  eax, ebx
   jae  short Lab1
Lab5:   
   add  ecx, 4
   push rax
   mov  eax, dword ptr[rbp-8]
   cmp  eax, 0
   jns  short Lab7
   push 0F6h      // to get "-" after adding 0x30
   add  ecx, 2
Lab7:
   mov  esi, edi
   mov  edx, 0FFh
   sub  ecx, 2             // to skip zero
Lab2:
   pop  rax
   cmp  eax, 0Ah
   jb   short Lab8
   add  eax, 7
Lab8:
   add  eax, 30h
   and  eax, edx
   mov  word ptr [rsi], ax
   add  esi, 2
   sub  ecx, 2
   jnz  short Lab2
   mov  ecx, esi
   sub  ecx, edi
   shr  ecx, 1
   lea  rsp, [rsp+8]
   pop  rbp

   ret
   
end

// ; uinttostr(s,b,t)
procedure coreapi'core_uinttostr

   mov  rsi, [rsp+16]
   mov  rax, [rsp+8]
   mov  rdi, [rsp+24]

   push rbp
   mov  rbp, rsp
   xor  ecx, ecx

   cmp  eax, esi
   jb   short Lab5
Lab1:
   xor  edx, edx
   idiv esi
   push rdx
   add  ecx, 1
   cmp  eax, esi
   jae  short Lab1
Lab5:   
   add  ecx, 2
   push rax
   sub  ecx, 1
   mov  esi, edi
   mov  edx, 0FFh
Lab2:
   pop  rax
   cmp  eax, 0Ah
   jb   short Lab8
   add  eax, 7
Lab8:
   add  eax, 30h
   and  eax, edx
   mov  byte ptr [rsi], al
   add  rsi, 1
   sub  ecx, 1
   jnz  short Lab2
   mov  rdx, rsi
   sub  rdx, rdi
   pop  rbp

   ret

end

procedure coreapi'core_uinttowstr

   mov  rsi, [rsp+16]
   mov  rax, [rsp+8]
   mov  rdi, [rsp+24]

   push rbp
   mov  rbp, rsp
   xor  ecx, ecx
   cmp  eax, ebx
   jb   short Lab5
Lab1:
   xor  edx, edx
   idiv ebx
   push rdx
   add  ecx, 2
   cmp  eax, ebx
   jae  short Lab1
Lab5:   
   add  ecx, 4
   push rax
   mov  esi, edi
   mov  edx, 0FFh
   sub  ecx, 2             // to skip zero
Lab2:
   pop  rax
   cmp  eax, 0Ah
   jb   short Lab8
   add  eax, 7
Lab8:
   add  eax, 30h
   and  eax, edx
   mov  word ptr [rsi], ax
   add  esi, 2
   sub  ecx, 2
   jnz  short Lab2
   mov  ecx, esi
   sub  ecx, edi
   shr  ecx, 1
   pop  rbp

   ret
   
end

// --- System Core API  --

define CORE_ET_TABLE     2000Bh

// ; initProcess(frameHeader)
procedure coreapi'initProcess

  finit
  lea  rdx, [rsp+10h]
  mov  rax, [rsp+8]
  mov  [rax+8], rdx
  mov  [rax+10h], rbp
  ret

end

// ; initThread(frameHeader)
procedure coreapi'initThread

  lea  rdx, [rsp+10h]
  mov  rax, [rsp+8]
  mov  [rax+8], rdx
  mov  [rax+10h], rbp
  ret

end

//procedure coreapi'seh_handler
//
//  push rbp
//  mov  rbp, rsp
//  // ;** now [EBP+8]=pointer to EXCEPTION_RECORD
//  // ;** [EBP+0Ch]=pointer to ERR structure
//  // ;** [EBP+10h]=pointer to CONTEXT record
//  push rbx
//  push rdi
//  push rsi
//  mov  rbx, [rbp+16]
//  test rbx, rbx
//  jz   short lab5
//  test qword ptr[rbx+8],1h
//  jnz  lab5
//  test qword ptr[rbx+8],2h
//  jz   lab2
// //; ...
// //; ...
// //; ...
//  jmp lab5
//lab2:
// //; PUSH 0
// //; PUSH [EBP+8h]
// //; PUSH ADDR UN23
// //; PUSH [EBP+0Ch]
// //; CALL RtlUnwind
// //; UN23:
//  mov rsi, [rbp + 20h]
// //; MOV EDX,[EBP+0Ch]
////; MOV [ESI+0C4h],EDX   // ; esp
//
//  // ; get critical exception handler
//  mov  rax, [data : % CORE_ET_TABLE]
//  mov  [rsi+0B8h], rax    // ; eip
//
// //; MOV EAX,[EDX+14h]
// //; MOV [ESI+0B4h],EAX   // ; ebp
//  xor rax, rax
//  jmp short lab6
//lab5:
//  mov rax, 1
//lab6:
//  pop rsi
//  pop rdi
//  pop rbx 
//  mov rsp, rbp
//  pop rbp
//  ret
//end

procedure coreapi'veh_handler

  push rdx
  push rbp
  mov  rbp, rsp
  mov  rdx, rax   // ; set exception code
  mov  rax, [data : % CORE_ET_TABLE]
  jmp  rax

end

procedure coreapi'default_handler                                                       

  // ; exit code
  push 0
  call extern 'rt_dlls.Exit

end

// ; === internal ===

// strtowstr(target,source)
procedure coreapi'strtowstr

  mov  rax, [rsp+16]
  mov  rdi, [rsp+8]
  mov  esi, dword ptr [rax - elSizeOffset]
  and  esi, 0FFFFFh
  sub  esi, 1
  jz   labEnd

labNext:  

  xor  ebx, ebx
  mov  bl, byte ptr [rax]
  cmp  ebx, 00000080h
  jl   short lab1
  cmp  ebx, 000000E0h
  jl   short lab2
  cmp  ebx, 000000F0h
  jl   short lab3

  mov  ecx, ebx
  shl  ecx, 18
  
  lea  rax, [rax + 1]
  sub  esi, 1  
  mov  bl, byte ptr [rax]
  shl  ebx, 12
  add  ecx, ebx

  xor  ebx, ebx
  lea  rax, [rax + 1]
  sub  esi, 1  
  mov  bl, byte ptr [rax]
  shl  ebx, 6
  add  ecx, ebx

  xor  ebx, ebx
  lea  rax, [rax + 1]
  sub  esi, 1  
  mov  bl, byte ptr [rax]
  add  ecx, ebx
  sub  ecx, 3C82080h
  jmp  short labCont

lab2:  
  mov  ecx, ebx
  shl  ecx, 6

  lea  rax, [rax + 1]
  sub  esi, 1  
  mov  bl, byte ptr [rax]
  add  ecx, ebx
  sub  ecx, 3080h
  jmp  short labCont
  
lab3:
  mov  ecx, ebx
  shl  ecx, 12

  lea  rax, [rax + 1]
  sub  esi, 1  
  mov  bl, byte ptr [rax]
  shl  ebx, 6
  add  ecx, ebx

  xor  ebx, ebx
  lea  rax, [rax + 1]
  sub  esi, 1  
  mov  bl, byte ptr [rax]
  add  ecx, ebx
  sub  ecx, 0E2080h
  jmp  short labCont

lab1:
  mov  ecx, ebx

labCont:
  cmp  ecx, 010000h
  jl   short labw1

  mov  edx, ecx
  shr  edx, 10
  add  edx, 0D7C0h
  mov  word ptr [rdi], dx
  add  edi, 2
  
  and  ecx, 03FFh
  add  ecx, 0DC00h
   
labw1:
  mov  word ptr [rdi], ecx
  add  edi, 2  

  lea  rax, [rax + 1]
  sub  esi, 1
  jnz  labNext
labEnd:
  mov  rbx, [rsp+8]
  mov  edx, edi
  sub  edx, ebx
  shr  edx, 1
  ret
    
end

// strtowstr(target,source)
procedure coreapi'wstrtostr

  mov  rax, [rsp+16]
  mov  rdi, [rsp+8]
  mov  esi, dword ptr [rax - elSizeOffset]
  and  esi, 0FFFFFh
  sub  esi, 2
  jz   labEnd

labNext:  
  mov  ebx, dword ptr [rax]
  and  ebx, 0FFFFh
  cmp  ebx, 0D800h
  jb   short lab1

  mov  ecx, ebx
  shl  ecx, 10
  lea  rax, [rax + 2]
  sub  esi, 2
  mov  ebx, dword ptr [rax]
  and  ebx, 0FFFFh
  add  ebx, ecx
  sub  ebx, 35FDC00h

lab1:
  mov  ecx, ebx

  cmp  ebx, 00000080h
  jl   short labs1
  cmp  ebx, 0800h
  jl   short labs2
  cmp  ebx, 10000h
  jl   short labs3
  
  mov  edx, ebx
  shr  edx, 18
  add  edx, 0F0h 
  mov  byte ptr [rdi], dl
  add  edi, 1
   
  mov  edx, ecx
  shr  edx, 12
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [rdi], dl
  add  edi, 1
   
  mov  edx, ecx
  shr  edx, 6
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [rdi], dl
  add  edi, 1
   
  mov  edx, ecx
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [rdi], dl
  add  edi, 1
  jmp  short labSave

labs2:
  mov  edx, ecx
  shr  edx, 6
  add  edx, 0C0h
  mov  byte ptr [rdi], dl
  add  edi, 1
  
  mov  edx, ecx
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [rdi], dl
  add  edi, 1
  jmp  short labSave

labs3:
  mov  edx, ecx
  shr  edx, 12
  add  edx, 0E0h
  mov  byte ptr [rdi], dl
  add  edi, 1

  mov  edx, ecx
  shr  edx, 6
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [rdi], dl
  add  edi, 1
  
  mov  edx, ecx
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [rdi], dl
  add  edi, 1
  jmp  short labSave
  
labs1:
  mov  byte ptr [rdi], bl
  add  edi, 1
  
labSave:  
  lea  rax, [rax + 2]
  sub  esi, 2
  jnz  labNext

labEnd:
  mov  rbx, [rsp+8]
  mov  rdx, rdi
  sub  rdx, rbx
  ret

end

procedure coreapi'wequal

  mov  rdx, [rsp+16]                 // ; s1
  mov  rsi, [rsp+8]                 // ; s2

  mov  ecx, dword ptr [rdx-elSizeOffset]          // s1.length
  mov  ebx, dword ptr [rsi-elSizeOffset]
  and  ecx, 0FFFFCh 
  and  ebx, 0FFFFCh 
  mov  eax, 0
  cmp  ecx, ebx              // compare with s2.length
  jnz  short Lab1
Lab2:
  mov  ebx, dword ptr [rsi]
  cmp  ebx,  dword ptr [rdx]
  jnz  short Lab1
  lea  rsi, [rsi+4]
  lea  rdx, [rdx+4]
  sub  ecx, 4
  jnz  short Lab2
  mov  eax, 1
Lab1:
  mov  edx, eax
  ret

end

procedure coreapi'wless

  mov  rsi, [rsp+16]                 // ; s1
  mov  rdx, [rsp+8]                 // ; s2

  mov  ecx, dword ptr [rdx-elSizeOffset]          // s1 length
  mov  eax, 0
  and  ecx, 0FFFFFh
Lab2:
  mov  ebx, dword ptr [rdx]              // s1[i] 
  cmp  bx, word ptr [rsi]      // compare s2[i] with 
  jb   short Lab1
  nop
  nop
  ja   short LabEnd
  lea  rsi, [rsi+2]
  lea  rdx, [rdx+2]
  sub  ecx, 2
  jnz  short Lab2
  nop
  nop
  jmp  short LabEnd
  
Lab1:
  mov  eax, 1

LabEnd:
  mov  edx, eax
  ret

end

// tempObject(ptr), eax - vmt, edx - value
procedure coreapi'tempObject

  mov  rax, [rsp+8]
  mov  [rax-elVMTOffset], rbx
  mov  qword ptr [rax], rdx
  mov  dword ptr [rax-elSizeOffset], 40000008h
  ret

end

// ; chartowstr (char, target, out edx - length)
procedure coreapi'chartowstr

   mov  rax, [rsp+8]
   mov  rdi, [rsp+16]
   xor  ecx, ecx
   mov  dword ptr [rdi], ecx
   mov  dword ptr [rdi+4], ecx

   mov  ebx, dword ptr [rax]
   cmp  ebx, 010000h
   jl   short lab1

   mov  edx, ebx
   shr  edx, 10
   add  edx, 0D7C0h
   mov  word ptr [rdi], dx

   mov  edx, ebx
   and  edx, 03FFh
   add  edx, 0DC00h
   mov  word ptr [rdi+2], dx
   mov edx, 2
   ret
   
lab1:
   mov  dword ptr [rdi], ebx
   mov  edx, 1
   ret

end

procedure coreapi'wstrtochararray

  mov  rdx, [rsp+32]
  mov  rdi, [rsp+24]
  mov  ecx, dword ptr [rdx]
  mov  rsi, [rsp+16]
  mov  rax, [rsp+8]
  mov  ebx, dword ptr [rsi]

  lea  rdi, [rdi + rbx * 4]

labStart:
  mov  ebx, dword ptr [rax]
  add  rax, 2

  and  ebx, 0FFFFh
  cmp  ebx, 0D800h
  jl   short lab1

  shl  ebx, 10
  mov  edx, dword ptr [rax]
  add  eax, 2
  and  edx, 0FFFFh
  add  ebx, edx
  sub  ebx, 35FDC00h

lab1:
  mov   edx, ebx

labSave:
  mov  dword ptr [rdi], edx
  add  rdi, 4
  sub  ecx, 1
  jnz  labStart

  mov  rcx, rdi
  mov  rdi, [rsp+24]
  sub  rcx, rdi
  mov  rsi, [rsp+16]
  shr  ecx, 2
  sub  ecx, dword ptr [rsi]
  mov  rax, [rsp+32]
  mov  dword ptr [rax], ecx

  ret

end


// wsubcopy(target,index,size,arr)
procedure coreapi'wsubcopyto

  mov  rax, [rsp+32]
  mov  rdx, [rsp+24]
  mov  rsi, [rsp+8]
  mov  ecx, dword ptr [rdx]
  mov  rdi, [rsp+16]
  test ecx, ecx
  mov  ebx, dword ptr [rdi]
  jz   short labEnd

labNext:
  mov  edx, dword ptr [rax]
  mov  word ptr [rsi + rbx*2], dx
  add  rbx, 1
  add  rax, 2
  sub  ecx, 1
  jnz  short labNext

labEnd:
  ret

end

procedure coreapi'sless

  mov  rsi, [rsp+16]                 // ; s1
  mov  rdx, [rsp+8]                 // ; s2

  mov  ecx, dword ptr [rdx-elSizeOffset]          // s1 length
  mov  eax, 0
  and  ecx, 0FFFFFh
Lab2:
  mov  ebx, dword ptr [rdx]              // s1[i] 
  cmp  bl, byte ptr [rsi]      // compare s2[i] with 
  jb   short Lab1
  nop 
  nop
  ja   short LabEnd
  lea  rsi, [rsi+1]
  lea  rdx, [rdx+1]
  sub  ecx, 1
  jnz  short Lab2
  nop
  nop
  jmp  short LabEnd

Lab1:
  mov  eax, 1

LabEnd:
  mov  edx, eax
  ret

end

procedure coreapi'sequal

  mov  rdx, [rsp+16]                 // ; s1
  mov  rsi, [rsp+8]                 // ; s2

  mov  ecx, dword ptr [rdx-elSizeOffset]          // s1.length
  mov  ebx, dword ptr [rsi-elSizeOffset]
  and  ecx, 0FFFFFh 
  and  ebx, 0FFFFFh 
  mov  eax, 0
  cmp  ecx, ebx              // compare with s2.length
  jnz  short Lab1
Lab2:
  mov  ebx, dword ptr [rsi]
  cmp  bl,  byte ptr [rdx]
  jnz  short Lab1
  lea  rsi, [rsi+1]
  lea  rdx, [rdx+1]
  sub  ecx, 1
  jnz  short Lab2
  mov  eax, 1
Lab1:
  mov  edx, eax
  ret

end

// strtochararray(src,index,dst,len)
procedure coreapi'strtochararray

  mov  rdx, [rsp+32]
  mov  rdi, [rsp+24]
  mov  ecx, dword ptr [rdx]
  mov  rsi, [rsp+16]
  mov  rax, [rsp+8]
  mov  ebx, dword ptr[rsi]

  lea  rdi, [rdi + rbx * 4]

labStart:
  xor  ebx, ebx
  mov  bl, byte ptr [rax]
  add  rax, 1
  cmp  ebx, 00000080h
  jl   short lab1
  cmp  ebx, 000000E0h
  jl   short lab2
  cmp  ebx, 000000F0h
  jl   short lab3
  
lab4:
  mov  edx, ebx
  mov  bl, byte ptr [rax]
  add  rax, 1
  shl  edx, 18
  shl  ebx, 12
  add  edx, ebx

  xor  ebx, ebx
  mov  bl, byte ptr [rax]
  add  rax, 1
  shl  ebx, 6
  add  edx, ebx
  
  xor  ebx, ebx
  mov  bl, byte ptr [rax]
  add  rax, 1
  
  add  edx, ebx
  sub  edx, 3C82080h
  sub  ecx, 3
  jmp  labSave  

lab2:  
  mov  edx, ebx
  mov  bl, byte ptr [rax]
  add  rax, 1

  shl  edx, 6
  add  edx, ebx
  sub  edx, 3080h
  sub  ecx, 1
  jmp  short labSave  
  
lab3:
  mov  edx, ebx
  mov  bl, byte ptr [rax]
  add  rax, 1
  shl  edx, 12
  shl  ebx, 6
  add  edx, ebx
  xor  ebx, ebx
  mov  bl, byte ptr [rax]
  add  rax, 1
  add  edx, ebx
  sub  edx, 0E2080h
  sub  ecx, 2
  jmp  short labSave  
  
lab1:
  mov  edx, ebx

labSave:
  mov  dword ptr [rdi], edx
  add  rdi, 4
  sub  ecx, 1
  jnz  labStart

  mov  rcx, rdi
  mov  rdi, [rsp+24]
  sub  rcx, rdi
  mov  rsi, [rsp+16]
  shr  ecx, 2
  sub  ecx, dword ptr [rsi]
  mov  rax, [rsp+32]
  mov  dword ptr [rax], ecx

  ret

end

// ; chartostr (char,target, out edx - length)
procedure coreapi'chartostr

   mov  rax, [rsp+8]
   mov  rdi, [rsp+16]

   xor  ecx, ecx
   mov  dword ptr[rdi], ecx
   mov  dword ptr[rdi+4], ecx

   mov  ebx, dword ptr [rax]
   cmp  ebx, 00000080h
   jl   short lab1
   cmp  ebx, 0800h
   jl   short lab2
   cmp  ebx, 10000h
   jl   short lab3
   
   mov  edx, ebx
   and  edx, 03Fh
   add  edx, 00000080h
   shl  edx, 24
   mov  ecx, edx

   mov  edx, ebx
   and  edx, 0FC0h   
   shl  edx, 10
   add  edx, 800000h 
   or   ecx, edx

   mov  edx, ebx
   and  edx, 03F000h
   shr  edx, 4
   add  edx, 08000h 
   or   ecx, edx

   mov  edx, ebx
   shr  edx, 18
   and  edx, 03Fh
   add  edx, 0F0h 
   or   ecx, edx

   mov  dword ptr [rdi], ecx
   mov  edx, 4
   ret
   
lab1:
   mov  dword ptr[rdi], ebx
   mov  edx, 1
   ret

lab2:
   mov  edx, ebx
   shr  edx, 6
   add  edx, 0C0h
   mov  byte ptr [rdi], dl
   
   and  ebx, 03Fh
   add  ebx, 00000080h
   mov  byte ptr [rdi+1], bl

   mov  edx, 2
   ret

lab3:
   mov  edx, ebx
   shr  edx, 12
   add  edx, 0E0h
   mov  byte ptr [rdi], dl

   mov  edx, ebx
   shr  edx, 6
   and  edx, 03Fh
   add  edx, 00000080h
   mov  byte ptr [rdi+1], dl

   and  ebx, 03Fh
   add  ebx, 00000080h
   mov  byte ptr [rdi+2], bl

   mov  edx, 3
   ret

end

// subcopyz(target,index,size,arr)
procedure coreapi'subcopyto

  mov  rax, [rsp+32]
  mov  rdx, [rsp+24]
  mov  rsi, [rsp+8]
  mov  ecx, dword ptr [rdx]
  mov  rdi, [rsp+16]
  test ecx, ecx
  mov  ebx, dword ptr [rdi]
  jz   short labEnd

labNext:
  mov  edx, dword ptr [rax]
  mov  byte ptr [rsi + rbx], dl
  add  rbx, 1
  add  rax, 1
  sub  ecx, 1
  jnz  short labNext

labEnd:
  ret

end

procedure coreapi'core_callstack_load

  mov  rax, [rsp+8]
  mov  rcx, [rsp+16]

  mov  rdx, [rsp]                  
  xor  ebx, ebx
  mov  rsi, rbp

labNext:
  mov  rdx, [rsi + 8]
  cmp  [rsi], 0
  jnz  short labSave
  test rdx, rdx
  jz   short labEnd
  mov  rsi, rdx
  jmp  short labNext                              

labSave:
  mov  [rax + rbx * 8], rdx
  add  rbx, 1
  cmp  rbx, rcx
  jge  short labEnd
  mov  rsi, [rsi]
  jmp  short labNext                              
  
labEnd:
  mov  rdx, rbx
  ret  

end

procedure coreapi'strtoint

  mov  rax, [rsp+16]                 // ; radix
  mov  rsi, [rsp+8]                 // ; get str
  mov  ebx, dword ptr [rax]
  mov  ecx, dword ptr [rsi-elSizeOffset]
  xor  edx, edx                     // ; clear flag
  and  ecx, 0FFFFFh
  cmp  byte ptr [rsi], 2Dh
  lea  rcx, [rcx-1]                 // ; to skip zero
  jnz  short Lab4
  lodsb
  mov  edx, 1                        // ; set flag
  lea  rcx, [rcx-1]                  //  ; to skip minus
Lab4:
  push rdx
  xor  eax, eax
Lab1:
  mov  edx, ebx
  mul  edx
  mov  edx, eax
  xor  eax, eax
  lodsb
  cmp  eax, 3Ah
  jl   short lab11
  sub  al, 7
lab11:
  sub  al, 30h
  jb   short Lab2
  cmp  eax, ebx
  ja   short Lab2
  add  eax, edx
  sub  ecx, 1
  jnz  short Lab1
  nop
  pop  rbx
  test ebx, ebx                                
  jz   short Lab5
  neg  eax
Lab5:
  mov  edx, eax
  mov  rbx, [rsp+8]
  jmp  short Lab3
Lab2:
  add  rsp, 8
  xor  ebx, ebx
Lab3:
  ret

end

// ; rcopyl (eax:src, ecx : base, esi - result)
procedure coreapi'wstrtoint

  mov  rax, [rsp+16]                 // ; radix
  mov  rsi, [rsp+8]                 // ; get str

  mov  ebx, dword ptr[rax]                   // ; radix
  mov  ecx, dword ptr [rsi-elSizeOffset]
  xor  edx, edx                     // ; clear flag
  and  ecx, 0FFFFFh
  cmp  byte ptr [esi], 2Dh
  lea  rcx, [rcx-2]                 // ; to skip zero
  jnz  short Lab4
  lodsw
  mov  edx, 1                        // ; set flag
  lea  rcx, [rcx-2]                 //  ; to skip minus
Lab4:
  push rdx
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
  sub  ecx, 2
  jnz  short Lab1
  nop
  pop  rbx
  test ebx, ebx                                
  jz   short Lab5
  neg  eax
Lab5:
  mov  edx, eax
  mov  rbx, [rsp+8]
  jmp  short Lab3
Lab2:
  xor  ebx, ebx
Lab3:
  ret

end

procedure coreapi'longtowstr

   mov  rbx, [rsp+16]
   mov  rax, [rsp+8]
   mov  rsi, [rbx]
   mov  rdi, [rsp+24]

   push rbp
   mov  rax, [rax]
   mov  rbp, rsp
   xor  ecx, ecx
   push rax
   cmp  rax, 0
   jns  short Lab6
   neg  eax
Lab6:
   cmp  rax, rbx
   jb   short Lab5
Lab1:
   idiv rbx
   push rdx
   add  ecx, 2
   cmp  rax, rbx
   jae  short Lab1
Lab5:   
   add  ecx, 4
   push rax
   mov  rax, [rbp-8]
   cmp  rax, 0
   jns  short Lab7
   push 0F6h      // to get "-" after adding 0x30
   add  ecx, 2
Lab7:
   mov  rsi, rdi
   mov  edx, 0FFh
   sub  ecx, 2             // to skip zero
Lab2:
   pop  rax
   cmp  eax, 0Ah
   jb   short Lab8
   add  eax, 7
Lab8:
   add  eax, 30h
   and  eax, edx
   mov  word ptr [rsi], ax
   add  rsi, 2
   sub  rcx, 2
   jnz  short Lab2
   mov  rcx, rsi
   sub  rcx, rdi
   shr  ecx, 1
   lea  rsp, [rsp+8]
   pop  rbp

   ret
   
end

// ; realtowstr(str,radix,r)
procedure coreapi'realtowstr

   mov  rdi, [rsp+24]                // ; r 
   mov  rsi, [rsp+16]                // ; radix
   mov  rax, [rsp+8]                 // ; get str
   mov  ebx, dword ptr [rsi]

   mov   rcx, rax
   push  rbp
   mov   rbp, rsp

   sub   rsp, 104  

   push  rdi
   
   lea   rbx, [rbx-3]         // get the number of decimal digits (minus 2 for sign and dot)
   cmp   ebx, 13
   jbe   short ftoa1   
   mov   ebx, 13
ftoa1:
   xor   edx, edx

   //-------------------------------------------
   //first examine the value on FPU for validity
   //-------------------------------------------

   fld   qword ptr [rcx]
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
   stosw                      // write it
   jmp   finish

   //---------------------------
   // get the size of the number
   //---------------------------

getnumsize:
   fldlg2                     // log10(2)
   fld   st(1)                // copy Src
   fabs                       // insures a positive value
   fyl2x                      // ->[log2(Src)]*[log10(2)] = log10(Src)
      
   fstcw word ptr [rbp-8]     // get current control word
   mov   ax, word ptr [rbp-8]
   or    ax,0C00h             // code it for truncating
   mov   word ptr [rbp-16],ax
   fldcw word ptr [rbp-16]     // insure rounding code of FPU to truncating
      
   fist  [rbp-24]             // store characteristic of logarithm
   fldcw word ptr [rbp-8]     // load back the former control word

   ftst                       // test logarithm for its sign
   fstsw ax                   // get result
   sahf                       // transfer to CPU flags
   sbb   [rbp-24],0           // decrement esize if log is negative
   fstp  st(0)                // get rid of the logarithm

   //-----------------------------------------------------------------------
   // get the power of 10 required to generate an integer with the specified
   // number of significant digits
   //-----------------------------------------------------------------------
   
   mov   eax, dword ptr [rbp-12]
   lea   rax, [rax+1]  // one digit is required
   or    eax, eax
   js    short ftoa21
   cmp   eax, 13
   jbe   short ftoa20
   mov   edx, -1
   mov   ebx, 13
   mov   ecx, ebx
   sub   ecx, eax
   mov   dword ptr[rbp-32], ecx
   jmp   short ftoa22

ftoa20:
   add   eax, ebx
   cmp   eax, 13
   jbe   short ftoa21
   sub   eax, 13
   sub   ebx, eax      

ftoa21:
   mov   dword ptr[rbp-32], ebx

ftoa22:

   //----------------------------------------------------------------------------------------
   // multiply the number by the power of 10 to generate required integer and store it as BCD
   //----------------------------------------------------------------------------------------

   fild  dword ptr [rbp-32]
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

   fbstp tbyte ptr[rbp-56]    // ->TBYTE containing the packed digits
   fstsw ax                   // retrieve exception flags from FPU
   shr   eax,1                // test for invalid operation
   jc    srcerr               // clean-up and return error

   //------------------------------------------------------------------------------
   // unpack BCD, the 10 bytes returned by the FPU being in the little-endian style
   //------------------------------------------------------------------------------

   lea   rsi, [rbp-47]        // go to the most significant byte (sign byte)
   push  rdi
   lea   rdi,[rbp-104]
   mov   eax,3020h
   movzx ecx,byte ptr[rsi]     // sign byte
   cmp   ecx, 00000080h
   jnz   short ftoa5
   mov   al, 45               // insert sign if negative number
ftoa5:

   stosw
   mov   ecx,9
ftoa6:
   sub   esi, 1
   movzx eax,byte ptr[rsi]
   ror   ax,4
   ror   ah,4
   add   eax,3030h
   stosw
   sub   ecx, 1
   jnz   short ftoa6

   pop   rdi
   lea   rsi,[rbp-104]
   
   cmp   edx, 0
   jnz   short scientific

   //************************
   // REGULAR STRING NOTATION
   //************************

   movsb                      // insert sign
   xor   eax, eax
   stosb

   cmp   byte ptr[rsi-1], 20h // test if we insert space
   jnz   short ftoa60
   lea   rdi, [rdi-2]         // erase it

ftoa60:
   mov   ecx,1                // at least 1 integer digit
   mov   eax, dword ptr [rbp-24]
   or    eax, eax             // is size negative (i.e. number smaller than 1)
   js    short ftoa61
   add   ecx, eax

ftoa61:
   mov   eax, ebx
   add   eax, ecx             // ->total number of digits to be displayed
   sub   eax, 19
   sub   esi, eax             // address of 1st digit to be displayed
   cmp   byte ptr[rsi-1], 49  // "1"
   jnz   ftoa8 
   sub   esi, 1
   add   ecx, 1 
ftoa8:
   test  ecx, ecx
   jz    short ftoa8End
ftoa8Next:                    // copy required integer digits
   movzx  eax, byte ptr [rsi]
   mov   word ptr [rdi], ax
   lea   rsi, [rsi+1]
   lea   rdi, [rdi+2]
   sub   ecx, 1
   jnz   short ftoa8Next
ftoa8End:
   mov   ecx,ebx
   or    ecx,ecx
   jz    short ftoa9
   mov   eax,46               // "."
   stosw

ftoa9Next:                    // copy required decimal digits
   movzx  eax, byte ptr [rsi]
   mov   word ptr [rdi], ax
   lea   rsi, [rsi+1]
   lea   rdi, [rdi+2]
   sub   ecx, 1
   jnz   short ftoa9Next
ftoa9:
   jmp   finish

scientific:
   movsb                      // insert sign
   xor   eax, eax
   stosb

   cmp   byte ptr[rsi-1], 20h // test if we insert space
   jnz   short ftoa90
   lea   rdi, [rdi-2]         // erase it

ftoa90:
   mov   ecx, ebx
   mov   eax, 18
   sub   eax, ecx
   add   esi, eax
   cmp   byte ptr[rsi-1],49   // "1"
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
   movzx  eax, byte ptr [rsi]
   mov   word ptr [rdi], ax
   lea   rsi, [rsi+1]
   lea   rdi, [rdi+2]
   sub   ecx, 1
   jnz   short ftoa10Next

   mov   eax,69                // "E"
   stosw
   mov   eax,43                // "+"
   mov   ecx,dword ptr [rbp-24]
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
   push  rax
   and   eax,0ffh             // keep only the thousands & hundreds
   mov   cl,10
   div   cl                   // ->thousands in al, hundreds in AH
   add   eax,3030h            // convert to characters
   mov   bl, al               // insert them 
   mov   word ptr [rdi], bx
   lea   rdi, [rdi+2]
   shr   eax, 8
   mov   bl, al
   mov   word ptr [rdi], bx
   lea   rdi, [rdi+2]
   pop   rax
   shr   eax,8                // get the tens & units in al
   div   cl                   // tens in al, units in AH
   add   eax,3030h            // convert to characters

   mov   bl, al               // insert them 
   mov   word ptr [rdi], bx
   lea   rdi, [rdi+2]
   shr   eax, 8
   mov   bl, al
   mov   word ptr [rdi], bx
   lea   rdi, [rdi+2]

finish:
   cmp   word ptr [rdi-2], 48 // '0'
   jnz   short finish1
   lea   rdi, [rdi-2]
   jmp   short finish

finish1:
   cmp   word ptr [rdi-2], 46 // '.'
   jnz   short finish2
   lea   rdi, [rdi+2]

finish2:
   mov   ebx, edi
   pop   rdi
   add   rsp, 104
   pop   rbp

   sub   rbx, rdi
   mov   rcx, rbx

   jmp   short finish3

srcerr:
   pop   rdi
   add   rsp, 104
   pop   rbp
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

  ret
end

// ; rcopyl (eax:char, edi - target)
procedure coreapi'chartoshorts

   mov  rsi, [rsp+8]
   mov  rax, [rsp+16]
   mov  ecx, dword ptr[rsi]
   mov  rdi, [rsp+24]
   mov  ebx, dword ptr[rax]

   cmp  ecx, 010000h
   jl   short lab1
   
   mov  edx, ecx
   shr  edx, 10
   add  edx, 0D7C0h
   mov  word ptr [rdi + rbx * 2], dx
   add  ebx, 1

   mov  edx, ecx
   and  edx, 03FFh
   add  edx, 0DC00h
   mov  word ptr [rdi+rbx * 2], dx
   mov  edx, 2
   ret
   
lab1:
   mov  dword ptr[rdi + rbx * 2], ecx
   mov  edx, 1
   ret

end

procedure coreapi'strtouint

  mov  rax, [rsp+16]                 // ; radix
  mov  rsi, [rsp+8]                 // ; get str

  mov  ebx, dword ptr[rax]                   // ; radix
  mov  ecx, dword ptr [rsi-elSizeOffset]
  and  ecx, 0FFFFFh
  xor  eax, eax
  lea  rcx, [rcx-1]                 // ; to skip zero
Lab1:
  mov  edx, ebx
  mul  edx
  mov  edx, eax
  xor  eax, eax
  lodsb
  cmp  eax, 3Ah
  jl   short lab11
  sub  al, 7
lab11:
  sub  al, 30h
  jb   short Lab2
  cmp  eax, ebx
  ja   short Lab2
  add  eax, edx
  sub  ecx, 1
  jnz  short Lab1
  mov  edx, eax
  mov  rbx, [rsp+8]
  jmp  short Lab3
Lab2:
  add  rsp, 8
  xor  ebx, ebx
Lab3:
  ret

end

procedure coreapi'rsqrt

  mov   rax, [rsp+8]
  fld   qword ptr [rax]  
  fsqrt
  fstp  qword ptr [rbx]    // store result 
  ret

end

procedure coreapi'rpi

  fldpi
  fstp  qword ptr [rbx]    // store result 
  ret

end

// ; rcopyl (eax:src, ecx : base, ebx - result)
procedure coreapi'strtoreal

  mov  rax, [rsp+16]                 // ; radix
  mov  rsi, [rsp+8]                 // ; get str
  mov  ecx, dword ptr[rax]

  sub   rsp, 12
  xor   edx, edx
  xor   eax, eax
  xor   ebx, ebx
  mov   rdi, rsp
  stosd
  stosd
  mov   dword ptr [rdi], eax

atof1:
  lodsb
  cmp   eax, 32                  // " "
  jz    short atof1
  or    eax, eax
  jnz   short atof2

atoflerr:
  add   rsp, 12
  xor   ebx, ebx
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
  mov   byte ptr [rdi+1], dh    // put sign byte in bcd string
  xor   edx,edx
  lodsb

  //------------------------------------
  // convert the digits to packed decimal
  //------------------------------------
integer:

  cmp   eax, 46                  // .
  jnz   short atof4
  test  bh, 1
  jnz   short atoflerr           // only one decimal point allowed
  or    bh, 1
  lodsb
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
  lodsb
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
  lodsb
  jmp   integer
atof91:
  jmp   atoflerr
atof10:
  mov   dh,al
  
integer1:
  lodsb
  cmp   eax, 46                 // "."
  jnz   short atof20
  test  bh,1
  jnz   atoflerr               // only one decimal point allowed
  or    bh, 1                  // use bh bit0 as the decimal point flag
  lodsb
atof20:
  cmp   eax, 101                // "e"
  jnz   short atof30
  mov   ah, dh
  mov   al,0
  rol   al,4
  ror   ax,4
  mov   byte ptr [rdi],al
  mov   dh, ah
  jmp   scient
atof30:
  cmp   eax, 69                 // "E"
  jnz   short atof40
  mov   ah, dh
  mov   al,0
  rol   al,4
  ror   ax,4
  mov   byte ptr [rdi],al
  mov   dh, ah
  jmp   scient
atof40:  
  or    eax,eax
  jnz   short atof50
  mov   ah, dh
  rol   al,4
  ror   ax,4
  mov   byte ptr [rdi],al
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
  mov   byte ptr [rdi],al
  mov   dh, ah
  sub   edi, 1
  lodsb
  jmp   integer

laststep1:
  cmp   cl,19
  jnz   short laststep
  fldz
  jmp   short laststep2

laststep:

  mov   ah, dh
/*  
      push  eax               //;reserve space on stack
      fstcw word ptr [esp]             // ;get current control word
      pop   eax
      or    eax,0300h          // ;code it for truncating
      push  eax
      fldcw word ptr [esp]             // ;change rounding code of FPU to truncate
      pop   eax
*/    
  xor   edx, edx
  fbld  [rsp]
  sub   cl, 1
  add   bl,cl
  movzx eax,bl
  sub   edx,eax

  push  rdx
  fild  dword ptr [rsp]     // load the exponent
  fldl2t                    // load log2(10)
  fmulp                     // ->log2(10)*exponent
  pop   rdx

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

  add   rsp, 12
  mov   rdi, [rsp+24]
  fstp  qword ptr[rdi]    // store result at specified address
  jmp   short atoflend

scient:
  cmp   cl,19
  jnz   short atof80
  fldz
  jmp   short laststep2
  xor   edx, edx

atof80:
  xor   eax,eax
  lodsb
  cmp   ax, 43            // "+"
  jz    atof90
  cmp   ax, 45            // "-"
  jnz   short scient1
  stc
  rcr   eax,1             // keep sign of exponent in most significant bit of EAX
     
atof90:

  lodsb                   // get next digit after sign

scient1:
  push  rax
  and   eax,0ffh
  jnz   short atof100      // continue if 1st byte of exponent is not terminating 0

scienterr:
  xor   ebx, ebx
  jmp   atoflerr         // no exponent

atof100:
  sub   eax,30h
  jc    short scienterr    // unacceptable character
  cmp   eax,9
  ja    short scienterr    // unacceptable character
  imul  edx,10
  add   edx,eax
  cmp   edx,4931h
  ja    short scienterr    // exponent too large
  lodsb
  or    eax,eax
  jnz   short atof100
  pop   rax               // retrieve exponent sign flag
  rcl   eax,1             // is most significant bit set?
  jnc   short atof200
  neg   edx

atof200:
  jmp   laststep  

atoflend:
   ret

end

// ; rcopyl (eax:src, ecx : base, esi - result)
procedure coreapi'wstrtoreal

  mov  rdi, [rsp+24]                 
  mov  rax, [rsp+16]                 // ; radix
  mov  rsi, [rsp+8]                 // ; get str
  mov  ecx, dword ptr[rax]

  push  rdi
  sub   rsp, 12
  xor   edx, edx
  xor   eax, eax
  xor   ebx, ebx
  mov   rdi, rsp
  stosd
  stosd
  mov   word ptr [rdi], ax
  mov   ecx, 19

atof1:
  lodsw
  cmp   eax, 32                  // " "
  jz    short atof1
  or    eax, eax
  jnz   short atof2

atoflerr:
  add   rsp, 12
  pop   rdi
  xor   esi, esi
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
  mov   byte ptr [rdi+1], dh    // put sign byte in bcd string
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
  mov   byte ptr [rdi],al
  mov   dh, ah
  jmp   scient
atof30:
  cmp   eax, 69                 // "E"
  jnz   short atof40
  mov   ah, dh
  mov   al,0
  rol   al,4
  ror   ax,4
  mov   byte ptr [rdi],al
  mov   dh, ah
  jmp   scient
atof40:  
  or    eax,eax
  jnz   short atof50
  mov   ah, dh
  rol   al,4
  ror   ax,4
  mov   byte ptr [rdi],al
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
  mov   byte ptr [rdi],al
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
  fbld  [rsp]
  sub   cl, 1
  add   bl,cl
  movzx eax,bl
  sub   edx,eax

  push  rdx
  fild  dword ptr [rsp]     // load the exponent
  fldl2t                    // load log2(10)
  fmulp                     // ->log2(10)*exponent
  pop   rdx

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

  add   rsp, 12
  pop   rdi
  fstp  qword ptr[rdi]    // store result at specified address
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
  push  rax
  and   eax,0ffh
  jnz   short atof100      // continue if 1st byte of exponent is not terminating 0

scienterr:
  pop   rdi
  xor   esi, esi
  jmp   atoflerr         // no exponent

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
  pop   rax               // retrieve exponent sign flag
  rcl   eax,1             // is most significant bit set?
  jnc   short atof200
  neg   edx

atof200:
  jmp   laststep  

atoflend:
   mov  rax, rsi
   ret

end

// ; rcopyl (src,tgt)
procedure coreapi'longtoreal

  mov  rax, [rsp+8]
  mov  rdi, [rsp+16]
  fild qword ptr [rax]
  fstp qword ptr [rdi]
  ret

end

// ; rcopyl (eax:src, ecx : base, esi - result)
procedure coreapi'strtolong

  mov  rax, [rsp+16]                // ; radix
  mov  rsi, [rsp+8]                 // ; get str
  mov  ecx, dword ptr [rax]

  push rcx
  mov  ecx, dword ptr[rsi-elSizeOffset]
  xor  edx, edx
  and  ecx, 0FFFFFh

  cmp  byte ptr [rsi], 2Dh
  lea  rcx, [rcx-1]
  jnz  short labStart

  lea  rsi, [rsi+1]
  lea  rcx, [rcx-1]
  mov  edx, 1        // set flag in ebx

labStart:
  push rdx           // save sign flag
  xor  rax, rax      

labConvert:
  mov  edx, dword ptr [rsp+8]
  mul  rdx 
  mov  rbx, rax
  xor  eax, eax
  lodsb
  cmp  eax, 3Ah
  jl   short lab11
  sub  al, 7
lab11:
  sub  al, 30h
  jb   short labErr
  mov  edx, dword ptr [rsp+8]
  cmp  eax, edx
  ja   short labErr
  add  rax, rbx
  sub  ecx, 1
  jnz  short labConvert

  mov  rbx, rax
  pop  rax           // restore flag
  test eax, eax
  jz   short labSave
  neg  rbx

labSave:
  pop  rsi

  mov  rax, [rsp+24]
  mov  [rax], rbx
  mov  rbx, [rsp+8]

  jmp  short labQuit

labErr:
  xor  ebx, ebx
  pop  rdx

labQuit:
  ret

end

// ; rcopyl (eax:src, ecx : base, esi - result)
procedure coreapi'wstrtolong

  mov  rax, [rsp+16]                 // ; radix
  mov  rsi, [rsp+8]                 // ; get str
  mov  rcx, [rax]

  push rcx
  mov  esi, eax
  mov  ecx, dword ptr [rsi-elSizeOffset]
  xor  edx, edx
  and  ecx, 0FFFFFh

  cmp  byte ptr [rsi], 2Dh
  lea  rcx, [rcx-2]
  jnz  short labStart

  lea  rsi, [rsi+2]
  lea  rcx, [rcx-2]
  mov  edx, 1        // set flag in ebx

labStart:
  push rdx           // save sign flag
  xor  edi, edi      // edi   - DHI
  xor  ebx, ebx      // ebx   - DLO

labConvert:
  mov  rdx, [rsp+4]
  mov  eax, edi
  mul  edx           // DHI * 10
  mov  edi, eax

  mov  eax, ebx
  mov  rdx, [rsp+4]
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

  pop  rax           // restore flag
  test eax, eax
  jz   short labSave

  not  edi           // invert number
  neg  ebx

labSave:

  mov  edx, edi
  pop  rsi

  mov  rax, [rsp+12]
  mov  dword ptr [rax], ebx
  mov  dword ptr [rax+4], edx
  mov  rbx, [rsp+8]

  jmp  short labEnd

labErr:
  xor  ebx, ebx
  pop  rbx

labEnd:
  ret

end

// ; (esi - index, ecx - char, edi - target ; out : ecx : length)
procedure coreapi'chartobytes

   mov  rsi, [rsp+8]
   mov  rax, [rsp+16]
   mov  ecx, dword ptr [rsi]
   mov  rdi, [rsp+24]
   mov  ebx, dword ptr[rax]

   cmp  ecx, 00000080h
   jl   short lab1
   cmp  ecx, 0800h
   jl   short lab2
   cmp  ecx, 10000h
   jl   short lab3
   
   mov  edx, ecx
   and  edx, 03Fh
   add  edx, 00000080h
   mov  byte ptr [rdi + rbx], dl
   add  ebx, 1

   mov  edx, ecx
   shr  edx, 12
   and  edx, 0000003Fh
   add  edx, 00000080h
   mov  byte ptr [rdi + rbx], dl
   add  ebx, 1
   
   mov  edx, ecx
   shr  edx, 6
   and  edx, 0000003Fh
   add  edx, 00000080h
   mov  byte ptr [rdi + rbx], dl
   add  ebx, 1
    
   mov  edx, ecx
   and  edx, 03Fh
   add  edx, 00000080h
   mov  byte ptr [rdi + rbx], dl
   add  ebx, 1
   mov  edx, 4
   ret
   
lab1:
   mov  byte ptr [rdi + rbx], cl
   add  ebx, 1
   mov  edx, 1
   ret

lab2:
   mov  edx, ecx
   shr  edx, 6
   add  edx, 0C0h
   mov  byte ptr [rdi + rbx], dl
   add  ebx, 1
   
   and  ecx, 03Fh
   add  ecx, 00000080h
   mov  byte ptr [rdi+rbx], cl
   add  ebx, 1
   mov  edx, 2
   ret

lab3:
   mov  edx, ecx
   shr  edx, 12
   add  edx, 0E0h
   mov  byte ptr [rdi + rbx], dl
   add  ebx, 1
   
   mov  edx, ecx
   shr  edx, 6
   and  edx, 03Fh
   add  edx, 00000080h
   mov  byte ptr [rdi+rbx], dl
   add  ebx, 1

   and  ecx, 03Fh
   add  ecx, 00000080h
   mov  byte ptr [rdi+rbx], cl
   add  ebx, 1
   mov  edx, 3
   ret

end

// ; move(target,index,len,offs)
procedure coreapi'move

  mov  rdx, [rsp+16]
  mov  rax, [rsp+24]
  mov  rdi, [rsp+8]      // ; target
  mov  ecx, dword ptr[rax]
  mov  rsi, [rsp+32]
  mov  ebx, dword ptr [rdx]        // ; index

  test ecx, ecx          // ; len
  jz   short labEnd

  movsxd rdx, dword ptr[rsi]        // ; offs
  cmp    rdx, 0
  jl     short labDelete

  add  ebx, ecx
  sub  ebx, 1

  add  edx, ebx

  add  edx, edi
  add  ebx, edi

labNext:
  mov  esi, dword ptr [rsi]
  mov  dword ptr [rdx], esi
  sub  ebx, 1
  sub  edx, 1
  sub  ecx, 1
  jnz  short labNext

labEnd:
  ret

labDelete:
  add  rdx, rbx

  add  rdx, rdi
  add  rbx, rdi

labNext2:
  mov  esi, dword ptr [rbx]
  mov  dword ptr [rdx], esi
  add  ebx, 1
  add  edx, 1
  sub  ecx, 1
  jnz  short labNext2
  ret

end

// subcopyz(target,index,size,arr)
procedure coreapi'subcopy

  mov  rax, [rsp+32]
  mov  rdx, [rsp+24]
  mov  rsi, [rsp+8]
  mov  ecx, dword ptr[rdx]
  mov  rdi, [rsp+16]
  test ecx, ecx
  mov  ebx, dword ptr[rdi]
  jz   short labEnd

labNext:
  mov  rdx, [rax + rbx]
  mov  byte ptr [rsi], dl
  add  rbx, 1
  add  rsi, 1
  sub  ecx, 1
  jnz  short labNext

labEnd:
  ret

end

procedure coreapi'longtostr

   mov  rbx, [rsp+16]
   mov  rax, [rsp+8]
   mov  rsi, [rbx]
   mov  rdi, [rsp+24]

   push rbp
   mov  rax, [rax]
   mov  rbp, rsp
   xor  ecx, ecx
   push rax
   // ; take sign into account only for the decimal representation
   cmp  rsi, 10        
   jnz  short Lab6
   cmp  rax, 0
   jns  short Lab6
   neg  rax
Lab6:
   cmp  rax, rsi
   jb   short Lab5
Lab1:
   cqo
   idiv rsi
   push rdx
   add  ecx, 1
   cmp  rax, rsi
   jae  short Lab1
Lab5:   
   add  ecx, 2
   push rax
   cmp  rsi, 10        
   jnz  short Lab7
   mov  rax, [rbp-8]
   cmp  eax, 0
   jns  short Lab7
   push 0F6h      // to get "-" after adding 0x30
   add  ecx, 1
Lab7:
   sub  ecx, 1
   mov  rsi, rdi
   mov  edx, 0FFh
Lab2:
   pop  rax
   cmp  eax, 0Ah
   jb   short Lab8
   add  eax, 7
Lab8:
   add  eax, 30h
   and  eax, edx
   mov  byte ptr [rsi], al
   add  esi, 1
   sub  ecx, 1
   jnz  short Lab2
   mov  edx, esi
   sub  edx, edi
   lea  rsp, [rsp+8]
   pop  rbp

   ret
   
end

// ; realtostr(str,radix,r)
procedure coreapi'realtostr

   mov  rdi, [rsp+24]                // ; r 
   mov  rsi, [rsp+16]                // ; radix
   mov  rax, [rsp+8]                 // ; get str
   mov  ebx, dword ptr [rsi]

   mov   rcx, rax
   push  rbp
   mov   rbp, rsp

   sub   rsp, 104  

   push  rdi
   
   lea   rbx, [rbx-3]         // get the number of decimal digits (minus 2 for sign and dot)
   cmp   ebx, 13
   jbe   short ftoa1   
   mov   ebx, 13
ftoa1:
   xor   edx, edx

   //-------------------------------------------
   //first examine the value on FPU for validity
   //-------------------------------------------

   fld   qword ptr [rcx]
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
   mov   eax,4E49h        // "NI"
   stosw
   mov   eax,4946h        // "IF"
   stosw
   mov   eax,494Eh        // "IN"
   stosw
   mov   eax,5954h        // "YT"
   stosw
   jmp   finish      

   //-------------------------
   //value to be converted = 0
   //-------------------------
         
maybezero:
   jpe   short getnumsize     // would be denormalized number
   fstp  st(0)                // flush that 0 value off the FPU
   mov   eax,2E30h            // ".0" szstring
   stosw                      // write it
   mov   eax,30h              // "0" szstring
   stosb                      // write it
   jmp   finish

   //---------------------------
   // get the size of the number
   //---------------------------

getnumsize:
   fldlg2                     // log10(2)
   fld   st(1)                // copy Src
   fabs                       // insures a positive value
   fyl2x                      // ->[log2(Src)]*[log10(2)] = log10(Src)
      
   fstcw word ptr [rbp-8]     // get current control word
   mov   ax, word ptr [rbp-8]
   or    ax,0C00h             // code it for truncating
   mov   word ptr [rbp-16],ax
   fldcw word ptr [rbp-16]     // insure rounding code of FPU to truncating
      
   fist  [rbp-24]             // store characteristic of logarithm
   fldcw word ptr [rbp-8]     // load back the former control word

   ftst                       // test logarithm for its sign
   fstsw ax                   // get result
   sahf                       // transfer to CPU flags
   sbb   [rbp-24],0           // decrement esize if log is negative
   fstp  st(0)                // get rid of the logarithm

   //-----------------------------------------------------------------------
   // get the power of 10 required to generate an integer with the specified
   // number of significant digits
   //-----------------------------------------------------------------------
   
   mov   eax, dword ptr[rbp-24]
   lea   rax, [rax+1]  // one digit is required
   or    eax, eax
   js    short ftoa21
   cmp   eax, 13
   jbe   short ftoa20
   mov   edx, -1
   mov   ebx, 13
   mov   ecx, ebx
   sub   ecx, eax
   mov   dword ptr [rbp-32], ecx
   jmp   short ftoa22

ftoa20:
   add   eax, ebx
   cmp   eax, 13
   jbe   short ftoa21
   sub   eax, 13
   sub   ebx, eax      

ftoa21:
   mov   dword ptr[rbp-32], ebx

ftoa22:

   //----------------------------------------------------------------------------------------
   // multiply the number by the power of 10 to generate required integer and store it as BCD
   //----------------------------------------------------------------------------------------

   fild  dword ptr [rbp-32]
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

   fbstp tbyte ptr[rbp-56]    // ->TBYTE containing the packed digits
   fstsw ax                   // retrieve exception flags from FPU
   shr   eax,1                // test for invalid operation
   jc    srcerr               // clean-up and return error

   //------------------------------------------------------------------------------
   // unpack BCD, the 10 bytes returned by the FPU being in the little-endian style
   //------------------------------------------------------------------------------

   lea   rsi, [rbp-47]        // go to the most significant byte (sign byte)
   push  rdi
   lea   rdi,[rbp-104]
   mov   eax,3020h
   movzx  ecx,byte ptr[rsi]     // sign byte
   cmp   ecx, 00000080h
   jnz   short ftoa5
   mov   al, 45               // insert sign if negative number
ftoa5:

   stosw
   mov   ecx,9
ftoa6:
   sub   esi, 1
   movzx eax,byte ptr[rsi]
   ror   ax,4
   ror   ah,4
   add   eax,3030h
   stosw
   sub   ecx, 1
   jnz   short ftoa6

   pop   rdi
   lea   rsi,[rbp-104]
   
   cmp   edx, 0
   jnz   short scientific

   //************************
   // REGULAR STRING NOTATION
   //************************

   movsb                      // insert sign

   cmp   byte ptr[rsi-1], 20h // test if we insert space
   jnz   short ftoa60
   lea   rdi, [rdi-1]         // erase it

ftoa60:
   mov   ecx,1                // at least 1 integer digit
   mov   eax, dword ptr[rbp-24]
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
   movzx  eax, byte ptr [rsi]
   mov   byte ptr [rdi], al
   lea   rsi, [rsi+1]
   lea   rdi, [rdi+1]
   sub   ecx, 1
   jnz   short ftoa8Next
ftoa8End:
   mov   ecx,ebx
   or    ecx,ecx
   jz    short ftoa9
   mov   eax,46               // "."
   stosb

ftoa9Next:                    // copy required decimal digits
   movzx  eax, byte ptr [rsi]
   mov   byte ptr [rdi], al
   lea   rsi, [rsi+1]
   lea   rdi, [rdi+1]
   sub   ecx, 1
   jnz   short ftoa9Next
ftoa9:
   jmp   finish

scientific:
   movsb                      // insert sign

   cmp   byte ptr[rsi-1], 20h // test if we insert space
   jnz   short ftoa90
   lea   rdi, [rdi-1]         // erase it

ftoa90:
   mov   ecx, ebx
   mov   eax, 18
   sub   eax, ecx
   add   esi, eax
   cmp   byte ptr[rsi-1],49   // "1"
   pushfd                     // save flags for extra "1"
   jnz   short ftoa10
   sub   esi, 1
ftoa10:
   movsb                      // copy the integer
   xor   eax, eax
   stosb

   mov   eax,46               // "."
   stosb

ftoa10Next:                    // copy the decimal digits
   movzx  eax, byte ptr [rsi]
   mov   byte ptr [rdi], al
   lea   rsi, [rsi+1]
   lea   rdi, [rdi+1]
   sub   ecx, 1
   jnz   short ftoa10Next

   mov   eax,69                // "E"
   stosb
   mov   eax,43                // "+"
   mov   ecx,dword ptr [rbp-24]
   popfd                      // retrieve flags for extra "1"
   jnz   short ftoa11          // no extra "1"
   add   ecx, 1               // adjust exponent
ftoa11:
   or    ecx,ecx
   jns   short ftoa12
   mov   eax,45                // "-"
   neg   ecx                  // make number positive
ftoa12:
   stosb                      // insert proper sign

// Note: the absolute value of the size could not exceed 4931
   
   xor   ebx, ebx   
   mov   eax,ecx
   mov   cl,100
   div   cl                   // ->thousands & hundreds in al, tens & units in AH
   push  rax
   and   eax,0ffh             // keep only the thousands & hundreds
   mov   cl,10
   div   cl                   // ->thousands in al, hundreds in AH
   add   eax,3030h            // convert to characters
   mov   bl, al               // insert them 
   mov   byte ptr [rdi], bl
   lea   rdi, [rdi+1]
   shr   eax, 8
   mov   bl, al
   mov   byte ptr [rdi], bl
   lea   rdi, [rdi+1]
   pop   rax
   shr   eax,8                // get the tens & units in al
   div   cl                   // tens in al, units in AH
   add   eax,3030h            // convert to characters

   mov   bl, al               // insert them 
   mov   byte ptr [rdi], bl
   lea   rdi, [rdi+1]
   shr   eax, 8
   mov   bl, al
   mov   byte ptr [rdi], bl
   lea   rdi, [rdi+1]

finish:
   cmp   byte ptr [rdi-1], 48 // '0'
   jnz   short finish1
   lea   rdi, [rdi-1]
   jmp   short finish

finish1:
   cmp   byte ptr [rdi-1], 46 // '.'
   jnz   short finish2
   lea   rdi, [rdi+1]

finish2:
   mov   rbx, rdi
   pop   rdi
   add   rsp, 104
   pop   rbp

   sub   rbx, rdi
   mov   rdx, rbx

   jmp   short finish3

srcerr:
   pop   rdi
   add   rsp, 104
   pop   rbp
   xor   ebx,ebx
finish3:

/*
oldcw   :-4  (4)
truncw  :-8  (4)
esize   :-12 (4)
tempdw  :-16 (4)
bcdstr  :-28 (12)  // -20
unpacked:- (52)  // -32
*/

  ret
end

// ; s_encode(index,out length, src, dst, out len2)
procedure coreapi's_encode

  mov  rbx, [rsp+16]
  mov  rsi, [rsp+8]
  mov  ecx, dword ptr [rbx]
  mov  rdi, [rsp+32]
  mov  rax, [rsp+24]

  push rdi
  add  eax, dword ptr [rsi]
  push rax
  push rbx
  
labNext:
  xor  ebx, ebx
  mov  bl, byte ptr [rax]
  add  eax, 1
  cmp  ebx, 00000080h
  jl   lab1
  cmp  ebx, 000000C0h
  jl   err2
  cmp  ebx, 000000E0h
  jl   short lab2
  cmp  ebx, 000000F0h
  jl   lab3
  cmp  ebx, 000000F8h
  jl   lab4
  nop
  nop
  jmp err2

lab2:  
  sub  ecx, 2
  jb   short err
  mov  esi, ebx
  mov  bl, byte ptr [rax]
  add  eax, 1
  mov  edx, ebx
  and  edx, 0C0h
  cmp  edx, 00000080h
  jnz  err2
  shl  esi, 6
  add  esi, ebx
  sub  esi, 3080h
  jmp  labSave
  
lab3:
  sub  ecx, 3
  jb   err
  mov  esi, ebx
  mov  bl, byte ptr [rax]
  add  eax, 1
  mov  edx, ebx
  and  edx, 0C0h
  cmp  edx, 00000080h
  jnz  err2
  cmp  esi, 000000E0h
  jnz  short lab3_1
  cmp  ebx, 000000A0h
  jl   err2

lab3_1:
  shl  esi, 12
  shl  ebx, 6
  add  esi, ebx
  xor  ebx, ebx
  mov  bl, byte ptr [rax]
  add  eax, 1
  mov  edx, ebx
  and  edx, 0C0h
  cmp  edx, 00000080h
  jnz  err2
  add  esi, ebx
  sub  esi, 0E2080h
  jmp  labSave
  
lab4:
  sub  ecx, 4
  jb   short err
  mov  esi, ebx
  mov  bl, byte ptr [rax]
  add  eax, 1
  mov  edx, ebx
  and  edx, 0C0h
  cmp  edx, 00000080h
  jnz  err2
  cmp  esi, 000000F0h
  jnz  short lab4_1
  cmp  ebx, 00000090h
  jl   err2

lab4_1:
  cmp  esi, 000000F4h
  jnz  short lab4_2
  cmp  ebx, 00000090h
  jae  err2

lab4_2:
  shl  esi, 18
  shl  ebx, 12
  add  esi, ebx

  xor  ebx, ebx
  mov  bl, byte ptr [rax]
  add  eax, 1
  mov  edx, ebx
  and  edx, 000000C0h
  cmp  edx, 00000080h
  jnz  err2

  shl  ebx, 6
  add  esi, ebx
  
  xor  ebx, ebx
  mov  bl, byte ptr [rax]
  add  eax, 1
  mov  edx, ebx
  and  edx, 000000C0h
  cmp  edx, 00000080h
  jnz  err2

  add  esi, ebx
  sub  esi, 3C82080h
  jmp  labSave

lab1:
  mov  esi, ebx  
  sub  ecx, 1

labSave:
  mov  dword ptr [rdi], esi
  add  edi, 4

  test ecx, ecx
  jnz  labNext

err:
  pop  rbx
  mov  edx, eax
  pop  rax
  sub  edx, eax
  mov  ecx, edi
  pop  rdi
  sub  ecx, edi
  shr  ecx, 2
  mov  rax, [rsp+40]
  mov  dword ptr [rax], ecx
  mov  rsi, [rsp+16]
  mov  dword ptr [rsi], edx

  ret
  
err2:
  add  rsp, 24
  xor  ebx, ebx
  ret 

end

// ; s_decode(index,out length, src, dst, out len2)
procedure coreapi's_decode

   mov  rbx, [rsp+16]
   mov  rsi, [rsp+8]
   mov  ecx, dword ptr [rbx]
   mov  rdi, [rsp+32]
   mov  ebx, dword ptr [rsi]
   mov  rax, [rsp+24]

   push rdi
   lea  rax, [rax + rbx * 4]
   push rax
   push rbx

labNext:
   mov  ebx, dword ptr [rax]
   cmp  ebx, 00000080h
   jl   short lab1
   cmp  ebx, 0800h
   jl   short lab2
   cmp  ebx, 10000h
   jl   short lab3

   sub  ecx, 1

   mov  edx, ebx
   shr  edx, 18
   and  edx, 03Fh
   add  edx, 000000F0h 
   mov  byte ptr [rdi], dl
   add  edi, 1

   mov  edx, ebx
   and  edx, 03F000h
   shr  edx, 12
   add  edx, 00000080h 
   mov  byte ptr [rdi], dl
   add  edi, 1

   mov  edx, ebx
   and  edx, 0FC0h   
   shr  edx, 6
   add  edx, 00000080h
   mov  byte ptr [rdi], dl
   add  edi, 1

   mov  edx, ebx
   and  edx, 03Fh
   add  edx, 00000080h
   mov  byte ptr [rdi], dl
   add  edi, 1

   jmp  labSave

lab2:
   sub  ecx, 1

   mov  edx, ebx
   shr  edx, 6
   add  edx, 0C0h
   mov  byte ptr [rdi], dl
   add  edi, 1
   
   and  ebx, 03Fh
   add  ebx, 00000080h
   mov  byte ptr [rdi], bl
   add  edi, 1
   jmp  labSave

lab3:
   sub  ecx, 1

   mov  edx, ebx
   shr  edx, 12
   add  edx, 0E0h
   mov  byte ptr [rdi], dl
   add  edi, 1

   mov  edx, ebx
   shr  edx, 6
   and  edx, 03Fh
   add  edx, 00000080h
   mov  byte ptr [rdi], dl
   add  edi, 1

   and  ebx, 03Fh
   add  ebx, 00000080h
   mov  byte ptr [rdi], bl
   jmp  short labSave
   
lab1:
   mov  byte ptr [rdi], bl
   add  edi, 1
   sub  ecx, 1

labSave:
   add  eax, 4
   test ecx, ecx
   jnz  labNext

err:
   pop  rbx
   mov  edx, eax
   pop  rax
   sub  edx, eax
   shr  edx, 2
   mov  ecx, edi
   pop  rdi
   sub  ecx, edi
   mov  rax, [rsp+40]
   mov  dword ptr [rax], ecx
   mov  rsi, [rsp+16]
   mov  dword ptr [rsi], edx
   mov  ebx, eax

   ret

end

procedure coreapi'ws_encode

  mov  rbx, [rsp+16]
  mov  rsi, [rsp+8]
  mov  ecx, dword ptr [rbx]
  mov  rdi, [rsp+32]
  mov  rax, [rsp+24]

  push rdi
  add  eax, dword ptr [rsi]
  push rax
  push rbx

labNext:
  mov  ebx, dword ptr [rax]
  add  eax, 2
  and  ebx, 0FFFFh
  cmp  ebx, 0D800h
  jl   short lab1
  cmp  ebx, 0DBFFh
  jg   short err2

  sub  ecx, 2
  jl   short err

  mov  esi, ebx
  shl  esi, 10
  mov  ebx, dword ptr [rax]
  add  eax, 2
  and  ebx, 0FFFFh
  cmp  ebx, 0DC00h
  jl   short lab2
  cmp  ebx, 0DFFFh
  jg   short err2
  
lab2:
  add  ebx, esi
  sub  ebx, 35FDC00h

lab1:
  mov  esi, ebx
  sub  ecx, 2

labSave:
  mov  dword ptr [rdi], esi
  add  edi, 4

  test ecx, ecx
  jnz  labNext

err:
  pop  rbx
  mov  edx, eax
  pop  rax
  sub  edx, eax
  mov  ecx, edi
  pop  rdi
  sub  ecx, edi
  shr  ecx, 2
  mov  rax, [rsp+40]
  mov  dword ptr[rax], ecx
  mov  rsi, [rsp+16]
  mov  dword ptr [rsi], edx

  ret
  
err2:
  add  rsp, 24
  xor  ebx, ebx
  ret 

end


procedure coreapi'ws_encodew

  mov  rbx, [rsp+16]
  mov  rsi, [rsp+8]
  mov  ecx, dword ptr [rbx]
  mov  rdi, [rsp+32]
  shl  ecx, 1
  mov  rax, [rsp+24]

  push rdi
  add  eax, dword ptr [rsi]
  add  eax, dword ptr [rsi]
  push rax
  push rbx

labNext:
  mov  ebx, dword ptr [rax]
  add  eax, 2
  and  ebx, 0FFFFh
  cmp  ebx, 0D800h
  jl   short lab1
  cmp  ebx, 0DBFFh
  jg   short err2

  sub  ecx, 2
  jl   short err

  mov  esi, ebx
  shl  esi, 10
  mov  ebx, dword ptr [rax]
  add  eax, 2
  and  ebx, 0FFFFh
  cmp  ebx, 0DC00h
  jl   short lab2
  cmp  ebx, 0DFFFh
  jg   short err2
  
lab2:
  add  ebx, esi
  sub  ebx, 35FDC00h

lab1:
  mov  esi, ebx
  sub  ecx, 2

labSave:
  mov  dword ptr [rdi], esi
  add  edi, 4

  test ecx, ecx
  jnz  labNext

err:
  pop  rbx
  mov  edx, eax
  pop  rax
  sub  edx, eax
  mov  ecx, edi
  pop  rdi
  sub  ecx, edi
  shr  ecx, 2
  mov  rax, [rsp+40]
  mov  dword ptr [rax], ecx

  ret
  
err2:
  add  rsp, 24
  xor  ebx, ebx
  ret 

end

procedure coreapi'ws_decode

   mov  rbx, [rsp+16]
   mov  rsi, [rsp+8]
   mov  ecx, dword ptr [rbx]
   mov  rdi, [rsp+32]
   mov  ebx, dword ptr [rsi]
   mov  rax, [rsp+24]

   push rdi
   lea  rax, [rax + rbx * 4]
   push rax
   push rbx

labNext:
   mov  ebx, dword ptr [rax]
   cmp  ebx, dword ptr 128
   jl   short labCH1

   cmp  ebx, 0800h
   jl   short lab1

   sub  ecx, 2
   jl   short err

   mov  edx, ebx
   shr  edx, 10
   add  edx, 0D7C0h
   mov  word ptr [rdi], dx
   add  rdi, 2

   mov  edx, ebx
   and  edx, 03FFh
   add  edx, 0DC00h
   mov  word ptr [rdi], dx
   add  rdi, 2
   jmp  short labSave
   
lab1:
   mov  word ptr [rdi], bx
   add  edi, 2
   sub  ecx, 1
   jmp  short labSave

labCH1:
   mov  word ptr [rdi], bx
   add  edi, 2
   sub  ecx, 1
      
labSave:
   add  eax, 4
   test ecx, ecx
   jnz  labNext

err:
   pop  rbx
   mov  edx, eax
   pop  rax
   sub  edx, eax
   shr  edx, 2
   mov  ecx, edi
   pop  rdi
   sub  ecx, edi
   shr  ecx, 1
   mov  rax, [rsp+40]
   mov  dword ptr[rax], ecx
   mov  esi, dword ptr [rsp+16]
   mov  dword ptr [rsi], edx
   mov  ebx, eax

   ret
  
err2:
   add  rsp, 24
   xor  ebx, ebx
   ret    

end

procedure coreapi'ws_decodew

   mov  rbx, [rsp+16]
   mov  rsi, [rsp+8]
   mov  ecx, dword ptr [rbx]
   mov  rdi, [rsp+32]
   mov  ebx, dword ptr [rsi]
   mov  rax, [rsp+24]

   push rdi
   lea  rax, [rax + rbx * 4]
   push rax
   push rbx

labNext:
   mov  ebx, dword ptr [rax]
   cmp  ebx, dword ptr 128
   jl   short labCH1

   cmp  ebx, 0800h
   jl   short lab1

   sub  ecx, 2
   jl   short err

   mov  edx, ebx
   shr  edx, 10
   add  edx, 0D7C0h
   mov  word ptr [rdi], dx
   add  edi, 2

   mov  edx, ebx
   and  edx, 03FFh
   add  edx, 0DC00h
   mov  word ptr [rdi], dx
   add  edi, 2
   jmp  short labSave
   
lab1:
   mov  word ptr [rdi], bx
   add  edi, 2
   sub  ecx, 1
   jmp  short labSave

labCH1:
   mov  word ptr [rdi], bx
   add  edi, 2
   sub  ecx, 1
      
labSave:
   add  eax, 4
   test ecx, ecx
   jnz  labNext

err:
   pop  rbx
   mov  edx, eax
   pop  rax
   sub  edx, eax
   shr  edx, 2
   mov  ecx, edi
   pop  rdi
   sub  ecx, edi
   shr  ecx, 1
   mov  rax, [rsp+40]
   mov  dword ptr [rax], ecx
   mov  rsi, [rsp+16]
   mov  dword ptr [rsi], edx
   mov  ebx, eax

   ret
  
err2:
   add  rsp, 24
   xor  ebx, ebx
   ret    

end

procedure coreapi'wstrcharlen

  mov  rax, [rsp+8]
  mov  rcx, [rsp+16]                                         
  push rax

labNext:
  mov  edx, dword ptr [rax]
  and  edx, 0FFFFh
  cmp  edx, 0D800h
  jl   short lab1
  
  add  ebx, 1
  add  eax, 4
  sub  ecx, 2
  jnz  short labNext
  pop  rax
  ret

lab1:
  add  ebx, 1
  add  eax, 2
  sub  ecx, 1
  jnz  short labNext
  mov  edx, ebx
  pop  rax
  ret

end

// ; strcharlen(s,len)
procedure coreapi'strcharlen

  mov  rax, [rsp+8]
  mov  rcx, [rsp+16]                                         
  push rax

  xor  edx, edx
  xor  ebx, ebx

labNext:
  mov  dl, byte ptr [rax]
  cmp  edx, 00000080h
  jl   short lab1
  cmp  edx, 000000E0h
  jl   short lab2
  cmp  edx, 000000F0h
  jl   short lab3
  cmp  edx, 000000F5h
  jl   short lab4

lab1:
  add  ebx, 1
  add  eax, 1
  sub  ecx, 1
  jnz  short labNext
  mov  edx, ebx
  pop  rax
  ret
  
lab2:
  add  ebx, 1
  add  eax, 2
  sub  ecx, 2
  jnz  short labNext
  mov  edx, ebx
  pop  rax
  ret
  
lab3:
  add  ebx, 1
  add  eax, 3
  sub  ecx, 3
  jnz  short labNext
  mov  edx, ebx
  pop  rax
  ret
  
lab4:
  add  ebx, 1
  add  eax, 4
  sub  ecx, 4
  jnz  short labNext
  mov  edx, ebx
  pop  rbx
  ret

end

procedure coreapi'register_critical_exception_handler

  mov  [data : % CORE_ET_TABLE], rbx
  ret

end

procedure coreapi'get_seh_handler

  mov  rdx, data : % CORE_ET_TABLE
  ret

end

// core_utf8hashcode(s,len)
procedure coreapi'core_utf8hashcode

  mov  rax, [rsp+8]
  mov  rcx, [rsp+16]

  mov  edi, 15051505h
  mov  esi, edi

labLoop:
  cmp  ecx, 8
  jb   short labSkip

  mov  ebx, edi
  shl  ebx, 5
  mov  edx, edi
  add  ebx, edi
  shr  edx, 27
  add  ebx, edx
  mov  edx, dword ptr [rax]
  xor  ebx, edx
  mov  edi, ebx

  mov  ebx, esi
  shl  ebx, 5
  mov  edx, esi
  add  ebx, esi
  shr  edx, 27
  add  ebx, edx
  mov  edx, dword ptr [rax+4]
  xor  ebx, edx                    
  mov  esi, ebx

  add  eax, 8
  sub  ecx, 8
  jmp  short labLoop

  add  ecx, 8
  and  ecx, 7

labSkip:  
  cmp  ecx, 4
  jb   short labSkip2

  mov  ebx, edi
  shl  ebx, 5
  mov  edx, edi
  add  ebx, edi
  shr  edx, 27
  add  ebx, edx
  mov  edx, dword ptr [rax]
  xor  ebx, edx
  mov  edi, ebx
  add  eax, 4
  sub  ecx, 4

labSkip2:
  mov  ebx, 3
  sub  ebx, ecx
  shl  ebx, 3
  mov  ecx, ebx

  mov  ebx, 0FFFFFFFFh
  shr  ebx, cl
  mov  ecx, dword ptr [rax]
  and  ecx, ebx

  mov  ebx, esi
  shl  ebx, 5
  mov  edx, esi
  add  ebx, esi
  shr  edx, 27
  add  ebx, edx
  xor  ebx, ecx
  mov  esi, ebx

  mov  edx, esi
  imul  edx, 1566083941
  add  edx, edi
  mov  eax, edx 
  ret

end

// core_utf16hashcode(s,len)
procedure coreapi'core_utf16hashcode

  mov  rax, [rsp+8]
  mov  rcx, [rsp+16]

  mov  edi, 15051505h
  mov  esi, edi

labLoop:
  cmp  ecx, 4
  jb   short labSkip

  mov  ebx, edi
  shl  ebx, 5
  mov  edx, edi
  add  ebx, edi
  shr  edx, 27
  add  ebx, edx
  mov  edx, dword ptr [rax]
  xor  ebx, edx
  mov  edi, ebx

  mov  ebx, esi
  shl  ebx, 5
  mov  edx, esi
  add  ebx, esi
  shr  edx, 27
  add  ebx, edx
  mov  edx, dword ptr[rax+4]
  xor  ebx, edx                    
  mov  esi, ebx

  add  eax, 8
  sub  ecx, 4
  jmp  short labLoop

  add  ecx, 4
  and  ecx, 7

labSkip:  
  cmp  ecx, 2
  jb   short labSkip2

  mov  ebx, edi
  shl  ebx, 5
  mov  edx, edi
  add  ebx, edi
  shr  edx, 27
  add  ebx, edx
  mov  edx, dword ptr [rax]
  xor  ebx, edx
  mov  edi, ebx
  add  eax, 4
  sub  ecx, 2

labSkip2:
  mov  ecx, dword ptr [rax]

  mov  ebx, esi
  shl  ebx, 5
  mov  edx, esi
  add  ebx, esi
  shr  edx, 27
  add  ebx, edx
  xor  ebx, ecx
  mov  esi, ebx

  mov  ebx, esi
  imul ebx, 1566083941
  add  ebx, edi
  ret

end

procedure coreapi'nsubcopy

  mov  rdi, [rsp+8]
  mov  rsi, [rsp+24]
  mov  rdx, [rsp+8]
  mov  rax, [rsp+32]
  mov  ecx, dword ptr [rsi]
  mov  ebx, dword ptr [rdx]

  mov  esi, edi
  test ecx, ecx
  jz   short labEnd

labNext:
  mov  rdx, [rax + rbx * 8]
  mov  [rsi], rdx
  add  ebx, 1
  lea  rsi, [rsi + 8]
  sub  ecx, 1
  jnz  short labNext

labEnd:
  ret

end

// ninsert(target,index,len,source)
procedure coreapi'ninsert

  mov  rdx, [rsp+16]
  mov  rax, [rsp+24]
  mov  rdi, [rsp+8]
  mov  ecx, dword ptr [rax]
  mov  rsi, [rsp+32]
  mov  ebx, dword ptr [rdx]
  test ecx, ecx
  jz   short labEnd

labNext:
  mov  edx, dword ptr [rsi]
  mov  dword ptr [rdi + rbx*4], edx
  add  ebx, 1
  lea  rsi, [rsi + 4]
  sub  ecx, 1
  jnz  short labNext

labEnd:
  mov  edx, ebx
  mov  rbx, rdi
  ret

end

// ; nmove(target,index,len,offs)
procedure coreapi'nmove

  mov  rdi, [rsp+8]
  mov  rax, [rsp+24]
  mov  rbx, [rsp+16]
  mov  ecx, dword ptr[rax]
  mov  ebx, dword ptr [rbx]
  mov  rdx, [rsp+32]

  test ecx, ecx
  jz   short labEnd

  movsxd rsi, dword ptr [rdx]
  cmp    esi, 0
  jl     short labDelete

  add  ebx, ecx
  sub  ebx, 1

  add  esi, ebx
  shl  esi, 2

  add  esi, edi
  shl  ebx, 2
  add  ebx, edi
  
labNext:
  mov  edx, dword ptr [rbx]
  mov  dword ptr [rsi], edx
  sub  ebx, 4
  sub  esi, 4
  sub  ecx, 1
  jnz  short labNext

labEnd:
  ret

labDelete:
  add  rsi, rbx
  shl  rsi, 2

  add  rsi, rdi
  shl  rbx, 2
  add  rbx, rdi

labNext2:
  mov  edx, dword ptr [rbx]
  mov  dword ptr [rsi], edx
  add  ebx, 4
  add  esi, 4
  sub  ecx, 1
  jnz  short labNext2
  ret

end

// ; longtoint(l)
procedure coreapi'longtoint

  mov  rbx, [rsp+8]
  mov  ecx, dword ptr [rbx+4]
  cmp  ecx, 0
  jl   labNegative
  nop
  jnz  labErr
  mov  edx, dword ptr [rbx]
  ret
labNegative:
  cmp  ecx, 0FFFFFFFFh
  jnz  labErr
  mov  edx, dword ptr [rbx]
  ret

labErr:
  xor  ebx, ebx
  ret

end

procedure coreapi'core_rnd_init

  mov  rdi, [rsp+8]
  call code : % INIT_RND
  mov  [rdi], rax 
  mov  rbx, rax
  ret
  
end

procedure coreapi'core_rnd_next

   mov  rdi, [rsp+8]
   mov  rax, [rsp+24]
   mov  rbx, [rsp+16]

   xor  rdx, rdx
   mov  rcx, rbx
   cmp  rcx, rdx
   jle  short labEnd

   push rax
   push rbx

   mov  ebx, dword ptr [rdi+4] // NUM.RE
   mov  esi, dword ptr [rdi]   // NUM.FR             
   mov  eax, ebx
   mov  ecx, 15Ah
   mov  ebx, 4E35h                              
   test eax, eax
   jz   short Lab1
   mul  ebx
Lab1: 
   xchg eax, ecx
   mul  esi
   add  eax, ecx
   xchg eax, esi
   mul  ebx
   add  edx, esi
   add  eax, 1
   adc  edx, 0
   mov  ebx, eax
   mov  esi, edx
   mov  rcx, rdi
   mov  dword ptr [rcx+4], ebx
   mov  eax, esi
   and  eax, 7FFFFFFFh
   mov  dword ptr[rcx] , esi
   cdq
   pop  rcx
   idiv ecx
   pop  rax
labEnd:
   mov  dword ptr [rax], edx
   ret

end

procedure coreapi'core_rnd_nextint

   mov  rdi, [rsp+8]
   mov  rax, [rsp+16]

   push rax
   
   mov  ebx, dword ptr [rdi+4] // NUM.RE
   mov  esi, dword ptr [rdi]   // NUM.FR             
   mov  eax, ebx
   mov  ecx, 15Ah
   mov  ebx, 4E35h                              
   test eax, eax
   jz   short Lab1
   mul  ebx
Lab1: 
   xchg eax, ecx
   imul  esi
   add  eax, ecx
   xchg eax, esi
   imul  ebx
   add  edx, esi
   add  eax, 1
   adc  edx, 0
   mov  ebx, eax
   mov  esi, edx
   mov  rcx, rdi
   mov  dword ptr [rcx+4], ebx
   mov  eax, esi
   and  eax, 7FFFFFFFh
   mov  dword ptr [rcx], esi
   mov  edx, eax
   pop  rax
labEnd:
   mov  dword ptr [rax], edx
   ret

end
