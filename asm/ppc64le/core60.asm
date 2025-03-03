// ; --- Predefined References  --
define GC_ALLOC	            10002h
define VEH_HANDLER          10003h
define GC_COLLECT	    10004h
define GC_ALLOCPERM	    10005h
define PREPARE	            10006h
define THREAD_WAIT          10007h

define CORE_TOC             20001h
define SYSTEM_ENV           20002h
define CORE_GC_TABLE        20003h
define CORE_MATH_TABLE      20004h
define CORE_SINGLE_CONTENT  2000Bh
define VOID           	    2000Dh
define VOIDPTR              2000Eh
define CORE_THREAD_TABLE    2000Fh

define ACTION_ORDER              9
define ACTION_MASK            1E0h
define ARG_MASK               01Fh
define ARG_ACTION_MASK        1DFh

// ; TOC TABLE OFFSETS
define toc_import            0000h
define toc_rdata             0008h
define toc_mdata             0010h
define toc_code              0018h
define toc_gctable           0020h
define toc_alloc             0028h
define toc_data              0030h
define toc_stat              0038h
define toc_allocperm         0040h
define toc_prepare           0048h

// ; --- Object header fields ---
define elSizeOffset          0004h
define elVMTOffset           0010h 
define elObjectOffset        0010h

// ; --- VMT header fields ---
define elVMTSizeOffset       0008h
define elVMTFlagOffset       0018h
define elPackageOffset       0020h

// ; --- GC TABLE OFFSETS ---
define gc_header             0000h
define gc_start              0008h
define gc_yg_start           0010h
define gc_yg_current         0018h
define gc_yg_end             0020h
define gc_mg_start           0038h
define gc_mg_current         0040h
define gc_end                0048h
define gc_mg_wbar            0050h
define gc_perm_start         0058h 
define gc_perm_end           0060h 
define gc_perm_current       0068h 

define et_current            0008h
define tt_stack_frame        0010h
define tt_stack_root         0028h

define es_prev_struct        0000h
define es_catch_addr         0008h
define es_catch_level        0010h
define es_catch_frame        0018h

// ; --- Page Size ----
define page_ceil               2Fh
define page_mask        0FFFFFFE0h
define page_size_order          5h
define struct_mask_inv     7FFFFFh
define struct_mask_inv_lo   0FFFFh
define struct_mask_inv_hi      7Fh

define struct_mask       40000000h
define struct_mask_lo        0000h
define struct_mask_hi        4000h

// ; --- System Core Preloaded Routines --

structure % CORE_TOC

  dq import : 0             // ; address of import section
  dq rdata  : 0             // ; address of rdata section
  dq mdata  : 0             // ; address of rdata section
  dq code   : 0             // ; address of code section
  dq data   : %CORE_GC_TABLE
  dq code   : %GC_ALLOC     // ; address of alloc function
  dq data   : 0             // ; address of data section
  dq stat   : 0             // ; address of stat section
  dq code   : %GC_ALLOCPERM // ; address of alloc function
  dq code   : %PREPARE      // ; address of alloc function

end

structure % CORE_MATH_TABLE

  dbl "1.4426950408889634074"      // ; 00 : FM_DOUBLE_LOG2OFE
  dbl "2.30933477057345225087e-2"  // ; 08 : 
  dbl "2.02020656693165307700e1"   // ; 16 : 
  dbl "1.51390680115615096133e3"   // ; 24 : 
  dbl "1.0"                        // ; 32 : 
  dbl "0.5"                        // ; 40 : 
  dbl "2.33184211722314911771e2"   // ; 48
  dbl "4.36821166879210612817e3"   // ; 56

  dbl "1.41421356237309504880"     // ; 64

  dbl "1.01875663804580931796e-4"  // ; 72
  dbl "4.97494994976747001425e-1"  // ; 80
  dbl "4.70579119878881725854e0"   // ; 88
  dbl "1.44989225341610930846e1"   // ; 96
  dbl "1.79368678507819816313e1"   // ; 104
  dbl "7.70838733755885391666e0"   // ; 112

  dbl "1.12873587189167450590e1"   // ; 120
  dbl "4.52279145837532221105e1"   // ; 128
  dbl "8.29875266912776603211e1"   // ; 136
  dbl "7.11544750618563894466e1"   // ; 144
  dbl "2.31251620126765340583e1"   // ; 152

  dbl "6.9314718055994530942e-1"   // ; 160

end
 
structure % CORE_SINGLE_CONTENT

  dq 0 // ; et_critical_handler    ; +x00   - pointer to ELENA critical handler
  dq 0 // ; et_current             ; +x08   - pointer to the current exception struct
  dq 0 // ; tt_stack_frame         ; +x10   - pointer to the stack frame
  dq 0 // ; reserved
  dq 0 // ; reserved
  dq 0 // ; tt_stack_root

end
 
structure % CORE_THREAD_TABLE

  // ; dummy for STA

end

structure %CORE_GC_TABLE

  dq 0 // ; gc_header             : +00h
  dq 0 // ; gc_start              : +08h
  dq 0 // ; gc_yg_start           : +10h
  dq 0 // ; gc_yg_current         : +18h
  dq 0 // ; gc_yg_end             : +20h
  dq 0 // ; gc_shadow             : +28h
  dq 0 // ; gc_shadow_end         : +30h
  dq 0 // ; gc_mg_start           : +38h
  dq 0 // ; gc_mg_current         : +40h
  dq 0 // ; gc_end                : +48h
  dq 0 // ; gc_mg_wbar            : +50h

  dq 0 // ; gc_perm_start         : +58h 
  dq 0 // ; gc_perm_end           : +60h 
  dq 0 // ; gc_perm_current       : +68h 

  dq 0 // ; gc_lock               : +70h 
  dq 0 // ; reserved              : +78h 

end
 
// ; NOTE : the table is tailed with GCMGSize,GCYGSize and MaxThread fields
structure %SYSTEM_ENV

  dq 0  
  dq 0  
  dq data : %CORE_GC_TABLE
  dq data : %CORE_SINGLE_CONTENT
  dq 0
  dq 0
  dq code : %VEH_HANDLER
  // ; dd GCMGSize
  // ; dd GCYGSize
  // ; dd ThreadCounter

end

structure %VOID

  dq 0
  dq 0  // ; a reference to the super class class
  dq 0
  dq 0  
  dq 0

end

structure %VOIDPTR

  dq rdata : %VOID + elPackageOffset
  dq 0
  dq 0

end

// ; --- GC_ALLOC ---
// ; in: r18 - size ; out: r15 - created object
inline % GC_ALLOC

  ld      r19, toc_gctable(r2)
  ld      r17, gc_yg_current(r19) 
  ld      r16, gc_yg_end(r19) 
  add     r18, r18, r17 
  cmp     r18, r16
  bge     labYGCollect
  std     r18, gc_yg_current(r19) 
  addi    r15, r17, elObjectOffset
  blr

labYGCollect:
  // ; save registers
  sub     r18, r18, r17 

  mflr    r0
  std     r31, -20h(r1)  // ; save frame pointer
  std     r0,  -18h(r1)  // ; save return address
  std     r3,  -10h(r1)
  std     r4,  -08h(r1)

  addi    r1, r1, -32    // ; allocate raw stack
  mr      r31, r1        // ; set frame pointer

  // ; lock frame
  ld      r16, toc_data(r2)
  addis   r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi    r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT
  std     r1, tt_stack_frame(r16)

  std     r18, -08h(r1)  
  addi    r1, r1, -16    // ; allocate raw stack

  // ; create set of roots
  mr      r31, r1
  li      r18, 0
  std     r18, -10h(r1)
  std     r18, -08h(r1)
  addi    r1, r1, -16

  // ;   save static roots
  ld      r17, toc_rdata(r2)
  addis   r17, r17, rdata_disp32hi : %SYSTEM_ENV
  addi    r17, r17, rdata_disp32lo : %SYSTEM_ENV

  ld      r19, toc_stat(r2)
  ld      r18, 0(r17)
  sldi    r18, r18, 3
  std     r18, -10h(r1)
  std     r19, -08h(r1)
  addi    r1, r1, -16

  // ;   collect frames
  ld      r19, tt_stack_frame(r16)
  mr      r18, r19

labYGNextFrame:
  mr      r17, r19
  ld      r19, 0(r17)
  cmpwi   r19, 0
  bne     labYGNextFrame

  std     r18, -8h(r1)
  subf    r18, r17, r18
  neg     r18, r18
  std     r18, -10h(r1)
  addi    r1, r1, -16

  ld      r19, 8(r17)
  cmpwi   r19, 0
  mr      r18, r19
  bne     labYGNextFrame

  std     r1, 0(r31)

  ld      r4, 8(r31)
  mr      r3, r1

  // ; call GC routine
  std     r2, -8h(r1)     // ; storing toc pointer
  std     r31, -10h(r1)   // ; storing toc pointer
  addi    r1, r1, -48     // ; allocating stack

  // ; restore frame to correctly display a call stack
  ld      r31, 0(r31)

  ld      r12, toc_import(r2)
  addis   r12, r12, import_disp32hi : "$rt.CollectGCLA"
  addi    r12, r12, import_disp32lo : "$rt.CollectGCLA"
  ld      r12,0(r12)

  mtctr   r12            // ; put code address into ctr
  bctrl                  // ; and call it

  ld      r2, 40(r1)     // ; restoring toc pointer
  ld      r31, 32(r1)    // ; restoring toc pointer

  mr      r15, r3

  mr      r1, r31              // ; restore stack pointer

  ld      r31, 10h(r1)         // ; restore frame pointer
  ld      r0,  18h(r1) 
  ld      r3,  20h(r1)
  ld      r4,  28h(r1)
  addi    r1, r1, 48           // ; free raw stack

  mtlr    r0
  blr

end

// ; --- GC_COLLECT ---
// ; in: ecx - fullmode (0, 1)
inline % GC_COLLECT

  mflr    r0
  std     r31, -20h(r1)  // ; save frame pointer
  std     r0,  -18h(r1)  // ; save return address
  std     r3,  -10h(r1)
  std     r4,  -08h(r1)

  addi    r1, r1, -32    // ; allocate raw stack
  mr      r31, r1        // ; set frame pointer

  // ; lock frame
  ld      r16, toc_data(r2)
  addis   r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi    r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT
  std     r1, tt_stack_frame(r16)

  std     r18, -08h(r1)  
  addi    r1, r1, -16    // ; allocate raw stack

  // ; create set of roots
  mr      r31, r1
  li      r18, 0
  std     r18, -10h(r1)
  std     r18, -08h(r1)
  addi    r1, r1, -16

  // ;   save static roots
  ld      r17, toc_rdata(r2)
  addis   r17, r17, rdata_disp32hi : %SYSTEM_ENV
  addi    r17, r17, rdata_disp32lo : %SYSTEM_ENV

  ld      r19, toc_stat(r2)
  ld      r18, 0(r17)
  sldi    r18, r18, 3
  std     r18, -10h(r1)
  std     r19, -08h(r1)
  addi    r1, r1, -16

  // ;   collect frames
  ld      r19, tt_stack_frame(r16)
  mr      r18, r19

labYGNextFrame:
  mr      r17, r19
  ld      r19, 0(r17)
  cmpwi   r19, 0
  bne     labYGNextFrame

  std     r18, -8h(r1)
  subf    r18, r17, r18
  neg     r18, r18
  std     r18, -10h(r1)
  addi    r1, r1, -16

  ld      r19, 8(r17)
  cmpwi   r19, 0
  mr      r18, r19
  bne     labYGNextFrame

  std     r1, 0(r31)

  ld      r4, 8(r31)
  mr      r3, r1

  // ; call GC routine
  std     r2, -8h(r1)     // ; storing toc pointer
  std     r31, -10h(r1)   // ; storing toc pointer
  addi    r1, r1, -48     // ; allocating stack

  // ; restore frame to correctly display a call stack
  ld      r31, 0(r31)

  ld      r12, toc_import(r2)
  addis   r12, r12, import_disp32hi : "$rt.CollectGCLA"
  addi    r12, r12, import_disp32lo : "$rt.CollectGCLA"
  ld      r12,0(r12)

  mtctr   r12            // ; put code address into ctr
  bctrl                  // ; and call it

  ld      r2, 40(r1)     // ; restoring toc pointer
  ld      r31, 32(r1)    // ; restoring toc pointer

  mr      r15, r3

  mr      r1, r31              // ; restore stack pointer

  ld      r31, 10h(r1)         // ; restore frame pointer
  ld      r0,  18h(r1) 
  ld      r3,  20h(r1)
  ld      r4,  28h(r1)
  addi    r1, r1, 48           // ; free raw stack

  mtlr    r0
  blr

end

// --- GC_ALLOCPERM ---
procedure %GC_ALLOCPERM

  ld      r19, toc_gctable(r2)
  ld      r17, gc_perm_current(r19) 
  ld      r16, gc_perm_end(r19) 
  add     r18, r18, r17 
  cmp     r18, r16
  bge     labYGCollect
  std     r18, gc_perm_current(r19) 
  addi    r15, r17, elObjectOffset
  blr

labYGCollect:
  // ; save registers
  sub     r18, r18, r17 

  mflr    r0
  std     r31, -20h(r1)  // ; save frame pointer
  std     r0,  -18h(r1)  // ; save return address
  std     r3,  -10h(r1)
  std     r4,  -08h(r1)

  addi    r1, r1, -32    // ; allocate raw stack
  mr      r31, r1        // ; set frame pointer

  // ; lock frame
  ld      r16, toc_data(r2)
  addis   r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi    r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT
  std     r1, tt_stack_frame(r16)

  ld      r12, toc_import(r2)
  addis   r12, r12, import_disp32hi : "$rt.CollectPermGCLA"
  addi    r12, r12, import_disp32lo : "$rt.CollectPermGCLA"
  ld      r12,0(r12)

  mtctr   r12            // ; put code address into ctr
  bctrl                  // ; and call it

  ld      r2, 40(r1)     // ; restoring toc pointer
  ld      r31, 32(r1)    // ; restoring toc pointer

  mr      r15, r3

  mr      r1, r31              // ; restore stack pointer

  ld      r31, 10h(r1)         // ; restore frame pointer
  ld      r0,  18h(r1) 
  ld      r3,  20h(r1)
  ld      r4,  28h(r1)
  addi    r1, r1, 48           // ; free raw stack

  mtlr    r0
  blr

end

