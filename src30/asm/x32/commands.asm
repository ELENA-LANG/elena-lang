// --- System Core API  --

define GC_ALLOC	         10001h
define HOOK              10010h
define GETCLASSNAME      10011h
define INIT_RND          10012h
define EVALSCRIPT        10013h
define LOADSYMBOL        10014h

// --- System Core Data  --
define CORE_EXCEPTION_TABLE 01h
define CORE_GC_TABLE        02h
define CORE_GC_SIZE         03h
define CORE_STAT_COUNT      04h
define CORE_STATICROOT      05h
define CORE_VM_TABLE        06h

// GC TABLE
define gc_header             0000h
define gc_start              0004h
define gc_yg_start           0008h
define gc_yg_current         000Ch
define gc_yg_end             0010h
define gc_shadow             0014h
define gc_shadow_end         0018h
define gc_mg_start           001Ch
define gc_mg_current         0020h
define gc_end                0024h
define gc_promotion          0028h
define gc_ext_stack_frame    002Ch
define gc_mg_wbar            0030h
define gc_stack_bottom       0034h

// Page Size
define page_size               10h
define page_size_order          4h
define page_size_order_minus2   2h
define page_mask        0FFFFFFF0h
define page_ceil               1Bh

define subj_mask         80FFFFF0h

// Object header fields
define elObjectOffset    000Ch
define elSizeOffset      000Ch
define elCountOffset     0008h
define elVMTOffset       0004h 
define elVMTFlagOffset   0008h
define elVMTSizeOffset   000Ch
define elVMTTypeOffset   0010h

// ; ==== Command Set ==

// ; throw
inline % 7

  jmp  [data : %CORE_EXCEPTION_TABLE]

end

// ; bsredirect

inline % 0Eh // (eax - object, ecx - message)

  mov  edi, [eax-4]
  xor  ebx, ebx
  mov  edx, [edi - elVMTSizeOffset]

labSplit:
  test edx, edx
  jz   short labEnd

labStart:
  shr  edx, 1
  setnc bl
  cmp  ecx, [edi+edx*8]
  jb   short labSplit
  nop
  nop
  jz   short labFound
  lea  edi, [edi+edx*8+8]
  sub  edx, ebx
  jnz  short labStart
  nop
  nop
  jmp  labEnd
  nop
  nop
labFound:
  jmp  [edi+edx*8+4]
  nop
  nop

labEnd:
                                                                
end

// ; count

inline % 11h

  mov  ecx, [edi-8]
  shr  ecx, 2

labEnd:

end

// ; bcopya
inline % 12h

  mov  edi, eax

end

// ; close

inline % 15h

  mov  esp, ebp
  pop  ebp
  
end

// ; get

inline % 18h

   mov  eax, [edi + esi * 4]

end

// ; set

inline % 19h
                                
   mov  ebx, esi
   shl  ebx, 2
    
   // ; calculate write-barrier address
   mov  ecx, edi
   sub  ecx, [data : %CORE_GC_TABLE + gc_start]
   mov  edx, [data : %CORE_GC_TABLE + gc_header]
   shr  ecx, page_size_order
   mov  byte ptr [ecx + edx], 1  
   mov  [edi + ebx], eax
lEnd:

end

// ; equit
inline % 1Bh

  pop  ebx
  lea  esp, [esp + ecx * 4 + 4]
  jmp  ebx
  nop
  nop
 
end

// ; unhook
inline % 1Dh

  mov  esp, [data : %CORE_EXCEPTION_TABLE + 4]
  mov  ebp, [data : %CORE_EXCEPTION_TABLE + 8]
  pop  ebx
  mov [data : %CORE_EXCEPTION_TABLE + 8], ebx
  pop  ebx
  mov  [data : %CORE_EXCEPTION_TABLE + 4], ebx
  pop  edx
  mov  [data : %CORE_EXCEPTION_TABLE], edx
  
end

