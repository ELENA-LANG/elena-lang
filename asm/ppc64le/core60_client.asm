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
define toc_stat              0038h
define toc_allocperm         0040h
define toc_prepare           0048h

// ; system startup
inline %4CFh

  lis     r2, rdata32_hi : %CORE_TOC
  addi    r2, r2, rdata32_lo : %CORE_TOC

  ld      r16, toc_data(r2)
  addis   r16, r16, data_disp32hi : %CORE_ET_TABLE
  addi    r16, r16, data_disp32lo : %CORE_ET_TABLE
  std     r1, tt_stack_root(r16)

end
