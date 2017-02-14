#import system.
#import cellular.
#import extensions.

#symbol(const,int)maxX = 3.
#symbol(const,int)maxY = 3.

//// ---- program ---
//
//#class GameOfLine
//{
//    #field(intmatrix)theBoard.
//    
//    #constructor new &n:n &m:m
//    [
//        theBoard := IntMatrix new:n:m.
//    ]
//    
//    #method next
//    [
//        
//    ]
//}

#symbol program =
[
    #var(space) space := IntMatrixSpace new &int:maxX &int:maxY &nnfunc:(&int:i &int:j) [ randomGenerator eval:0:1 ].
    
    #var(int)cell := space getAt &int:0 &int:0.
    
//    #var aSpace := Space new:(Cellular new &x:3 &y:3, Cellular new &x:5 &y:22, Cellular new &x:5 &y:13, Cellular new &x:13 &y:13) &scale:10.
].