// --- System Core API  --

define CORE_ET_TABLE     2000Bh

define elSizeOffset      0008h

// strtowstr(target,source)
procedure coreapi'strtowstr

  mov  eax, [esp+8]
  mov  edi, [esp+4]
  mov  esi, [eax - elSizeOffset]
  and  esi, 0FFFFFh
  sub  esi, 1
  jz   labEnd

labNext:  

  xor  ebx, ebx
  mov  bl, byte ptr [eax]
  cmp  ebx, 00000080h
  jl   short lab1
  cmp  ebx, 000000E0h
  jl   short lab2
  cmp  ebx, 000000F0h
  jl   short lab3

  mov  ecx, ebx
  shl  ecx, 18
  
  lea  eax, [eax + 1]
  sub  esi, 1  
  mov  bl, byte ptr [eax]
  shl  ebx, 12
  add  ecx, ebx

  xor  ebx, ebx
  lea  eax, [eax + 1]
  sub  esi, 1  
  mov  bl, byte ptr [eax]
  shl  ebx, 6
  add  ecx, ebx

  xor  ebx, ebx
  lea  eax, [eax + 1]
  sub  esi, 1  
  mov  bl, byte ptr [eax]
  add  ecx, ebx
  sub  ecx, 3C82080h
  jmp  short labCont

lab2:  
  mov  ecx, ebx
  shl  ecx, 6

  lea  eax, [eax + 1]
  sub  esi, 1  
  mov  bl, byte ptr [eax]
  add  ecx, ebx
  sub  ecx, 3080h
  jmp  short labCont
  
lab3:
  mov  ecx, ebx
  shl  ecx, 12

  lea  eax, [eax + 1]
  sub  esi, 1  
  mov  bl, byte ptr [eax]
  shl  ebx, 6
  add  ecx, ebx

  xor  ebx, ebx
  lea  eax, [eax + 1]
  sub  esi, 1  
  mov  bl, byte ptr [eax]
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
  mov  word ptr [edi], dx
  add  edi, 2
  
  and  ecx, 03FFh
  add  ecx, 0DC00h
   
labw1:
  mov  word ptr [edi], ecx
  add  edi, 2  

  lea  eax, [eax + 1]
  sub  esi, 1
  jnz  labNext
labEnd:
  mov  ebx, [esp+4]
  mov  edx, edi
  sub  edx, ebx
  shr  edx, 1
  ret
    
end


// strtowstr(target,source)
procedure coreapi'wstrtostr

  mov  eax, [esp+8]
  mov  edi, [esp+4]
  mov  esi, [eax - elSizeOffset]
  and  esi, 0FFFFFh
  sub  esi, 2
  jz   labEnd

labNext:  
  mov  ebx, [eax]
  and  ebx, 0FFFFh
  cmp  ebx, 0D800h
  jb   short lab1

  mov  ecx, ebx
  shl  ecx, 10
  lea  eax, [eax + 2]
  sub  esi, 2
  mov  ebx, dword ptr [eax]
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
  mov  byte ptr [edi], dl
  add  edi, 1
   
  mov  edx, ecx
  shr  edx, 12
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [edi], dl
  add  edi, 1
   
  mov  edx, ecx
  shr  edx, 6
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [edi], dl
  add  edi, 1
   
  mov  edx, ecx
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [edi], dl
  add  edi, 1
  jmp  short labSave

labs2:
  mov  edx, ecx
  shr  edx, 6
  add  edx, 0C0h
  mov  byte ptr [edi], dl
  add  edi, 1
  
  mov  edx, ecx
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [edi], dl
  add  edi, 1
  jmp  short labSave

labs3:
  mov  edx, ecx
  shr  edx, 12
  add  edx, 0E0h
  mov  byte ptr [edi], dl
  add  edi, 1

  mov  edx, ecx
  shr  edx, 6
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [edi], dl
  add  edi, 1
  
  mov  edx, ecx
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [edi], dl
  add  edi, 1
  jmp  short labSave
  
labs1:
  mov  byte ptr [edi], bl
  add  edi, 1
  
