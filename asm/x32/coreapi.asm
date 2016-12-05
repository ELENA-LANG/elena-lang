// --- System Core API  --
define GC_ALLOC	         10001h
define HOOK              10010h
define INIT_RND          10012h
define INIT              10013h
define NEWFRAME          10014h
define INIT_ET           10015h
define OPENFRAME         10019h
define CLOSEFRAME        1001Ah
define NEWTHREAD         1001Bh
define CLOSETHREAD       1001Ch
define EXIT              1001Dh
define CALC_SIZE         1001Fh
define GET_COUNT         10020h
define PREPARE           10027h
define EXITTHREAD        1002Ah

define CORE_OS_TABLE     20009h

define elSizeOffset      0008h
define elVMTSizeOffset   000Ch

// verbs
define EXEC_MESSAGE_ID  085000000h
define START_MESSAGE_ID 0B7000000h

define VERB_MASK        0FF000000h
define SUBJECT_MASK     000FFFFF0h
define INV_SUBJECT_MASK 0FF00000Fh

// ; --- API ---

procedure coreapi'core_callstack_load

  mov  eax, [esp+4]
  mov  ecx, [esp+8]

  mov  edx, [esp]
  xor  ebx, ebx
  mov  esi, ebp

labNext:
  mov  edx, [esi + 4]
  cmp  [esi], 0
  jnz  short labSave
  test edx, edx
  jz   short labEnd
  mov  esi, edx
  jmp  short labNext                              

labSave:
  mov  [eax + ebx * 4], edx
  add  ebx, 1
  cmp  ebx, ecx
  jge  short labEnd
  mov  esi, [esi]
  jmp  short labNext                              
  
labEnd:
  ret  

end

procedure coreapi'core_thread_handler

  // ; exit code
  call code : % EXITTHREAD

end

// eb - index, ecx - result
procedure coreapi'core_getargc

  mov  esi, [data : % CORE_OS_TABLE]
  mov  ebx, [esi]
  ret

end

procedure coreapi'core_getarg

  mov  ebx, [esp + 4]
  mov  edi, [esp + 8]
  mov  ecx, [esp + 12]

  push edi
  mov  esi, [data : % CORE_OS_TABLE]
  mov  eax, [esi + ebx * 4]

labNext:
  mov  edx, [eax]
  and  edx, 0FFh
  mov  byte ptr [edi], dl
  add  eax, 1
  add  edi, 1
  test edx, edx
  jz   short labEnd  
  sub  ecx, 1
  ja   short labNext

labEnd:

  mov  ebx, edi
  pop  edi
  sub  ebx, edi
  
  ret

end

// ; filter_vmt (filter,class,index,len,dump) = len
procedure coreapi'core_filtervmt

   xor  esi, esi
   mov  eax, [esp+8]
   mov  ecx, [eax - elVMTSizeOffset]
   xor  ebx, ebx
   mov  edi, [esp+20]   

labNext:   
   mov  edx, [eax + ebx * 8]
   and  edx, INV_SUBJECT_MASK
   cmp  edx, [esp + 4]
   jnz  labContinue
   mov  edx, [esp+12]
   test edx, edx
   jz   short labAdding
   sub  [esp+12], 1
   jmp  labContinue

labAdding:
   mov  edx, [eax + ebx * 8]
   mov  [edi+esi*4], edx
   add  esi, 1
   mov  edx, [esp+16]
   sub  edx, 1
   jz   labEnd
   mov  [esp+16], edx
   
labContinue:
   add  ebx, 1
   sub  ecx, 1
   jnz  labNext
   
labEnd:
   mov  ebx, esi
   ret
   
end

procedure coreapi'core_rnd_init

  mov  edi, [esp+4]
  call code : % INIT_RND
  mov  [edi], eax 
  mov  [edi+4], edx
  mov  ebx, eax
  ret
  
end

procedure coreapi'core_rnd_next

   mov  edi, [esp+4]
   mov  eax, [esp+12]
   mov  ebx, [esp+8]

   xor  edx, edx
   mov  ecx, ebx
   cmp  ecx, edx
   jle  short labEnd

   push eax
   push ebx

   mov  ebx, [edi+4] // NUM.RE
   mov  esi, [edi]   // NUM.FR             
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
   mov  ecx, edi
   mov  [ecx+4], ebx
   mov  eax, esi
   and  eax, 7FFFFFFFh
   mov  [ecx] , esi
   cdq
   pop  ecx
   idiv ecx
   pop  eax
labEnd:
   mov  [eax], edx
   ret

end

procedure coreapi'core_rnd_nextint

   mov  edi, [esp+4]
   mov  eax, [esp+8]

   push eax
   
   mov  ebx, [edi+4] // NUM.RE
   mov  esi, [edi]   // NUM.FR             
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
   mov  ecx, edi
   mov  [ecx+4], ebx
   mov  eax, esi
   and  eax, 7FFFFFFFh
   mov  [ecx], esi
   mov  edx, eax
   pop  eax
labEnd:
   mov  [eax], edx
   ret

end

// ; === internal ===

// ; rcopyl (eax:src, edi:tgt)
procedure coreapi'longtoreal

  fild qword ptr [eax]
  fstp qword ptr [edi]
  ret

end

// ; rcopyl (eax:src, ecx : base, esi - result)
procedure coreapi'strtoint

  mov  ebx, ecx                     // ; radix
  mov  esi, eax                     // ; get str
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
  mov  ebx, eax
  mov  eax, edi
  jmp  short Lab3
Lab2:
  add  esp, 4
  xor  eax, eax
Lab3:
  ret

end

// ; rcopyl (eax:src, ecx : base, esi - result)
procedure coreapi'wstrtoint

  mov  ebx, ecx                     // ; radix
  mov  esi, eax                     // ; get str
  mov  ecx, [esi-8]
  xor  edx, edx                     // ; clear flag
  and  ecx, 0FFFFFh
  cmp  byte ptr [esi], 2Dh
  lea  ecx, [ecx-2]                 // ; to skip zero
  jnz  short Lab4
  lodsw
  mov  edx, 1                        // ; set flag
  lea  ecx, [ecx-2]                 //  ; to skip minus
Lab4:
  push edx
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
  pop  ebx
  test ebx, ebx                                
  jz   short Lab5
  neg  eax
