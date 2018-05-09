// based on http://cboard.cprogramming.com/cplusplus-programming/30001-determinant-calculation.html prelude sample

// --- default namespaces ---
//import system'dynamic.
import extensions.
import extensions'math.

// --- factory ---

class Matrix
{
    T<RealMatrix> theMatrix.
    
    constructor new(RealMatrix aMatrix)
    [
        theMatrix := aMatrix.
    ]
    
    constructor newOf:anN name:aName
        <= new (RealMatrix new(anN,anN); populate(:i:j)<int,int,real>
            [  
                ^ console print("Enter ",aName,"[",i,",",j,"]="); readLineTo(Real new)
            ]).
    
    constructor newIdentityOf:anN
        <= new(RealMatrix new(anN,anN); populate(:i:j)<int,int,real>
            ( (i == j)iif(1.0r,0.0r) )).
    
    add : aMatrix
        = Matrix new(theMatrix add:aMatrix).

    subtract : aMatrix
        = Matrix new(theMatrix subtract:aMatrix).

    multiply : aMatrix
        = Matrix new(theMatrix multiply:aMatrix).
    
    dispatch => theMatrix.
}

// ---- Program ---

public control =
{
    prompt = "Choose an option" + forwards'newLine +
             "[1] - Sum Matrix" + forwards'newLine +
             "[2] - Subtract Matrix" + forwards'newLine +
             "[3] - Multiply Matrix" + forwards'newLine +
             "[4] - Matrix Determinant" + forwards'newLine +
             "[5] - Identity Matrix" + forwards'newLine +
             "[6] - Exit" + forwards'newLine +
             "Your choice:".
    
    proceed : aChoice
    [
        aChoice =>
            "1"
            [
                console write:"Enter the order of matrix: ".
                var N := console readLineTo(Integer new).
                
                var aMatrixA := Matrix newOf:N name:"A".
                
                console writeLine.
                
                var aMatrixB := Matrix newOf:N name:"B".
                
                console writeLine; writeLine:"Output:".

                T<RealMatrix> aSum := aMatrixA + aMatrixB.

                ^ aSum toLiteral
            ];
            "2" [
                console write:"Enter the order of matrix: ".
                var N := console readLineTo(Integer new).
                
                var aMatrixA := Matrix newOf:N name:"A".
                
                console writeLine.
                
                var aMatrixB := Matrix newOf:N name:"B".
                
                console writeLine; writeLine:"Output:".

                T<RealMatrix> aDiff := aMatrixA - aMatrixB.

                ^ aDiff toLiteral
            ];
            "3" [
                console write:"Enter the order of matrix: ".
                var N := console readLineTo(Integer new).
                
                var aMatrixA := Matrix newOf:N name:"A".
                
                console writeLine.
                
                var aMatrixB := Matrix newOf:N name:"B".
                
                console writeLine; writeLine:"Output:".

                T<RealMatrix> aProduct := aMatrixA * aMatrixB.

                ^ aProduct toLiteral
            ];
            "4" [
                console write:"Enter the order of matrix: ".
                var N := console readLineTo(Integer new).

                T<RealMatrix> aMatrixA := Matrix newOf:N name:"A".
                                
                console writeLine; writeLine:"Output:".

                ^ aMatrixA determinant
            ];
            "5" [
                console write:"Enter the order of matrix: ".
                var N := console readLineTo(Integer new).

                T<RealMatrix> identity := Matrix newIdentityOf:N.

                ^ identity toLiteral
            ];
            "6" [
                forwards'program stop
            ].
    ]
}.
