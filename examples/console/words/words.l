import system'routines.
import system'collections.
import extensions.
import extensions'routines.
import extensions'text.

// --- WordList ---

class WordList
{
    object theTotal.
    object theList.

    constructor new
    [
        theList := ArrayList new.
        theTotal := Integer new int:0.
    ]

    append : aWord
    [
        theTotal append:1.

        ifnot (theList ifExists:aWord)
            [ theList append:aWord ]
    ]

    total_words = theTotal.

    unique_words = theList length.

    cast : aVerb to:aTarget
        => theList.
    
    cast : aVerb to:aTarget with:aParam
        => theList.
        
    dispatch => theList.
}

extension helpOp
{
    saveLine to:target
    [
        target write:self; write:'newLine.
        
        ^ self.
    ]
}

// --- Program ---

Program =
[
    console writeLine:"Enter the text(to stop press enter two times):".

    var aText := String new.

    // read until the empty string is not entered
    control while:$( console readLine; saveLine to:aText; length != 0 ).

    var aList := WordList new.

    aText run eachWord: (:anItem)[ aList append:anItem ].

    console writeLine:"There are ":(aList unique_words):" unique words out of ":(aList total_words).
    if (aList total_words > 0)
        [ console writeLine:"the list of unique words:":(aList ascendant) ].

    console readChar // wait for any key
].