Lab5:
  mov  ebx, eax
  mov  eax, edi
  jmp  short Lab3
Lab2:
  xor  eax, eax
Lab3:
  ret

end

// ; rcopyl (eax:src, ecx : base, esi - result)
procedure coreapi'strtolong

  push edi
  push ecx
  mov  esi, eax
  mov  ecx, [esi-8]
  xor  edx, edx
  and  ecx, 0FFFFFh

  cmp  byte ptr [esi], 2Dh
  lea  ecx, [ecx-1]
  jnz  short labStart

  lea  esi, [esi+1]
  lea  ecx, [ecx-1]
  mov  edx, 1        // set flag in ebx

labStart:
  push edx           // save sign flag
  xor  edi, edi      // edi   - DHI
  xor  ebx, ebx      // ebx   - DLO

labConvert:
  mov  edx, [esp+4]
  mov  eax, edi
  mul  edx           // DHI * 10
  mov  edi, eax

  mov  eax, ebx
  mov  edx, [esp+4]
  mul  edx           // DLO * 10
  add  edi, edx
  mov  ebx, eax

  xor  eax, eax
  lodsb
  sub  al, 30h
  jb   short labErr
  cmp  al, 9
  ja   short labErr

  add ebx, eax       // DLO + EAX
  adc edi, 0         // DHI + CF

  sub  ecx, 1
  jnz  short labConvert

  pop  eax           // restore flag
  test eax, eax
  jz   short labSave

  not  edi           // invert number
  neg  ebx

labSave:

  mov  edx, edi
  pop  esi
  pop  edi

  mov  eax, edi
  mov  ecx, edx
  jmp  short labEnd

labErr:
  xor  eax, eax
  pop  ebx
  pop  edi

labEnd:
  ret

end

// ; rcopyl (eax:src, ecx : base, esi - result)
procedure coreapi'wstrtolong

  push edi
  push ecx
  mov  esi, eax
  mov  ecx, [esi-8]
  xor  edx, edx
  and  ecx, 0FFFFFh

  cmp  byte ptr [esi], 2Dh
  lea  ecx, [ecx-2]
  jnz  short labStart

  lea  esi, [esi+2]
  lea  ecx, [ecx-2]
  mov  edx, 1        // set flag in ebx

labStart:
  push edx           // save sign flag
  xor  edi, edi      // edi   - DHI
  xor  ebx, ebx      // ebx   - DLO

labConvert:
  mov  edx, [esp+4]
  mov  eax, edi
  mul  edx           // DHI * 10
  mov  edi, eax

  mov  eax, ebx
  mov  edx, [esp+4]
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
  pop  esi
  pop  edi
  mov  eax, edi

  mov  ecx, edx
  jmp  short labEnd

labErr:
  xor  eax, eax
  pop  ebx
  pop  edi

labEnd:
  ret

end

// ; rcopyl (eax:src, ecx : base, ebx - result)
procedure coreapi'strtoreal

  mov   esi, eax
  push  edi
  sub   esp, 12
  xor   edx, edx
  xor   eax, eax
  xor   ebx, ebx
  mov   edi, esp
  stosd
  stosd
  mov   [edi], eax
  mov   ecx, 19

atof1:
  lodsb
  cmp   eax, 32                  // " "
  jz    short atof1
  or    eax, eax
  jnz   short atof2

atoflerr:
  add   esp, 12
  pop   edi
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
  mov   byte ptr [edi+1], dh    // put sign byte in bcd string
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
  fstp  qword ptr[edi]    // store result at specified address
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
  push  eax
  and   eax,0ffh
  jnz   short atof100      // continue if 1st byte of exponent is not terminating 0

scienterr:
  pop   edi
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
  pop   eax               // retrieve exponent sign flag
  rcl   eax,1             // is most significant bit set?
  jnc   short atof200
  neg   edx

atof200:
  jmp   laststep  

atoflend:
   mov  eax, ebx
   ret

end

// ; rcopyl (eax:src, ecx : base, esi - result)
procedure coreapi'wstrtoreal

  mov   esi, eax
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
  fstp  qword ptr[edi]    // store result at specified address
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
  pop   edi
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
  pop   eax               // retrieve exponent sign flag
  rcl   eax,1             // is most significant bit set?
  jnc   short atof200
  neg   edx

atof200:
  jmp   laststep  

atoflend:
   mov  eax, esi
   ret

end

// ; chartostr (eax:char, edi - target, out ecx - length)
procedure coreapi'chartostr

   xor  ecx, ecx
   mov  [edi], ecx
   mov  [edi+4], ecx

   mov  ebx, [eax]
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

   mov  [edi], ecx
   mov  ecx, 4
   ret
   
lab1:
   mov  [edi], ebx
   mov  ecx, 1
   ret

lab2:
   mov  edx, ebx
   shr  edx, 6
   add  edx, 0C0h
   mov  byte ptr [edi], dl
   
   and  ebx, 03Fh
   add  ebx, 00000080h
   mov  byte ptr [edi+1], bl

   mov  ecx, 2
   ret

lab3:
   mov  edx, ebx
   shr  edx, 12
   add  edx, 0E0h
   mov  byte ptr [edi], dl

   mov  edx, ebx
   shr  edx, 6
   and  edx, 03Fh
   add  edx, 00000080h
   mov  byte ptr [edi+1], dl

   and  ebx, 03Fh
   add  ebx, 00000080h
   mov  byte ptr [edi+2], bl

   mov  ecx, 3
   ret

end

// ; rcopyl (eax:char, edi - target, out ecx - length)
procedure coreapi'chartowstr

   xor  ecx, ecx
   mov  [edi], ecx
   mov  [edi+4], ecx

   mov  ebx, [eax]
   cmp  ebx, 010000h
   jl   short lab1

   mov  edx, ebx
   shr  edx, 10
   add  edx, 0D7C0h
   mov  word ptr [edi], dx

   mov  edx, ebx
   and  edx, 03FFh
   add  edx, 0DC00h
   mov  word ptr [edi+2], dx
   mov ecx, 2
   ret
   
lab1:
   mov  [edi], ebx
   mov  ecx, 1
   ret

end

