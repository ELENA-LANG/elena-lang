#define system.
#define system'routines.

// --- shuffle ---

#symbol shuffle = (:aLiteral)
[
    #var aLength := aLiteral length.
    #var aShuffled := literalControl toArray:aLiteral.

    control forrange &int:0 &int:(aLength-1) &do: (&int:i)
    [
        #var aChar := aLiteral@i.
        (aChar == (aShuffled@i)) ?
        [
            control forrange &int:0 &int:(aLength-1) &do: (&int:j)
            [
                #var anAltChar := aShuffled@j.
                (anAltChar != aChar)and:[ anAltChar != (aLiteral@i) ]and:[aChar != (aLiteral@j)] ?
                    [
                        arrayControl exchange:i &with:j &in:aShuffled.
                        
                        #break.
                    ].
            ].
        ].
    ].

    ^ Summing new:(String new) foreach:aShuffled literal.
].

// --- ShuffleScore ---

#symbol scoreShuffle = (:anOriginal:aShuffled)
[
    #var aScore := Integer new.
    
    control forrange &int:0 &int:(anOriginal length - 1) &do: (&int:i)
        [ (anOriginal @ i) == (aShuffled @ i) ? [ aScore += 1. ]. ].
    
    ^ aScore number.
].

// --- Program ---

#symbol program =
[
    control foreach:("abracadabra", "seesaw", "grrrrrr", "pop", "up", "a") &do: aWord
    [
        #var aShuffled := shuffle:aWord.

        console write:"The best shuffle of " write:aWord write:" is " write:aShuffled write:"(" 
            write:(scoreShuffle:aWord:aShuffled) writeLine:")".
    ].

    console readChar.
].