// ; create
inline % 1Fh

  mov  ebx, esi
  shl  ebx, 2  
  push eax  
  mov  ecx, ebx
  add  ebx, page_ceil
  and  ebx, page_mask  
  call code : %GC_ALLOC
  mov  [eax-8], ecx
  pop  ebx
  mov  [eax-4], ebx

end

// ; dswape
inline % 2Ch

  mov  ebx, ecx
  mov  ecx, esi
  mov  esi, ebx

end

// ; bswapa
inline % 2Dh

  mov  ebx, edi
  mov  edi, eax
  mov  eax, ebx

end

// ; copy

inline % 2Eh

  mov  ecx, [edi-8]
  mov  esi, eax
  mov  ebx, edi
labCopy:
  mov  edx, [esi]
  mov  [ebx], edx
  lea  esi, [esi+4]
  lea  ebx, [ebx+4]
  add  ecx, 4
  js   short labCopy

end

// ; xset

inline % 2Fh
                                
   mov  [edi + esi * 4], eax

lEnd:

end

// ; type

inline % 30h

  mov  ebx, [edi - 4]
  mov  esi, [ebx - elVMTTypeOffset]
  
end

// ; getblen
inline % 31h

  mov  esi, [eax-8]
  neg  esi

end

// ; getwlen
// ;in : eax - object, esi - size
inline % 32h

  mov  esi, [eax-8]
  neg  esi
  shr  esi, 1
  
end

// ; flag

inline % 33h

  mov  ebx, [eax - 4]
  mov  esi, [ebx - elVMTFlagOffset]
  
end

// ; weval
inline % 34h

  mov  edx, edi
  call code : % LOADSYMBOL
  mov  ecx, eax

end

// ; wname
inline % 35h

  push esi
  
  mov  edx, [eax-elVMTOffset]
  call code : % GETCLASSNAME  

  pop  ecx
  xor  esi, esi

  test eax, eax
  jz   short labEnd

  mov  esi, eax
  mov  edx, edi

labCopy:
  mov  ebx, [esi]                                                                                           
  mov  word ptr [edx], bx
  test ebx, 0FFFFh
  jz   short labFixLen
  lea  esi, [esi+2]
  sub  ecx, 1
  lea  edx, [edx+2]
  jnz  short labCopy

labFixLen:
  mov  esi, edx
  sub  esi, edi
  mov  eax, edi
  
labEnd:

end

// ; class
inline % 36h

  mov  eax, [edi - elVMTOffset]

end

// ; mindex
inline % 37h

  mov  esi, [eax - elVMTOffset]
  xor  ebx, ebx
  mov  ecx, [esi - elVMTSizeOffset]
labNext:
  cmp  edx, [esi+ebx*8]
  jz   short labFound
  add  ebx, 1
  sub  ecx, 1
  jnz  short labNext
  mov  ebx, -1

labFound:
  mov   esi, ebx

end

// ; clone

inline % 3Eh

  mov  ecx, [edi-8]
  mov  esi, eax
  mov  ebx, edi
labCopy:
  mov  edx, [esi]
  mov  [ebx], edx
  lea  esi, [esi+4]
  lea  ebx, [ebx+4]
  sub  ecx, 1
  jnz  short labCopy

end

// ; nequal

inline % 40h

  mov  ebx, [eax]
  cmp  ebx, [edi]
  mov  esi, 1
  jz   short labEnd
  mov  esi, 0  
labEnd:

end

// ; nless
inline % 41h

  mov  ebx, [eax]
  cmp  ebx, [edi]
  mov  esi, 1
  jl   short labEnd
  mov  esi, 0  
labEnd:

end

// ; ncopy (src, tgt)
inline % 42h

  mov  ebx, [eax]
  mov  [edi], ebx
    
end

// ; nadd
inline % 43h

  mov  ecx, [eax]
  add  [edi], ecx

end