// ; eax - str, ebx - index; eax = 0 if err ; ecx - out
procedure coreapi'strtochar   

  xor  edx, edx
  mov  dl, byte ptr [eax + ebx]
  cmp  edx, 00000080h
  jl   short lab1
  cmp  edx, 000000C2h
  jl   short err
  cmp  edx, 000000E0h
  jl   short lab2
  cmp  edx, 000000F0h
  jl   short lab3
  cmp  edx, 000000F5h
  jl   lab4

err:
  xor  eax, eax
  ret 

lab1:
  mov  ecx, edx
  ret

lab2:  
  mov  ecx, edx
  mov  dl, byte ptr [eax + ebx + 1]
  mov  esi, edx
  and  esi, 0C0h
  cmp  esi, 00000080h
  jnz  err
  shl  ecx, 6
  add  ecx, edx
  sub  ecx, 3080h
  ret
  
lab3:
  mov  ecx, edx
  mov  dl, byte ptr [eax + ebx + 1]
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
  mov  dl, byte ptr [eax + ebx + 2]
  mov  esi, edx
  and  esi, 0C0h
  cmp  esi, 00000080h
  jnz  err
  add  ecx, edx
  sub  ecx, 0E2080h
  ret
  
lab4:
  mov  ecx, edx
  mov  bl, byte ptr [eax + ebx + 1]
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
  mov  dl, byte ptr [eax + ebx + 2]
  mov  esi, edx
  and  esi, 000000C0h
  cmp  esi, 00000080h
  jnz  err

  shl  edx, 6
  add  ecx, edx
  
  xor  edx, edx
  mov  dl, byte ptr [eax + ebx + 3]
  mov  esi, edx
  and  esi, 000000C0h
  cmp  esi, 00000080h
  jnz  err

  add  ecx, edx
  sub  ecx, 3C82080h
  ret
  
end                                                       

// ; eax - str, out esi - char, or eax = 0
procedure coreapi'wstrtochar

  mov  esi, dword ptr [eax + ebx * 2]
  and  esi, 0FFFFh
  cmp  esi, 0D800h
  jl   short lab1
  cmp  esi, 0DBFFh
  jg   short err

  mov  ecx, esi
  shl  ecx, 10
  mov  esi, dword ptr [eax + ebx * 2 + 2]
  and  esi, 0FFFFh
  cmp  esi, 0DC00h
  jl   short lab2
  cmp  esi, 0DFFFh
  jg   short err
  
lab2:
  add  ecx, ebx
  sub  ecx, 35FDC00h
  ret  

lab1:
  mov   ecx, esi
  ret

err:
  xor  eax, eax
  ret 

end

procedure coreapi'inttostr

   mov  esi, ebx
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
   mov  ecx, esi
   sub  ecx, edi
   lea  esp, [esp+4]
   pop  ebp

   ret

end

procedure coreapi'inttowstr

   push ebp
   mov  eax, [eax]
   mov  ebp, esp
   xor  ecx, ecx
   push eax
   cmp  eax, 0
   jns  short Lab6
   neg  eax
Lab6:
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
   mov  eax, [ebp-4]
   cmp  eax, 0
   jns  short Lab7
   push 0F6h      // to get "-" after adding 0x30
   add  ecx, 2
Lab7:
   mov  esi, edi
   mov  edx, 0FFh
   sub  ecx, 2             // to skip zero
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
   sub  ecx, 2
   jnz  short Lab2
   mov  ecx, esi
   sub  ecx, edi
   shr  ecx, 1
   lea  esp, [esp+4]
   pop  ebp

   ret
   
end

procedure coreapi'longtostr

   mov  ecx, ebx
   push edi
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
   mov  ecx, esi
   sub  ecx, [ebp+8]
   lea  esp, [esp+8]
   pop  ebp
   pop  edi
   ret
   
end

procedure coreapi'longtowstr

   mov  ecx, ebx
   push edi
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
   mov  word ptr [esi], ax
   add  esi, 2
   sub  ecx, 1
   jnz  short Lab2
   mov  ecx, esi
   sub  ecx, [ebp+8]
   shr  ecx, 2 
   lea  esp, [esp+8]
   pop  ebp
   pop  edi
   ret
   
end

procedure coreapi'realtostr

   mov   ecx, eax
   push  ebp
   mov   ebp, esp

   sub   esp, 52  

   push  edi
   
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

   cmp   byte ptr[esi-1], 20h // test if we insert space
   jnz   short ftoa60
   lea   edi, [edi-1]         // erase it

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
   mov   byte ptr [edi], al
   lea   esi, [esi+1]
   lea   edi, [edi+1]
   sub   ecx, 1
   jnz   short ftoa8Next
ftoa8End:
   mov   ecx,ebx
   or    ecx,ecx
   jz    short ftoa9
   mov   eax,46               // "."
   stosb

ftoa9Next:                    // copy required decimal digits
   movzx  eax, byte ptr [esi]
   mov   byte ptr [edi], al
   lea   esi, [esi+1]
   lea   edi, [edi+1]
   sub   ecx, 1
   jnz   short ftoa9Next
ftoa9:
   jmp   finish

scientific:
   movsb                      // insert sign

   cmp   byte ptr[esi-1], 20h // test if we insert space
   jnz   short ftoa90
   lea   edi, [edi-1]         // erase it

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
   stosb

ftoa10Next:                    // copy the decimal digits
   movzx  eax, byte ptr [esi]
   mov   byte ptr [edi], al
   lea   esi, [esi+1]
   lea   edi, [edi+1]
   sub   ecx, 1
   jnz   short ftoa10Next

   mov   eax,69                // "E"
   stosb
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
   stosb                      // insert proper sign

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
   mov   byte ptr [edi], bl
   lea   edi, [edi+1]
   shr   eax, 8
   mov   bl, al
   mov   byte ptr [edi], bl
   lea   edi, [edi+1]
   pop   eax
   shr   eax,8                // get the tens & units in al
   div   cl                   // tens in al, units in AH
   add   eax,3030h            // convert to characters

   mov   bl, al               // insert them 
   mov   byte ptr [edi], bl
   lea   edi, [edi+1]
   shr   eax, 8
   mov   bl, al
   mov   byte ptr [edi], bl
   lea   edi, [edi+1]

finish:
   cmp   byte ptr [edi-1], 48 // '0'
   jnz   short finish1
   lea   edi, [edi-1]
   jmp   short finish

