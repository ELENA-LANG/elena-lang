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
