// --- System Core Data  --
define CORE_EXCEPTION_TABLE 01h
define CORE_GC_TABLE        02h

// --- System Core API  --
define GC_ALLOC	         10001h
define HOOK              10010h

// constants
define elVMTSizeOffset    000Ch

// GC TABLE
define gc_header             0000h
define gc_start              0004h
define gc_stack_frame        002Ch
define gc_stack_bottom       0034h

// Page Size
define page_mask        0FFFFFFF0h
define page_size_order          4h
define page_ceil               1Bh

// throw
inline % 7

  mov  edx, [data : %CORE_EXCEPTION_TABLE]
  jmp  edx

end

// bsredirect

inline % 0Eh // (eax - object, edx - message)

  mov  esi, [eax-4]
  xor  ebx, ebx
  mov  ecx, [esi - elVMTSizeOffset]

labSplit:
  test ecx, ecx
  jz   short labEnd

labStart:
  shr  ecx, 1
  setnc bl
  cmp  edx, [esi+ecx*8]
  jb   short labSplit
  nop
  jz   short labFound
  lea  esi, [esi+ecx*8+8]
  sub  ecx, ebx
  jnz  short labStart

  nop
  jmp  labEnd

labFound:
  mov  ecx, [esi+ecx*8+4] 
  jmp  ecx

labEnd:
                                                                
end

// mccreverse

inline % 10h

  mov ecx, 0Fh
  and ecx, edx
  lea esi, [esp + ecx * 4]
  mov ecx, esp

labNext:
  mov  ebx, [ecx]  
  xchg ebx, [esi]
  mov  [ecx], ebx
  lea  ecx, [ecx + 4]
  lea  esi, [esi - 4] 
  cmp  ecx, esi
  jl   short labNext

end

// init
inline % 12h

  push edi
  mov  edi, eax

end

// close

inline % 15h

  mov  esp, ebp
  pop  ebp

end

// jumpacc
inline % 16h

  mov  esi, [eax]
  jmp  esi
 
end

// get

inline % 18h
   mov  ebx, eax
   mov  ecx, [edi - 8]
   shl  ebx, 2
   xor  esi, esi
   test ebx, ebx
   js   short lEnd
   cmp  ebx, ecx
   jge  short lEnd
   mov  esi, [edi + ebx]
   
lEnd:
  push  esi

end

// set

inline % 19h

   pop  esi   
   mov  ebx, eax
   mov  ecx, [edi - 8]
   shl  ebx, 2
   xor  eax, eax
   test ebx, ebx
   js   short lEnd
   cmp  ebx, ecx
   jge  short lEnd

   mov  eax, esi
    
   // ; calculate write-barrier address
   mov  esi, edi
   sub  esi, [data : %CORE_GC_TABLE + gc_start]
   mov  edx, [data : %CORE_GC_TABLE + gc_header]
   shr  esi, page_size_order
   mov  byte ptr [esi + edx], 1  
   mov  [edi + ebx], eax
lEnd:

end

// quitmcc
inline % 1Bh

  mov  ecx, edx
  and  ecx, 0Fh
  pop  esi
  lea  esp, [esp + ecx * 4 + 4]
  jmp  esi
 
end

// unhook

inline % 1Dh

  mov  esp, [data : %CORE_EXCEPTION_TABLE + 4]
  mov  edx, [data : %CORE_EXCEPTION_TABLE + 8]
  pop  ebx
  mov  [data : %CORE_GC_TABLE + gc_stack_frame], edx
  mov [data : %CORE_EXCEPTION_TABLE + 8], ebx
  pop  ebx
  mov  [data : %CORE_EXCEPTION_TABLE + 4], ebx
  pop  edx
  mov  [data : %CORE_EXCEPTION_TABLE], edx
  pop  ebp

end

// exclude
inline % 1Eh

  mov  ebx, [data : %CORE_GC_TABLE + gc_stack_frame]
  mov  ecx, ebx
  sub  ecx, esp
  mov  [ebx], ecx              // lock managed stack frame

end

// include
inline % 1Fh

  mov  esi, [data : %CORE_GC_TABLE + gc_stack_frame]
  push esi                              // save previous pointer 
  push 0                                // size field
  mov  [data : %CORE_GC_TABLE + gc_stack_frame], esp

