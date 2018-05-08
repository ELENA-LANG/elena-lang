// based on msdn random sample

import extensions.

// --- program ---

const int rows = 4.
const int cols = 6.

const int runCount = 1000000.
const int distGroupCount = 20.

public program =
[
    var intGroupSize := (1.0r + IntNumber max) / distGroupCount.

    var intCounts := IntArray new:distGroupCount.
    var realCounts := IntArray new:distGroupCount.

    console printLine:"Random reals:".
    0 till:rows do(:i)
    [
        0 to:cols do(:j)
        [
            console printPaddingRight(12, randomGenerator nextReal).
        ].
        
        console writeLine.
    ].

    console writeLine.
    console writeLine:"Random integers:".
    0 till:rows do(:i)
    [
        0 to:cols do(:j)
        [
            console printPaddingRight(12, randomGenerator nextInt).
        ].
        
        console writeLine.
    ].

    console writeLine.
    console writeLine:"Distribution".

    0 till:runCount do(:i)
    [
        var index := (randomGenerator nextInt / intGroupSize) toInt.
        if(index < distGroupCount)
            [ intCounts[index] := intCounts[index] + 1. ].
        
        index := (randomGenerator nextReal * distGroupCount) toInt.
        if(index < distGroupCount)
            [ realCounts[index] := realCounts[index] + 1. ].
    ].    

    0 till:distGroupCount do(:i)
    [
        console printPaddingLeft(12, (i * intGroupSize) toInt,"-").
//        console printPaddingLeft(12, ((i + 1) * intGroupSize - 1) toInt).
//        console printPaddingLeft(12, intCounts[i]).

//        console printPaddingLeft(14, i real / distGroupCount,"-").
//        console printPaddingLeft(12, (i + 1) real / distGroupCount).
//        console printPaddingLeft(12, realCounts[i]).
                
        console writeLine.
    ].
].