procedure %PREPARE

  mflr    r0
  std     r2,  -10h(r1)
  std     r0,  -08h(r1)

  addi    r1, r1, -16    // ; allocate raw stack

  std     r4,  -08h(r1)
  std     r3,  -10h(r1)
  addi    r1, r1, -16    // ; allocate raw stack

  mr      r3, r1

  ld      r12, toc_import(r2)
  addis   r12, r12, import_disp32hi : "$rt.PrepareLA"
  addi    r12, r12, import_disp32lo : "$rt.PrepareLA"
  ld      r12,0(r12)

  mtctr   r12            // ; put code address into ctr
  bctrl                  // ; and call it

  addi    r1, r1, 16     // ; free raw stack

  ld      r2, 0(r1)     // ; restore frame pointer
  ld      r0, 08h(r1) 
  addi    r1, r1, 16     // ; free raw stack

  mtlr    r0
  blr

end

procedure %THREAD_WAIT

end

// ; ==== Command Set ==

// ; snop
inline % 2

end

// ; redirect
inline % 03h //; (r15 - object, r14 - message)

  ld      r16, -elVMTOffset(r15)      //; edi
  xor     r17, r17, r17               //; ecx 
  ld      r7, -elVMTSizeOffset(r16)   //; esi
  li      r19, 1

labSplit:
  cmpwi   r7, 0
  beq     labEnd

labStart:
  andi.   r0, r7, 1
  srdi    r7, r7, 1
  iseleq  r21, r19, r17                  //; ecx

  sldi    r22, r7, 4
  add     r22, r22, r16                  //; edx

  ld      r23, 0(r22)
  cmp     r14, r23
  beq     labFound
  addi    r22, r22, 16  
  blt     labSplit
  mr      r16, r22
  subf    r7, r21, r7
  b       labSplit
labFound:
  ld      r23, 8(r22) 
  mtctr   r23
  bctr

labEnd:
                               
end

// ; quit
inline %4

  blr

end

// ; movenv
inline %5

  ld      r14, toc_rdata(r2)
  addis   r14, r14, rdata_disp32hi : %SYSTEM_ENV
  addi    r14, r14, rdata_disp32lo : %SYSTEM_ENV

end

// ; load
inline %6

  lwz     r14, 0(r15)

end

// ; len
inline %7

  li      r16, struct_mask_inv_lo
  addis   r16, r16, struct_mask_inv_hi

  ld      r14, -elSizeOffset(r15)
  and     r14, r14, r16
  srdi    r14, r14, 3

end

// ; class
inline %8

  ld      r15, -elVMTOffset(r15)

end

// ; save
inline %9

  stw      r14, 0(r15)

end

// ; throw
inline %0Ah

  ld      r16, toc_data(r2)
  addis   r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi    r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT

  ld      r17, et_current(r16)
  ld      r0, es_catch_addr(r17)
  mtctr   r0
  bctr

end

// ; unhook
inline %0Bh

  ld      r16, toc_data(r2)
  addis   r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi    r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT

  ld      r19, et_current(r16)

  ld      r17, es_prev_struct(r19)
  ld      r1, es_catch_level(r19)
  ld      r31, es_catch_frame(r19)

  std     r17, et_current(r16)

end

// ; loadv
inline % 0Ch

  andi.   r14, r14, ARG_MASK

  li      r19, ~ARG_MASK
  andi.   r19, r19, 0FFFFh
  addis   r19, r19, 0FFFFh // ; note: to adjust hi word                    

  lwz     r18, 0(r15)
  and     r18, r18, r19

  or      r14, r14, r18

end

// ; xcmp
inline %0Dh

  lwz      r18, 0(r15)
  cmp      r14, r18

end

// ; bload
inline %0Eh

  lwz     r14, 0(r15)
  andi.   r14, r14, 0FFh

end

// ; wload
inline %0Fh

  lha     r14, 0(r15)

end

// ; exclude
inline % 10h

  li      r18, 0

  std     r31, -10h(r1)  // ; save frame pointer
  std     r18, -08h(r1)  // ; save return address
  addi    r1, r1, -16    // ; allocate raw stack

  ld      r16, toc_data(r2)
  addis   r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi    r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT
  std     r1, tt_stack_frame(r16)

end

// ; include
inline % 11h

  addi    r1, r1, 10h          // ; free stack

end

// ; assign
inline %12h

  sldi    r16, r14, 3
  add     r16, r16, r15

  // calculate write-barrier address
  ld      r19, toc_gctable(r2)

  ld      r17, gc_start(r19)
  ld      r18, gc_header(r19)

  subf    r19, r17, r15
  srdi    r19, r19, page_size_order

  std     r3, 0(r16)
  li      r20, 1
  add     r18, r18, r19
  stb     r20, 0(r18)

end

// ; movfrm
inline %13h

  mr  r14, r31

end

// ; loads
inline % 14h

  ld      r22, 0(r15)
  ld      r24, toc_mdata(r2)
  srdi    r22, r22, ACTION_ORDER
  add     r24, r24, r22
  ld      r14, 0(r24)  

end

// ; mlen
inline % 15h

  andi.   r14, r14, ARG_MASK

end

// ; dalloc
inline % 16h

  sldi    r16, r14, 3

  addi    r16, r16, 8     // ; rounding to 10h
  srdi    r16, r16, 4
  sldi    r16, r16, 4

  sub     r1, r1,  r16   // ; allocate stack
  li      r17, 0
  mr      r18, r1 

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:

end

// ; tststck
inline %17h

  ld      r16, toc_data(r2)
  addis   r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi    r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT
  ld      r16, tt_stack_root(r16)

  li      r17, 0
  li      r18, 1
  cmpld   r15, r1
  isellt  r19, r18, r17
  cmpld   r15, r16
  iselgt  r20, r18, r17
  or      r19, r19, r20

  cmpwi   r19, 0

end

// ; dtrans
inline %18h

  mr      r16, r14
  mr      r19, r3
  mr      r18, r15

labLoop:
  cmpwi   r16,0
  beq     labEnd
  ld      r17, 0(r19)
  addi    r16, r16, -1
  std     r17, 0(r18)
  addi    r18, r18, 8
  addi    r19, r19, 8
  b       labLoop

labEnd:

end

// ; xassign
inline %19h

  sldi    r16, r14, 3
  add     r16, r16, r15
  std     r3, 0(r16)

end

// ; lload
inline %1Ah

  ld      r14, 0(r15)

end

// ; convl
inline %1Bh

  extsw   r14, r14

end

// ; xlcmp
inline % 1Ch

  ld      r18, 0(r15)
  cmp     r18, r14

end

// ; xload
inline %1Dh

  add     r18, r18, r15
  lwz     r14, 0(r18)

end

// ; xlload
inline %1Eh

  add     r18, r18, r15
  ld      r14, 0(r18)

end

// ; lneg
inline % 1Fh

   neg    r14, r14

end

// ; coalesce
inline % 20h

  cmpwi   r15,0
  iseleq  r15, r3, r15

end

// ; not
inline % 21h

   nand    r14, r14, r14

end

// ; neg
inline % 22h

   neg    r14, r14

end

// ; bread
inline %23h

  add     r19, r3, r14
  lbz     r17, 0(r19)
  std     r17, 0(r15)

end

// ; lsave
inline %24h

  std     r14, 0(r15)

end

// ; fsave
inline %25h

  extsw   r17, r14
  std     r17, 0(r1) 
  lfd     f17, 0(r1) 
  fcfid   f17, f17
  stfd    f17, 0(r15)

end

// ; wread
inline %26h

  sldi    r19, r14, 1
  add     r19, r3, r19
  lwz     r17, 0(r19)
  stw     r17, 0(r15)

end

// ; xjump
inline %027h

  mtctr    r15            // ; put code address into ctr
  bctr                    // ; and jump to it

end

// ; bcopy
inline %28h

  lbz     r17, 0(r3)
  std     r17, 0(r15)

end

// ; wcopy
inline %29h

  lwz     r17, 0(r3)
  stw     r17, 0(r15)

end

// ; xpeekeq
inline %02Ah

  iseleq  r15, r3, r15

end


// ; trylock
inline %02Bh

end

// ; freelock
inline %02Ch

end

// ; parent
inline %02Dh

  ld      r15, -elPackageOffset(r15)

end

// ; xget
inline %02Eh

  sldi    r22, r14, 3
  add     r22, r22, r15
  ld      r15, 0(r22)

end

// ; xcall
inline %02Fh

  mtctr    r15            // ; put code address into ctr
  bctrl                   // ; and call it

end

// ; xfsave
inline %30h

  stfd    f3, 0(r15)

end

// ; xquit
inline %034h

  mr      r3, r14
  blr

end

// ; fiadd
inline %070h

  lfd     f18, 0(r3) 
  fcfid   f18, f18
  lfd     f17, 0(r15)
  fadd    f17, f17, f18  
  stfd    f17, 0(r15)

end

// ; fsub
inline %071h

  lfd     f18, 0(r3) 
  fcfid   f18, f18
  lfd     f17, 0(r15)
  fsub    f17, f18, f17
  stfd    f17, 0(r15)

end

// ; fmul
inline %072h

  lfd     f18, 0(r3) 
  fcfid   f18, f18
  lfd     f17, 0(r15)
  fmul    f17, f17, f18  
  stfd    f17, 0(r15)

end

// ; fdiv
inline %073h

  lfd     f18, 0(r3) 
  fcfid   f18, f18
  lfd     f17, 0(r15)
  fdiv    f17, f18, f17
  stfd    f17, 0(r15)

end

// ; shl
inline %75h

  li      r18, __n16_1
  sld     r14, r14, r18

end

// ; shr
inline %76h

  li      r18, __n16_1
  srd     r14, r14, r18

end

// ; xsaven
inline %77h

  li      r18, __n16_1
  stw     r18, 0(r15)

end

// ; xsaven
inline %977h

  lis     r18, __n16hi_1
  li      r19, __n16lo_1
  andi.   r19, r19, 0FFFFh
  add     r18, r18, r19
  stw     r18, 0(r15)

end

// ; xsaven
inline %0A77h

  li      r18, __n16lo_1
  andi.   r18, r18, 0FFFFh
  addis   r18, r18, __n16hi_1
  stw     r18, 0(r15)

end

// ; fabsdp
inline %078h

  addi    r19, r31, __arg16_1

  lfd     f17, 0(r3)
  fabs    f17, f17
  stfd    f17, 0(r19)

end

// ; fsqrtdp
inline %079h

  addi    r19, r31, __arg16_1

  lfd     f17, 0(r3)
  fsqrt   f17, f17
  stfd    f17, 0(r19)

end

// ; fexpdp
inline %07Ah

  ld      r17, toc_rdata(r2)
  addis   r17, r17, rdata_disp32hi : %CORE_MATH_TABLE
  addi    r17, r17, rdata_disp32lo : %CORE_MATH_TABLE

  addi    r19, r31, __arg16_1 // ; dest (r19)

  lfd     f17, 0(r3)          // ; x (f17)

  // ; x = x * FM_DOUBLE_LOG2OFE
  lfd     f20, 0(r17)
  fmul    f17, f17, f20

  // ; ipart = x + 0.5
  lfd     f20, 40(r17)
  fadd    f18, f17, f20
                              // ; ipart(f18)
  friz    f18, f18            // ; ipart = floor(ipart)

  fsub    f19, f17, f18       // ; fpart = x - ipart; (f19)

  // ; FM_DOUBLE_INIT_EXP(epart,ipart);
  fctidz  f20, f18
  stfd    f20, 0(r19) 
  ld      r18, 0(r19) 
  li      r20, 1023
  add     r18, r18, r20 
  li      r20, 20
  sld     r18, r18, r20
  li      r20, 0
  std     r20, 0(r19)
  stw     r18, 4(r19)

  fmul    f17, f19, f19       // ; x = fpart*fpart;

  lfd     f20, 8(r17)         // ; px =        fm_exp2_p[0];

  // ; px = px*x + fm_exp2_p[1];
  fmul    f20, f20, f17
  lfd     f21, 16(r17)
  fadd    f20, f20, f21

  // ; qx =    x + fm_exp2_q[0];
  lfd     f22, 48(r17)
  fadd    f22, f22, f17

  // ; px = px*x + fm_exp2_p[2];
  fmul    f20, f20, f17
  lfd     f21, 24(r17)
  fadd    f20, f20, f21

  // ; qx = qx*x + fm_exp2_q[1];
  fmul    f22, f22, f17
  lfd     f21, 56(r17)
  fadd    f22, f22, f21

  // ; px = px * fpart;
  fmul    f20, f20, f19

  // ; x = 1.0 + 2.0*(px/(qx-px))
  lfd     f16, 32(r17)

  // ; mr      f17, f16
  stfd    f16, -8(r1)
  lfd     f17, -8(r1)

  fadd    f16, f16, f16

  fsub    f21, f22, f20
  fdiv    f21, f20, f21
  fmul    f16, f16, f21
  fadd    f17, f17, f16

  // ; epart.f*x;
  lfd     f20, 0(r19)
  fmul    f20, f20, f17
  stfd    f20, 0(r19) 

end

// ; fln
inline %07Bh

  ld      r17, toc_rdata(r2)
  addis   r17, r17, rdata_disp32hi : %CORE_MATH_TABLE
  addi    r17, r17, rdata_disp32lo : %CORE_MATH_TABLE

  addi    r19, r31, __arg16_1 // ; dest (r19)

  lfd     f17, 0(r3)          // ; x (f17)

//;   udi_t val;
//;   double z (f21), px(f18), qx(f19);
//;   int32_t ipart (r18), fpart (r16);

//;   val.f = x;
  stfd    f17, 0(r19) 

  //; extract exponent and part of the mantissa */

//;   fpart = val.s.i1 & FM_DOUBLE_MMASK;
  lis     r20, 0Fh
  addi    r20, r18, 0FFFFh
  lwz     r16, 4(r19)
  and     r16, r16, r20
//;   ipart = val.s.i1 & FM_DOUBLE_EMASK;
  lis     r20, 7FF0h
  lwz     r18, 4(r19)
  and     r18, r18, r20

//;   /* set exponent to 0 to get the prefactor to 2**ipart */
//;   fpart |= FM_DOUBLE_EZERO;
  lis     r20, 3FF0h
  or      r16, r16, r20
//;   val.s.i1 = fpart;
  stw     r16, 4(r19)
//;   x = val.f;
  lfd     f17, 0(r19)

//;   /* extract exponent */
//;   ipart >>= FM_DOUBLE_MBITS;
  li      r20, 20
  srd     r18, r18, r20

//;   ipart -= FM_DOUBLE_BIAS;
  li      r20, 1023
  sub     r18, r18, r20