// ; nsub
inline % 44h

  mov  ecx, [eax]
  sub  [edi], ecx

end

// ; nmul
inline % 45h

  mov  esi, eax
  mov  eax, [edi]
  imul [esi]
  mov  [edi], eax
  mov  eax, esi

end

// ; ndiv
inline % 46h
                                                   
  mov  esi, eax
  mov  ebx, [esp]
  mov  eax, [edi]
  cdq
  idiv [esi]
  mov  [edi], eax
  mov  eax, esi

end

// ; nwrite
inline % 47h

  mov [edi], esi

end

// ; nread
inline % 48h

  mov esi, [eax]

end

// ; ncopyws
inline % 49h
  mov  ebx, esi                     // ; radix
  mov  esi, eax                     // ; get str
  mov  ecx, [esi-8]
  xor  edx, edx                     // ; clear flag
  cmp  byte ptr [esi], 2Dh
  lea  ecx, [ecx+2]                 // ; to skip zero
  jnz  short Lab4
  lodsw
  mov  edx, 1                        // ; set flag
  lea  ecx, [ecx+2]                 //  ; to skip minus
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
  add  ecx, 2
  jnz  short Lab1
  nop
  pop  ebx
  test ebx, ebx                                
  jz   short Lab5
  neg  eax
Lab5:
  mov  [edi], eax
  mov  esi, 1
  jmp  short Lab3
Lab2:
  xor  esi, esi
Lab3:
  mov  eax, edi

end

// ; nand
inline % 4Ah

  mov  ecx, [eax]
  and  [edi], ecx

end

// ; nor
inline % 4Bh

  mov  ecx, [eax]
  or   [edi], ecx

end

// ; nxor
inline % 4Ch

  mov  ecx, [eax]
  xor  [edi], ecx

end

// ; nshift
inline % 4Dh

  mov ecx, esi
  mov ebx, [edi]
  and ecx, ecx
  jns short lab1
  neg ecx
  shl ebx, cl
  jmp short lab2
lab1:
  shr ebx, cl
lab2:
  mov [edi], ebx

end

// ; nnot
inline % 4Eh

  mov  ebx, [eax]  
  not  ebx
  mov  [edi], ebx

end

// ; wsequal
inline % 50h

  mov  esi, edi              // s2
  mov  edx, eax              // s1
  mov  ecx, [edx-8]          // s1.length
  mov  eax, 0
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
  mov  eax, 1
Lab1:
  mov  esi, eax
  mov  eax, edi

end

// ; wsless
inline % 51h

  mov  esi, edi              // s2
  mov  edx, eax              // s1
  mov  ecx, [edx-8]          // s1 length

  cmp  ecx, [esi-8]
  mov  eax, 0
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
  mov  eax, 1

LabEnd:
  mov  esi, eax
  mov  eax, edi
  
end

// ; wsreadw
inline % 52h

  mov ecx, [edi + esi * 2]
  and ecx, 0FFFFh

end

// ; winsert
inline % 53h

  mov  edx, eax
  test ecx, ecx
  jz   short labEnd

labNext:
  mov  ebx, [edx]
  mov  word ptr [edi + esi*2], bx
  add  esi, 1
  lea  edx, [edx + 2]
  sub  ecx, 1
  jnz  short labNext

labEnd:

end

// ; wscopy
inline % 54h

   mov  ebx, esi   
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
   neg  ecx
   mov  esi, edi
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
   lea  esp, [esp+4]
   pop  ebp

end

// ; wscopylong
inline % 55h

   mov  ecx, esi
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
   mov  edx, ecx                                                                  
   add  ecx, 1    // ;  including trailing zero
   shl  ecx, 1
   mov  esi, [ebp+8]
   neg  ecx
   mov  [esi-8], ecx
   mov  ebx, 0FFh
   mov  ecx, edx
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
   xor  eax, eax
   mov  word ptr [esi], ax
   lea  esp, [esp+8]
   pop  ebp
   pop  edi