finish1:
   cmp   byte ptr [edi-1], 46 // '.'
   jnz   short finish2
   lea   edi, [edi+1]

finish2:
   mov   ebx, edi
   pop   edi
   add   esp, 52
   pop   ebp

   sub   ebx, edi
   mov   ecx, ebx

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

  ret
end

procedure coreapi'realtowstr

   mov   ecx, eax
   push  ebp
   mov   ebp, esp

   sub   esp, 52  

   push  edi
   
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
   mov   ebx, edi
   pop   edi
   add   esp, 52
   pop   ebp

   sub   ebx, edi
   mov   ecx, ebx

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

  ret
end

procedure coreapi'wstrtostr

  push edi
  push eax
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
  mov  ecx, edi
  pop  eax
  pop  edi
  sub  ecx, edi
  ret

end

procedure coreapi'strtowstr

  push edi
  push eax
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
  mov  ecx, edi
  pop  eax
  pop  edi
  sub  ecx, edi
  shr  ecx, 1
  ret
    
end

procedure coreapi'subcopy

  mov  esi, edi
  test ecx, ecx
  jz   short labEnd

labNext:
  mov  edx, [eax + ebx]
  mov  byte ptr [esi], dl
  add  ebx, 1
  add  esi, 1
  sub  ecx, 1
  jnz  short labNext

labEnd:
  ret

end

procedure coreapi'subcopyz

  mov  esi, edi
  test ecx, ecx
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

procedure coreapi'insert

  mov  esi, eax
  test ecx, ecx
  jz   short labEnd

labNext:
  mov  edx, [esi]
  mov  byte ptr [edi + ebx], dl
  add  ebx, 1
  lea  esi, [esi + 1]
  sub  ecx, 1
  jnz  short labNext

labEnd:
  ret

end

procedure coreapi'wsubcopy

  mov  esi, edi
  test ecx, ecx
  jz   short labEnd

labNext:
  mov  edx, [eax + ebx*2]
  mov  word ptr [esi], dx
  add  ebx, 1
  lea  esi, [esi + 2]
  sub  ecx, 1
  jnz  short labNext

labEnd:
  ret

end

procedure coreapi'wsubcopyz

  mov  esi, edi
  test ecx, ecx
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

procedure coreapi'winsert

  mov  esi, eax
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
  ret

end

procedure coreapi'nsubcopy

  mov  esi, edi
  test ecx, ecx
  jz   short labEnd

labNext:
  mov  edx, [eax + ebx * 4]
  mov  [esi], edx
  add  ebx, 1
  lea  esi, [esi + 4]
  sub  ecx, 1
  jnz  short labNext

labEnd:
  ret

end

procedure coreapi'ninsert

  mov  esi, eax
  test ecx, ecx
  jz   short labEnd

labNext:
  mov  edx, [esi]
  mov  [edi + ebx*4], edx
  add  ebx, 1
  lea  esi, [esi + 4]
  sub  ecx, 1
  jnz  short labNext

labEnd:
  ret

end

procedure coreapi'sequal

  mov  esi, edi              // s2
  mov  edx, eax              // s1
  mov  ecx, [edx-8]          // s1.length
  mov  ebx, [esi-8]
  and  ecx, 0FFFFFh 
  and  ebx, 0FFFFFh 
  mov  eax, 0
  cmp  ecx, ebx              // compare with s2.length
  jnz  short Lab1
Lab2:
  mov  ebx, [esi]
  cmp  bl,  byte ptr [edx]
  jnz  short Lab1
  lea  esi, [esi+1]
  lea  edx, [edx+1]
  sub  ecx, 1
  jnz  short Lab2
  mov  eax, 1
Lab1:
  mov  ebx, eax
  mov  eax, edi
  ret

end

procedure coreapi'wequal

  mov  esi, edi              // s2
  mov  edx, eax              // s1
  mov  ecx, [edx-8]          // s1.length
  mov  ebx, [esi-8]
  and  ecx, 0FFFFCh 
  and  ebx, 0FFFFCh 
  mov  eax, 0
  cmp  ecx, ebx              // compare with s2.length
  jnz  short Lab1
Lab2:
  mov  ebx, [esi]
  cmp  ebx,  [edx]
  jnz  short Lab1
  lea  esi, [esi+4]
  lea  edx, [edx+4]
  sub  ecx, 4
  jnz  short Lab2
  mov  eax, 1
Lab1:
  mov  ebx, eax
  mov  eax, edi
  ret

end

procedure coreapi'wless

  mov  esi, edi              // s2
  mov  edx, eax              // s1
  mov  ecx, [edx-8]          // s1 length
  mov  eax, 0
  and  ecx, 0FFFFFh
Lab2:
  mov  ebx, [edx]              // s1[i] 
  cmp  bx, word ptr [esi]      // compare s2[i] with 
  jb   short Lab1
  nop
  nop
  ja   short LabEnd
  lea  esi, [esi+2]
  lea  edx, [edx+2]
  sub  ecx, 2
  jnz  short Lab2
  nop
  nop
  jmp  short LabEnd
  
Lab1:
  mov  eax, 1

LabEnd:
  mov  ebx, eax
  mov  eax, edi
  ret

end

procedure coreapi'sless

  mov  esi, edi              // s2
  mov  edx, eax              // s1
  mov  ecx, [edx-8]          // s1 length
  mov  eax, 0
  and  ecx, 0FFFFFh
Lab2:
  mov  ebx, [edx]              // s1[i] 
  cmp  bl, byte ptr [esi]      // compare s2[i] with 
  jb   short Lab1
  nop 
  nop
  ja   short LabEnd
  lea  esi, [esi+1]
  lea  edx, [edx+1]
  sub  ecx, 1
  jnz  short Lab2
  nop
  nop
  jmp  short LabEnd

Lab1:
  mov  eax, 1

LabEnd:
  mov  ebx, eax
  mov  eax, edi
  ret

end

procedure coreapi'sseek

  push eax
  mov  edx, ebx     // index
  mov  esi, eax     // subs

  push edi
  
  mov  ebx, [edi-8]   // get total length  
  and  ebx, 0FFFFFh
  
  sub  ebx, edx
  jbe  short labEnd

  add  ebx, 1
  sub  edx, 1