//;   /* the polynomial is computed for sqrt(0.5) < x < sqrt(2),
//;      but we have the mantissa in the interval 1 < x < 2.
//;      adjust by dividing x by 2 and incrementing ipart, if needed. */
//;   if (x > FM_DOUBLE_SQRT2) {
  lfd     f20, 64(r17)
  fcmpu   f17, f20
  blt     labSkip
  beq     labSkip

//;      x *= 0.5;
  lfd     f20, 40(r17)
  fmul    f17, f17, f20
//;      ++ipart;
  addi    r18, r18, 1   
//;   }
labSkip:

//;   /* use polynomial approximation for log(1+x) */
//;   x -= 1.0;
  lfd     f20, 32(r17)
  fsub    f17, f17, f20

//;   px = fm_log2_p[0];
  lfd     f18, 72(r17)

//;   px = px * x + fm_log2_p[1];
  lfd     f20, 80(r17)
  fmul    f18, f18, f17
  fadd    f18, f18, f20

//;   px = px * x + fm_log2_p[2];
  lfd     f20, 88(r17)
  fmul    f18, f18, f17
  fadd    f18, f18, f20

//;   px = px * x + fm_log2_p[3];
  lfd     f20, 96(r17)
  fmul    f18, f18, f17
  fadd    f18, f18, f20

//;   px = px * x + fm_log2_p[4];
  lfd     f20, 104(r17)
  fmul    f18, f18, f17
  fadd    f18, f18, f20

//;   px = px * x + fm_log2_p[5];
  lfd     f20, 112(r17)
  fmul    f18, f18, f17
  fadd    f18, f18, f20

//;   qx = x + fm_log2_q[0];
  lfd     f20, 120(r17)
  fadd    f19, f17, f20

//;   qx = qx * x + fm_log2_q[1];
  lfd     f20, 128(r17)
  fmul    f19, f19, f17
  fadd    f19, f19, f20

//;   qx = qx * x + fm_log2_q[2];
  lfd     f20, 136(r17)
  fmul    f19, f19, f17
  fadd    f19, f19, f20

//;   qx = qx * x + fm_log2_q[3];
  lfd     f20, 144(r17)
  fmul    f19, f19, f17
  fadd    f19, f19, f20

//;   qx = qx * x + fm_log2_q[4];
  lfd     f20, 152(r17)
  fmul    f19, f19, f17
  fadd    f19, f19, f20

//;   z = x * x;
  fmul    f21, f17, f17

//;   z = x * (z * px / qx) - 0.5 * z + x;
  fmul    f20, f21, f18
  fmul    f20, f20, f19
  fmul    f22, f20, f17

  lfd     f20, 40(r17)
  fmul    f20, f20, f21
  fadd    f20, f20, f17

  fsub    f21, f22, f20 

//;   z += ((double)ipart) * FM_DOUBLE_LOGEOF2;
  std     r18, 0(r19) 
  lfd     f20, 0(r19) 
  fcfid   f20, f20
  lfd     f18, 160(r17)
  fmul    f20, f20, f18
  fadd    f21, f21, f20

  stfd    f20, 0(r19) 

end

// ; fsin
inline %07Ch
end

// ; fcos
inline %07Dh
end

// ; farchtan
inline %07Eh
end

// ; fpi
inline %07Fh
end

// ; setr
inline %80h

  ld      r15, toc_code(r2)
  addis   r15, r15, __xdisp32hi_1 
  addi    r15, r15, __xdisp32lo_1 

end 

// ; setr 0
inline %180h

  li      r15, 0

end 

// ; setr -1
inline %980h

  li      r15, 0
  addi    r15, r15, -1

end 

// ; setdp
inline %81h

  addi    r15, r31, __arg16_1

end 

// ; nlen n
inline %82h

  li      r16, struct_mask_inv_lo
  addis   r16, r16, struct_mask_inv_hi

  li      r18, __n16_1

  lwz     r14, -elSizeOffset(r15)
  and     r14, r14, r16 
  divw    r14, r14, r18  

end

// ; nlen 1
inline %182h

  li      r16, struct_mask_inv_lo
  addis   r16, r16, struct_mask_inv_hi

  lwz     r14, -elSizeOffset(r15)
  and     r14, r14, r16

end

// ; nlen 2
inline %282h

  li      r16, struct_mask_inv_lo
  addis   r16, r16, struct_mask_inv_hi

  lwz     r14, -elSizeOffset(r15)
  and     r14, r14, r16
  srdi    r14, r14, 1

end

// ; nlen 4
inline %382h

  li      r16, struct_mask_inv_lo
  addis   r16, r16, struct_mask_inv_hi

  lwz     r14, -elSizeOffset(r15)
  and     r14, r14, r16
  srdi    r14, r14, 2

end

// ; nlen 8
inline %482h

  li      r16, struct_mask_inv_lo
  addis   r16, r16, struct_mask_inv_hi

  lwz     r14, -elSizeOffset(r15)
  and     r14, r14, r16
  srdi    r14, r14, 3

end

// ; xassigni
inline %83h

  addi    r16, r15, __arg16_1
  std     r3, 0(r16)

end

// ; peekr
inline %84h

  ld      r16, toc_code(r2)
  addis   r16, r16, __xdisp32hi_1 
  addi    r16, r16, __xdisp32lo_1 

  ld      r15, 0(r16)

end 

// ; storer
inline %85h

  ld      r16, toc_code(r2)
  addis   r16, r16, __xdisp32hi_1 
  addi    r16, r16, __xdisp32lo_1 

  std     r15, 0(r16)

end 

// ; xswapsi
inline %86h

  mr       r16, r3
  ld       r3, __arg16_1(r1)
  std      r16, __arg16_1(r1)

end

// ; xswapsi 0
inline %186h

end

// ; xswapsi 1
inline %286h

  mr      r16, r3
  mr      r3, r4
  mr      r4, r16

end

// ; swapsi
inline %87h

  mr       r16, r15
  ld       r15, __arg16_1(r1)
  std      r16, __arg16_1(r1)

end

// ; swapsi 0
inline %187h

  mr      r16, r15
  mr      r15, r3
  mr      r3, r16

end

// ; swapsi 1
inline %287h

  mr      r16, r15
  mr      r15, r4
  mr      r4, r16

end

// ; movm
inline %88h

  lis     r14, __arg32hi_1
  addi    r14, r14, __arg32lo_1

end

// ; movn
inline %89h

  li      r14, __n16_1

end

// ; loaddp
inline %8Ah

  addi    r16, r31, __arg16_1
  lwz     r14, 0(r16)

end 

// ; xcmpdp
inline %8Bh

  addi    r16, r31, __arg16_1
  lwz     r18, 0(r16)
  cmp     r14, r18

end 

// ; subn
inline %8Ch

  li      r18, __n16_1
  subf    r14, r18, r14

end

// ; addn
inline %8Dh

  li      r18, __n16_1
  add     r14, r14, r18

end

// ; setfp
inline %08Eh

  li      r16, __arg16_1
  add     r15, r31, r16

end 

// ; create r
inline %08Fh

  ld      r12, 0(r3)
  sldi    r12, r12, 3
  addi    r12, r12, page_ceil
  andi.   r18, r12, page_mask

  ld      r12, toc_alloc(r2)
  mtctr   r12            
  bctrl                   

  ld      r12, 0(r3)
  sldi    r18, r12, 3

  ld      r17, toc_rdata(r2)
  addis   r17, r17, __disp32hi_1
  addi    r17, r17, __disp32lo_1
  std     r18, -elSizeOffset(r15)
  std     r17, -elVMTOffset(r15)

end

// ; copy
inline %90h

  li      r16, __n16_1
  mr      r19, r3
  mr      r18, r15

labLoop:
  cmpwi   r16,0
  beq     labEnd
  ld      r17, 0(r19)
  addi    r16, r16, -1
  stb     r17, 0(r18)
  addi    r18, r18, 1
  addi    r19, r19, 1
  b       labLoop

labEnd:

end

// ; closen
inline %91h

  addi    r31, r31, __n16_1  // ; skip unframed stack
  mr      r1, r31              // ; restore stack pointer

  ld      r31, 00h(r1)         // ; restore frame pointer
  ld      r0,  08h(r1)         // ; restore  return address

  mtlr    r0
  addi    r1, r1, 10h          // ; free stack
  
end

// ; closen 0
inline %191h

  mr      r1, r31              // ; restore stack pointer

  ld      r31, 00h(r1)         // ; restore frame pointer
  ld      r0,  08h(r1)         // ; restore  return address

  mtlr    r0
  addi    r1, r1, 10h          // ; free stack
  
end

// ; alloci
inline %92h

  addi    r1, r1,  -__arg16_1   // ; allocate stack
  li      r16, __arg16_1
  li      r17, 0
  mr      r18, r1 

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:

end

// ; alloci 0
inline %192h


end

// ; alloci 1
inline %292h

  addi    r1, r1,  -__arg16_1   // ; allocate stack
  li      r17, 0
  std     r17, 0(r1)

end

// ; alloci 2
inline %392h

  addi    r1, r1,  -__arg16_1   // ; allocate stack
  li      r17, 0
  std     r17, 8(r1)
  std     r17, 0(r1)

end

// ; alloci 3
inline %492h

  addi    r1, r1,  -__arg16_1   // ; allocate stack
  li      r17, 0
  std     r17, 0(r1)
  std     r17, 8(r1)
  std     r17, 16(r1)

end

// ; alloci 4
inline %592h

  addi    r1, r1,  -__arg16_1   // ; allocate stack
  li      r17, 0
  std     r17, 0(r1)
  std     r17, 8(r1)
  std     r17, 16(r1)
  std     r17, 24(r1)

end

// ; freei
inline %93h

  addi    r1, r1, __arg16_1     // ; free stack

end

// ; andn
inline %94h
          	
  andi.   r14, r14, __n16_1     

end

// ; readn
inline %95h

  li      r16, __n16_1
  mr      r18, r15
  mulld   r20, r16, r14
  add     r19, r3, r20

labLoop:
  cmpwi   r16,0
  beq     labEnd
  ld      r17, 0(r19)
  addi    r16, r16, -1
  stb     r17, 0(r18)
  addi    r18, r18, 1
  addi    r19, r19, 1
  b       labLoop

labEnd:

end

// ; writen
inline %96h

  li      r16, __n16_1      // ; n
  mr      r18, r15
  mulld   r20, r16, r14
  add     r19, r3, r20

labLoop:
  cmpwi   r16,0
  beq     labEnd
  ld      r17, 0(r18)
  addi    r16, r16, -1
  stb     r17, 0(r19)
  addi    r18, r18, 1
  addi    r19, r19, 1
  b       labLoop

labEnd:

end

// ; cmpn n
inline %97h

  li      r18, __n16_1

  cmp     r14, r18

end

// ; cmpn n  (n < 0)
inline %0997h

  lis     r18, __n16hi_1
  li      r19, __n16lo_1
  andi.   r19, r19, 0FFFFh
  add     r18, r18, r19
  cmp     r14, r18

end

// ; cmpn n  (n > 07FFFh)
inline %0A97h

  li      r18, __n16lo_1
  andi.   r18, r18, 0FFFFh
  addis   r18, r18, __n16hi_1

  cmp     r14, r18

end

// ; nconfdp
inline %098h

  addi    r19, r31, __arg16_1

  lfd     f17, 0(r15)
//;  friz    f17, f17
  fctidz  f18, f17
  stfd    f18, 0(r19) 

// stfiwx

end

// ; ftruncdp
inline %099h

  addi    r19, r31, __arg16_1

  lfd     f17, 0(r3)
  friz    f17, f17
  stfd    f17, 0(r19)

end

// ; dcopy
inline %9Ah

  li      r16, __n16_1
  mr      r19, r3
  mulld   r16, r16, r14
  mr      r18, r15

labLoop:
  cmpwi   r16,0
  beq     labEnd
  ld      r17, 0(r19)
  addi    r16, r16, -1
  stb     r17, 0(r18)
  addi    r18, r18, 1
  addi    r19, r19, 1
  b       labLoop

labEnd:

end

// ; orn
inline %9Bh

  ori     r14, r14, __n16_1

end

// ; muln
inline %9Ch

  li      r16, __n16_1
  mulld   r14, r14, r16

end

// ; xadddp
inline %9Dh

  addi    r16, r31, __arg16_1
  lwz     r16, 0(r16)
  add     r14, r14, r16

end 

// ; xsetfp
inline %09Eh

  li      r16, __arg16_1
  add     r15, r31, r16
  sldi    r18, r14, 3
  add     r15, r15, r18

end 

// ; frounddp
inline %09Fh

  addi    r19, r31, __arg16_1

  lfd     f17, 0(r3)
  frin    f17, f17
  stfd    f17, 0(r19)

end

// ; savedp
inline %0A0h

  stw     r14, __arg16_1(r31)  // ; save frame pointer

end

// ; storefp
inline %0A1h

  std     r15, __arg16_1(r31)  

end

// ; savesi
inline %0A2h

  std     r14, __arg16_1(r1)  

end 

// ; savesi 0
inline %1A2h

  mr      r3, r14

end 

// ; savesi 1
inline %2A2h

  mr      r4, r14

end 

// ; storesi
inline %0A3h

  std     r15, __arg16_1(r1)  

end 

// ; storesi 0
inline %1A3h

  mr      r3, r15

end 

// ; storesi 1
inline %2A3h

  mr      r4, r15

end 

// ; xflushsi i
inline %0A4h

end 

// ; xflushsi 0
inline %1A4h

  std     r3, 0(r1)  

end 

// ; xflushsi 1
inline %2A4h

  std     r4, 8(r1)

end 

// ; geti
inline %0A5h

  addi    r16, r15, __arg16_1
  ld      r15, 0(r16)

end

// ; assigni
inline %0A6h

  addi    r16, r15, __arg16_1

  // calculate write-barrier address
  ld      r19, toc_gctable(r2)

  ld      r17, gc_start(r19)
  ld      r18, gc_header(r19)

  subf    r19, r17, r15
  srdi    r19, r19, page_size_order

  std     r3, 0(r16)
  li      r20, 1
  add     r18, r18, r19
  stb     r20, 0(r18)

end

// ; xrefreshsi i
inline %0A7h

end 

// ; xrefreshsi 0
inline %1A7h

  ld     r3, 0(r1)  

end 

// ; xrefreshsi 1
inline %2A7h

  ld     r4, 8(r1)

end 

// ; peekfi
inline %0A8h

  ld       r15, __arg16_1(r31)

end 

// ; peeksi
inline %0A9h

  ld      r15, __arg16_1(r1)  

end 

// ; peeksi 0
inline %1A9h

  mr      r15, r3

end 

// ; peeksi 1
inline %2A9h

  mr      r15, r4

