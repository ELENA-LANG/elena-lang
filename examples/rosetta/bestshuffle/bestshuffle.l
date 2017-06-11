/// Shuffles the characters of a string in such a way that as many of the character values are in a different position as possible.

import system'routines.
import extensions.

extension op
{
    shuffled
    [
        var anOriginal := self toArray.
        var aShuffled := self toArray.
    
        0 to:(anOriginal length - 1) doEach: (:i)
        [
            0 to:(anOriginal length - 1) doEach: (:j)
            [
                if ((i != j) && $(anOriginal@i != (aShuffled@j)) && $(anOriginal@j != (aShuffled@i)))
                [
                    aShuffled exchange:i:j
                ].
            ].
        ].
    
        ^ aShuffled summarize:(String new); literal
    ]
    
    score : anOriginalText
    [
        var aShuffled := self toArray.
        var anOriginal := anOriginalText toArray.
        var aScore := Integer new.

        0 to:(anOriginal length - 1) doEach: (:i)
            [ if ((anOriginal @ i) == (aShuffled @ i)) [ aScore append int:1 ] ].
        
        ^ aScore get.
    ]
}

// --- Program ---

program =
[
    ("abracadabra", "seesaw", "grrrrrr", "pop", "up", "a") run each: (:aWord)
    [
        var aShuffled := aWord shuffled.

        console printLine:"The best shuffle of ":aWord:" is ":aShuffled:"(":(aShuffled score:aWord):")".
    ].

    console readChar.
].