labNext:
  add  edx, 1
  mov  esi, [esp+4]
  mov  ecx, [esi-8]
  sub  ebx, 1
  lea  ecx, [ecx-1]
  jz   short labEnd
  and  ecx, 0FFFFFh
  cmp  ebx, ecx
  jb   short labEnd
  mov  edi, [esp]
  add  edi, edx

labCheck:
  mov  eax, [edi]
  cmp  al, byte ptr [esi]
  jnz  short labNext
  lea  edi, [edi+1]
  lea  esi, [esi+1]
  sub  ecx, 1
  jnz  short labCheck
  pop  edi
  pop  eax
  mov  ebx, edx
  jmp  short labEnd2

labEnd:
  pop  edi
  pop  eax
  mov  ebx, -1
labEnd2:
  ret

end

procedure coreapi'wseek

  push eax
  mov  edx, ebx     // index
  mov  esi, eax     // subs

  push edi
  
  mov  ebx, [edi-8]   // get total length  
  and  ebx, 0FFFFFh

  shl  edx, 1
  sub  ebx, edx
  jbe  short labEnd

  add  ebx, 2
  sub  edx, 2

labNext:
  add  edx, 2
  mov  esi, [esp+4]
  mov  ecx, [esi-8]
  sub  ebx, 2
  lea  ecx, [ecx-2]
  jz   short labEnd
  and  ecx, 0FFFFFh
  cmp  ebx, ecx
  jb   short labEnd
  mov  edi, [esp]
  add  edi, edx

labCheck:
  mov  eax, [edi]
  cmp  ax, word ptr [esi]
  jnz  short labNext
  lea  edi, [edi+2]
  lea  esi, [esi+2]
  sub  ecx, 2
  jnz  short labCheck
  pop  edi
  pop  eax
  shr  edx, 1
  mov  ebx, edx
  jmp  short labEnd2

labEnd:
  pop  edi
  pop  eax
  mov  ebx, -1
labEnd2:
  ret

end

procedure coreapi'sadd

  mov  edx, ebx         // ; dst index
  mov  esi, ecx         // ; src index
  
  mov  ebx, [eax-8]
  and  ebx, 0FFFFFh
  add  edx, edi
  sub  ecx, ebx
  add  esi, eax
  
labNext2:
  mov  ebx, [esi]
  mov  byte ptr [edx], bl
  lea  esi, [esi+1]
  lea  edx, [edx+1]
  add  ecx, 1
  jnz  short labNext2

  ret
  
end

procedure coreapi'wadd

  shl  ebx, 1
  shl  ecx, 1

  mov  edx, ebx         // ; dst index
  mov  esi, ecx         // ; src index
  
  mov  ebx, [eax-8]
  and  ebx, 0FFFFFh

  add  edx, edi
  sub  ecx, ebx
  add  esi, eax
  
labNext2:
  mov  ebx, [esi]
  mov  word ptr [edx], ebx
  lea  esi, [esi+2]
  lea  edx, [edx+2]
  add  ecx, 2
  jnz  short labNext2

  ret
  
end

// ; wslen_ch - ecx - len, eax - charr, esi - result 
procedure coreapi'wslen_ch

   xor  ebx, ebx

labNext:
   mov  edx, [eax]
   cmp  edx, 010000h
   jl   short lab1

   add  ebx, 2
   lea  eax, [eax + 4]
   sub  ecx, 1
   jnz  short labNext
   ret   
   
lab1:
   add  ebx, 1
   lea  eax, [eax + 4]
   sub  ecx, 1
   jnz  short labNext
   ret   

end

procedure coreapi'ws_copychars

  push edi
  lea  esi, [eax + ebx * 4]

labNext:
  mov  ebx, [esi]
  cmp  ebx, 010000h
  jl   short lab1

  mov  edx, ebx
  shr  edx, 10
  add  edx, 0D7C0h
  mov  word ptr [edi], dx
  add  edi, 2

  and  ebx, 03FFh
  add  ebx, 0DC00h
   
lab1:
  mov  word ptr [edi], bx
  add  edi, 2
  lea  esi, [esi + 4]
  sub  ecx, 1
  jnz  short labNext

  mov  ebx, edi
  pop  edi
  sub  ebx, edi
  shr  ebx, 1

  ret

end

// ; slen_ch - ecx - len, eax - charr, esi - result 
procedure coreapi'slen_ch

   xor  ebx, ebx

labNext:
   mov  edx, [eax]
   cmp  edx, 00000080h
   jl   short lab1
   cmp  edx, 0800h
   jl   short lab2
   cmp  edx, 10000h
   jl   short lab3
   
   add  ebx, 4
   lea  eax, [eax + 4]
   sub  ecx, 1
   jnz  short labNext
   ret
   
lab1:
   add  ebx, 1
   lea  eax, [eax + 4]
   sub  ecx, 1
   jnz  short labNext
   ret

lab2:
   add  ebx, 2
   lea  eax, [eax + 4]
   sub  ecx, 1
   jnz  short labNext
   ret

lab3:
   add  ebx, 3
   lea  eax, [eax + 4]
   sub  ecx, 1
   jnz  short labNext
   ret

   ret

end

procedure coreapi's_copychars

  push edi
  lea  esi, [eax + ebx * 4]

labNext:
  mov  ebx, [esi]
  
  cmp  ebx, 00000080h
  jl   short labs1
  cmp  ebx, 0800h
  jl   short labs2
  cmp  ebx, 10000h
  jl   short labs3
  
  mov  edx, ebx
  shr  edx, 18
  add  edx, 000000F0h 
  mov  byte ptr [edi], dl
  add  edi, 1
   
  mov  edx, ebx
  shr  edx, 12
  and  edx, 0000003Fh
  add  edx, 00000080h
  mov  byte ptr [edi], dl
  add  edi, 1
   
  mov  edx, ebx
  shr  edx, 6
  and  edx, 0000003Fh
  add  edx, 00000080h
  mov  byte ptr [edi], dl
  add  edi, 1
   
  mov  edx, ebx
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [edi], dl
  add  edi, 1
  jmp  labSave

labs2:
  mov  edx, ebx
  shr  edx, 6
  add  edx, 000000C0h
  mov  byte ptr [edi], dl
  add  edi, 1
  
  mov  edx, ebx
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [edi], dl
  add  edi, 1
  jmp  short labSave

