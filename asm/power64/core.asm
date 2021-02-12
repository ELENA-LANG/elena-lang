// r1 - stack frame pointer
// r2 - TOC
// r31 - frame pointer

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

// --- GC_ALLOC ---
procedure %GC_ALLOC

  // ; mov  rax, [data : %CORE_GC_TABLE + gc_yg_current]
  ld  r12, data : %TOC_GC_TABLE_INDEX(r2)
  ld  r12, gc_yg_current(r12)

  // mov  rdx, [data : %CORE_GC_TABLE + gc_yg_end]
  // add  rcx, rax
  // cmp  rcx, rdx
  // jae  short labYGCollect
  // mov  [data : %CORE_GC_TABLE + gc_yg_current], rcx
  // lea  rbx, [rax + elObjectOffset]
  // ret

end