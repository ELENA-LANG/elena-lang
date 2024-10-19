// ; ==== System commands ===

// ; ==== Overridden Command Set ==

// ; --- Predefined References  --
define VEH_HANDLER       10003h

define CORE_TOC          20001h
define CORE_ET_TABLE     2000Bh

// ; TOC TABLE OFFSETS
define toc_import            0000h
define toc_rdata             0008h
define toc_mdata             0010h
define toc_code              0018h
define toc_gctable           0020h
define toc_alloc             0028h
define toc_data              0030h

// ; ==== System commands ===

// VEH_HANDLER() 
procedure % VEH_HANDLER

  mr      r16, r3
  mr      r3, r14
  mr      r14, r16

  // ; temporally reloading TOC pointer
  lis   r2, rdata32_hi : %CORE_TOC
  addi  r2, r2, rdata32_lo : %CORE_TOC

  ld      r16, toc_data(r2)
  addis   r16, r16, data_disp32hi : %CORE_ET_TABLE
  addi    r16, r16, data_disp32lo : %CORE_ET_TABLE
  ld      r0, 0(r16)

  mtctr   r0
  bctr

end
