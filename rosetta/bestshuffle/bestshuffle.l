#define system.
#define system'routines.
#define extensions.

// --- shuffle ---

#symbol shuffle = (:aLiteral)
[
    #var aLength := aLiteral length.
    #var aShuffled := convertor toArray:aLiteral.

    control seek &int:0 &int:(aLength-1) &every: (&index:i)
    [
        #var aChar := aLiteral@i.
        (aChar == (aShuffled@i)) ?
        [
            control run &int:0 &int:(aLength-1) &forEvery: (&index:j)
            [
                #var anAltChar := aShuffled@j.
                (anAltChar != aChar)and:[ anAltChar != (aLiteral@i) ]and:[aChar != (aLiteral@j)] ?
                    [
                        aShuffled~indexable exchange:i:j.
                        
                        ^ true.
                    ].
            ].
        ].
        
        ^ false.
    ].

    ^ control run:aShuffled &forEach:(Summing new:(String new)) literal.
].

// --- ShuffleScore ---

#symbol scoreShuffle = (:anOriginal:aShuffled)
[
    #var aScore := Integer new.
    
    control run &int:0 &int:(anOriginal length - 1) &forEvery: (&index:i)
        [ (anOriginal @ i) == (aShuffled @ i) ? [ aScore += 1. ]. ].
    
    ^ aScore value.
].

// --- Program ---

#symbol program =
[
    control run:("abracadabra", "seesaw", "grrrrrr", "pop", "up", "a") &forEach: aWord
    [
        #var aShuffled := shuffle:aWord.

        consoleEx writeLine:"The best shuffle of ":aWord:" is ":aShuffled:"(":(scoreShuffle:aWord:aShuffled):")".
    ].

    console readChar.
].