labs3:
  mov  edx, ebx
  shr  edx, 12
  add  edx, 000000E0h
  mov  byte ptr [edi], dl
  add  edi, 1

  mov  edx, ebx
  shr  edx, 6
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [edi], dl
  add  edi, 1
  
  mov  edx, ebx
  and  edx, 03Fh
  add  edx, 00000080h
  mov  byte ptr [edi], dl
  add  edi, 1
  jmp  short labSave
  
labs1:
  mov  byte ptr [edi], bl
  add  edi, 1

labSave:
  lea  esi, [esi + 4]
  sub  ecx, 1
  jnz  labNext

  mov  ebx,  edi
  pop  edi
  sub  ebx, edi

  ret

end

procedure coreapi'strtochararray

  push eax
  push edi
  push ebx
  lea  edi, [edi + ebx * 4]

labStart:
  xor  ebx, ebx
  mov  bl, byte ptr [eax]
  add  eax, 1
  cmp  ebx, 00000080h
  jl   short lab1
  cmp  ebx, 000000E0h
  jl   short lab2
  cmp  ebx, 000000F0h
  jl   short lab3
  
lab4:
  mov  edx, ebx
  mov  bl, byte ptr [eax]
  add  eax, 1
  mov  edx, ebx
  shl  edx, 18
  shl  ebx, 12
  add  edx, ebx

  xor  ebx, ebx
  mov  bl, byte ptr [eax]
  add  eax, 1
  shl  ebx, 6
  add  edx, ebx
  
  xor  ebx, ebx
  mov  bl, byte ptr [eax]
  add  eax, 1
  
  add  edx, ebx
  sub  edx, 3C82080h
  jmp  labSave  

lab2:  
  mov  edx, ebx
  mov  bl, byte ptr [eax]
  add  eax, 1

  shl  edx, 6
  add  edx, ebx
  sub  edx, 3080h
  jmp  short labSave  
  
lab3:
  mov  edx, ebx
  mov  bl, byte ptr [eax]
  add  eax, 1
  shl  edx, 12
  shl  ebx, 6
  add  edx, ebx
  xor  ebx, ebx
  mov  bl, byte ptr [eax]
  add  eax, 1
  add  edx, ebx
  sub  edx, 0E2080h
  jmp  short labSave  
  
lab1:
  mov  edx, ebx

labSave:
  mov  [edi], edx
  add  edi, 4
  sub  ecx, 1
  jnz  labStart

  mov  ecx, edi
  pop  ebx
  pop  edi
  pop  eax
  sub  ecx, edi
  shr  ecx, 2
  sub  ecx, ebx

  ret

end

procedure coreapi'wstrtochararray

  push eax
  push edi
  push ebx
  lea  edi, [edi + ebx * 4]

labStart:
  mov  ebx, dword ptr [eax]
  add  eax, 2

  and  ebx, 0FFFFh
  cmp  ebx, 0D800h
  jl   short lab1

  shl  ebx, 10
  mov  edx, dword ptr [eax]
  add  eax, 2
  and  edx, 0FFFFh
  add  ebx, edx
  sub  ebx, 35FDC00h

lab1:
  mov   edx, ebx

labSave:
  mov  [edi], edx
  add  edi, 4
  sub  ecx, 1
  jnz  labStart

  mov  ecx, edi
  pop  ebx
  pop  edi
  pop  eax
  sub  ecx, edi
  shr  ecx, 2
  sub  ecx, ebx

  ret

end

// ; [esp+4] -  offset, edi -target, ebx - index, ecx - length
procedure coreapi'nmove

  test ecx, ecx
  jz   short labEnd

  mov  esi, [esp+4]
  cmp  esi, 0
  jl   short labDelete

  add  ebx, ecx
  sub  ebx, 1

  add  esi, ebx
  shl  esi, 2

  add  esi, edi
  shl  ebx, 2
  add  ebx, edi
  
labNext:
  mov  edx, dword ptr [ebx]
  mov  dword ptr [esi], edx
  sub  ebx, 4
  sub  esi, 4
  sub  ecx, 1
  jnz  short labNext

labEnd:
  ret

labDelete:
  add  esi, ebx
  shl  esi, 2

  add  esi, edi
  shl  ebx, 2
  add  ebx, edi

labNext2:
  mov  edx, dword ptr [ebx]
  mov  dword ptr [esi], edx
  add  ebx, 4
  add  esi, 4
  sub  ecx, 1
  jnz  short labNext2
  ret

end

// ; [esp+4] -  offset, edi -target, esi - index, ecx - length
procedure coreapi'move

  test ecx, ecx
  jz   short labEnd

  mov  edx, [esp+4]
  cmp  edx, 0
  jl   short labDelete

  add  ebx, ecx
  sub  ebx, 1

  add  edx, ebx

  add  edx, edi
  add  ebx, edi

labNext:
  mov  esi, dword ptr [esi]
  mov  dword ptr [edx], esi
  sub  ebx, 1
  sub  edx, 1
  sub  ecx, 1
  jnz  short labNext

labEnd:
  ret

labDelete:
  add  edx, ebx

  add  edx, edi
  add  ebx, edi

labNext2:
  mov  esi, dword ptr [ebx]
  mov  dword ptr [edx], esi
  add  ebx, 1
  add  edx, 1
  sub  ecx, 1
  jnz  short labNext2
  ret

end

// ; eax - sour, edi - dest, ecx - length, esi offset
procedure coreapi's_encode

  push edi
  add  eax, ebx
  push eax
  push ebx
  
labNext:
  xor  ebx, ebx
  mov  bl, byte ptr [eax]
  add  eax, 1
  cmp  ebx, 00000080h
  jl   lab1
  cmp  ebx, 000000C2h
  jl   err2
  cmp  ebx, 000000E0h
  jl   short lab2
  cmp  ebx, 000000F0h
  jl   lab3
  cmp  ebx, 000000F5h
  jl   lab4

lab2:  
  sub  ecx, 2
  jb   short err
  mov  esi, ebx
  mov  bl, byte ptr [eax]
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
  mov  bl, byte ptr [eax]
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
  mov  bl, byte ptr [eax]
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
  mov  bl, byte ptr [eax]
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
  mov  bl, byte ptr [eax]
  add  eax, 1
  mov  edx, ebx
  and  edx, 000000C0h
  cmp  edx, 00000080h
  jnz  err2

  shl  ebx, 6
  add  esi, ebx
  
  xor  ebx, ebx
  mov  bl, byte ptr [eax]
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
  mov  [edi], esi
  add  edi, 4

  test ecx, ecx
  jnz  labNext