end 

// ; lsavedp
inline %0AAh

  std     r14, __arg16_1(r31)  // ; save frame pointer

end

// ; lsavesi
inline %0ABh

  std     r14, __arg16_1(r1)  

end 

// ; savesi 0
inline %1ABh

  mr      r3, r14

end 

// ; savesi 1
inline %2ABh

  mr      r4, r14

end 

// ; lloaddp
inline %0ACh

  addi    r16, r31, __arg16_1
  ld      r14, 0(r16)

end

// ; xfillr
inline % 0ADh

  ld      r16, 0(r3)
  sldi    r16, r16, 3

  ld      r17, toc_code(r2)
  addis   r17, r17, __xdisp32hi_1 
  addi    r17, r17, __xdisp32lo_1 

  mr      r18, r15 

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:

end

// ; xfillr
inline % 1ADh

  ld      r16, 0(r3)
  sldi    r16, r16, 3
  li      r17, 0
  mr      r18, r15 

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:

end

// ; xstorei
inline % 0AEh

  addi    r16, r15, __arg16_1
  ld      r3, 0(r16)

end

// ; xassignsp
inline % 0AFh

  li      r16, __arg16_1
  add     r15, r1, r16

end

// ; callr
inline %0B0h

  ld       r12, toc_code(r2)
  addis    r12, r12, __disp32hi_1 
  addi     r12, r12, __disp32lo_1
  mtctr    r12            // ; put code address into ctr
  bctrl                   // ; and call it

end

// ; callvi (ecx - offset to VMT entry)
inline % 0B1h

  ld       r16, -elVMTOffset(r15)     
  ld       r17, __arg16_1(r16)
  mtctr    r17            // ; put code address into ctr
  bctrl                   // ; and call it

end

// ; jumpvi (ecx - offset to VMT entry)
inline % 0B5h

  ld       r16, -elVMTOffset(r15)     
  ld       r17, __arg16_1(r16)
  mtctr    r17            // ; put code address into ctr
  bctr                    // ; and jump to it

end

// ; xredirect
inline % 0B6h //; (r15 - object, r14 - message)

  mr      r20, r14
  ld      r16, -elVMTOffset(r15)      //; edi
  xor     r17, r17, r17               //; ecx 
  ld      r7, -elVMTSizeOffset(r16)   //; esi

  lis     r18, __arg32hi_1
  addi    r18, r18, __arg32lo_1

  andi.   r14, r14, ARG_ACTION_MASK

  li      r19, ~ARG_MASK
  andi.   r19, r19, 0FFFFh
  addis   r19, r19, 0FFFFh

  and     r18, r18, r19
  or      r14, r14, r18

labSplit:
  cmpwi   r7, 0
  beq     labEnd

labStart:
  andi.   r0, r7, 1
  srdi    r7, r7, 1
  iseleq  r21, r19, r17                  //; ecx

  sldi    r22, r7, 4
  add     r22, r22, r16                  //; edx

  ld      r23, 0(r22)
  cmp     r14, r23
  beq     labFound
  addi    r22, r22, 16  
  blt     labSplit
  mr      r16, r22
  subf    r7, r21, r7
  b       labSplit
labFound:
  mr      r14, r20
  ld      r23, 8(r22) 
  mtctr   r23
  bctr

labEnd:
  mr      r14, r20
                               
end

// ; peektls
inline %0BBh

end

// ; storetls
inline %0BCh

end

// ; cmpr
inline %0C0h

  ld      r16, toc_code(r2)
  addis   r16, r16, __xdisp32hi_1 
  addi    r16, r16, __xdisp32lo_1 
  cmp     r15, r16

end 

// ; cmpr 0
inline %1C0h

  li      r16, 0
  cmp     r15, r16

end 

// ; cmpr 1
inline %9C0h

  li      r16, 0
  addi    r16, r16, -1
  cmp     r15, r16

end 

// ; fcmpn 8
inline %0C1h

  lfd      f17, 0(r3)
  lfd      f18, 0(r15)

  fcmpu    f17, f18

end

// ; icmpn 4
inline %0C2h

  lwz      r17, 0(r3)
  lwz      r18, 0(r15)

  cmp      r17, r18

end

// ; icmpn 1
inline %1C2h

  lbz     r17, 0(r3)
  lbz     r18, 0(r15)

  cmp      r17, r18

end

// ; icmpn 2
inline %2C2h

  lhz     r17, 0(r3)
  lhz     r18, 0(r15)

  cmp      r17, r18

end

// ; icmpdpn 8
inline %4C2h

  ld      r17, 0(r3)
  ld      r18, 0(r15)

  cmp     r17, r18

end

// ; tstflg
inline %0C3h

  ld      r16, -elVMTOffset(r15)
  ld      r16, -elVMTFlagOffset(r16)

  li      r17, __n16lo_1
  addis   r17, r17, __n16hi_1

  and.    r17, r17, r16

end

// ; tstn
inline %0C4h

  li      r17, __n16lo_1
  addis   r17, r17, __n16hi_1

  and.    r17, r14, r17

end

// ; tstm
inline % 0C5h

  lis     r20, __arg32hi_1
  addi    r20, r20, __arg32lo_1

  ld      r16, -elVMTOffset(r15)      //; edi
  xor     r17, r17, r17               //; ecx 
  ld      r7, -elVMTSizeOffset(r16)   //; esi
  li      r19, 1

labSplit:
  cmpwi   r7, 0
  beq     labEnd

labStart:
  andi.   r0, r7, 1
  srdi    r7, r7, 1
  iseleq  r21, r19, r17                  //; ecx

  sldi    r22, r7, 4
  add     r22, r22, r16                  //; edx

  ld      r23, 0(r22)
  cmp     r20, r23
  beq     labFound
  addi    r22, r22, 16  
  blt     labSplit
  mr      r16, r22
  subf    r7, r21, r7
  b       labSplit
labFound:
  li      r7, 1 

labEnd:
  cmpwi   r7, 1
                               
end

// ; cmpsi
inline %0C6h

  ld      r16, __arg16_1(r1)  
  cmp     r14, r16

end 

// ; cmpsi 0
inline %1C6h

  cmp     r14, r3

end 

// ; cmpsi 1
inline %2C6h

  cmp     r14, r4

end 

// ; cmpfi
inline %0C8h

  ld      r16, __arg16_1(r31)
  cmp     r15, r16

end 

// ; cmpsi
inline %0C9h

  ld      r16, __arg16_1(r1)  
  cmp     r15, r16

end 

// ; cmpsi 0
inline %1C9h

  cmp     r15, r3

end 

// ; cmpsi 1
inline %2C9h

  cmp     r15, r4

end 

// ; extclosen
inline %0CAh

  addi  r31, r31, __n16_1  // ; skip unframed stack
  mr    r1, r31            // ; restore stack pointer
  addi  r1, r1, 32

  ld   r31, 168(r1)
  ld   r30, 160(r1)
  ld   r29, 152(r1)
  ld   r28, 144(r1)
  ld   r27, 136(r1)
  ld   r0,  128(r1)

  mtlr r0

  addi  r1, r1, 176
  
end

// ; extclosen 0
inline %1CAh

  mr    r1, r31              // ; restore stack pointer
  addi  r1, r1, 32

  ld   r31, 168(r1)
  ld   r30, 160(r1)
  ld   r29, 152(r1)
  ld   r28, 144(r1)
  ld   r27, 136(r1)
  ld   r0,  128(r1)

  mtlr r0

  addi  r1, r1, 176
  
end

// ; lloadsi
inline %0CBh

  ld      r14, __arg16_1(r1)  

end 

// ; lloadsi 0
inline %1CBh

  mr      r14, r3

end 

// ; lloadsi 1
inline %2CBh

  mr      r14, r4

end 

// ; loadsi
inline %0CCh

  lwz     r14, __arg16_1(r1)  

end 

// ; loadsi 0
inline %1CCh

  mr      r14, r3

end 

// ; loadsi 1
inline %2CCh

  mr      r14, r4

end 

// ; xloadargsi
inline %0CDh

  ld      r14, __arg16_1(r1)  

end 

// ; xloadargsi 0
inline %1CDh

  mr      r14, r3

end 

// ; xloadargsi 1
inline %2CDh

  mr      r14, r4

end 

// ; xcreater r
inline %0CEh

  ld      r12, 0(r3)
  sldi    r12, r12, 3
  addi    r12, r12, page_ceil
  andi.   r18, r12, page_mask

  ld      r12, toc_allocperm(r2)
  mtctr   r12            
  bctrl                   

  ld      r12, 0(r3)
  sldi    r18, r12, 3

  ld      r17, toc_rdata(r2)
  addis   r17, r17, __disp32hi_1
  addi    r17, r17, __disp32lo_1
  std     r18, -elSizeOffset(r15)
  std     r17, -elVMTOffset(r15)

end

// ; system
inline %0CFh

end

// ; system startup
inline %4CFh

  lis     r2, rdata32_hi : %CORE_TOC
  addi    r2, r2, rdata32_lo : %CORE_TOC

  ld      r16, toc_data(r2)
  addis   r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi    r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT
  std     r1, tt_stack_root(r16)

  ld      r12, toc_prepare(r2)
  mtctr   r12            
  bctrl                   

  lis     r2, rdata32_hi : %CORE_TOC
  addi    r2, r2, rdata32_lo : %CORE_TOC

end

// ; faddndp
inline %0D0h

  addi    r19, r31, __arg16_1

  lfd     f17, 0(r3)
  lfd     f18, 0(r19)

  fadd    f17, f17, f18  

  stfd    f17, 0(r19)

end

// ; fsubndp
inline %0D1h

  addi    r19, r31, __arg16_1

  lfd     f17, 0(r3)
  lfd     f18, 0(r19)

  fsub    f17, f18, f17

  stfd    f17, 0(r19)

end

// ; fmulndp
inline %0D2h

  addi    r19, r31, __arg16_1

  lfd     f17, 0(r3)
  lfd     f18, 0(r19)

  fmul    f17, f17, f18  

  stfd    f17, 0(r19)

end

// ; fdivndp
inline %0D3h

  addi    r19, r31, __arg16_1

  lfd     f17, 0(r3)
  lfd     f18, 0(r19)

  fdiv    f18, f18, f17  

  stfd    f18, 0(r19)

end

// ; udivndp
inline %0D4h

  addi    r19, r31, __arg16_1

  lwz     r17, 0(r3)
  lwz     r18, 0(r19)

  divwu   r18, r18, r17  

  stw     r18, 0(r19)

end

// ; xsavedispn
inline %0D5h

  addi    r19, r15, __arg16_1
  li      r17, __n16_2
  stw     r17, 0(r19)

end

// ; xsavedispn
inline %1D5h

  addi    r19, r15, __arg16_1

  li      r17, __n16lo_1
  andi.   r17, r17, 0FFFFh
  addis   r17, r17, __n16hi_1
  stw     r17, 0(r19)

end

// ; xlabeldpr
inline %0D6h

  addi    r19, r31, __arg16_1

  ld      r12, toc_code(r2)
  addis   r12, r12, __disp32hi_2
  addi    r12, r12, __disp32lo_2

  std     r12, 0(r19)

end

// ; selgrrr
inline %0D7h

  ld      r16, toc_code(r2)
  addis   r17, r16, __xdisp32hi_1 
  addis   r18, r16, __xdisp32hi_2 
  addi    r17, r17, __xdisp32lo_1 
  addi    r18, r18, __xdisp32lo_2 

  iselgt  r15, r17, r18

end 

// ; ianddpn
inline %0D8h

  addi    r19, r31, __arg16_1

  lwz      r17, 0(r3)
  lwz      r18, 0(r19)

  and     r17, r17, r18  

  stw     r17, 0(r19)

end

// ; ianddpn
inline %1D8h

  addi    r19, r31, __arg16_1

  lbz     r17, 0(r3)
  lbz     r18, 0(r19)

  and     r17, r17, r18  

  stb     r17, 0(r19)

end

// ; ianddpn
inline %2D8h

  addi    r19, r31, __arg16_1

  lhz     r17, 0(r3)
  lhz     r18, 0(r19)

  and     r17, r17, r18  

  sth     r17, 0(r19)

end

// ; ianddpn
inline %4D8h

  addi    r19, r31, __arg16_1

  ld      r17, 0(r3)
  ld      r18, 0(r19)

  and     r17, r17, r18  

  std     r17, 0(r19)

end

// ; iordpn
inline %0D9h

  addi    r19, r31, __arg16_1

  lwz     r17, 0(r3)
  lwz     r18, 0(r19)

  or      r17, r17, r18  

  stw     r17, 0(r19)

end

// ; iordpn
inline %1D9h

  addi    r19, r31, __arg16_1

  lbz     r17, 0(r3)
  lbz     r18, 0(r19)

  or      r17, r17, r18  

  stb     r17, 0(r19)

end

// ; iordpn
inline %2D9h

  addi    r19, r31, __arg16_1

  lhz     r17, 0(r3)
  lhz     r18, 0(r19)

  or      r17, r17, r18  

  sth     r17, 0(r19)

end

// ; iordpn
inline %4D9h

  addi    r19, r31, __arg16_1

  ld      r17, 0(r3)
  ld      r18, 0(r19)

  or      r17, r17, r18  

  std     r17, 0(r19)

end

// ; ixordpn
inline %0DAh

  addi    r19, r31, __arg16_1

  lwz     r17, 0(r3)
  lwz     r18, 0(r19)

  xor     r17, r17, r18  

  stw     r17, 0(r19)

end

// ; ixordpn
inline %1DAh

  addi    r19, r31, __arg16_1

  lbz     r17, 0(r3)
  lbz     r18, 0(r19)

  xor     r17, r17, r18  

  stb     r17, 0(r19)

end

// ; ixordpn
inline %2DAh

  addi    r19, r31, __arg16_1

  lhz     r17, 0(r3)
  lhz     r18, 0(r19)

  xor     r17, r17, r18  

  sth     r17, 0(r19)

end

// ; ixordpn
inline %4DAh

  addi    r19, r31, __arg16_1

  ld      r17, 0(r3)
  ld      r18, 0(r19)

  xor     r17, r17, r18  

  std     r17, 0(r19)

end

// ; inotdpn
inline %0DBh

  addi    r19, r31, __arg16_1

  lwz     r17, 0(r3)

  nand    r18, r17, r17

  stw     r18, 0(r19)

end

// ; inotdpn
inline %1DBh

  addi    r19, r31, __arg16_1

  lwz     r18, 0(r3)

  nand    r17, r18, r18

  stb     r17, 0(r19)

end

