/*import extensions;

public program()
{
   real r := 1.0;

   real z := r.exp();
   console.printLine(z);

   console.printLine(z.ln());
}
*/

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

/*
public program()
{
    var t := 0;
    int t2 := 0;
    console.then(
    {
        Assert.ifTrue(t == 0);
        Assert.ifTrue(t2 == 0);
        
        t := 1;
        t2 += 1;
    }).then(
    {
        Assert.ifTrue(t == 1);
        Assert.ifTrue(t2 == 1);
        
        t := 2;
        t2 += 1;
    });
}
*/