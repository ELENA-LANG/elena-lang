import extensions;

A(k,x1,x2,x3,x4,x5)
{
   var m := new Reference<int>(k);
    
   var b := { m -= 1; ^ A(m,this self,x1,x2,x3,x4) };
    
   if (m <= 0)
   { 
      ^ x4() + x5() 
   }
   else
   { 
      ^ b() 
   }
}

public program()
{
   for(int n := 0; n <= 14; n += 1)
   {
      Console.printLine(A(n,{ ^1 },{ ^-1 },{ ^-1 },{ ^1 },{ ^0 }))
   }
}
