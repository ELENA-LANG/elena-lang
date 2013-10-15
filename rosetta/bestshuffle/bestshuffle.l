#define std'dictionary'*.
#define std'basic'*.
#define std'patterns'*.
#define std'routines'*.

#subject original, shuffled.

// --- BestShuffle ---

#symbol BestShuffle : aLiteral =
[
    #var aLength := aLiteral length.
    #var aShuffled := aLiteral clone.

    It::aShuffled run: aChar =
    [
        #if (aChar == aLiteral @ aChar index)?
        [
            #if It::aShuffled seek: anAltChar =
            [
                #if Control if:(aChar != anAltChar) if:(anAltChar != aLiteral @ aChar index) if:(aChar != aLiteral @ anAltChar index)
                [
                    anAltChar~EContent exchange:aChar.

                    ^ true.
                ].
            ].
        ].
    ].

    ^ aShuffled.
].

// --- ShuffleScore ---

#symbol ShuffleScore &original:anOriginal &shuffled:aShuffled =
[
    #var aScore := Integer.
    
    It::aShuffled run: aChar = [ #if aChar ifequal:(anOriginal @ aChar index) [ aScore append:1. ]. ].
    
    ^ aScore.
].

// --- Program ---

#symbol Program =
[
    Scan::("abracadabra", "seesaw", "grrrrrr", "pop", "up", "a") run: aWord =
    [
        #var aShuffled := BestShuffle::aWord.
        
        'program'output << "The best shuffle of " << aWord << " is " << aShuffled << "(" 
            << (ShuffleScore &&original:aWord &shuffled:aShuffled) << ")%n".
    ].
    
    'program'input get.
].