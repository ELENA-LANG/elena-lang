//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Machine common implementation
//
//                                              (C)2018-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"

using namespace _ELENA_;

void SystemRoutineProvider :: OpenSTAFrame(SystemEnv* env, FrameHeader* frameHeader)
{
   frameHeader->previousFrame = env->Table->gc_stack_frame;
   frameHeader->reserved = 0;
}

void SystemRoutineProvider :: CloseSTAFrame(SystemEnv* env, FrameHeader* frameHeader)
{
   env->Table->gc_stack_frame = frameHeader->previousFrame;
}

void SystemRoutineProvider :: InitSTA(SystemEnv* env, ProgramHeader* frameHeader)
{
   Init(env);

   // setting current exception handler (only for STA)
   env->Table->gc_et_current = &frameHeader->root_exception_struct;
   env->Table->gc_stack_frame = 0;
}

void SystemRoutineProvider :: InitMTA(SystemEnv* env, ProgramHeader* frameHeader)
{
   Init(env);
   InitTLSEntry(0, *env->TLSIndex, frameHeader, env->ThreadTable);

   // set the thread table size
   env->ThreadTable[-1] = env->MaxThread;
}

int SystemRoutineProvider :: ExecuteInFrame(SystemEnv* env, _Entry& entry)
{
   FrameHeader frameHeader;
   if (env->MaxThread <= 1) {
      OpenSTAFrame(env, &frameHeader);
   }

   int retVal = Execute(entry.address, &frameHeader);

   if (env->MaxThread <= 1) {
      CloseSTAFrame(env, &frameHeader);
   }

   return retVal;
}

int SystemRoutineProvider :: ExecuteInNewFrame(SystemEnv* env, _Entry& entry)
{
   FrameHeader frameHeader = { 0 };

   int retVal = Execute(entry.address, &frameHeader);

   return retVal;
}

void SystemRoutineProvider :: OpenFrame(SystemEnv* env, FrameHeader* frameHeader)
{
   if (env->MaxThread <= 1) {
      OpenSTAFrame(env, frameHeader);
   }
}

void SystemRoutineProvider :: CloseFrame(SystemEnv* env, FrameHeader* frameHeader)
{
   if (env->MaxThread <= 1) {
      CloseSTAFrame(env, frameHeader);
   }
}

