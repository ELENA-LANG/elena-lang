defmacro #callTest(a,b)
  mov  esi, [esp + a]
  mov  edx, [esi]
  mov  ebx, [stat : "$elena'@referencetable"]
  mov  eax, [ebx + edx * a]
  
  ret a
endmacro

procedure % 444h
  #callTest(4, "$elena'@referencetable")
end
