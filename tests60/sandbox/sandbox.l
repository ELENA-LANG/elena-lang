import extensions;

ackermann(m,n)
{
   if(n < 0 || m < 0)
   {
      InvalidArgumentException.raise()
   };
    
   m =>
      0 { ^n + 1 }
      : {
         n => 
            0 { ^ackermann(m - 1,1) }
            : { ^ackermann(m - 1,ackermann(m,n-1)) }
         }
}


public program()
{
   console.printLine(ackermann(3,4));
//    var o := nil;
//    o.call()

//   var o := new Object();
//   o.fail()

//      try
//      {
//         InvalidArgumentException.raise()
//      }
//      catch(Exception e)
//      {
//         console.printLine:"failed"
//      }

}