end

// pushacci
inline % 24h

  push [eax+__arg1]

end

// mcccopyprmfi
inline %29h

  mov  ecx, edx
  and  ecx, 0Fh
  lea  esi, [ebp + __arg1]
  jz   short labEnd
  lea  esi, [esi + ecx * 4 + 4]
labNext:
  push [esi]
  sub  ecx, 1
  lea  esi, [esi+4]
  jnz  short labNext
labEnd:

end

// pushfpi
inline % 2Eh

  lea  ebx, [ebp + __arg1]
  push ebx

end

// pushspi
inline % 2Fh

  lea  ebx, [esp + __arg1]
  push ebx

end

// popselfi
inline % 31h
  pop  eax

  // ; calculate write-barrier address
  mov  esi, edi
  sub  esi, [data : %CORE_GC_TABLE + gc_start]
  mov  edx, [data : %CORE_GC_TABLE + gc_header]
  shr  esi, page_size_order
  mov  byte ptr [esi + edx], 1  
  mov  [edi + __arg1], eax

end

// x_popacci
inline % 33h

  pop  edx
  mov  [eax+__arg1], edx

end

// popacci
inline % 035h
  // ; calculate write-barrier address
  mov  esi, eax
  sub  esi, [data : %CORE_GC_TABLE + gc_start]
  mov  edx, [data : %CORE_GC_TABLE + gc_header]
  shr  esi, page_size_order
  mov  byte ptr [esi + edx], 1  

  pop  edx
  mov [eax + __arg1], edx

end

// callextr
inline % 040h

  call extern __arg1

end

// call (ecx - offset to VMT entry)
inline % 42h

  mov  ecx, __arg1
  mov  esi, [eax - 4]
  call [esi + ecx]

end

// mcccopyacci (__arg1 - index)

inline % 47h

  mov  edx, [eax + __arg1]

end

// mcccopysi

inline % 048h

  mov  edx, [esp + __arg1]

end

// mcccopyfi

inline % 049h

  mov edx, [ebp + __arg1]
  
end

// mccaddacci

inline % 4Dh

  or  edx, [eax + __arg1]

end

// incsi (__arg - index)

inline % 50h

  add [esp + __arg1], 4

end

// incfi (__arg - index)

inline % 51h

  add [ebp + __arg1], 4

end

// accloadfi
inline % 54h

  mov  eax, [ebp+__arg1]

end

// accloadsi
inline % 55h

  mov  eax, [esp+__arg1]

end

// accsaveselfi
inline %61h

  mov  esi, edi                     
  // calculate write-barrier address
  sub  esi, [data : %CORE_GC_TABLE + gc_start]
  mov  edx, [data : %CORE_GC_TABLE + gc_header]
  shr  esi, page_size_order
  mov  byte ptr [esi + edx], 1  

  mov [edi + __arg1], eax

end

// accsavesi
inline % 63h

  mov  [esp+__arg1], eax

end

// accsavefi
inline % 64h

  mov  [ebp+__arg1], eax

end

// accsavedstsi
inline %67h

  mov  ebx, edx
  and  ebx, 0Fh
  lea  esi, [esp + __arg1]
  mov  [esi + ebx*4], eax

end

// swapsi
inline % 6Ch

  mov ebx, [esp]
  mov ecx, [esp+__arg1]
  mov [esp], ecx
  mov [esp+__arg1], ebx
  
end

// accswapsi
inline % 6Dh

  mov ebx, eax
  mov ecx, [esp+__arg1]
  mov [esp+__arg1], ebx
  mov eax, ecx
  
end

// accaddn

inline % 7Eh

  add eax, __arg1
  
end

// rethrow
inline % 80h

  mov  ecx, esp
  add  ecx, __arg1
  mov  esp, [data : %CORE_EXCEPTION_TABLE + 4]
  sub  ecx, esp
  pop  ebx
  mov  [data : %CORE_EXCEPTION_TABLE + 4], ebx
  pop  edx
  mov  [data : %CORE_EXCEPTION_TABLE], edx
  jz   short labEnd
  jmp  edx

labEnd:

end