// ; inotdpn
inline %2DBh

  addi    r19, r31, __arg16_1

  lwz     r18, 0(r3)

  nand    r17, r18, r18

  sth     r17, 0(r19)

end

// ; inotdpn
inline %4DBh

  addi    r19, r31, __arg16_1

  lwz     r18, 0(r3)

  nand    r17, r18, r18

  std     r17, 0(r19)

end

// ; ishldpn
inline %0DCh

  addi    r19, r31, __arg16_1

  lwz     r17, 0(r3)
  lwz     r18, 0(r19)

  sld     r18, r18, r17

  stw     r18, 0(r19)

end

// ; ishldpn 1
inline %1DCh

  addi    r19, r31, __arg16_1

  lwz     r17, 0(r3)
  lwz     r18, 0(r19)

  sld     r18, r18, r17

  stb     r17, 0(r19)

end

// ; ishldpn
inline %2DCh

  addi    r19, r31, __arg16_1

  lwz     r17, 0(r3)
  lwz     r18, 0(r19)

  sld     r18, r18, r17

  sth     r17, 0(r19)

end

// ; ishldpn
inline %4DCh

  addi    r19, r31, __arg16_1

  lwz     r17, 0(r3)
  ld      r18, 0(r19)

  sld     r18, r18, r17

  std     r17, 0(r19)

end

// ; ishrdpn
inline %0DDh

  addi    r19, r31, __arg16_1

  lwz     r17, 0(r3)
  lwz     r18, 0(r19)

  srd     r18, r18, r17

  stw     r18, 0(r19)

end

// ; ishrdpn 1
inline %1DDh

  addi    r19, r31, __arg16_1

  lwz     r17, 0(r3)
  lwz     r18, 0(r19)

  srd     r18, r18, r17

  stb     r17, 0(r19)

end

// ; ishrdpn
inline %2DDh

  addi    r19, r31, __arg16_1

  lwz     r17, 0(r3)
  lwz     r18, 0(r19)

  srd     r18, r18, r17

  sth     r17, 0(r19)

end

// ; ishrdpn
inline %4DDh

  addi    r19, r31, __arg16_1

  lwz     r17, 0(r3)
  ld      r18, 0(r19)

  srd     r18, r18, r17

  std     r17, 0(r19)

end

// ; selultrr
inline %0DFh

  lwz      r17, 0(r3)
  lwz      r18, 0(r15)

  cmplw    r17, r18

  ld      r16, toc_code(r2)
  addis   r17, r16, __xdisp32hi_1 
  addis   r18, r16, __xdisp32hi_2 
  addi    r17, r17, __xdisp32lo_1 
  addi    r18, r18, __xdisp32lo_2 

  isellt  r15, r17, r18

end 

// ; copydpn
inline %0E0h

  li      r16, __n16_2
  addi    r18, r31, __arg16_1
  mr      r19, r3

labLoop:
  cmpwi   r16,0
  beq     labEnd
  ld      r17, 0(r19)
  addi    r16, r16, -1
  stb     r17, 0(r18)
  addi    r18, r18, 1
  addi    r19, r19, 1
  b       labLoop

labEnd:

end

// ; copydpn dpn, 1
inline %1E0h

  addi    r18, r31, __arg16_1
  ld      r17, 0(r3)
  stb     r17, 0(r18)

end

// ; copydpn dpn, 2
inline %2E0h

  addi    r18, r31, __arg16_1
  ld      r17, 0(r3)
  sth     r17, 0(r18)

end

// ; copydpn dpn, 4
inline %3E0h

  addi    r18, r31, __arg16_1
  ld      r17, 0(r3)
  stw     r17, 0(r18)

end

// ; copydpn dpn, 8
inline %4E0h

  addi    r18, r31, __arg16_1
  ld      r17, 0(r3)
  std     r17, 0(r18)

end

// ; iaddndp
inline %0E1h

  addi    r19, r31, __arg16_1

  lwz      r17, 0(r3)
  lwz      r18, 0(r19)

  add     r17, r17, r18  

  stw     r17, 0(r19)

end

// ; iaddndp
inline %1E1h

  addi    r19, r31, __arg16_1

  lbz     r17, 0(r3)
  lbz     r18, 0(r19)

  add     r17, r17, r18  

  stb     r17, 0(r19)

end

// ; iaddndp
inline %2E1h

  addi    r19, r31, __arg16_1

  lhz     r17, 0(r3)
  lhz     r18, 0(r19)

  add     r17, r17, r18  

  sth     r17, 0(r19)

end

// ; iaddndp
inline %4E1h

  addi    r19, r31, __arg16_1

  ld      r17, 0(r3)
  ld      r18, 0(r19)

  add     r17, r17, r18  

  std     r17, 0(r19)

end

// ; isubndp
inline %0E2h

  addi    r19, r31, __arg16_1

  lwz     r17, 0(r3)
  lwz     r18, 0(r19)

  subf    r17, r17, r18  

  stw     r17, 0(r19)

end

// ; isubndp
inline %1E2h

  addi    r19, r31, __arg16_1

  lbz     r17, 0(r3)
  lbz     r18, 0(r19)

  subf    r17, r17, r18  

  stb     r17, 0(r19)

end

// ; isubndp
inline %2E2h

  addi    r19, r31, __arg16_1

  lhz     r17, 0(r3)
  lhz     r18, 0(r19)

  subf    r17, r17, r18  

  sth     r17, 0(r19)

end

// ; isubndp
inline %4E2h

  addi    r19, r31, __arg16_1

  ld      r17, 0(r3)
  ld      r18, 0(r19)

  subf    r17, r17, r18  

  std     r17, 0(r19)

end

// ; imulndp
inline %0E3h

  addi    r19, r31, __arg16_1

  lwz     r17, 0(r3)
  lwz     r18, 0(r19)

  mullw   r17, r17, r18  

  stw     r17, 0(r19)

end

// ; imulndp
inline %1E3h

  addi    r19, r31, __arg16_1

  lbz     r17, 0(r3)
  lbz     r18, 0(r19)

  mullw   r17, r17, r18  

  stb     r17, 0(r19)

end

// ; imulndp
inline %2E3h

  addi    r19, r31, __arg16_1

  lhz     r17, 0(r3)
  lhz     r18, 0(r19)

  mullw   r17, r17, r18  

  sth     r17, 0(r19)

end

// ; imulndp
inline %4E3h

  addi    r19, r31, __arg16_1

  ld      r17, 0(r3)
  ld      r18, 0(r19)

  mulld   r17, r17, r18  

  std     r17, 0(r19)

end

// ; idivndp
inline %0E4h

  addi    r19, r31, __arg16_1

  lwz     r17, 0(r3)
  lwz     r18, 0(r19)

  divw    r18, r18, r17  

  stw     r18, 0(r19)

end

// ; idivndp
inline %1E4h

  addi    r19, r31, __arg16_1

  lbz     r17, 0(r3)
  lbz     r18, 0(r19)

  divw    r18, r18, r17  

  stb     r18, 0(r19)

end

// ; idivndp
inline %2E4h

  addi    r19, r31, __arg16_1

  lhz     r17, 0(r3)
  lhz     r18, 0(r19)

  divw    r18, r18, r17  

  sth     r18, 0(r19)

end

// ; idivndp
inline %4E4h

  addi    r19, r31, __arg16_1

  ld      r17, 0(r3)
  ld      r18, 0(r19)

  divd    r18, r18, r17  

  std     r18, 0(r19)

end

// ; nsavedpn
inline %0E5h

  addi    r19, r31, __arg16_1
  li      r17, __n16_2
  stw     r17, 0(r19)

end

// ; xhookdpr
inline %0E6h

  addi    r19, r31, __arg16_1

  ld      r14, toc_data(r2)
  addis   r14, r14, data_disp32hi : %CORE_SINGLE_CONTENT
  addi    r14, r14, data_disp32lo : %CORE_SINGLE_CONTENT

  ld      r15, et_current(r14)

  ld       r12, toc_code(r2)
  addis    r12, r12, __disp32hi_2
  addi     r12, r12, __disp32lo_2

  std     r15, es_prev_struct(r19)
  std     r12, es_catch_addr(r19)
  std     r1, es_catch_level(r19)
  std     r31, es_catch_frame(r19)

  std     r19, et_current(r14)

end

// ; xnewnr n, r
inline %0E7h

  addi    r15, r15, elObjectOffset

  li      r18, __n16lo_1
  addis   r18, r18, __n16hi_1

  ld      r17, toc_rdata(r2)
  addis   r17, r17, __disp32hi_2 
  addi    r17, r17, __disp32lo_2
  std     r18, -elSizeOffset(r15)
  std     r17, -elVMTOffset(r15)

end

// ; nadddpn
inline %0E8h

  addi    r19, r31, __arg16_1
  li      r17, __n16_2
  lwz     r18, 0(r19)
  add     r18, r18, r17
  stw     r18, 0(r19)

end

// ; dcopydpn
inline %0E9h

  li      r16, __n16_2
  addi    r18, r31, __arg16_1
  mulld   r16, r16, r14
  mr      r19, r3

labLoop:
  cmpwi   r16,0
  beq     labEnd
  ld      r17, 0(r19)
  addi    r16, r16, -1
  stb     r17, 0(r18)
  addi    r18, r18, 1
  addi    r19, r19, 1
  b       labLoop

labEnd:

end

// ; xwriteon
inline %0EAh

  li      r16, __n16_2
  mr      r18, r3
  addi    r19, r15, __n16_1

labLoop:
  cmpwi   r16,0
  beq     labEnd
  ld      r17, 0(r19)
  addi    r16, r16, -1
  stb     r17, 0(r18)
  addi    r18, r18, 1
  addi    r19, r19, 1
  b       labLoop

labEnd:

end

// ; xcopyon
inline %0EBh

  li      r16, __n16_2
  mr      r19, r3
  addi    r18, r15, __n16_1

labLoop:
  cmpwi   r16,0
  beq     labEnd
  ld      r17, 0(r19)
  addi    r16, r16, -1
  stb     r17, 0(r18)
  addi    r18, r18, 1
  addi    r19, r19, 1
  b       labLoop

labEnd:

end

// ; vjumpmr
inline % 0ECh

  ld       r16, -elVMTOffset(r15)     
  ld       r17, __arg16_1(r16)
  mtctr    r17            // ; put code address into ctr
  bctr                    // ; and jump to it

end

// ; vjumpmr
inline % 6ECh

  ld       r16, -elVMTOffset(r15)     
  ld       r17, -elVMTSizeOffset(r16)     
  sldi     r17, r17, 4
  add      r16, r16, r17
  ld       r17, __arg16_1(r16)
  mtctr    r17            // ; put code address into ctr
  bctr                    // ; and jump to it

end

// ; jumpmr
inline %0EDh

  ld       r12, toc_code(r2)
  addis    r12, r12, __disp32hi_2 
  addi     r12, r12, __disp32lo_2
  mtctr    r12            // ; put code address into ctr
  bctr                    // ; and jump to it

end

// ; seleqrr
inline %0EEh

  ld      r16, toc_code(r2)
  addis   r17, r16, __xdisp32hi_1 
  addis   r18, r16, __xdisp32hi_2 
  addi    r17, r17, __xdisp32lo_1 
  addi    r18, r18, __xdisp32lo_2 

  iseleq  r15, r17, r18

end 

// ; selltrr
inline %0EFh

  ld      r16, toc_code(r2)
  addis   r17, r16, __xdisp32hi_1 
  addis   r18, r16, __xdisp32hi_2 
  addi    r17, r17, __xdisp32lo_1 
  addi    r18, r18, __xdisp32lo_2 

  isellt  r15, r17, r18

end 

// ; openin
inline %0F0h

  mflr    r0

  std     r31, -10h(r1)  // ; save frame pointer
  std     r0,  -08h(r1)  // ; save return address
  addi    r1, r1, -16    // ; allocate raw stack

  mr      r31, r1        // ; set frame pointer

  addi    r1, r1, -__n16_2 // ; allocate raw stack

  li      r16, 0
  std     r31, -08h(r1)  // ; save frame pointer
  std     r16, -10h(r1)  // ; save dummy
  addi    r1, r1, -10h   // ; allocate stack
  mr      r31, r1        // ; set frame pointer

  addi    r1, r1,  -__arg16_1   // ; allocate stack
  li      r16, __arg16_1
  li      r17, 0
  mr      r18, r1 

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:

end 

// ; openin 0, n
inline %1F0h

  mflr    r0

  std     r31, -10h(r1)  // ; save frame pointer
  std     r0,  -08h(r1)  // ; save return address
  addi    r1, r1, -16    // ; allocate raw stack

  mr      r31, r1        // ; set frame pointer

  addi    r1, r1, -__n16_2 // ; allocate raw stack

  li      r16, 0
  std     r31, -08h(r1)  // ; save frame pointer
  std     r16, -10h(r1)  // ; save dummy
  addi    r1, r1, -10h   // ; allocate stack
  mr      r31, r1        // ; set frame pointer

end 

// ; openin 1, n
inline %2F0h

  mflr    r0

  std     r31, -10h(r1)  // ; save frame pointer
  std     r0,  -08h(r1)  // ; save return address
  addi    r1, r1, -16    // ; allocate raw stack

  mr      r31, r1        // ; set frame pointer

  addi    r1, r1, -__n16_2 // ; allocate raw stack

  li      r16, 0
  std     r31, -08h(r1)  // ; save frame pointer
  std     r16, -10h(r1)  // ; save dummy
  addi    r1, r1, -10h   // ; allocate stack
  mr      r31, r1        // ; set frame pointer

  std     r16, -08h(r1)  // ; save frame pointer
  std     r16, -10h(r1)  // ; save frame pointer
  addi    r1, r1, -10h   // ; allocate stack

end 

// ; openin 2, n
inline %3F0h

  mflr    r0

  std     r31, -10h(r1)  // ; save frame pointer
  std     r0,  -08h(r1)  // ; save return address
  addi    r1, r1, -16    // ; allocate raw stack

  mr      r31, r1        // ; set frame pointer

  addi    r1, r1, -__n16_2 // ; allocate raw stack

  li      r16, 0
  std     r31, -08h(r1)  // ; save frame pointer
  std     r16, -10h(r1)  // ; save dummy
  addi    r1, r1, -10h   // ; allocate stack
  mr      r31, r1        // ; set frame pointer

  std     r16, -08h(r1)  // ; save frame pointer
  std     r16, -10h(r1)  // ; save frame pointer
  addi    r1, r1, -10h   // ; allocate stack

end 

