// based on msdn random sample

import extensions.

// --- program ---

const int rows = 4.
const int cols = 6.

const int runCount = 1000000.
const int distGroupCount = 20.

program =
[
    var intGroupSize := (1.0r + IntNumber max) / distGroupCount.

    var intCounts := IntArray new:distGroupCount.
    var realCounts := IntArray new:distGroupCount.

    console writeLine:"Random reals:".
    0 till:rows doEach: (:i)
    [
        0 to:cols doEach: (:j)
        [
            console print:(randomGenerator nextReal) paddingRight:12.
        ].
        
        console writeLine.
    ].

    console writeLine.
    console writeLine:"Random integers:".
    0 till:rows doEach: (:i)
    [
        0 to:cols doEach: (:j)
        [
            console print:(randomGenerator nextInt) paddingRight:12.
        ].
        
        console writeLine.
    ].

    console writeLine.
    console writeLine:"Distribution".

    0 till:runCount doEach: (:i)
    [
        var index := (randomGenerator nextInt / intGroupSize) toInt.
        if(index < distGroupCount)
            [ intCounts@index := intCounts@index + 1. ].
        
        index := (randomGenerator nextReal * distGroupCount) toInt.
        if(index < distGroupCount)
            [ realCounts@index := realCounts@index + 1. ].
    ].    

    0 till:distGroupCount doEach: (:i)
    [
        console print:((i * intGroupSize) toInt; literal):"-" paddingLeft:12.
        console print:(((i + 1) * intGroupSize - 1) toInt; literal) paddingLeft:12.
        console print:(intCounts@i literal) paddingLeft:12.

        console print:((i real / distGroupCount) literal):"-" paddingLeft:14.
        console print:(((i + 1) real / distGroupCount) literal) paddingLeft:12.
        console print:(realCounts@i literal) paddingLeft:12.
                
        console writeLine.
    ].
].
