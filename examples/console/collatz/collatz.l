#import system.
#import system'dynamic.
#import extensions.

#class(extension)op
{
    #method y
        = self int is &even iif:0:1.
}

#symbol factory =
{
    new : aF : aG
        = () [ this int is &even iif:(this::aF eval):(this::aG eval) ].
    
    new : Func : C1 : C2
//        = () [ (this * this y + (1 - this y) * C1)::Func eval + (1 - this y) * (C2 + this) ].
        = ()
        [
            #var b := this y.
            
            #var a1 := this * b + (1 - b) * C1.
            #var a2 := (1 - b) * (C2 + this).
            
            #var r1 := a1::Func eval.
            
            #var r := r1 + a2.
            
            ^ r.
        ].
        
    compose : aF : aG
        = () [ (this::aG eval)::aF eval ].
}.

#symbol F =
    () [ this / 2 ].

#symbol G =
    () [ this * 2 + 3 ].

#symbol hFh
    = factory new:F:0:0.
    
#symbol hGh
    = factory new:G:(-3/2):0.

#static C2
    = factory compose:hFh:hGh.

#static C1
    = factory new:F:G.

#class(extension)CollatzOp
{
    #method Collatz1
        = self::C1 eval.
        
    #method Collatz2
        = self::C2 eval.
}

#symbol program=
[
    console writeLine:3.
    console writeLine:(3 Collatz2).
    
/*    
    #var n := console write:"Enter x(0):" readLine toInt.
    #var m := n.
    console writeLine:n.
    
    #loop (n < 100000)?
    [       
        n := n Collatz1.
        
        console writeLine:n.
    ].
    
    console writeLine:n.
    
    #loop (m < 100000)?
    [       
        m := m Collatz2.
        
        console writeLine:m.
    ].
*/    
    console readChar.
].