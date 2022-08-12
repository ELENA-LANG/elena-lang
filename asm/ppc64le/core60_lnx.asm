// ; ==== System commands ===

// ; ==== Overridden Command Set ==

// ; --- Predefined References  --
define INVOKER           10001h
define VEH_HANDLER       10003h

define CORE_TOC          20001h
define CORE_ET_TABLE     2000Bh

// ; ==== System commands ===

// INVOKER(function, arg)
procedure % INVOKER

  // ; loading TOC pointer
  lis   r2, rdata32_hi : %CORE_TOC
  addi  r2, r2, rdata32_lo : %CORE_TOC

  // ; R3 - function
  // ; R4	 - arg
  stdu  r1, -176(r1)       // ; 0xf821ff51

  mflr  r0

  std   r0,  192(r1)
  std   r31, 168(r1)
  std   r30, 160(r1)
  std   r29, 152(r1)
  std   r28, 144(r1)
  std   r27, 136(r1)

  mtctr r3                // ; put code address into ctr
  mr    r3, r4
  bctrl                   // ; and call it

  ld   r31, 168(r1)
  ld   r30, 160(r1)
  ld   r29, 152(r1)
  ld   r28, 144(r1)
  ld   r27, 136(r1)

  ld   r0,  192(r1)
  mtlr r0

  addi  r1, r1, 176
  blr                                          

end

// VEH_HANDLER() 
procedure % VEH_HANDLER

  mr      r16, r3
  mr      r3, r14
  mr      r14, r3

  // ; temporally reloading TOC pointer
  lis   r2, rdata32_hi : %CORE_TOC
  addi  r2, r2, rdata32_lo : %CORE_TOC

  ld      r14, toc_data(r2)
  addis   r14, r14, data_disp32hi : %CORE_ET_TABLE
  addi    r14, r14, data_disp32lo : %CORE_ET_TABLE
  ld      r0, [r14]

  mtctr   r0
  bctr

end