err:
  pop  ebx
  mov  edx, eax
  pop  eax
  sub  edx, eax
  add  ebx, edx
  mov  ecx, edi
  pop  edi
  sub  ecx, edi
  shr  ecx, 2

  ret
  
err2:
  add  esp, 12
  xor  eax, eax
  ret 

end

procedure coreapi'ws_encode

  push edi
  add  eax, ebx
  push eax
  push ebx

labNext:
  mov  ebx, dword ptr [eax]
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
  mov  ebx, dword ptr [eax]
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
  mov  [edi], esi
  add  edi, 4

  test ecx, ecx
  jnz  labNext

err:
  pop  ebx
  mov  edx, eax
  pop  eax
  sub  edx, eax
  shr  edx, 1
  add  ebx, edx
  mov  ecx, edi
  pop  edi
  sub  ecx, edi
  shr  ecx, 2

  ret
  
err2:
  add  esp, 12
  xor  eax, eax
  ret 

end

procedure coreapi's_decode

   push edi
   lea  eax, [eax + ebx * 4]
   push eax
   push ebx

labNext:
   mov  ebx, [eax]
   cmp  ebx, 00000080h
   jl   short lab1
   cmp  ebx, 0800h
   jl   short lab2
   cmp  ebx, 10000h
   jl   short lab3

   sub  ecx, 4
   jb   short err
   mov  edx, ebx
   and  edx, 03Fh
   add  edx, 00000080h
   mov  byte ptr [edi], dl
   add  edi, 1

   mov  edx, ebx
   shr  edx, 12
   and  edx, 0000003Fh
   add  edx, 00000080h
   mov  byte ptr [edi], dl
   add  edi, 1
   
   mov  edx, ebx
   shr  edx, 6
   and  edx, 0000003Fh
   add  edx, 00000080h
   mov  byte ptr [edi], dl
   add  edi, 1
    
   mov  edx, ebx
   and  edx, 03Fh
   add  edx, 00000080h
   mov  byte ptr [edi], dl
   add  edi, 1
   jmp  labSave

lab2:
   sub  ecx, 2
   jb   short err

   mov  edx, ebx
   shr  edx, 6
   add  edx, 0C0h
   mov  byte ptr [edi], dl
   add  edi, 1
   
   and  ebx, 03Fh
   add  ebx, 00000080h
   mov  byte ptr [edi], bl
   add  edi, 1
   jmp  labSave

lab3:
   sub  ecx, 2
   jb   short err

   mov  edx, ebx
   shr  edx, 12
   add  edx, 0E0h
   mov  byte ptr [edi], dl
   add  edi, 1

   mov  edx, ebx
   shr  edx, 6
   and  edx, 03Fh
   add  edx, 00000080h
   mov  byte ptr [edi], dl
   add  edi, 1

   and  ebx, 03Fh
   add  ebx, 00000080h
   mov  byte ptr [edi], bl
   add  edi, 1
   jmp  short labSave
   
lab1:
   mov  byte ptr [edi], bl
   add  edi, 1
   sub  ecx, 1

labSave:
   add  eax, 4
   test ecx, ecx
   jnz  labNext

err:
   pop  ebx
   mov  edx, eax
   pop  eax
   sub  edx, eax
   shr  edx, 2
   add  ebx, edx
   mov  ecx, edi
   pop  edi
   sub  ecx, edi

   ret

end

procedure coreapi'ws_decode

   push edi
   lea  eax, [eax + ebx * 4]
   push eax
   push ebx

labNext:
   mov  ebx, [eax]
   cmp  ebx, dword ptr 128
   jl   short labCH1

   cmp  ebx, 0800h
   jl   short lab1

   sub  ecx, 2
   jl   short err

   mov  edx, ebx
   shr  edx, 10
   add  edx, 0D7C0h
   mov  word ptr [edi], dx
   add  edi, 2

   mov  edx, ebx
   and  edx, 03FFh
   add  edx, 0DC00h
   mov  word ptr [edi], dx
   add  edi, 2
   jmp  short labSave
   
lab1:
   mov  word ptr [edi], bx
   add  edi, 2
   sub  ecx, 1
   jmp  short labSave

labCH1:
   mov  word ptr [edi], bx
   add  edi, 2
   sub  ecx, 1
      
labSave:
   add  eax, 4
   test ecx, ecx
   jnz  labNext

err:
   pop  ebx
   mov  edx, eax
   pop  eax
   sub  edx, eax
   shr  edx, 2
   add  ebx, edx
   mov  ecx, edi
   pop  edi
   sub  ecx, edi
   shr  ecx, 1

   ret
  
err2:
   add  esp, 12
   xor  eax, eax
   ret    

end

procedure coreapi'strcharlen

  push eax

  xor  edx, edx
  xor  ebx, ebx

labNext:
  mov  dl, byte ptr [eax]
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
  pop  eax
  ret
  
lab2:
  add  ebx, 1
  add  eax, 2
  sub  ecx, 2
  jnz  short labNext
  pop  eax
  ret
  
lab3:
  add  ebx, 1
  add  eax, 3
  sub  ecx, 3
  jnz  short labNext
  pop  eax
  ret
  
lab4:
  add  ebx, 1
  add  eax, 4
  sub  ecx, 4
  jnz  short labNext
  pop  eax
  ret

end

procedure coreapi'wstrcharlen

  push eax
  xor  ebx, ebx

labNext:
  mov  edx, dword ptr [eax]
  and  edx, 0FFFFh
  cmp  edx, 0D800h
  jl   short lab1
  
  add  ebx, 1
  add  eax, 4
  sub  ecx, 2
  jnz  short labNext
  pop  eax
  ret

lab1:
  add  ebx, 1
  add  eax, 2
  sub  ecx, 1
  jnz  short labNext
  pop  eax
  ret

end

// ; rcopyl (eax:char, edi - target)
procedure coreapi'chartoshorts

   cmp  ecx, 010000h
   jl   short lab1
   
   mov  edx, ecx
   shr  edx, 10
   add  edx, 0D7C0h
   mov  word ptr [edi + ebx * 2], dx
   add  ebx, 1

   mov  edx, ecx
   and  edx, 03FFh
   add  edx, 0DC00h
   mov  word ptr [edi+ebx * 2], dx
   add  ebx, 1
   mov  ecx, 2
   ret
   
