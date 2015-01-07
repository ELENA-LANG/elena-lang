// based on http://cboard.cprogramming.com/cplusplus-programming/30001-determinant-calculation.html prelude sample

// --- default namespaces ---
#define system.
#define extensions.
#define extensions'math.

// --- factory ---
#symbol newMatrix = (&n:anN &m:anM &name:aName)
    [ 
        matrixControl new &n:anN &m:anM &everyij: (&index:i &index:j)
        [  
            consoleEx write:"Enter " write:aName write:"[" write:i write:"," write:j write:"]=" readLine:(Real new)
        ]
    ].

#symbol IdentityMatrix = 
{
    new : aCount
        = matrixControl new &n:aCount &m:aCount &everyij: (&index:i &index:j)
        [ (i == j) ? [ 1 ] ! [ 0 ] ].
}.

// ---- Program ---

#symbol control =
{
    prompt : anOutput
    [
        anOutput
            writeLine:"%nChoose an option"
            writeLine:"[1] - Sum Matrix"
            writeLine:"[2] - Subtract Matrix"
            writeLine:"[3] - Multiply Matrix"
            writeLine:"[4] - Matrix Determinant"
            writeLine:"[5] - Identity Matrix"
            writeLine:"[6] - Exit"
            write:"Your choice:".
    ]
    
    read : anInput
    [
        #var aChoice := anInput readLine:(Integer new).
        
        ^ aChoice =>
            1 ? 
            [
                consoleEx write:"Enter the order of matrix: ".
                #var N := anInput readLine:(Integer new).
                
                #var aMatrixA := newMatrix eval &n:N &m:N &name:"A".
                
                consoleEx writeLine.
                
                #var aMatrixB := newMatrix eval &n:N &m:N &name:"B".
                
                consoleEx writeLine writeLine:"Output:".

                #var aSum := matrixControl sumOf:aMatrixA:aMatrixB.

                ^ matrixControl toLiteral:aSum.
            ]
            2 ? [
                consoleEx write:"Enter the order of matrix: ".
                #var N := anInput readLine:(Integer new).
                
                #var aMatrixA := newMatrix eval &n:N &m:N &name:"A".
                
                consoleEx writeLine.
                
                #var aMatrixB := newMatrix eval &n:N &m:N &name:"B".
                
                consoleEx writeLine writeLine:"Output:".

                #var aDiff := matrixControl differenceOf:aMatrixA:aMatrixB.

                ^ matrixControl toLiteral:aDiff.
            ]
            3 ? [
                consoleEx write:"Enter the order of matrix: ".
                #var N := anInput readLine:(Integer new).
                
                #var aMatrixA := newMatrix eval &n:N &m:N &name:"A".
                
                consoleEx writeLine.
                
                #var aMatrixB := newMatrix eval &n:N &m:N &name:"B".
                
                consoleEx writeLine writeLine:"Output:".

                #var aProduct := matrixControl productOf:aMatrixA:aMatrixB.

                ^ matrixControl toLiteral:aProduct.
            ]
            4 ? [
                consoleEx write:"Enter the order of matrix: ".
                #var N := anInput readLine:(Integer new).

                #var aMatrixA := newMatrix eval &n:N &m:N &name:"A".
                                
                consoleEx writeLine writeLine:"Output:".

                ^ matrixControl determinantOf:aMatrixA.
            ]
            5 ? [
                consoleEx write:"Enter the order of matrix: ".
                #var N := anInput readLine:(Integer new).

                #var identity := IdentityMatrix new:N.

                ^ matrixControl toLiteral:identity.
            ]
            6 ? [
                'program stop.
            ].
    ]
}.
