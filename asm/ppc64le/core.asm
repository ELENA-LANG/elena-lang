// r1 - stack frame pointer
// r2 - TOC??

// r4 - acc

// r31 - base frame pointer

compare - 79

// --- Predefined References  --
define GC_ALLOC	            10001h
define HOOK                 10010h
define INVOKER              10011h
define INIT_RND             10012h
define CALC_SIZE            1001Fh
define GET_COUNT            10020h
define THREAD_WAIT          10021h
define GC_ALLOCPERM	    10031h

//define CORE_GC_TABLE        20002h
//define CORE_STATICROOT      20005h
//define CORE_TLS_INDEX       20007h
//define THREAD_TABLE         20008h
//define CORE_MESSAGE_TABLE   2000Ah
//define CORE_ET_TABLE        2000Bh
//define SYSTEM_ENV           2000Ch
//define VOID                 2000Dh
//define VOIDPTR              2000Eh



// lis Rx,addr -> addis Rx,0,addr
// mr Rx,Ry    -> or Rx, Ry, Ry
// li Rx,val 

// --- GC_ALLOC (r3 - size) ---

procedure %GC_ALLOC

  // ; r17 - gc_yg_current, r19 - gc_yg_end

  // ; mov  rax, [data : %CORE_GC_TABLE + gc_yg_current]
  lis  r16, data : %CORE_GC_TABLE @ha
  li   r15, gc_yg_current
  ld   r12, data : %CORE_GC_TABLE @l (r16)
  add  r17, r12, r15

  // mov  rdx, [data : %CORE_GC_TABLE + gc_yg_end]
  li   r18, gc_yg_current
  add  r19, r12, r15

  // add  rcx, rax
  add   r20,r17,r3

  // cmp  rcx, rdx
  cmp   r20, r19

  // jae  short labYGCollect
  bgt   labYGCollect

  // mov  [data : %CORE_GC_TABLE + gc_yg_current], rcx
  add  r19, r12, r15
  ld   r19, r20

  // lea  rbx, [rax + elObjectOffset]
  addi  r4, r17, elObjectOffset

  // ret
  blr

labYGCollect:

end