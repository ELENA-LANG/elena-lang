// --- Predefined References  --
define EXIT                 1001Dh
define PREPARE              10027h

define CORE_OS_TABLE        20009h

structure % CORE_OS_TABLE

  dq 0 // ; dummy

end

procedure % EXIT

  mov ecx, 0

  // ; exit
  sub  rsp, 20h
  call extern 'dlls'KERNEL32.ExitProcess     

end

procedure % PREPARE

  ret    // ; idle

end