lab1:
   mov  [edi + ebx * 2], ecx
   add  ebx, 1
   mov  ecx, 1
   ret

end

// ; (esi - index, ecx - char, edi - target ; out : ecx : length)
procedure coreapi'chartobytes

   cmp  ecx, 00000080h
   jl   short lab1
   cmp  ecx, 0800h
   jl   short lab2
   cmp  ecx, 10000h
   jl   short lab3
   
   mov  edx, ecx
   and  edx, 03Fh
   add  edx, 00000080h
   mov  byte ptr [edi + ebx], dl
   add  ebx, 1

   mov  edx, ecx
   shr  edx, 12
   and  edx, 0000003Fh
   add  edx, 00000080h
   mov  byte ptr [edi + ebx], dl
   add  ebx, 1
   
   mov  edx, ecx
   shr  edx, 6
   and  edx, 0000003Fh
   add  edx, 00000080h
   mov  byte ptr [edi + ebx], dl
   add  ebx, 1
    
   mov  edx, ecx
   and  edx, 03Fh
   add  edx, 00000080h
   mov  byte ptr [edi + ebx], dl
   add  ebx, 1
   mov  ecx, 4
   ret
   
lab1:
   mov  byte ptr [edi + ebx], cl
   add  ebx, 1
   mov  ecx, 1
   ret

lab2:
   mov  edx, ecx
   shr  edx, 6
   add  edx, 0C0h
   mov  byte ptr [edi + ebx], dl
   add  ebx, 1
   
   and  ecx, 03Fh
   add  ecx, 00000080h
   mov  byte ptr [edi+ebx], cl
   add  ebx, 1
   mov  ecx, 2
   ret

lab3:
   mov  edx, ecx
   shr  edx, 12
   add  edx, 0E0h
   mov  byte ptr [edi + ebx], dl
   add  ebx, 1
   
   mov  edx, ecx
   shr  edx, 6
   and  edx, 03Fh
   add  edx, 00000080h
   mov  byte ptr [edi+ebx], dl
   add  ebx, 1

   and  ecx, 03Fh
   add  ecx, 00000080h
   mov  byte ptr [edi+ebx], cl
   add  ebx, 1
   mov  ecx, 3
   ret

end

procedure coreapi'rpi

  fldpi
  fstp  qword ptr [edi]    // store result 
  ret

end

procedure coreapi'rsqrt

  fld   qword ptr [eax]  
  fsqrt
  fstp  qword ptr [edi]    // store result 
  ret

end

// ; start_thread(param)
procedure coreapi'start_thread

  mov  eax, [esp + 4]
           
  // ; init thread
  call code : % NEWTHREAD
  mov  ecx, 1
  test eax, eax
  jz   short lErr

  push  eax
  mov   ecx, EXEC_MESSAGE_ID
  mov   esi, [eax - 4]
  call [esi + 4]
  
  // ; close thread
  call code : % CLOSETHREAD

  xor  eax, eax

lErr:
  
  ret 4
end

procedure coreapi'openframe

  mov ebx, code : % OPENFRAME
  jmp ebx

end

procedure coreapi'closeframe

  mov ebx, code : % CLOSEFRAME
  jmp ebx

end

procedure coreapi'default_handler                                                       

  // ; exit code
  call code : % EXIT

end

// ; console_vm_entry()
procedure coreapi'vm_console_entry

  push ebx
  push ecx
  push edi
  push esi
  push ebp
  
  call code : % INIT

  pop  ebp
  pop  esi
  pop  edi
  pop  ecx
  pop  ebx
                                                           
  ret

end

procedure coreapi'allocate

  push ebx
  shl  ebx, 2
  call code : %CALC_SIZE
  nop
  nop
  call code : %GC_ALLOC
  xor  edx, edx
  pop  ecx
labCopy:
  sub  ecx, 1
  mov  [eax+ecx*4], edx
  test ecx, ecx
  jnz  short labCopy

  ret

end

procedure coreapi'reallocate

  push ebx
  shl  ebx, 2
  push eax
  call code : %CALC_SIZE
  nop
  nop
  call code : %GC_ALLOC
  mov  edi, eax
  pop  eax
  call code : %GET_COUNT
  xor  esi, esi
labCopy:
  mov  edx, [eax+esi*4]
  mov  [edi+esi*4], edx
  add  esi, 1
  cmp  esi, ebx
  jb   short labCopy
  pop  ebx
  xor  edx, edx
labFill:
  mov  [edi+esi*4], edx
  add  esi, 1
  cmp  esi, ebx
  jb   short labFill

  ret

end

procedure coreapi'alloc_index

  mov  eax, [stat : "$elena'@referencetable"]
  
  test eax, eax
  jnz  short labStart

  mov  ebx, 020h
  call code : "$native'coreapi'allocate"

  mov  [stat : "$elena'@referencetable"], eax

labStart:
  // ; try to reuse existing slots
  call code : %GET_COUNT
  mov  ecx, ebx
  xor  ebx, ebx
labNext:
  cmp  [eax+ebx*4], 0
  jz   short labReuse
  add  ebx, 1
  cmp  ebx, ecx
  jb   short labNext                                                                                               

  // ; if no place reallocate the reference table
  call code : %GET_COUNT
  add  ebx, 10h

  call code : "$native'coreapi'reallocate"

  mov  [stat : "$elena'@referencetable"], eax
  jmp  labStart

labReuse:
  mov  [eax + ebx * 4], const : "system'nil"

  ret

end

// ; free_index
procedure coreapi'free_index

  mov  esi, [stat : "$elena'@referencetable"]
  mov  [esi + ebx * 4], 0
  
  ret

end

// ; resolve_index (index)
procedure coreapi'resolve_index

  mov  esi, [esp + 4]
  mov  edx, [esi]
  mov  ebx, [stat : "$elena'@referencetable"]
  mov  eax, [ebx + edx * 4]
  
  ret 4

end

procedure coreapi'resolve_index_value

  mov  esi, [stat : "$elena'@referencetable"]
  mov  eax, [esi + ebx * 4]
  
  ret

end
