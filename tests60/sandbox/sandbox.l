import extensions;
/*
public program()
{
   real r := 1.0;

   real z := r.exp();
   console.printLine(z);

   console.printLine(z.ln());
}
*/
/*
import extensions;
import system'math;
   
public extension lazyOp
{
     whileFalse(Func action)
     {
         until(self)
         {
             action()
         }
     }        
}

public program()
{
     var n := 1;
    
     (:lazy (n.sqr().mod(100) == 64)).whileFalse({ n := n + 1 });
   
     console.printLine(n)
}
*/

public program()
{
    var mode := -1;
    var value := true;
    value ? { mode := 0 } ! { mode := 1 };
    
    Assert.ifTrue(mode == 0);
}
