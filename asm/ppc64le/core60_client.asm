// ; --- Predefined References  --
define VEH_HANDLER       10003h

define CORE_TOC          20001h
define CORE_ET_TABLE     2000Bh

// ; system startup
inline %4CFh

  lis     r2, rdata32_hi : %CORE_TOC
  addi    r2, r2, rdata32_lo : %CORE_TOC

  ld      r16, toc_data(r2)
  addis   r16, r16, data_disp32hi : %CORE_ET_TABLE
  addi    r16, r16, data_disp32lo : %CORE_ET_TABLE
  std     r1, tt_stack_root(r16)

end
