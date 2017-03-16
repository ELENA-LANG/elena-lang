#import system.
#import system'routines.
#import extensions.

class GameMaster
{
    object theNumbers.
    object theAttempt.

    constructor new
    [
        // generate secret number
        var aRandomNumbers := (1,2,3,4,5,6,7,8,9) randomize:9.
 
        theNumbers := aRandomNumbers Subarray &index:0 &length:4.
        theAttempt := Integer new:1.
    ]

    ask
    [
        var aRow := console writeLiteral:"Your Guess #":theAttempt:" ?" readLine.
        
        ^ aRow toArray.
    ]

    proceed : aGuess
    [
        var aCows := Integer new:0.
        var aBulls := Integer new:0.

        (aGuess length != 4)
            ? [ aBulls << -1. ]
            ! [
                0 to:3 &doEach: (:i)
                [
                    var ch := aGuess@i.
                    var aNumber := ch literal toInt.
                    
                    // check range
                    ((aNumber > 0)and:(aNumber < 10))
                        ! [ InvalidArgumentException new raise. ].
                    
                    // check duplicates
                    var duplicate := aGuess seek &each: x [ (x == ch)and:[ x equal &reference:ch not ] ].
                    (nil != duplicate) ?
                    [
                        InvalidArgumentException new raise.
                    ].
                    
                    (aNumber == (theNumbers@i))
                        ? [ aBulls += 1. ]
                        ! [
                            (theNumbers ifExists:aNumber)
                                ? [ aCows += 1. ].
                        ].
                ]
                | if &Error: e
                [
                    aBulls set:(-1).
                ].
            ].

        ^ aBulls =>
            -1 ? [ console writeLine:"Not a valid guess.". ^ true. ]
             4 ? [ console writeLine:"Congratulations! You have won!". ^ false. ]
             ! [
                 theAttempt += 1.
                 
                 console writeLine:"Your Score is " : aBulls : " bulls and " : aCows : " cows".
                 
                 ^ true.
             ].
    ]
}

program =
[
    var aGameMaster := GameMaster new.

    [ aGameMaster proceed:(aGameMaster ask) ] doWhile.
    
    console readChar.
].