#define system.
#define system'routines.
#define system'collections.
#define system'text.
#define extensions.
#define extensions'routines.

// --- WordList ---

#class WordList
{
    #field theTotal.
    #field theList.

    #constructor new
    [
        theList := ArrayList new.
        theTotal := Integer new &int:0.
    ]

    #method append : aWord
    [
        theTotal += 1.

        (theList ifExists:aWord)
            ! [ theList append:aWord. ].
    ]

    #method total_words = theTotal.

    #method unique_words = theList length.

    #method cast : aVerb &to:aTarget
        => theList.
    
    #method cast : aVerb &to:aTarget &with:aParam
        => theList.
        
    #method => theList.
}

// --- Program ---

#symbol Program =
[
    console writeLine:"Enter the text(to stop press enter two times):".

    #var aText := TextBuffer new.
    #var aLine := String new.

    // read until the empty string is not entered
    [
        aText << console readLine:aLine << " ".
    ]
    run &until:[ aLine length == 0 ].

    #var aList := WordList new.

    aText run &eachWord:anItem [ aList append:anItem ].

    console writeLine:"There are ":(aList unique_words):" unique words out of ":(aList total_words).
    (aList total_words > 0)
        ? [ console writeLine:"the list of unique words:":(aList ascendant). ].

    console readChar. // wait for any key
].