end

// ; wscopyreal
inline % 56h

   mov   ebx, esi                                 
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

   sub   ebx, edi
   neg   ebx
   mov   [edi-8], ebx

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

// ; wsseek
inline % 57h

  push eax
  mov  edx, esi     // index
  mov  esi, eax     // subs

  push edi
  
  mov  ebx, [edi-8]   // get total length  

  neg  ebx
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
  lea  ecx, [ecx+2]
  jz   short labEnd
  neg  ecx
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
  mov  esi, edx
  jmp  short labEnd2

labEnd:
  pop  edi
  pop  eax
  mov  esi, -1
labEnd2:

end

// ; wsave
inline % 58h

  mov  word ptr [edi + esi*2], cx

end

// ; wadd
inline % 59h

  mov  edx, esi         // ; dst index
  mov  esi, ecx         // ; src index
  shl  edx, 1
  
  add  ecx, esi
  shl  esi, 1
  add  ecx, [eax-8]  

  add  edx, edi
  add  esi, eax
  
labNext2:
  mov  ebx, [esi]
  mov  word ptr [edx], ebx
  lea  esi, [esi+2]
  lea  edx, [edx+2]
  add  ecx, 2
  jnz  short labNext2
  
end

// ; wsubcopy
inline % 5Ah

  mov  edx, edi
  test ecx, ecx
  jz   short labEnd

labNext:
  mov  ebx, [eax + esi*2]
  mov  word ptr [edx], bx
  add  esi, 1
  lea  edx, [edx + 2]
  sub  ecx, 1
  jnz  short labNext

labEnd:

end

// ; createw
inline % 5Fh

  //lea  ebx, [esi * 2 + 2]
  mov  ebx, esi
  shl  ebx, 1
  push eax  
  mov  ecx, ebx
  add  ebx, page_ceil
  neg  ecx
  and  ebx, page_mask  
  call code : %GC_ALLOC
  mov  [eax-8], ecx
  pop  ebx
  mov  [eax-4], ebx

end

// ; bsreadw
inline % 60h

  mov ecx, [edi + esi]
  and ecx, 0FFFFh

end

// ; load
inline % 61h

  mov ecx, [edi + esi]

end

// ; insert
inline % 62h

  mov  edx, eax
  test ecx, ecx
  jz   short labEnd

labNext:
  mov  ebx, [edx]
  mov  byte ptr [edi + esi], bl
  add  esi, 1
  lea  edx, [edx + 1]
  sub  ecx, 1
  jnz  short labNext

labEnd:

end

// ; ninsert
inline % 63h

  mov  edx, eax
  test ecx, ecx
  jz   short labEnd

labNext:
  mov  ebx, [edx]
  mov  [edi + esi*4], ebx
  add  esi, 1
  lea  edx, [edx + 4]
  sub  ecx, 1
  jnz  short labNext

labEnd:

end

// ; xseek
inline % 64h

labNext:
  cmp  [edi + esi * 4], ecx
  lea  esi, [esi + 1]
  jnz  short labNext

  lea  esi, [esi - 1]
  
end

// ; save
inline % 69h

  mov [edi + esi], ecx
  
end

// ; subcopy
inline % 6Ah

  mov  edx, edi
  test ecx, ecx
  jz   short labEnd

labNext:
  mov  ebx, [eax + esi]
  mov  byte ptr [edx], bl
  add  esi, 1
  add  edx, 1
  sub  ecx, 1
  jnz  short labNext

labEnd:

end

// ; nsubcopy

inline % 6Bh

  mov  edx, edi
  test ecx, ecx
  jz   short labEnd

labNext:
  mov  ebx, [eax + esi * 4]
  mov  [edx], ebx
  add  esi, 1
  lea  edx, [edx + 4]
  sub  ecx, 1
  jnz  short labNext

labEnd:

end

