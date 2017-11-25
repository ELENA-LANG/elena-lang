// based on http://cboard.cprogramming.com/cplusplus-programming/30001-determinant-calculation.html prelude sample

// --- default namespaces ---
import system'dynamic.
import extensions.
import extensions'math.

// --- factory ---

class Matrix
{
    realmatrix theMatrix.
    
    constructor new realmatrix:aMatrix
    [
        theMatrix := aMatrix.
    ]
    
    constructor new n:anN name:aName
        <= new realmatrix(RealMatrix new(anN,anN); populate(:i:j)<int,int>
            [  
                ^ console print("Enter ",aName,"[",i,",",j,"]="); readLineTo(Real new)
            ]).
    
    constructor newIdentity n:anN
        <= new realmatrix(RealMatrix new(anN,anN); populate(:i:j)<int,int>
            ( (i == j)iif(1,0) )).
    
    add : aMatrix
        = Matrix new realmatrix(theMatrix add:aMatrix).

    subtract : aMatrix
        = Matrix new realmatrix(theMatrix subtract:aMatrix).

    multiply : aMatrix
        = Matrix new realmatrix(theMatrix multiply:aMatrix).
    
    dispatch => theMatrix.
}

// ---- Program ---

control =
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
        aChoice =>
            "1"
            [
                console write:"Enter the order of matrix: ".
                var N := console readLineTo(Integer new).
                
                var aMatrixA := Matrix new n:N name:"A".
                
                console writeLine.
                
                var aMatrixB := Matrix new n:N name:"B".
                
                console writeLine; writeLine:"Output:".

                realmatrix aSum := aMatrixA + aMatrixB.

                ^ aSum literal
            ];
            "2" [
                console write:"Enter the order of matrix: ".
                var N := console readLineTo(Integer new).
                
                var aMatrixA := Matrix new n:N name:"A".
                
                console writeLine.
                
                var aMatrixB := Matrix new n:N name:"B".
                
                console writeLine; writeLine:"Output:".

                realmatrix aDiff := aMatrixA - aMatrixB.

                ^ aDiff literal
            ];
            "3" [
                console write:"Enter the order of matrix: ".
                var N := console readLineTo(Integer new).
                
                var aMatrixA := Matrix new n:N name:"A".
                
                console writeLine.
                
                var aMatrixB := Matrix new n:N name:"B".
                
                console writeLine; writeLine:"Output:".

                realmatrix aProduct := aMatrixA * aMatrixB.

                ^ aProduct literal
            ];
            "4" [
                console write:"Enter the order of matrix: ".
                var N := console readLineTo(Integer new).

                realmatrix aMatrixA := Matrix new n:N name:"A".
                                
                console writeLine; writeLine:"Output:".

                ^ aMatrixA determinant
            ];
            "5" [
                console write:"Enter the order of matrix: ".
                var N := console readLineTo(Integer new).

                realmatrix identity := Matrix newIdentity n:N.

                ^ identity literal
            ];
            "6" [
                'program stop
            ].
    ]
}.
