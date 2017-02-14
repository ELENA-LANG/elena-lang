#import system.
#import cellular.
#import extensions.

#symbol(const,int)maxX = 6.
#symbol(const,int)maxY = 4.

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
    #var(space) space := IntMatrixSpace new &int:maxX &int:maxY &nnfunc:(&int:i &int:j) [ randomGenerator eval:0:2 ].
    
    space print.
    
//    #var aSpace := Space new:(Cellular new &x:3 &y:3, Cellular new &x:5 &y:22, Cellular new &x:5 &y:13, Cellular new &x:13 &y:13) &scale:10.
].