void SystemRoutineProvider :: GCRoutine(GCTable* table, GCRoot* roots)
{
   //labYGCollect:
   //   // ; restore ecx
   //   sub  ecx, eax
   //
   //   // ; save registers
   //   push ebp
   //
   //   // ; lock frame
   //   mov[data:% CORE_GC_TABLE + gc_stack_frame], esp
   //
   //   push ecx
   //
   //   // ; create set of roots
   //   mov  ebp, esp
   //   xor ecx, ecx
   //   push ecx        // ; reserve place
   //   push ecx
   //   push ecx
   //
   //   // ; save static roots
   //   mov  esi, data :% CORE_STATICROOT
   //   mov  ecx, [data:% CORE_GC_TABLE + gc_rootcount]
   //   push esi
   //   push ecx
   //
   //   // ; collect frames
   //   mov  eax, [data:% CORE_GC_TABLE + gc_stack_frame]
   //   mov  ecx, eax

   //labYGNextFrame :
   //   mov  esi, eax
   //   mov  eax, [esi]
   //   test eax, eax
   //   jnz  short labYGNextFrame
   //
   //   push ecx
   //   sub  ecx, esi
   //   neg  ecx
   //   push ecx
   //
   //   mov  eax, [esi + 4]
   //   test eax, eax
   //   mov  ecx, eax
   //   jnz  short labYGNextFrame

   //// === Minor collection ===
   //   mov[ebp - 4], esp      // ; save position for roots

   //// ; save mg -> yg roots
   //   mov  ebx, [data:% CORE_GC_TABLE + gc_mg_current]
   //   mov  edi, [data:% CORE_GC_TABLE + gc_mg_start]
   //   sub  ebx, edi                                        // ; we need to check only MG region
   //   jz   labWBEnd                                        // ; skip if it is zero
   //   mov  esi, [data:% CORE_GC_TABLE + gc_mg_wbar]
   //   shr  ebx, page_size_order
   //   // ; lea  edi, [edi + elObjectOffset]
   //
   //labWBNext :
   //   cmp[esi], 0
   //   lea  esi, [esi + 4]
   //   jnz  short labWBMark
   //   sub  ebx, 4
   //   ja   short labWBNext
   //   nop
   //   nop
   //   jmp  short labWBEnd
   //
   //labWBMark :
   //   lea  eax, [esi - 4]
   //   sub  eax, [data:% CORE_GC_TABLE + gc_mg_wbar]
   //   mov  edx, [esi - 4]
   //   shl  eax, page_size_order
   //   lea  eax, [edi + eax + elObjectOffset]
   //
   //   test edx, 0FFh
   //   jz   short labWBMark2
   //   mov  ecx, [eax - elSizeOffset]
   //   push eax
   //   and ecx, 0FFFFFh
   //   push ecx
   //
   //labWBMark2 :
   //   lea  eax, [eax + page_size]
   //   test edx, 0FF00h
   //   jz   short labWBMark3
   //   mov  ecx, [eax - elSizeOffset]
   //   push eax
   //   and ecx, 0FFFFFh
   //   push ecx
   //
   //labWBMark3 :
   //   lea  eax, [eax + page_size]
   //      test edx, 0FF0000h
   //      jz   short labWBMark4
   //      mov  ecx, [eax - elSizeOffset]
   //      push eax
   //      and ecx, 0FFFFFh
   //      push ecx
   //
   //      labWBMark4 :
   //   lea  eax, [eax + page_size]
   //      test edx, 0FF000000h
   //      jz   short labWBNext
   //      mov  ecx, [eax - elSizeOffset]
   //      push eax
   //      and ecx, 0FFFFFh
   //      push ecx
   //      jmp  short labWBNext
   //
   //      labWBEnd :
   //   // ; save the stack restore-point
   //   push ebp
   //
   //      // ; init registers
   //      mov  ebx, [data:% CORE_GC_TABLE + gc_yg_start]
   //      mov  edx, [data:% CORE_GC_TABLE + gc_yg_end]
   //      mov  ebp, [data:% CORE_GC_TABLE + gc_shadow]



   // collect roots


   //      lea  eax, [esp + 4]
   //      mov  ecx, [eax]
   //      mov  esi, [esp + 8]
   //      mov[data:% CORE_GC_TABLE + gc_yg_current], ebp
   //
   //      labCollectFrame :
   //   push eax
   //      call labCollectYG
   //      pop  eax
   //      lea  eax, [eax + 8]
   //      mov  esi, [eax + 4]
   //      test esi, esi
   //      mov  ecx, [eax]
   //      jnz short labCollectFrame



   //      // ; save gc_yg_current to mark survived objects
   //      mov[data:% CORE_GC_TABLE + gc_yg_current], ebp
   //
   //      // ; switch main YG heap with a shadow one
   //      mov  eax, [data:% CORE_GC_TABLE + gc_yg_start]
   //      mov  ebx, [data:% CORE_GC_TABLE + gc_shadow]
   //      mov  ecx, [data:% CORE_GC_TABLE + gc_yg_end]
   //      mov  edx, [data:% CORE_GC_TABLE + gc_shadow_end]
   //
   //      mov[data:% CORE_GC_TABLE + gc_yg_start], ebx
   //      mov[data:% CORE_GC_TABLE + gc_yg_end], edx
   //      mov  ebx, [esp]
   //      mov[data:% CORE_GC_TABLE + gc_shadow], eax
   //      mov  ebx, [ebx + 4]                           // ; restore object size
   //      mov[data:% CORE_GC_TABLE + gc_shadow_end], ecx
   //
   //      sub  edx, ebp
   //
   //      pop  ebp
   //      mov  esp, [ebp - 4]  // ; remove wb-roots
   //
   //      // ; check if it is enough place
   //      cmp  ebx, edx
   //      jae  short labFullCollect
   //
   //      // ; free root set
   //      mov  esp, ebp
   //
   //      // ; restore registers
   //      pop  ecx
   //      pop  ebp
   //
   //      // ; try to allocate once again
   //      mov  eax, [data:% CORE_GC_TABLE + gc_yg_current]
   //      add  ecx, eax
   //      lea  ebx, [eax + elObjectOffset]
   //      mov[data:% CORE_GC_TABLE + gc_yg_current], ecx
   //      ret
   //
   //      labFullCollect :
   //   // ; ====== Major Collection ====
   //   // ; save the stack restore-point
   //   push ebp
   //
   //      // ; expand MG if required
   //      mov  ecx, [data:% CORE_GC_TABLE + gc_end]
   //      sub  ecx, [data:% CORE_GC_TABLE + gc_mg_current]
   //      mov  edx, [data:% CORE_GC_TABLE + gc_yg_current]
   //      sub  edx, [data:% CORE_GC_TABLE + gc_yg_start]
   //      cmp  ecx, edx
   //      ja   labSkipExpand
   //
   //      mov  eax, [data:% CORE_GC_TABLE + gc_end]
   //      mov  ecx, 2A000h
   //      call code : % EXPAND_HEAP
   //
   //      mov  eax, [data:% CORE_GC_TABLE + gc_header]
   //      mov  ecx, [data:% CORE_GC_TABLE + gc_end]
   //      sub  ecx, [data:% CORE_GC_TABLE + gc_start]
   //      shr  ecx, page_size_order_minus2
   //      add  eax, ecx
   //      mov  ecx, 0A800h
   //      call code : % EXPAND_HEAP
   //
   //      mov  ecx, [data:% CORE_GC_TABLE + gc_end]
   //      add  ecx, 15000h
   //      mov[data:% CORE_GC_TABLE + gc_end], ecx
   //
   //      labSkipExpand :
   //
   //   // ; mark both yg and mg objects
   //   mov  ebx, [data:% CORE_GC_TABLE + gc_yg_start]
   //      mov  edx, [data:% CORE_GC_TABLE + gc_mg_current]
   //
   //      // ; collect roots
   //      lea  eax, [esp + 4]
   //      mov  ecx, [eax]
   //      mov  esi, [esp + 8]
   //
   //      labMGCollectFrame:
   //   push eax
   //      call labCollectMG
   //      pop  eax
   //      lea  eax, [eax + 8]
   //      mov  esi, [eax + 4]
   //      test esi, esi
   //      mov  ecx, [eax]
   //      jnz short labMGCollectFrame
   //
   //      // ; compact mg
   //      mov  esi, [data:% CORE_GC_TABLE + gc_mg_start]
   //      mov  edi, esi
   //      sub  edi, [data:% CORE_GC_TABLE + gc_start]
   //      shr  edi, page_size_order_minus2
   //      add  edi, [data:% CORE_GC_TABLE + gc_header]
   //
   //      // ; skip the permanent part
   //      labMGSkipNext:
   //   mov  ecx, [esi]
   //      test ecx, ecx
   //      jns  short labMGSkipEnd
   //      add  ecx, page_ceil
   //      mov  eax, esi
   //      and ecx, page_align_mask
   //      lea  eax, [eax + elObjectOffset]
   //      add  esi, ecx
   //      mov[edi], eax
   //      shr  ecx, page_size_order_minus2
   //      add  edi, ecx
   //      cmp  esi, edx
   //      jb   short labMGSkipNext
   //      // ; !! undefined behaviour
   //      xor ecx, ecx
   //
   //      labMGSkipEnd :
   //   mov  ebp, esi
   //
   //      // ; compact
   //      labMGCompactNext :
   //   add  ecx, page_ceil
   //      and ecx, page_align_mask
   //      add  esi, ecx
   //
   //      shr  ecx, page_size_order_minus2
   //      add  edi, ecx
   //      cmp  esi, edx
   //      jae  short labMGCompactEnd
   //
   //      labMGCompactNext2 :
   //   mov  ecx, [esi]
   //      test ecx, ecx
   //      jns  short labMGCompactNext
   //      add  ecx, page_ceil
   //      mov  eax, ebp
   //      and ecx, page_align_mask
   //      lea  eax, [eax + elObjectOffset]
   //      mov[edi], eax
   //      mov  eax, ecx
   //      shr  eax, page_size_order_minus2
   //      add  edi, eax
   //
   //      labMGCopy :
   //   mov  eax, [esi]
   //      mov[ebp], eax
   //      sub  ecx, 4
   //      lea  esi, [esi + 4]
   //      lea  ebp, [ebp + 4]
   //      jnz  short labMGCopy
   //      cmp  esi, edx
   //      jb   short labMGCompactNext2
   //      labMGCompactEnd :
   //
   //   // ; promote yg
   //   mov  ebx, [data:% CORE_GC_TABLE + gc_end]
   //      mov  esi, [data:% CORE_GC_TABLE + gc_yg_start]
   //      sub  ebx, ebp
   //      mov  edi, esi
   //      sub  edi, [data:% CORE_GC_TABLE + gc_start]
   //      shr  edi, page_size_order_minus2
   //      mov  edx, [data:% CORE_GC_TABLE + gc_yg_current]
   //      add  edi, [data:% CORE_GC_TABLE + gc_header]
   //      jmp  short labYGPromNext2
   //
   //      labYGPromNext :
   //   add  ecx, page_ceil
   //      and ecx, page_align_mask
   //      add  esi, ecx
   //      shr  ecx, page_size_order_minus2
   //      add  edi, ecx
   //      cmp  esi, edx
   //      jae  short labYGPromEnd
   //      labYGPromNext2 :
   //   mov  ecx, [esi]
   //      test ecx, ecx
   //      jns  short labYGPromNext
   //      add  ecx, page_ceil
   //      mov  eax, ebp
   //      and ecx, page_align_mask
   //      // ; raise an exception if it is not enough memory to promote object
   //      lea  eax, [eax + elObjectOffset]
   //      sub  ebx, ecx
   //      js   short labError
   //      mov[edi], eax
   //      mov  eax, ecx
   //      shr  eax, page_size_order_minus2
   //      add  edi, eax
   //      labYGProm :
   //   mov  eax, [esi]
   //      sub  ecx, 4
   //      mov[ebp], eax
   //      lea  esi, [esi + 4]
   //      lea  ebp, [ebp + 4]
   //      jnz  short labYGProm
   //      cmp  esi, edx
   //      jb   short labYGPromNext2
   //      labYGPromEnd :
   //
   //   // ; get previous heap end
   //   mov  edx, [data:% CORE_GC_TABLE + gc_mg_current]
   //
   //      // ; set mg_current, clear yg and survive
   //      mov[data:% CORE_GC_TABLE + gc_mg_current], ebp
   //      mov  eax, [data:% CORE_GC_TABLE + gc_yg_start]
   //      mov[data:% CORE_GC_TABLE + gc_yg_current], eax
   //
   //      // ; fix roots
   //      lea  eax, [esp + 4]
   //      mov  ecx, [eax]
   //      mov  esi, [esp + 8]
   //
   //      mov  ebx, [data:% CORE_GC_TABLE + gc_yg_start]
   //      mov  ebp, [data:% CORE_GC_TABLE + gc_start]
   //
   //      labFixRoot:
   //   push eax
   //      call labFixObject
   //      pop  eax
   //      lea  eax, [eax + 8]
   //      mov  esi, [eax + 4]
   //      test esi, esi
   //      mov  ecx, [eax]
   //      jnz  short labFixRoot
   //
   //      // ; clear WBar
   //      mov  ecx, [data:% CORE_GC_TABLE + gc_end]
   //      mov  edi, [data:% CORE_GC_TABLE + gc_mg_wbar]
   //      sub  ecx, [data:% CORE_GC_TABLE + gc_mg_start]
   //      xor eax, eax
   //      shr  ecx, page_size_order
   //      rep  stos
   //
   //      // ; free root set
   //      mov  esp, [esp]
   //      pop  edx
   //      pop  ebp
   //
   //      // ; allocate
   //      mov  eax, [data:% CORE_GC_TABLE + gc_yg_current]
   //      mov  ecx, [data:% CORE_GC_TABLE + gc_yg_end]
   //      add  edx, eax
   //      cmp  edx, ecx
   //      jae  labBigAlloc
   //      lea  ebx, [eax + elObjectOffset]
   //      mov[data:% CORE_GC_TABLE + gc_yg_current], edx
   //      ret
   //
   //      labError :
   //   // ; restore stack
   //   mov  esp, [esp]
   //      pop  ecx
   //      pop  ebp
   //
   //      labError2 :
   //   mov  ebx, 17h
   //      call code : % BREAK
   //      ret
   //
   //      // ; bad luck, we have to expand GC
   //      labBigAlloc2 :
   //   push ecx
   //
   //      mov  eax, [data:% CORE_GC_TABLE + gc_end]
   //      mov  ecx, 2A000h
   //      call code : % EXPAND_HEAP
   //
   //      mov  eax, [data:% CORE_GC_TABLE + gc_header]
   //      mov  ecx, [data:% CORE_GC_TABLE + gc_end]
   //      sub  ecx, [data:% CORE_GC_TABLE + gc_start]
   //      shr  ecx, page_size_order_minus2
   //      add  eax, ecx
   //      mov  ecx, 0A800h
   //      call code : % EXPAND_HEAP
   //
   //      mov  ecx, [data:% CORE_GC_TABLE + gc_end]
   //      add  ecx, 15000h
   //      mov[data:% CORE_GC_TABLE + gc_end], ecx
   //
   //      pop  ecx
   //
   //      labBigAlloc :
   //   // ; try to allocate in the mg
   //   sub  ecx, eax
   //      cmp  ecx, 800000h
   //      jae  labError2
   //
   //      mov  eax, [data:% CORE_GC_TABLE + gc_mg_current]
   //      mov  edx, [data:% CORE_GC_TABLE + gc_end]
   //      add  ecx, eax
   //      cmp  ecx, edx
   //      jae  labBigAlloc2
   //      mov[data:% CORE_GC_TABLE + gc_mg_current], ecx
   //      lea  ebx, [eax + elObjectOffset]
   //
   //      ret
   //
   //      // ; start collecting: esi => ebp, [ebx, edx] ; ecx - count
   //      labCollectYG :
   //   push 0
   //
   //      lea  ecx, [ecx + 4]
   //      lea  esi, [esi - 4]
   //
   //      labYGNext :
   //      lea  esi, [esi + 4]
   //      sub  ecx, 4
   //      jz   labYGResume
   //
   //      labYGCheck :
   //   mov  eax, [esi]
   //
   //      // ; check if it valid reference
   //      mov  edi, eax
   //      cmp  edi, ebx
   //      setb al
   //      cmp  edx, edi
   //      setb ah
   //      test eax, 0FFFFh
   //      mov  eax, edi
   //      jnz  labYGNext
   //
   //      // ; check if it was collected
   //      mov  edi, [eax - elSizeOffset]
   //      test edi, edi
   //      js   labYGContinue
   //
   //      // ; save previous ecx field
   //      push ecx
   //
   //      // ; copy object size
   //      mov[ebp], edi
   //
   //      // ; copy object vmt
   //      mov  ecx, [eax - elVMTOffset]
   //      mov[ebp + 04h], ecx
   //
   //      // ; mark as collected
   //      or [eax - elSizeOffset], 80000000h
   //
   //      // ; reserve shadow YG
   //      mov  ecx, edi
   //      add  ecx, page_ceil
   //      lea  edi, [ebp + elObjectOffset]
   //      and ecx, page_align_mask
   //      mov[esi], edi          // ; update reference
   //      add  ebp, ecx
   //
   //      // ; get object size
   //      mov  ecx, [eax - elSizeOffset]
   //      and ecx, 8FFFFFh
   //
   //      // ; save ESI
   //      push esi
   //      mov  esi, eax
   //
   //      // ; save new reference
   //      mov[eax - elVMTOffset], edi
   //
   //      // ; check if the object has fields
   //      cmp  ecx, 800000h
   //
   //      // ; save original reference
   //      push eax
   //
   //      // ; collect object fields if it has them
   //      jb   labYGCheck
   //
   //      lea  esp, [esp + 4]
   //      jz   short labYGSkipCopyData
   //
   //      // ; copy meta data object to shadow YG
   //      add  ecx, 3
   //      and ecx, 0FFFFCh
   //
   //      labYGCopyData :
   //   mov  eax, [esi]
   //      sub  ecx, 4
   //      mov[edi], eax
   //      lea  esi, [esi + 4]
   //      lea  edi, [edi + 4]
   //      jnz  short labYGCopyData
   //
   //      labYGSkipCopyData :
   //   pop  esi
   //      pop  ecx
   //      jmp  labYGNext
   //
   //      labYGResume :
   //   // ; copy object to shadow YG
   //   pop  edi
   //      test edi, edi
   //      jz   short labYGEnd
   //
   //      mov  ecx, [edi - elSizeOffset]
   //      mov  esi, [edi - elVMTOffset]
   //      and ecx, 0FFFFFh
   //
   //      labYGCopy :
   //   mov  eax, [edi]
   //      sub  ecx, 4
   //      mov[esi], eax
   //      lea  esi, [esi + 4]
   //      lea  edi, [edi + 4]
   //      jnz  short labYGCopy
   //
   //      pop  esi
   //      pop  ecx
   //      jmp  labYGNext
   //
   //      nop
   //      labYGEnd :
   //   ret
   //
   //      labYGContinue :
   //   // ; update reference
   //   mov  edi, [eax - elVMTOffset]
   //      mov[esi], edi
   //      jmp  labYGNext
   //
   //      // ---- start collecting: esi => ebp, [ebx, edx] ; ecx - count ---
   //      labCollectMG :
   //
   //   lea  ecx, [ecx + 4]
   //      push 0
   //      lea  esi, [esi - 4]
   //      push 0
   //
   //      labMGNext :
   //      sub  ecx, 4
   //      lea  esi, [esi + 4]
   //      jz   short labMGResume
   //
   //      labMGCheck :
   //   mov  eax, [esi]
   //
   //      // ; check if it valid reference
   //      mov  edi, eax
   //      cmp  edi, ebx
   //      setb al
   //      cmp  edx, edi
   //      setb ah
   //      test eax, 0FFFFh
   //      mov  eax, edi
   //      jnz  labMGNext
   //
   //      // ; check if it was collected
   //      mov  edi, [eax - elSizeOffset]
   //      test edi, edi
   //      js   short labMGNext
   //
   //      // ; mark as collected
   //      or [eax - elSizeOffset], 080000000h
   //
   //      cmp  edi, 0800000h
   //      jae  short labMGNext
   //
   //      // ; save previous ecx field
   //      push ecx
   //
   //      // ; get object size
   //      mov  ecx, [eax - elSizeOffset]
   //
   //      // ; save ESI
   //      push esi
   //      and ecx, 0FFFFFh
   //
   //      mov  esi, eax
   //
   //      // ; collect object fields if it has them
   //      jmp   short labMGCheck
   //
   //      labMGResume :
   //   pop  esi
   //      pop  ecx
   //      test esi, esi
   //      jnz  short labMGNext
   //
   //      nop
   //      labMGEnd :
   //   ret
   //
   //      labFixObject :
   //
   //   lea  ecx, [ecx + 4]
   //      push 0
   //      lea  esi, [esi - 4]
   //      push 0
   //
   //      labFixNext :
   //      sub  ecx, 4
   //      lea  esi, [esi + 4]
   //      jz   short labFixResume
   //
   //      labFixCheck :
   //   mov  eax, [esi]
   //
   //      // ; check if it valid reference
   //      mov  edi, eax
   //      cmp  edi, ebx
   //      setb al
   //      cmp  edx, edi
   //      setb ah
   //      test eax, 0FFFFh
   //      mov  eax, edi
   //      jnz  labFixNext
   //
   //      lea  edi, [eax - elObjectOffset]
   //
   //      sub  edi, ebp
   //      shr  edi, page_size_order_minus2
   //      add  edi, [data:% CORE_GC_TABLE + gc_header]
   //
   //      mov  eax, [edi]
   //      mov[esi], eax
   //
   //      // ; make sure the object was not already fixed
   //      mov  edi, [eax - elSizeOffset]
   //      test edi, edi
   //      jns  short labFixNext
   //
   //      and edi, 7FFFFFFFh
   //      mov[eax - elSizeOffset], edi
   //
   //      cmp  edi, 0800000h
   //      jae  short labFixNext
   //
   //      // ; save previous ecx field
   //      push ecx
   //
   //      // ; get object size
   //      mov  ecx, [eax - elSizeOffset]
   //      and ecx, 0FFFFFh
   //
   //      // ; save ESI
   //      push esi
   //      mov  esi, eax
   //
   //      // ; collect object fields if it has them
   //      jmp   short labFixCheck
   //
   //      labFixResume :
   //   pop  esi
   //      pop  ecx
   //      test esi, esi
   //      jnz  short labFixNext
   //      nop
   //
   //      ret
}