// ; createb
inline % 6Fh

  mov  ebx, esi
  push eax  
  mov  ecx, ebx
  add  ebx, page_ceil
  neg  ecx
  and  ebx, page_mask  
  call code : %GC_ALLOC
  mov  [eax-8], ecx
  pop  ebx
  mov  [eax-4], ebx

end

// ; lcopy
inline % 70h

  mov  ecx, [eax]
  mov  ebx, [eax+4]
  mov  [edi], ecx
  mov  [edi+4], ebx
    
end

// ; lcopyint
inline % 71h

  mov  ebx, [eax]
  xor  edx, edx
  mov  [edi], ebx
  mov  [edi+4], edx
    
end

// ; lequal

inline % 72h

  mov  ebx, [eax]
  mov  edx, [eax+4]  
  cmp  ebx, [edi]
  mov  esi, 0
  jnz  short labEnd
  cmp  edx, [edi+4]
  jnz  short labEnd
  mov  esi, 1

labEnd:

end

// ; lless(lo, ro, tr, fr)
inline % 73h

  mov  ebx, [eax]
  mov  edx, [eax+4]  
  cmp  edx, [edi+4]
  mov  esi, 1
  jl   short Lab1
  nop
  jnz  short Lab2
  cmp  ebx, [edi]
  jl   short Lab1
Lab2:
  mov  esi, 0
Lab1:

end

// ; ladd
inline % 74h

  mov  edx, [eax+4]
  mov  ecx, [eax]
  add [edi], ecx
  adc [edi+4], edx

end

// ; lsub
inline % 75h

  mov  edx, [edi+4]
  mov  ecx, [edi]
  sub  ecx, [eax]
  sbb  edx, [eax+4]
  mov  [edi], ecx
  mov  [edi+4], edx

end

// ; lmul
inline % 76h
  mov  esi, eax        // sour
  mov  edx, edi        // dest

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
  mov  [edi], eax
  mov  [edi+4], edx
  pop  eax

end

// ; ldiv
inline % 77h
               
  mov  ebx, eax   // ; DVSR
  mov  esi, edi   // ; DVND

  push eax
  push edi

  push [esi+4]    // ; DVND hi dword
  push [esi]      // ; DVND lo dword
  push [ebx+4]    // ; DVSR hi dword
  push [ebx]      // ; DVSR lo dword

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

  mov  [edi], eax
  mov  [edi+4], edx
  mov  eax, ebx
  pop  eax
                                    
end

// ; land
inline % 78h
  mov ebx, [edi]
  mov edx, [eax]

  mov ecx, [edi+4]
  mov esi, [eax+4]

  and ebx, edx
  and ecx, esi

  mov [edi], ebx
  mov [edi+4], ecx
end

// ; lor
inline % 79h
  mov ebx, [edi]
  mov edx, [eax]

  mov ecx, [edi+4]
  mov esi, [eax+4]

  or  ebx, edx
  or  ecx, esi

  mov [edi], ebx
  mov [edi+4], ecx
end

// ; lxor
inline % 7Ah
  mov ebx, [edi]
  mov edx, [eax]

  mov ecx, [edi+4]
  mov esi, [eax+4]

  xor ebx, edx
  xor ecx, esi

  mov [edi], ebx
  mov [edi+4], ecx
end

// ; lshift
inline % 7Bh

  mov  edx, [edi]
  mov  ecx, esi
  mov  ebx, [edi+4]

  and  ecx, ecx
  jns  short LR
  neg  ecx

  cmp  cl, 40h 
  jae  short lErr
  cmp  cl, 20h
  jae  short LL32
  shld edx, ebx, cl
  shl  ebx, cl
  jmp  short lEnd

LL32:
  mov  edx, ebx
  xor  ebx, ebx
  and  cl, 1Fh
  shl  edx, cl 
  jmp  short lEnd