labSave:  
  lea  eax, [eax + 2]
  sub  esi, 2
  jnz  labNext

labEnd:
  mov  ebx, [esp+4]
  mov  edx, edi
  sub  edx, ebx
  ret

end

// wsubcopyz(target,index,size,arr)
procedure coreapi'wsubcopyz

  mov  eax, [esp+16]
  mov  edx, [esp+12]
  mov  esi, [esp+4]
  mov  ecx, [edx]
  mov  edi, [esp+8]
  test ecx, ecx
  mov  ebx, [edi]
  jz   short labEnd

labNext:
  mov  edx, [eax + ebx*2]
  mov  word ptr [esi], dx
  add  ebx, 1
  lea  esi, [esi + 2]
  sub  ecx, 1
  jnz  short labNext
  mov  word ptr [esi], cx

labEnd:
  ret

end

// subcopyz(target,index,size,arr)
procedure coreapi'subcopyz

  mov  eax, [esp+16]
  mov  edx, [esp+12]
  mov  esi, [esp+4]
  mov  ecx, [edx]
  mov  edi, [esp+8]
  test ecx, ecx
  mov  ebx, [edi]
  jz   short labEnd

labNext:
  mov  edx, [eax + ebx]
  mov  byte ptr [esi], dl
  add  ebx, 1
  add  esi, 1
  sub  ecx, 1
  jnz  short labNext
  mov  byte ptr [esi], cl

labEnd:
  ret

end

// winsert(target,source,index,len)
procedure coreapi'winsert

  mov  edx, [esp+12]
  mov  eax, [esp+16]
  mov  edi, [esp+4]
  mov  ecx, [eax]
  mov  esi, [esp+8]
  mov  ebx, [edx]
  test ecx, ecx
  jz   short labEnd

labNext:
  mov  edx, [esi]
  mov  word ptr [edi + ebx*2], dx
  add  ebx, 1
  lea  esi, [esi + 2]
  sub  ecx, 1
  jnz  short labNext

labEnd:
  mov  ebx, edi
  ret

end

procedure coreapi'strtoint

  mov  eax, [esp+8]                 // ; radix
  mov  esi, [esp+4]                 // ; get str
  mov  ebx, [eax]
  mov  ecx, [esi-8]
  xor  edx, edx                     // ; clear flag
  and  ecx, 0FFFFFh
  cmp  byte ptr [esi], 2Dh
  lea  ecx, [ecx-1]                 // ; to skip zero
  jnz  short Lab4
  lodsb
  mov  edx, 1                        // ; set flag
  lea  ecx, [ecx-1]                  //  ; to skip minus
Lab4:
  push edx
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
  pop  ebx
  test ebx, ebx                                
  jz   short Lab5
  neg  eax
Lab5:
  mov  edx, eax
  mov  ebx, [esp+4]
  jmp  short Lab3
Lab2:
  add  esp, 4
  xor  eax, eax
Lab3:
  ret

end

// ; inttostr(s,b,t)
procedure coreapi'inttostr

   mov  ebx, [esp+8]
   mov  eax, [esp+4]
   mov  esi, [ebx]
   mov  edi, [esp+12]

   push ebp
   mov  eax, [eax]
   mov  ebp, esp
   xor  ecx, ecx
   push eax
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
   push edx
   add  ecx, 1
   cmp  eax, esi
   jae  short Lab1
Lab5:   
   add  ecx, 2
   push eax
   cmp  esi, 10        
   jnz  short Lab7
   mov  eax, [ebp-4]
   cmp  eax, 0
   jns  short Lab7
   push 0F6h      // to get "-" after adding 0x30
   add  ecx, 1
Lab7:
   sub  ecx, 1
   mov  esi, edi
   mov  edx, 0FFh
Lab2:
   pop  eax
   cmp  eax, 0Ah
   jb   short Lab8
   add  eax, 7
Lab8:
   add  eax, 30h
   and  eax, edx
   mov  byte ptr [esi], al
   add  esi, 1
   sub  ecx, 1
   jnz  short Lab2
   mov  edx, esi
   sub  edx, edi
   lea  esp, [esp+4]
   pop  ebp

   ret

