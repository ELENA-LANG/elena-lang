main:
  li %r5, 0xA
.loop:
  subi %r5, %r5, 0x5
  cmpi %r5, 0x0
  bne .loop
  
  li %r5, 0xFFFF
  rtn              
                                                                         