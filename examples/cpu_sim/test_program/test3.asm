; posicao inicial da pilha
li %r14, 0xFFFF

main:
  li %r3, 0xFFFFFFF
  li %r4, 0xFFFFFFF
  li %r5, 0xFFFFFFF
  li %r6, 0xFFFFFFF
  
  bl funcao
  rtn
  

funcao:
  push %r3, %r4, %r5, %r6
  mov %r3, %r0
  
  li %r4, 0xA
  mul %r3, %r4, %r4
  li %r6, 0x2
  div %r5, %r3, %r4
  
  pop %r3, %r4, %r5, %r6
  rtn
                                                                  