LR:

  cmp  cl, 64
  jae  short lErr

  cmp  cl, 32
  jae  short LR32
  shrd ebx, edx, cl
  sar  edx, cl
  jmp  short lEnd

LR32:
  mov  ebx, edx
  sar  edx, 31
  and  cl, 31
  sar  ebx, cl
  jmp  short lEnd
  
lErr:
  xor  eax, eax
  jmp  short lEnd2

lEnd:
  mov  [edi], edx
  mov  [edi+4], ebx

lEnd2:

end

// ; lnot
inline % 7Ch

  mov ebx, [eax]
  mov ecx, [eax+4]
                                                                        
  not ebx
  not ecx

  mov [edi], ebx
  mov [edi+4], ecx

end

// ; wtol
inline % 7Dh

  push edi
  push esi
  mov  esi, eax
  mov  ecx, [esi-8]
  xor  edx, edx
  neg  ecx

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

  mov  [edi], ebx
  mov  [edi+4], edx
  mov  esi, 1
  jmp  short labEnd

labErr:
  xor  esi, esi
  pop  edi

labEnd:
  mov  eax, edi

end

// ; rcopy (src, tgt)
inline % 80h

  mov  ecx, [eax]
  mov  ebx, [eax+4]
  mov  [edi], ecx
  mov  [edi+4], ebx

end

// ; rcopyint (src, tgt)
inline % 81h

  fild dword ptr [eax]
  fstp qword ptr [edi]

end

// ; rcopylong (src, tgt)
inline % 82h

  fild qword ptr [eax]
  fstp qword ptr [edi]

end

// ; requal

inline % 83h

  mov    ebx, [esp]
  fld    qword ptr [ebx]
  fld    qword ptr [eax]
  fcomip st, st(1)
  mov    esi, 1
  je     short lab1
  mov    esi, 0
lab1:
  fstp  st(0)

end

// ; rless(lo, ro, tr, fr)
inline % 84h

  mov    ebx, [esp]
  fld    qword ptr [ebx]
  fld    qword ptr [eax]
  fcomip st, st(1)
  mov    esi, 1
  jb     short lab1
  mov    esi, 0
lab1:
  fstp  st(0)

end

// ; radd
inline % 85h

  fld  qword ptr [eax]
  fadd qword ptr [edi] 
  fstp qword ptr [edi]

end

// ; rsub
inline % 86h

  fld  qword ptr [edi]
  fsub qword ptr [eax] 
  fstp qword ptr [edi]

end

// ; rmul
inline % 87h

  fld  qword ptr [edi]
  fmul qword ptr [eax] 
  fstp qword ptr [edi]

end

// ; rdiv
inline % 88h
                                                   
  fld  qword ptr [edi]
  fdiv qword ptr [eax] 
  fstp qword ptr [edi]

end

// ; wtor
inline % 89h

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

end

// ; restore

inline % 92h

  add  ebp, __arg1
  
end

// ; aloadfi
inline % 94h

  mov  eax, [ebp+__arg1]

end

// ; aloadsi
inline % 95h

  mov  eax, [esp+__arg1]

end

// ; bbox (esi - size, __arg1 - vmt)

inline % 96h

  mov  ebx, esi
  cmp  eax, [data : %CORE_GC_TABLE + gc_stack_bottom]
  ja   short labSkip                      
  cmp  eax, esp
  jb   short labSkip

  push eax
  mov  ecx, esi
  add  ebx, page_ceil
  neg  ecx
  and  ebx, page_mask  
  call code : %GC_ALLOC
  mov  [eax-8], ecx
  mov  [eax-4], __arg1
  pop  esi
  mov  ebx, eax
labCopy:
  mov  edx, [esi]
  mov  [ebx], edx
  lea  esi, [esi+4]
  lea  ebx, [ebx+4]
  add  ecx, 4
  jnz  short labCopy

labSkip:

end

// ; open
inline % 98h

  push ebp
  mov  ebp, esp

end

// ; ajumpvi

