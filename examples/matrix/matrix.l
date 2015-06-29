// based on http://cboard.cprogramming.com/cplusplus-programming/30001-determinant-calculation.html prelude sample

// --- default namespaces ---
#define system.
#define system'dynamic.
#define extensions.
#define extensions'math.

// --- factory ---

#class Matrix
{
    #field theMatrix.
    
    #constructor new &matrix:aMatrix
    [
        theMatrix := Extension(aMatrix, matrixOp).
    ]
    
    #constructor new &n:anN &name:aName
        <= new &matrix:(RealMatrix new &rows:anN &columns:anN set &every_ij: (&index:i &index:j)
            [  
                console writeLiteral:"Enter ":aName:"[":i:",":j:"]=" readLine:(Real new)
            ]).
    
    #constructor newIdentity &n:anN
        <= new &matrix:(RealMatrix new &rows:anN &columns:anN set &every_ij: (&index:i &index:j)
            [ (i == j) ? [ 1 ] ! [ 0 ] ]).
    
    #method => theMatrix.
}

// ---- Program ---

#symbol control =
{
    prompt = "Choose an option" + 'newLine +
             "[1] - Sum Matrix" + 'newLine +
             "[2] - Subtract Matrix" + 'newLine +
             "[3] - Multiply Matrix" + 'newLine +
             "[4] - Matrix Determinant" + 'newLine +
             "[5] - Identity Matrix" + 'newLine +
             "[6] - Exit" + 'newLine +
             "Your choice:".
    
    proceed : aChoice
    [
        ^ aChoice =>
            "1" ? 
            [
                console write:"Enter the order of matrix: ".
                #var N := console readLine:(Integer new).
                
                #var aMatrixA := Matrix new &n:N &name:"A".
                
                console writeLine.
                
                #var aMatrixB := Matrix new &n:N &name:"B".
                
                console writeLine writeLine:"Output:".

                #var aSum := aMatrixA + aMatrixB.

                ^ aSum literal.
            ]
            "2" ? [
                console write:"Enter the order of matrix: ".
                #var N := console readLine:(Integer new).
                
                #var aMatrixA := Matrix new &n:N &name:"A".
                
                console writeLine.
                
                #var aMatrixB := Matrix new &n:N &name:"B".
                
                console writeLine writeLine:"Output:".

                #var aDiff := aMatrixA - aMatrixB.

                ^ aDiff literal.
            ]
//            "3" ? [
//                console write:"Enter the order of matrix: ".
//                #var N := anInput readLine:(Integer new).
//                
//                #var aMatrixA := newMatrix eval &n:(N int) &name:"A".
//                
//                console writeLine.
//                
//                #var aMatrixB := newMatrix eval &n:(N int) &name:"B".
//                
//                consoleEx writeLine writeLine:"Output:".
//
//                #var aProduct := aMatrixA * aMatrixB.
//
//                ^ aProduct literal.
//            ]
//            "4" ? [
//                console write:"Enter the order of matrix: ".
//                #var N := anInput readLine:(Integer new).
//
//                #var aMatrixA := newMatrix eval &n:(N int) &name:"A".
//                                
//                console writeLine writeLine:"Output:".
//
//                ^ aMatrixA determinant.
//            ]
            "5" ? [
                console write:"Enter the order of matrix: ".
                #var N := console readLine:(Integer new).

                #var identity := Matrix newIdentity &n:N.

                ^ identity literal.
            ]
            "6" ? [
                'program stop.
            ].
    ]
}.