// restore

inline % 82h

  lea  esp, [esp+4]
  pop  edx
  mov  [data : %CORE_GC_TABLE + gc_stack_frame], edx
  lea  esp, [esp + __arg1]

end

// open
inline % 88h

  push ebp
  mov  ebp, esp

end

// jumpaccn

inline % 0A1h

  mov  ecx, __arg1
  mov  esi, [eax - 4]
  mov  esi, [esi + ecx]
  jmp  esi

end

// hook label (ecx - offset)

inline % 0A8h

  call code : %HOOK
  push ebp
  push [data : %CORE_EXCEPTION_TABLE]
  mov  ebx, [data : %CORE_GC_TABLE + gc_stack_frame]
  push [data : %CORE_EXCEPTION_TABLE + 4]
  push [data : %CORE_EXCEPTION_TABLE + 8]
  mov  [data : %CORE_EXCEPTION_TABLE], ecx
  mov  [data : %CORE_EXCEPTION_TABLE + 4], esp
  mov  [data : %CORE_EXCEPTION_TABLE + 8], ebx
  
end

// getlen

inline % 0B1h

  mov  eax, [eax - 8]
  neg  eax

end

// accgetsi (__arg1 - index)

inline % 0C0h

  mov  ecx, [edi - 8]             // !! use constant
  xor  eax, eax
  mov  ebx, [esp + __arg1]
  cmp  ecx, ebx
  jbe  short labSkip
  mov  eax, [edi + ebx]
labSkip:

end

// accgetfi (__arg1 - index)

inline % 0C1h

  mov  ecx, [edi - 8]             // !! use constant
  xor  eax, eax
  mov  ebx, [ebp + __arg1]
  cmp  ecx, ebx
  jbe  short labSkip
  mov  eax, [edi + ebx]
labSkip:

end

// acccreate

inline % 0C3h

  mov  ebx, eax
  mov  ecx, eax
  add  ebx, page_ceil
  and  ebx, page_mask  
  call code : %GC_ALLOC
  mov  [eax-8], ecx
  
end

// accfillr (eax - val, ebx - r)

inline % 0C4h

  mov  ecx, [eax - elVMTSizeOffset]
  mov  ebx, __arg1
  mov  esi, eax
labClear:
  mov  [esi], ebx
  sub  ecx, 4
  lea  esi, [esi+4]
  jnz  short labClear

end

// accloadselfi (__arg1 : index)

inline % 0EEh

  mov  eax, [edi + __arg1]

end
                                 
// create (ebx - size, __arg1 - length)

inline % 0F0h
	
  mov  ecx, __arg1
  call code : %GC_ALLOC

  mov  [eax-8], ecx

end

// createn (ebx - size, __arg1 - length)

inline % 0F1h

  mov  ecx, __arg1
  call code : %GC_ALLOC

  mov  [eax-8], ecx

end

// iaccfillr (eax - val, ebx - r,  __arg1 - count)

inline % 0F3h

  mov  ecx, __arg1
  mov  esi, eax
labClear:
  mov  [esi], ebx
  sub  ecx, 1
  lea  esi, [esi+4]
  jnz  short labClear

end

// acccreaten (eax - size)

inline % 0F5h

  mov  ebx, eax  
  mov  ecx, eax
  add  ebx, page_ceil
  neg  ecx
  and  ebx, page_mask  
  call code : %GC_ALLOC
  mov  [eax-8], ecx

end

// accboxn (ebx - size, __arg1 - vmt)

inline % 0F6h

  cmp  eax, [data : %CORE_GC_TABLE + gc_stack_bottom]
  ja   short labSkip
  cmp  eax, esp
  jb   short labSkip

  push eax
  mov  ecx, ebx
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

// callsi (__arg1 : vmt index , ebx - the target)

inline % 0FCh

  mov  ecx, __arg1
  mov  esi, [ebx - 4]
  call [esi + ecx]
  
end

// rcalln (ecx contains message, __arg1 contains vmt)
inline % 0FDh

  mov  esi, __arg1
  call [esi + ecx]

end

// rcallm (edx contains message, __arg1 contains vmtentry)
inline % 0FEh

   mov  esi, __arg1
   call esi

end