// ; openin 3, n
inline %4F0h

  mflr    r0

  std     r31, -10h(r1)  // ; save frame pointer
  std     r0,  -08h(r1)  // ; save return address
  addi    r1, r1, -16    // ; allocate raw stack

  mr      r31, r1        // ; set frame pointer

  addi    r1, r1, -__n16_2 // ; allocate raw stack

  li      r16, 0
  std     r31, -08h(r1)  // ; save frame pointer
  std     r16, -10h(r1)  // ; save dummy
  addi    r1, r1, -10h   // ; allocate stack
  mr      r31, r1        // ; set frame pointer

  std     r16, -08h(r1)  // ; save frame pointer
  std     r16, -10h(r1)  // ; save frame pointer
  addi    r1, r1, -10h   // ; allocate stack

  std     r16, -08h(r1)  // ; save frame pointer
  std     r16, -10h(r1)  // ; save frame pointer
  addi    r1, r1, -10h   // ; allocate stack

end 

// ; openin 4, n
inline %5F0h

  mflr    r0

  std     r31, -10h(r1)  // ; save frame pointer
  std     r0,  -08h(r1)  // ; save return address
  addi    r1, r1, -16    // ; allocate raw stack

  mr      r31, r1        // ; set frame pointer

  addi    r1, r1, -__n16_2 // ; allocate raw stack

  li      r16, 0
  std     r31, -08h(r1)  // ; save frame pointer
  std     r16, -10h(r1)  // ; save dummy
  addi    r1, r1, -10h   // ; allocate stack
  mr      r31, r1        // ; set frame pointer

  std     r16, -08h(r1)  // ; save frame pointer
  std     r16, -10h(r1)  // ; save frame pointer
  addi    r1, r1, -10h   // ; allocate stack

  std     r16, -08h(r1)  // ; save frame pointer
  std     r16, -10h(r1)  // ; save frame pointer
  addi    r1, r1, -10h   // ; allocate stack

end 

// ; openin i, 0
inline %6F0h

  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  std     r0,  -08h(r1)  // ; save return address

  addi    r1, r1, -16    // ; allocate raw stack
  mr      r31, r1        // ; set frame pointer

  addi    r1, r1,  -__arg16_1   // ; allocate stack
  li      r16, __arg16_1
  li      r17, 0
  mr      r18, r1 

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:

end 

// ; openin 0, 0
inline %7F0h

  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  std     r0,  -08h(r1)  // ; save return address

  addi    r1, r1, -10h   // ; allocate stack
  mr      r31, r1        // ; set frame pointer

end 

// ; openin 1, 0
inline %8F0h

  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  li      r16, 0
  std     r0,  -08h(r1)  // ; save return address
  addi    r31, r1, -10h  // ; set frame pointer
  std     r16, -18h(r1)  // ; save return address
  std     r16, -20h(r1)  // ; save return address

  addi    r1, r1, -20h   // ; allocate stack

end 

// ; openin 2, 0
inline %9F0h

  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  li      r16, 0
  std     r0,  -08h(r1)  // ; save return address
  addi    r31, r1, -10h  // ; set frame pointer
  std     r16, -18h(r1)  // ; save return address
  std     r16, -20h(r1)  // ; save return address

  addi    r1, r1, -20h   // ; allocate stack

end 

// ; openin 3, 0
inline %0AF0h

  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  li      r16, 0
  std     r0,  -08h(r1)  // ; save return address
  addi    r31, r1, -10h  // ; set frame pointer
  std     r16, -18h(r1)  // ; save return address
  std     r16, -20h(r1)  // ; save return address
  std     r16, -28h(r1)  // ; save return address
  std     r16, -30h(r1)  // ; save return address

  addi    r1, r1, -30h   // ; allocate stack
end 

// ; openin 4, 0
inline %0BF0h

  mflr    r0
  std     r31, -10h(r1)  // ; save frame pointer
  li      r16, 0
  std     r0,  -08h(r1)  // ; save return address
  addi    r31, r1, -10h  // ; set frame pointer
  std     r16, -18h(r1)  // ; save return address
  std     r16, -20h(r1)  // ; save return address
  std     r16, -28h(r1)  // ; save return address
  std     r16, -30h(r1)  // ; save return address

  addi    r1, r1, -30h   // ; allocate stack

end 

// ; xstoresir
inline %0F1h

  ld      r16, toc_code(r2)
  addis   r16, r16, __xdisp32hi_2
  addi    r16, r16, __xdisp32lo_2

  std   r16, __arg16_1(r1)

end

// ; xstoresir :0, ...
inline %1F1h

  ld      r16, toc_code(r2)
  addis   r16, r16, __xdisp32hi_2
  addi    r16, r16, __xdisp32lo_2

  mr      r3, r16

end

// ; xstoresir :1, ...
inline %2F1h

  ld      r16, toc_code(r2)
  addis   r16, r16, __xdisp32hi_2
  addi    r16, r16, __xdisp32lo_2

  mr      r4, r16

end

// ; xstoresir :n, 0
inline %5F1h

  li      r16, 0

  std   r16, __arg16_1(r1)

end

// ; xstoresir :0, 0
inline %6F1h

  li    r3, 0

end

// ; xstoresir :1, 0
inline %7F1h

  li    r4, 0

end

// ; xstoresir :n, -1
inline %8F1h

  li      r16, 0
  addi    r16, r16, -1

  std   r16, __arg16_1(r1)

end

// ; xstoresir :0, -1
inline %9F1h

  li    r3, 0
  addi  r3, r3, -1

end

// ; xstoresir :1, -1
inline %0AF1h

  li    r4, 0
  addi  r4, r4, -1

end

// ; extopenin
inline %0F2h

  // ; loading TOC pointer
  lis     r2, rdata32_hi : %CORE_TOC
  addi    r2, r2, rdata32_lo : %CORE_TOC

  stdu  r1, -176(r1)

  mflr    r0

  std   r31, 168(r1)
  std   r30, 160(r1)
  std   r29, 152(r1)
  std   r28, 144(r1)
  std   r27, 136(r1)
  std   r0,  128(r1)

  // ; recover previous frame
  ld    r16, toc_data(r2)
  addis r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi  r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT
  ld    r10, tt_stack_frame(r16)

  li    r16, 0
  std   r10, -10h(r1)     // ; save the previous frame pointer
  std   r16, -08h(r1)     // ; save zero

  addi  r1, r1, -16       // ; allocate raw stack
  mr    r31, r1           // ; set frame pointer

  addi    r1, r1, -__n16_2 // ; allocate raw stack

  li      r16, 0
  std     r31, -08h(r1)  // ; save frame pointer
  std     r16, -10h(r1)  // ; save dummy
  addi    r1, r1, -10h   // ; allocate stack
  mr      r31, r1        // ; set frame pointer

  addi    r1, r1,  -__arg16_1   // ; allocate stack
  li      r16, __arg16_1
  li      r17, 0
  mr      r18, r1 

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:
  // ; init r3, r4 with zero (and in all variants)

end 

// ; extopenin 0, n
inline %1F2h

  // ; loading TOC pointer
  lis     r2, rdata32_hi : %CORE_TOC
  addi    r2, r2, rdata32_lo : %CORE_TOC

  stdu  r1, -176(r1)

  mflr    r0

  std   r31, 168(r1)
  std   r30, 160(r1)
  std   r29, 152(r1)
  std   r28, 144(r1)
  std   r27, 136(r1)
  std   r0,  128(r1)

  // ; recover previous frame
  ld    r16, toc_data(r2)
  addis r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi  r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT
  ld    r10, tt_stack_frame(r16)

  li    r16, 0
  std   r10, -10h(r1)     // ; save the previous frame pointer
  std   r16, -08h(r1)     // ; save zero

  addi  r1, r1, -16       // ; allocate raw stack
  mr    r31, r1           // ; set frame pointer

  addi    r1, r1, -__n16_2 // ; allocate raw stack

  li      r16, 0
  std     r31, -08h(r1)  // ; save frame pointer
  std     r16, -10h(r1)  // ; save dummy
  addi    r1, r1, -10h   // ; allocate stack
  mr      r31, r1        // ; set frame pointer

  addi    r1, r1, -__n16_2 // ; allocate raw stack

  li      r16, 0
  std     r31, -10h(r1)  // ; save frame pointer
  std     r0,  -08h(r16) // ; save dummy

end 

// ; extopenin i, 0
inline %06F2h

  // ; loading TOC pointer
  lis     r2, rdata32_hi : %CORE_TOC
  addi    r2, r2, rdata32_lo : %CORE_TOC

  stdu  r1, -176(r1)

  mflr    r0

  std   r31, 168(r1)
  std   r30, 160(r1)
  std   r29, 152(r1)
  std   r28, 144(r1)
  std   r27, 136(r1)
  std   r0,  128(r1)

  // ; recover previous frame
  ld    r16, toc_data(r2)
  addis r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi  r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT
  ld    r10, tt_stack_frame(r16)

  li    r16, 0
  std   r10, -10h(r1)     // ; save the previous frame pointer
  std   r16, -08h(r1)     // ; save zero

  addi  r1, r1, -16       // ; allocate raw stack
  mr    r31, r1           // ; set frame pointer

  addi    r1, r1,  -__arg16_1   // ; allocate stack
  li      r16, __arg16_1
  li      r17, 0
  mr      r18, r1 

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:

end 

// ; extopenin 0, 0
inline %7F2h

  // ; loading TOC pointer
  lis     r2, rdata32_hi : %CORE_TOC
  addi    r2, r2, rdata32_lo : %CORE_TOC

  stdu  r1, -176(r1)

  mflr    r0

  std   r31, 168(r1)
  std   r30, 160(r1)
  std   r29, 152(r1)
  std   r28, 144(r1)
  std   r27, 136(r1)
  std   r0,  128(r1)

  // ; recover previous frame
  ld    r16, toc_data(r2)
  addis r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi  r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT
  ld    r10, tt_stack_frame(r16)

  li    r16, 0
  std   r10, -10h(r1)     // ; save the previous frame pointer
  std   r16, -08h(r1)     // ; save zero

  addi  r1, r1, -16       // ; allocate raw stack
  mr    r31, r1           // ; set frame pointer

  addi  r1, r1, -10h      // ; allocate stack

  std     r31, -10h(r1)   // ; save frame pointer
  mr      r31, r1         // ; set frame pointer

end 

// ; extopenin 1, 0
inline %8F2h

  // ; loading TOC pointer
  lis     r2, rdata32_hi : %CORE_TOC
  addi    r2, r2, rdata32_lo : %CORE_TOC

  stdu  r1, -176(r1)

  mflr    r0

  std   r31, 168(r1)
  std   r30, 160(r1)
  std   r29, 152(r1)
  std   r28, 144(r1)
  std   r27, 136(r1)
  std   r0,  128(r1)

  // ; recover previous frame
  ld    r16, toc_data(r2)
  addis r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi  r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT
  ld    r10, tt_stack_frame(r16)

  li    r16, 0
  std   r10, -10h(r1)     // ; save the previous frame pointer
  std   r16, -08h(r1)     // ; save zero

  addi  r1, r1, -16       // ; allocate raw stack
  mr    r31, r1           // ; set frame pointer

  addi    r1, r1, -20h    // ; allocate stack

  li      r16, 0
  std     r31, -10h(r1)   // ; save frame pointer
  addi    r31, r1, -10h   // ; set frame pointer
  std     r16, -18h(r1)  
  std     r16, -20h(r1)  

end 

// ; extopenin 2, 0
inline %9F2h

  // ; loading TOC pointer
  lis     r2, rdata32_hi : %CORE_TOC
  addi    r2, r2, rdata32_lo : %CORE_TOC

  stdu  r1, -176(r1)

  mflr    r0

  std   r31, 168(r1)
  std   r30, 160(r1)
  std   r29, 152(r1)
  std   r28, 144(r1)
  std   r27, 136(r1)
  std   r0,  128(r1)

  // ; recover previous frame
  ld    r16, toc_data(r2)
  addis r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi  r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT
  ld    r10, tt_stack_frame(r16)

  li    r16, 0
  std   r10, -10h(r1)     // ; save the previous frame pointer
  std   r16, -08h(r1)     // ; save zero

  addi  r1, r1, -16       // ; allocate raw stack
  mr    r31, r1           // ; set frame pointer

  addi    r1, r1, -20h    // ; allocate stack

  li      r16, 0
  std     r31, 10h(r1)    // ; save frame pointer
  addi    r31, r1, 10h    // ; set frame pointer
  std     r16, 8(r1)  
  std     r16, 0(r1)  

end 

// ; extopenin 3, 0
inline %0AF2h

  // ; loading TOC pointer
  lis     r2, rdata32_hi : %CORE_TOC
  addi    r2, r2, rdata32_lo : %CORE_TOC

  stdu  r1, -176(r1)

  mflr    r0

  std   r31, 168(r1)
  std   r30, 160(r1)
  std   r29, 152(r1)
  std   r28, 144(r1)
  std   r27, 136(r1)
  std   r0,  128(r1)

  // ; recover previous frame
  ld    r16, toc_data(r2)
  addis r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi  r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT
  ld    r10, tt_stack_frame(r16)

  li    r16, 0
  std   r10, -10h(r1)     // ; save the previous frame pointer
  std   r16, -08h(r1)     // ; save zero

  addi  r1, r1, -16       // ; allocate raw stack
  mr    r31, r1           // ; set frame pointer

  addi    r1, r1, -30h    // ; allocate stack

  li      r16, 0
  std     r31, -10h(r1)   // ; save frame pointer
  addi    r31, r1, -10h   // ; set frame pointer
  std     r16, -18h(r1)  
  std     r16, -20h(r1)  
  std     r16, -28h(r1)  // ; save return address
  std     r16, -30h(r1)  // ; save return address

end 

// ; extopenin 4, 0
inline %0BF2h

  // ; loading TOC pointer
  lis     r2, rdata32_hi : %CORE_TOC
  addi    r2, r2, rdata32_lo : %CORE_TOC

  stdu  r1, -176(r1)

  mflr    r0

  std   r31, 168(r1)
  std   r30, 160(r1)
  std   r29, 152(r1)
  std   r28, 144(r1)
  std   r27, 136(r1)
  std   r0,  128(r1)

  // ; recover previous frame
  ld    r16, toc_data(r2)
  addis r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
  addi  r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT
  ld    r10, tt_stack_frame(r16)

  li    r16, 0
  std   r10, -10h(r1)     // ; save the previous frame pointer
  std   r16, -08h(r1)     // ; save zero

  addi  r1, r1, -16       // ; allocate raw stack
  mr    r31, r1           // ; set frame pointer

  addi    r1, r1, -30h    // ; allocate stack

  li      r16, 0
  std     r31, -10h(r1)   // ; save frame pointer
  addi    r31, r1, -10h   // ; set frame pointer
  std     r16, -18h(r1)  
  std     r16, -20h(r1)  
  std     r16, -28h(r1)  // ; save return address
  std     r16, -30h(r1)  // ; save return address