end

procedure coreapi'longtostr

   mov  ebx, [esp+8]
   mov  eax, [esp+4]
   mov  ecx, [ebx]
   mov  edi, [esp+12]

   push ebp
   push [eax+4]
   mov  ebp, esp
   mov  edx, [eax]     // NLO
   mov  eax, [eax+4]   // NHI
   push 0
   or   eax, eax
   jge  short Lab6

   neg  eax 
   neg  edx 
   sbb  eax, 0

Lab6:                 // convert 
   mov  esi, edx      // NLO
   mov  edi, eax      // NHI

Lab1:
   test edi, edi
   jnz  short labConvert
   cmp  esi, ecx
   jb   short Lab5

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
   push 0F6h      // to get "-" after adding 0x30
   add  ecx, 1                  
Lab7:
   mov  esi, [ebp+8]
   mov  ebx, 0FFh
Lab2:
   pop  eax
   cmp  eax, 0Ah
   jb   short Lab8
   add  eax, 7
Lab8:
   add  eax, 30h
   and  eax, ebx
   mov  byte ptr [esi], al
   add  esi, 1
   sub  ecx, 1
   jnz  short Lab2
   mov  edx, esi
   lea  esp, [esp+8]
   pop  ebp
   sub  edx, [esp+12]
   ret
   
end

// ; uinttostr(s,b,t)
procedure coreapi'uinttostr

   mov  ebx, [esp+8]
   mov  eax, [esp+4]
   mov  esi, [ebx]
   mov  edi, [esp+12]

   push ebp
   mov  ebp, esp
   xor  ecx, ecx
   cmp  eax, esi
   jb   short Lab5
Lab1:
   xor  edx, edx
   idiv esi
   push edx
   add  ecx, 1
   cmp  eax, esi
   jae  short Lab1
Lab5:   
   add  ecx, 2
   push eax
   sub  ecx, 1
   mov  esi, edi
   mov  edx, 0FFh
Lab2:
   pop  eax
   cmp  eax, 0Ah
   jb   short Lab8
   add  eax, 7
Lab8:
   add  eax, 30h
   and  eax, edx
   mov  byte ptr [esi], al
   add  esi, 1
   sub  ecx, 1
   jnz  short Lab2
   mov  ecx, esi
   sub  ecx, edi
   pop  ebp

   ret

end

// ; rcopyl (src,tgt)
procedure coreapi'longtoreal

  mov  eax, [esp+4]
  mov  edi, [esp+8]
  fild qword ptr [eax]
  fstp qword ptr [edi]
  ret

end

// ; === internal ===

procedure coreapi'default_handler                                                       

  // ; exit code
  push 0
  call extern 'rt_dlls.Exit

end

procedure coreapi'get_seh_handler

  mov  ecx, data : % CORE_ET_TABLE
  ret

end

procedure coreapi'seh_handler

  push ebp
  mov  ebp, esp
  // ;** now [EBP+8]=pointer to EXCEPTION_RECORD
  // ;** [EBP+0Ch]=pointer to ERR structure
  // ;** [EBP+10h]=pointer to CONTEXT record
  push ebx
  push edi
  push esi
  mov  ebx, [ebp+8]
  test ebx, ebx
  jz   short lab5
  test dword ptr[ebx+4],1h
  jnz  lab5
  test dword ptr[ebx+4],2h
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
  mov esi, [ebp + 10h]
//; MOV EDX,[EBP+0Ch]
//; MOV [ESI+0C4h],EDX   // ; esp

  // ; get critical exception handler
  mov  eax, [data : % CORE_ET_TABLE]
  mov  [esi+0B8h], eax    // ; eip

//; MOV EAX,[EDX+14h]
//; MOV [ESI+0B4h],EAX   // ; ebp
  xor eax, eax
  jmp short lab6
lab5:
  mov eax, 1
lab6:
  pop esi
  pop edi
  pop ebx 
  mov esp, ebp
  pop ebp
  ret

end

