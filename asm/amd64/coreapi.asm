// Object header fields
define elSizeOffset          0004h
define elVMTOffset           000Ch 
define elObjectOffset        000Ch

// --- System Core API  --

define CORE_ET_TABLE     2000Bh

// ; --- API ---

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

procedure coreapi'seh_handler

  push rbp
  mov  rbp, rsp
  // ;** now [EBP+8]=pointer to EXCEPTION_RECORD
  // ;** [EBP+0Ch]=pointer to ERR structure
  // ;** [EBP+10h]=pointer to CONTEXT record
  push rbx
  push rdi
  push rsi
  mov  rbx, [rbp+16]
  test rbx, rbx
  jz   short lab5
  test qword ptr[rbx+8],1h
  jnz  lab5
  test qword ptr[rbx+8],2h
  jz   lab2
//; ...
//; ...
//; ...
  jmp lab5
lab2:
//; PUSH 0
//; PUSH [EBP+8h]
//; PUSH ADDR UN23
//; PUSH [EBP+0Ch]
//; CALL RtlUnwind
//; UN23:
  mov rsi, [rbp + 20h]
//; MOV EDX,[EBP+0Ch]
//; MOV [ESI+0C4h],EDX   // ; esp

  // ; get critical exception handler
  mov  rax, [data : % CORE_ET_TABLE]
  mov  [rsi+0B8h], rax    // ; eip

//; MOV EAX,[EDX+14h]
//; MOV [ESI+0B4h],EAX   // ; ebp
  xor rax, rax
  jmp short lab6
lab5:
  mov rax, 1
lab6:
  pop rsi
  pop rdi
  pop rbx 
  mov rsp, rbp
  pop rbp
  ret
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

// ; inttostr(s,b,t)
procedure coreapi'inttostr

   mov  rbx, [rsp+16]
   mov  rax, [rsp+8]
   mov  esi, dword ptr [rbx]
   mov  rdi, [rsp+24]

   push rbp
   mov  eax, dword ptr [rax]
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

// winsert(target,source,index,len)
procedure coreapi'winsert

  mov  rdx, [rsp+24]
  mov  rax, [rsp+32]
  mov  rdi, [rsp+8]
  mov  ecx, dword ptr [rax]
  mov  rsi, [rsp+16]
  mov  ebx, dword ptr [rdx]
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

// ; wstrtochar(index,str; ebx = 0 if err ; edx - out)
procedure coreapi'wstrtochar

  mov  rsi, [rsp+8]
  mov  rax, [rsp+16]
  mov  ebx, dword ptr [rsi]

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
  mov  rbx, [rsp+8]
  mov  edx, esi
  ret

err:
  xor  ebx, ebx
  ret 

end

// tempObject(ptr), eax - vmt, edx - value
procedure coreapi'tempObject

  mov  rax, [rsp+8]
  mov  [rax-12], rbx
  mov  dword ptr [rax], edx
  mov  dword ptr [rax-4], 0800004h
  ret

end

// wseek(s,subs,index)
procedure coreapi'wseek

  mov  rdi, [rsp+8] // s
  mov  rax, [rsp+24]
  mov  rsi, [rsp+16] // subs
  mov  edx, dword ptr [rax]
  
  mov  ebx, dword ptr [rdi-elSizeOffset]   // get total length  
  and  ebx, 0FFFFFh

  shl  edx, 1
  sub  ebx, edx
  jbe  short labEnd

  add  ebx, 2
  sub  edx, 2

labNext:
  add  edx, 2
  mov  rsi, [rsp+16]
  mov  ecx, dword ptr [rsi-elSizeOffset]
  sub  ebx, 2
  lea  ecx, [ecx-2]
  jz   short labEnd
  and  ecx, 0FFFFFh
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


// ; sadd(dest,sour,sindex,dindex)
procedure coreapi'wadd

  mov  rcx, [rsp+24]
  mov  rax, [rsp+16]
  mov  ecx, dword ptr [ecx]
  mov  rdx, [rsp+32]
  mov  rdi, [rsp+8]

  mov  ebx, dword ptr [rdx]       // ; dst index
  
  shl  ebx, 1
  shl  edx, 1

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

// wsubcopyz(target,index,size,arr)
procedure coreapi'wsubcopyz

  mov  rax, [rsp+32]
  mov  rdx, [rsp+24]
  mov  rsi, [rsp+8]
  mov  ecx, dword ptr [rdx]
  mov  rdi, [rsp+16]
  test ecx, ecx
  mov  ebx, dword ptr [rdi]
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

procedure coreapi'ws_copychars

  mov  rax, [rsp+32]
  mov  rdx, [rsp+24]
  mov  rsi, [rsp+16]
  mov  ecx, dword ptr [rdx]
  mov  rdi, [rsp+8]
  mov  ebx, dword ptr [rsi]

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