end 

// ; movsifi
inline %0F3h

  ld       r16, __arg16_2(r31)
  std      r16, __arg16_1(r1)

end

// ; movsifi sp:0, fp:i2
inline %1F3h

  ld       r3, __arg16_2(r31)

end

// ; movsifi sp:1, fp:i2
inline %2F3h

  ld       r4, __arg16_2(r31)

end

// ; newir i, r
inline %0F4h

  li      r18, __arg16_1
  ld      r12, toc_alloc(r2)
  mtctr   r12            
  bctrl                   

  li      r18, __n16_1
  ld      r17, toc_rdata(r2)
  addis   r17, r17, __disp32hi_2 
  addi    r17, r17, __disp32lo_2
  std     r18, -elSizeOffset(r15)
  std     r17, -elVMTOffset(r15)

end

// ; newnr n, r
inline %0F5h

  li      r18, __arg16_1
  ld      r12, toc_alloc(r2)
  mtctr   r12            
  bctrl                   

  li      r18, __n16lo_1
  addis   r18, r18, __n16hi_1

  ld      r17, toc_rdata(r2)
  addis   r17, r17, __disp32hi_2 
  addi    r17, r17, __disp32lo_2
  std     r18, -elSizeOffset(r15)
  std     r17, -elVMTOffset(r15)

end

// ; xmovsisi
inline %0F6h

  ld       r16, __arg16_2(r1)
  std      r16, __arg16_1(r1)

end

// ; xmovsisi 1, n
inline %1F6h

  ld       r3, __arg16_2(r1)

end

// ; xmovsisi n, 1
inline %2F6h

  std      r3, __arg16_1(r1)

end

// ; xmovsisi 2, n
inline %3F6h

  ld       r4, __arg16_2(r1)

end

// ; xmovsisi n, 1
inline %4F6h

  std      r4, __arg16_1(r1)

end

// ; xmovsisi 1, 2
inline %5F6h

  mr       r3, r4

end

// ; xmovsisi 2, 1
inline %6F6h

  mr       r4, r3

end

// ; create n, r
inline %0F7h

  ld      r12, 0(r3)
  li      r10, __n16_1
  mulld   r12, r12, r10
  addi    r12, r12, page_ceil
  andi.   r18, r12, page_mask

  ld      r12, toc_alloc(r2)
  mtctr   r12            
  bctrl                   

  ld      r12, 0(r3)
  li      r10, __n16_1
  mulld   r12, r12, r10

  li      r16, struct_mask_lo
  addis   r16, r16, struct_mask_hi
  or      r18, r12, r16

  ld      r17, toc_rdata(r2)
  addis   r17, r17, __disp32hi_2 
  addi    r17, r17, __disp32lo_2
  std     r18, -elSizeOffset(r15)
  std     r17, -elVMTOffset(r15)

end

// ; fillir
inline %0F8h

  li      r16, __arg16_1

  ld      r17, toc_rdata(r2)
  addis   r17, r17, __disp32hi_2 
  addi    r17, r17, __disp32lo_2 

  mr      r18, r15

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -1
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:

end

// ; fillir
inline %1F8h

  li      r16, __arg16_1
  li      r17, 0
  mr      r18, r15

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -1
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:

end

// ; fillir
inline %3F8h

  li      r16, __arg16_1
  li      r17, 0
  mr      r18, r15

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -1
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:

end

// ; fillir
inline %5F8h

  li      r16, __arg16_1
  li      r17, 0
  mr      r18, r15

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -1
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:

end

// ; fillir
inline %7F8h

  li      r16, __arg16_1
  li      r17, 0
  mr      r18, r15

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -1
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:

end

// ; fillir
inline %9F8h

  li      r16, __arg16_1
  li      r17, 0
  mr      r18, r15

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -1
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:

end

// ; xstorefir
inline %0F9h

  ld      r16, toc_code(r2)
  addis   r16, r16, __xdisp32hi_2
  addi    r16, r16, __xdisp32lo_2
  std     r16, __arg16_1(r31)

end

// ; xstorefir
inline %5F9h

  li      r16, 0
  std     r16, __arg16_1(r31)

end

// ; xdispatchmr
// ; NOTE : __arg32_1 - message; __n_1 - arg count; __ptr32_2 - list, __n_2 - argument list offset
inline % 0FAh

  std     r3, 0(r1)                         // ; saving arg0
//;  lea  rax, [rsp + __n_2]
  addi    r17, r1, __n16_2
  std     r4, 8(r1)                         // ; saving arg1

  addi    r17, r17, -8                      // ; HOTFIX : caller address is not in the stack

//;  mov  rsi, __ptr64_2
  ld      r21, toc_rdata(r2)
  addis   r21, r21, __disp32hi_2 
  addi    r21, r21, __disp32lo_2 

//;  xor  edx, edx
  li      r25, 0
//;  mov  rbx, [rsi] // ; message from overload list
  ld      r22, 0(r21)                           //;!!

labNextOverloadlist:
//;  mov  r9, mdata : %0
  ld      r24, toc_mdata(r2)

//;  shr  ebx, ACTION_ORDER
  srdi    r22, r22, ACTION_ORDER
//;  lea  r13, [rbx*8]
  sldi    r23, r22, 4

//;  mov  r13, [r9 + r13 * 2 + 8]
  add     r23, r23, r24
  ld      r23, 8(r23)
//;  mov  ecx, __n_1
  li      r16, __n16_1
//;  lea  rbx, [r13 - 8]
  addi    r22, r23, -8

labNextParam:
//;  sub  ecx, 1
  addi    r16, r16, -1
//;  jnz  short labMatching
  cmpwi   r16,0
  bne     labMatching

//;  mov  r9, __ptr64_2  - r21

//;  mov  r13, [r9 + rdx * 16 + 8] 
  sldi    r23, r25, 4  
  add     r23, r21, r23

  ld      r14, 0(r23)
  ld      r0,  8(r23)                

  mtctr   r0
  bctr

labMatching:
//;  mov  rdi, [rax + rcx * 8]
  sldi    r19, r16, 3
  add     r18, r19, r17
  ld      r18, 0(r18)

  //; check nil
//;  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  ld      r20, toc_rdata(r2)
  addis   r20, r20, rdata_disp32hi : %VOIDPTR
  addi    r20, r20, rdata_disp32lo : %VOIDPTR
  addi    r20, r20, elObjectOffset
//;  test  rdi, rdi                                              
  cmpwi   r18,0

//;  cmovz rdi, rsi
  iseleq  r18, r20, r18

//;  mov  rdi, [rdi - elVMTOffset]
  ld      r18, -elVMTOffset(r18)
//;  mov  rsi, [rbx + rcx * 8]
  add     r20,  r22, r19
  ld      r20, 0(r20)

labNextBaseClass:
//;  cmp  rsi, rdi
  cmp     r20, r18
//;  jz   labNextParam
  beq     labNextParam 
//;  mov  rdi, [rdi - elPackageOffset]
  ld      r18, -elPackageOffset(r18)
//;  and  rdi, rdi
  cmpwi   r18, 0
//;  jnz  short labNextBaseClass
  bne     labNextBaseClass

//;  add  rdx, 1
  addi    r25, r25, 1
//;  mov  r13, __ptr32_2
  mr      r23, r21

//;  lea  r9, [rdx * 8]
  sldi    r24, r25, 4

//;  mov  rbx, [r13 + r9 * 2] // ; message from overload list
  add     r22, r24, r23
  ld      r22, 0(r22)

//;  and  rbx, rbx
  cmpwi   r22, 0
//;  jnz  labNextOverloadlist
  bne     labNextOverloadlist

end

// ; xdispatchmr for variadic
// ; NOTE : __arg32_1 - message; __n_1 - arg count; __ptr32_2 - list, __n_2 - argument list offset
inline % 05FAh

  std     r3, 0(r1)                         // ; saving arg0
//;  lea  rax, [rsp + __n_2]
  addi    r17, r1, __n16_2
  std     r4, 8(r1)                         // ; saving arg1

  addi    r17, r17, -8                      // ; HOTFIX : caller address is not in the stack

//;  mov  rsi, __ptr64_2
  ld      r21, toc_rdata(r2)
  addis   r21, r21, __disp32hi_2 
  addi    r21, r21, __disp32lo_2 

//;  xor  edx, edx
  li      r25, 0
  li      r11, 0

  // ; count the number of args
  mr      r22, r17
  li      r23, 0
  addi    r23, r23, -1                        // ; define the terminator
labCountParam:
  addi    r22, r22, 8
  ld      r24, 0(r22)
  addi    r11, r11, 1
  cmp     r24, r23
  bne     labCountParam

//;  mov  rbx, [rsi] // ; message from overload list
  ld      r22, 0(r21)                           //;!!

labNextOverloadlist:
//;  mov  r9, mdata : %0
  ld      r24, toc_mdata(r2)

//;  shr  ebx, ACTION_ORDER
  srdi    r22, r22, ACTION_ORDER
//;  lea  r13, [rbx*8]
  sldi    r23, r22, 4

//;  mov  r13, [r9 + r13 * 2 + 8]
  add     r23, r23, r24
  ld      r23, 8(r23)

  li      r16, 0

//;  lea  rbx, [r13 - 8]
  addi    r22, r23, -8

labNextParam:
//;  add  ecx, 1
  addi    r16, r16, 1
//;  jnz  short labMatching
  cmp     r16,r11
  bne     labMatching

//;  mov  r9, __ptr64_2  - r21

//;  mov  r13, [r9 + rdx * 16 + 8] 
  sldi    r23, r25, 4  
  add     r23, r21, r23

  ld      r14, 0(r23)
  ld      r0,  8(r23)                

  mtctr   r0
  bctr

labMatching:
  addi    r20, r22, 8
  ld      r18, 0(r20)
  cmpwi   r18, 0
  iseleq  r22, r22, r20

//;  mov  rdi, [rax + rcx * 8]
  sldi    r19, r16, 3
  add     r18, r19, r17
  ld      r18, 0(r18)

  //; check nil
//;  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  ld      r20, toc_rdata(r2)
  addis   r20, r20, rdata_disp32hi : %VOIDPTR
  addi    r20, r20, rdata_disp32lo : %VOIDPTR
  addi    r20, r20, elObjectOffset
//;  test  rdi, rdi                                              
  cmpwi   r18,0

//;  cmovz rdi, rsi
  iseleq  r18, r20, r18

//;  mov  rdi, [rdi - elVMTOffset]
  ld      r18, -elVMTOffset(r18)
//;  mov  rsi, [rbx + rcx * 8]
  ld      r20, 0(r22)

labNextBaseClass:
//;  cmp  rsi, rdi
  cmp     r20, r18
//;  jz   labNextParam
  beq     labNextParam 
//;  mov  rdi, [rdi - elPackageOffset]
  ld      r18, -elPackageOffset(r18)
//;  and  rdi, rdi
  cmpwi   r18, 0
//;  jnz  short labNextBaseClass
  bne     labNextBaseClass

//;  add  rdx, 1
  addi    r25, r25, 1
//;  mov  r13, __ptr32_2
  mr      r23, r21

//;  lea  r9, [rdx * 8]
  sldi    r24, r25, 4

//;  mov  rbx, [r13 + r9 * 2] // ; message from overload list
  add     r22, r24, r23
  ld      r22, 0(r22)

//;  and  rbx, rbx
  cmpwi   r22, 0
//;  jnz  labNextOverloadlist
  bne     labNextOverloadlist

end

// ; xdispatchmr
// ; NOTE : __arg32_1 - message; __n_1 - arg count; __ptr32_2 - list, __n_2 - argument list offset
inline % 9FAh

//; !! temporally commented

end

// ; xdispatchmr
// ; NOTE : __arg32_1 - message; __n_1 - arg count; __ptr32_2 - list, __n_2 - argument list offset
inline % 0AFAh

//; !! temporally commented

end

// ; dispatchmr
// ; NOTE : __arg32_1 - message; __n_1 - arg count; __ptr32_2 - list, __n_2 - argument list offset
inline % 0FBh

  std     r3, 0(r1)                         // ; saving arg0
//;  lea  rax, [rsp + __n_2]
  addi    r17, r1, __n16_2
  std     r4, 8(r1)                         // ; saving arg1

  addi    r17, r17, -8                      // ; HOTFIX : caller address is not in the stack

//;  mov  rsi, __ptr64_2
  ld      r21, toc_rdata(r2)
  addis   r21, r21, __disp32hi_2 
  addi    r21, r21, __disp32lo_2 

//;  xor  edx, edx
  li      r25, 0
//;  mov  rbx, [rsi] // ; message from overload list
  ld      r22, 0(r21)                           //;!!

labNextOverloadlist:
//;  mov  r9, mdata : %0
  ld      r24, toc_mdata(r2)

//;  shr  ebx, ACTION_ORDER
  srdi    r22, r22, ACTION_ORDER
//;  lea  r13, [rbx*8]
  sldi    r23, r22, 4

//;  mov  r13, [r9 + r13 * 2 + 8]
  add     r23, r23, r24
  ld      r23, 8(r23)
//;  mov  ecx, __n_1
  li      r16, __n16_1
//;  lea  rbx, [r13 - 8]
  addi    r22, r23, -8

labNextParam:
//;  sub  ecx, 1
  addi    r16, r16, -1
//;  jnz  short labMatching
  cmpwi   r16,0
  bne     labMatching

  sldi    r23, r25, 4  
  add     r25, r21, r23

  ld      r23, 8(r25)
  ld      r14, 0(r25)

  ld      r16, -elVMTOffset(r15)

  add     r20, r16, r23
  ld      r0, 8(r20)                
  mtctr   r0
  bctr

labMatching:
//;  mov  rdi, [rax + rcx * 8]
  sldi    r19, r16, 3
  add     r18, r19, r17
  ld      r18, 0(r18)

  //; check nil
//;  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  ld      r20, toc_rdata(r2)
  addis   r20, r20, rdata_disp32hi : %VOIDPTR
  addi    r20, r20, rdata_disp32lo : %VOIDPTR
  addi    r20, r20, elObjectOffset
//;  test  rdi, rdi                                              
  cmpwi   r18,0

//;  cmovz rdi, rsi
  iseleq  r18, r20, r18

//;  mov  rdi, [rdi - elVMTOffset]
  ld      r18, -elVMTOffset(r18)