inline % 0A1h

  mov  edx, [eax - 4]
  jmp  [edx + __arg1]
  nop
  nop
  nop
  nop

end

// ; acallvi (ecx - offset to VMT entry)
inline % 0A2h

  mov  edx, [eax - 4]
  call [edx + __arg1]

end

// ; callextr
inline % 0A5h

  call extern __arg1
  mov  esi, eax

end

// ; hook label (ecx - offset)

inline % 0A6h

  call code : %HOOK
  push [data : %CORE_EXCEPTION_TABLE]
  push [data : %CORE_EXCEPTION_TABLE + 4]
  push [data : %CORE_EXCEPTION_TABLE + 8]
  mov  [data : %CORE_EXCEPTION_TABLE], ecx
  mov  [data : %CORE_EXCEPTION_TABLE + 4], esp
  mov  [data : %CORE_EXCEPTION_TABLE + 8], ebp
  
end

// ; ecall

inline % 0A7h

  call ecx

end

// ; message
inline % 0A8h

  mov  edx, [eax - 4]
  mov  esi, [edx + __arg1]

end

// ; next
inline % 0AFh

  add  esi, 1
  cmp  esi, ecx

end

// ; pushai
inline % 0B4h

  push [eax+__arg1]

end

// ; dloadfi

inline % 0B7h

  mov  esi, [ebp + __arg1]

end

// ; dloadsi

inline % 0B8h

  mov  esi, [esp + __arg1]

end

// ; dsavefi

inline % 0B9h

  mov  [ebp + __arg1], esi

end

// ; dsavesi

inline % 0BBh

  mov  [esp + __arg1], esi

end


// ; pushf
inline % 0BDh

  lea  ebx, [ebp + __arg1]
  push ebx

end

// ; reserve
inline % 0BFh

  sub  esp, __arg1
  push ebp
  push 0
  mov  ebp, esp

end

// ; asavebi
inline %0C0h

  mov  esi, edi                     
  // calculate write-barrier address
  sub  esi, [data : %CORE_GC_TABLE + gc_start]
  mov  edx, [data : %CORE_GC_TABLE + gc_header]
  shr  esi, page_size_order
  mov  byte ptr [esi + edx], 1  

  mov [edi + __arg1], eax

end

// ; swapsi
inline % 0C1h

  mov ebx, [esp]
  mov ecx, [esp+__arg1]
  mov [esp], ecx
  mov [esp+__arg1], ebx
  
end

// ; aswapsi
inline % 0C2h

  mov ebx, eax
  mov ecx, [esp+__arg1]
  mov [esp+__arg1], ebx
  mov eax, ecx
  
end

// ; asavesi
inline % 0C3h

  mov  [esp+__arg1], eax

end

// ; asavefi
inline % 0C4h

  mov  [ebp+__arg1], eax

end

// ; bloadfi

inline % 0C8h

  mov  edi, [ebp + __arg1]

end

// ; bloadsi

inline % 0C9h

  mov  edi, [esp + __arg1]

end

// ; aloadbi (__arg1 : index)

inline % 0CEh

  mov  eax, [edi + __arg1]

end

// ; axsavebi
inline %0CFh

  mov [edi + __arg1], eax

end

// ; new (ebx - size, __arg1 - length)

inline % 0F0h
	
  mov  ecx, __arg1
  call code : %GC_ALLOC

  mov  [eax-8], ecx

end

// ; newn (ebx - size, __arg1 - length)

inline % 0F1h

  mov  ecx, __arg1
  call code : %GC_ALLOC

  mov  [eax-8], ecx

end

// ; selectr (ebx - r1, __arg1 - r2)

inline % 0F6h

  mov    ecx, __arg1
  test   esi, esi
  mov    eax, ebx
  cmovnz eax, ecx

end

// xcallrm (edx contains message, __arg1 contains vmtentry)
inline % 0FEh

   call code : __arg1

end
