// --- Predefined References  --
define EXIT                 1001Dh
define PREPARE              10027h

procedure % EXIT

  mov rcx, 0

  // ; exit
  sub  rsp, 20h
  call extern 'dlls'KERNEL32.ExitProcess     

end

procedure % PREPARE

  ret    // ; idle

end