//;  mov  rsi, [rbx + rcx * 8]
  add     r20,  r22, r19
  ld      r20, 0(r20)

labNextBaseClass:
//;  cmp  rsi, rdi
  cmp     r20, r18
//;  jz   labNextParam
  beq     labNextParam 
//;  mov  rdi, [rdi - elPackageOffset]
  ld      r18, -elPackageOffset(r18)
//;  and  rdi, rdi
  cmpwi   r18, 0
//;  jnz  short labNextBaseClass
  bne     labNextBaseClass

//;  add  rdx, 1
  addi    r25, r25, 1
//;  mov  r13, __ptr32_2
  mr      r23, r21

//;  lea  r9, [rdx * 8]
  sldi    r24, r25, 4

//;  mov  rbx, [r13 + r9 * 2] // ; message from overload list
  add     r22, r24, r23
  ld      r22, 0(r22)

//;  and  rbx, rbx
  cmpwi   r22, 0
//;  jnz  labNextOverloadlist
  bne     labNextOverloadlist

end

// ; dispatchmr
// ; NOTE : __arg32_1 - message; __n_1 - arg count; __ptr32_2 - list, __n_2 - argument list offset
inline % 5FBh

  std     r3, 0(r1)                         // ; saving arg0
//;  lea  rax, [rsp + __n_2]
  addi    r17, r1, __n16_2
  std     r4, 8(r1)                         // ; saving arg1

  addi    r17, r17, -8                      // ; HOTFIX : caller address is not in the stack

//;  mov  rsi, __ptr64_2
  ld      r21, toc_rdata(r2)
  addis   r21, r21, __disp32hi_2 
  addi    r21, r21, __disp32lo_2 

//;  xor  edx, edx
  li      r25, 0
  li      r16, 0

  // ; count the number of args
  mr      r22, r17
  li      r23, 0
  addi    r23, r23, -1                        // ; define the terminator
labCountParam:
  addi    r22, r22, 8
  ld      r24, 0(r22)
  addi    r16, r16, 1
  cmp     r24, r23
  bne     labCountParam
  mr      r17, r16

//;  mov  rbx, [rsi] // ; message from overload list
  ld      r22, 0(r21)                           //;!!

labNextOverloadlist:
//;  mov  r9, mdata : %0
  ld      r24, toc_mdata(r2)

//;  shr  ebx, ACTION_ORDER
  srdi    r22, r22, ACTION_ORDER
//;  lea  r13, [rbx*8]
  sldi    r23, r22, 4

//;  mov  r13, [r9 + r13 * 2 + 8]
  add     r23, r23, r24
  ld      r23, 8(r23)
//;  mov  ecx, __n_1
  li      r16, __n16_1
//;  lea  rbx, [r13 - 8]
  addi    r22, r23, -8
  mr      r26, r22

labNextParam:
//;  add  ecx, 1
  addi    r16, r16, 1
//;  jnz  short labMatching
  cmpwi   r16,0
  bne     labMatching

  sldi    r23, r25, 4  
  add     r25, r21, r23

  ld      r23, 8(r25)
  ld      r14, 0(r25)

  ld      r16, -elVMTOffset(r15)

  add     r20, r16, r23
  ld      r0, 8(r20)                
  mtctr   r0
  bctr

labMatching:
  addi    r20, r26, 8
  ld      r18, 0(r20)
  cmpwi   r18, 0
  iseleq  r26, r26, r20

//;  mov  rdi, [rax + rcx * 8]
  sldi    r19, r16, 3
  add     r18, r19, r17
  ld      r18, 0(r18)

  //; check nil
//;  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  ld      r20, toc_rdata(r2)
  addis   r20, r20, rdata_disp32hi : %VOIDPTR
  addi    r20, r20, rdata_disp32lo : %VOIDPTR
  addi    r20, r20, elObjectOffset
//;  test  rdi, rdi                                              
  cmpwi   r18,0

//;  cmovz rdi, rsi
  iseleq  r18, r20, r18

//;  mov  rdi, [rdi - elVMTOffset]
  ld      r18, -elVMTOffset(r18)
//;  mov  rsi, [rbx + rcx * 8]
  ld      r20, 0(r26)

labNextBaseClass:
//;  cmp  rsi, rdi
  cmp     r20, r18
//;  jz   labNextParam
  beq     labNextParam 
//;  mov  rdi, [rdi - elPackageOffset]
  ld      r18, -elPackageOffset(r18)
//;  and  rdi, rdi
  cmpwi   r18, 0
//;  jnz  short labNextBaseClass
  bne     labNextBaseClass

//;  add  rdx, 1
  addi    r25, r25, 1
//;  mov  r13, __ptr32_2
  mr      r23, r21

//;  lea  r9, [rdx * 8]
  sldi    r24, r25, 4

//;  mov  rbx, [r13 + r9 * 2] // ; message from overload list
  add     r22, r24, r23
  ld      r22, 0(r22)

//;  and  rbx, rbx
  cmpwi   r22, 0
//;  jnz  labNextOverloadlist
  bne     labNextOverloadlist

end

// ; dispatchmr (alt mode)
// ; NOTE : __arg32_1 - message; __n_1 - arg count; __ptr32_2 - list, __n_2 - argument list offset
inline % 06FBh

  std     r3, 0(r1)                         // ; saving arg0
//;  lea  rax, [rsp + __n_2]
  addi    r17, r1, __n16_2
  std     r4, 8(r1)                         // ; saving arg1

  addi    r17, r17, -8                      // ; HOTFIX : caller address is not in the stack

//;  mov  rsi, __ptr64_2
  ld      r21, toc_rdata(r2)
  addis   r21, r21, __disp32hi_2 
  addi    r21, r21, __disp32lo_2 

//;  xor  edx, edx
  li      r25, 0
//;  mov  rbx, [rsi] // ; message from overload list
  ld      r22, 0(r21)                           //;!!

labNextOverloadlist:
//;  mov  r9, mdata : %0
  ld      r24, toc_mdata(r2)

//;  shr  ebx, ACTION_ORDER
  srdi    r22, r22, ACTION_ORDER
//;  lea  r13, [rbx*8]
  sldi    r23, r22, 4

//;  mov  r13, [r9 + r13 * 2 + 8]
  add     r23, r23, r24
  ld      r23, 8(r23)
//;  mov  ecx, __n_1
  li      r16, __n16_1
//;  lea  rbx, [r13 - 8]
  addi    r22, r23, -8

labNextParam:
//;  sub  ecx, 1
  addi    r16, r16, -1
//;  jnz  short labMatching
  cmpwi   r16,0
  bne     labMatching

  sldi    r23, r25, 4  
  add     r25, r21, r23

  ld      r23, 8(r25)
  ld      r14, 0(r25)
  ld      r16, -elVMTOffset(r15)
  ld      r20, -elVMTSizeOffset(r16)     
  sldi    r20, r20, 4
  add     r16, r16, r20
  add     r20, r16, r23
  ld      r0, 8(r20)                
  mtctr   r0
  bctr

labMatching:
//;  mov  rdi, [rax + rcx * 8]
  sldi    r19, r16, 3
  add     r18, r19, r17
  ld      r18, 0(r18)

  //; check nil
//;  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  ld      r20, toc_rdata(r2)
  addis   r20, r20, rdata_disp32hi : %VOIDPTR
  addi    r20, r20, rdata_disp32lo : %VOIDPTR
  addi    r20, r20, elObjectOffset
//;  test  rdi, rdi                                              
  cmpwi   r18,0

//;  cmovz rdi, rsi
  iseleq  r18, r20, r18

//;  mov  rdi, [rdi - elVMTOffset]
  ld      r18, -elVMTOffset(r18)
//;  mov  rsi, [rbx + rcx * 8]
  add     r20,  r22, r19
  ld      r20, 0(r20)

labNextBaseClass:
//;  cmp  rsi, rdi
  cmp     r20, r18
//;  jz   labNextParam
  beq     labNextParam 
//;  mov  rdi, [rdi - elPackageOffset]
  ld      r18, -elPackageOffset(r18)
//;  and  rdi, rdi
  cmpwi   r18, 0
//;  jnz  short labNextBaseClass
  bne     labNextBaseClass

//;  add  rdx, 1
  addi    r25, r25, 1
//;  mov  r13, __ptr32_2
  mr      r23, r21

//;  lea  r9, [rdx * 8]
  sldi    r24, r25, 4

//;  mov  rbx, [r13 + r9 * 2] // ; message from overload list
  add     r22, r24, r23
  ld      r22, 0(r22)

//;  and  rbx, rbx
  cmpwi   r22, 0
//;  jnz  labNextOverloadlist
  bne     labNextOverloadlist

end

// ; dispatchmr
// ; NOTE : __arg32_1 - message; __n_1 - arg count; __ptr32_2 - list, __n_2 - argument list offset
inline % 0BFBh

  std     r3, 0(r1)                         // ; saving arg0
//;  lea  rax, [rsp + __n_2]
  addi    r17, r1, __n16_2
  std     r4, 8(r1)                         // ; saving arg1

  addi    r17, r17, -8                      // ; HOTFIX : caller address is not in the stack

//;  mov  rsi, __ptr64_2
  ld      r21, toc_rdata(r2)
  addis   r21, r21, __disp32hi_2 
  addi    r21, r21, __disp32lo_2 

//;  xor  edx, edx
  li      r25, 0
  li      r16, 0

  // ; count the number of args
  mr      r22, r17
  li      r23, 0
  addi    r23, r23, -1                        // ; define the terminator
labCountParam:
  addi    r22, r22, 8
  ld      r24, 0(r22)
  addi    r16, r16, 1
  cmp     r24, r23
  bne     labCountParam
  mr      r17, r16

//;  mov  rbx, [rsi] // ; message from overload list
  ld      r22, 0(r21)                           //;!!

labNextOverloadlist:
//;  mov  r9, mdata : %0
  ld      r24, toc_mdata(r2)

//;  shr  ebx, ACTION_ORDER
  srdi    r22, r22, ACTION_ORDER
//;  lea  r13, [rbx*8]
  sldi    r23, r22, 4

//;  mov  r13, [r9 + r13 * 2 + 8]
  add     r23, r23, r24
  ld      r23, 8(r23)
//;  mov  ecx, __n_1
  li      r16, __n16_1
//;  lea  rbx, [r13 - 8]
  addi    r22, r23, -8
  mr      r26, r22

labNextParam:
//;  add  ecx, 1
  addi    r16, r16, 1
//;  jnz  short labMatching
  cmpwi   r16,0
  bne     labMatching

  sldi    r23, r25, 4  
  add     r25, r21, r23

  ld      r23, 8(r25)
  ld      r14, 0(r25)

  ld      r16, -elVMTOffset(r15)
  ld      r20, -elVMTSizeOffset(r16)     
  sldi    r20, r20, 4
  add     r16, r16, r20
  add     r20, r16, r23
  ld      r0, 8(r20)                
  mtctr   r0
  bctr

labMatching:
  addi    r20, r26, 8
  ld      r18, 0(r20)
  cmpwi   r18, 0
  iseleq  r26, r26, r20

//;  mov  rdi, [rax + rcx * 8]
  sldi    r19, r16, 3
  add     r18, r19, r17
  ld      r18, 0(r18)

  //; check nil
//;  mov   rsi, rdata : %VOIDPTR + elObjectOffset
  ld      r20, toc_rdata(r2)
  addis   r20, r20, rdata_disp32hi : %VOIDPTR
  addi    r20, r20, rdata_disp32lo : %VOIDPTR
  addi    r20, r20, elObjectOffset
//;  test  rdi, rdi                                              
  cmpwi   r18,0

//;  cmovz rdi, rsi
  iseleq  r18, r20, r18

//;  mov  rdi, [rdi - elVMTOffset]
  ld      r18, -elVMTOffset(r18)
//;  mov  rsi, [rbx + rcx * 8]
  ld      r20, 0(r26)

labNextBaseClass:
//;  cmp  rsi, rdi
  cmp     r20, r18
//;  jz   labNextParam
  beq     labNextParam 
//;  mov  rdi, [rdi - elPackageOffset]
  ld      r18, -elPackageOffset(r18)
//;  and  rdi, rdi
  cmpwi   r18, 0
//;  jnz  short labNextBaseClass
  bne     labNextBaseClass

//;  add  rdx, 1
  addi    r25, r25, 1
//;  mov  r13, __ptr32_2
  mr      r23, r21

//;  lea  r9, [rdx * 8]
  sldi    r24, r25, 4

//;  mov  rbx, [r13 + r9 * 2] // ; message from overload list
  add     r22, r24, r23
  ld      r22, 0(r22)

//;  and  rbx, rbx
  cmpwi   r22, 0
//;  jnz  labNextOverloadlist
  bne     labNextOverloadlist

end

// ; vcallmr
inline % 0FCh

  ld       r16, -elVMTOffset(r15)     
  addi     r16, r16, __arg16_1
  ld       r17, 8(r16)
  mtctr    r17            // ; put code address into ctr
  bctrl                   // ; and call it

end

// ; vcallmr
inline % 6FCh

  ld       r16, -elVMTOffset(r15)     
  ld       r17, -elVMTSizeOffset(r16)     
  sldi     r17, r17, 4
  addi     r16, r16, __arg16_1
  add      r16, r16, r17
  ld       r17, 8(r16)
  mtctr    r17            // ; put code address into ctr
  bctrl                   // ; and call it

end

// ; callmr
inline %0FDh

  ld       r12, toc_code(r2)
  addis    r12, r12, __disp32hi_2 
  addi     r12, r12, __disp32lo_2
  mtctr    r12            // ; put code address into ctr
  bctrl                   // ; and call it

end

// ; callext
inline %0FEh

  // ; Put the parameters in r3 through r10,
  // ; and additional parameters go on the stack
  // ; after the home space (not shown here).

  ld      r5, 16(r1)
  ld      r6, 24(r1)

  ld      r12, toc_import(r2)
  addis   r12, r12, __disp32hi_1 
  ld      r12,__disp32lo_1(r12)

//   ; ld      r11, 00(r2)    // ; r11 points to import address table entry
//   ; ld      r11, 00(r11)   // ; r11 = point address table entry
//   ; ld      r12, 00(r11)   // ; r12 = address of code
//   ; ld      r2,  08h(r11)  // ; load table of contents for destination
  mtctr   r12            // ; put code address into ctr
  bctrl                  // ; and call it

  mr      r14, r3

  // lwz      r2, n(r1)     // ; restore our table of contents
  // ; temporally reloading TOC pointer
  lis   r2, rdata32_hi : %CORE_TOC
  addi  r2, r2, rdata32_lo : %CORE_TOC

end