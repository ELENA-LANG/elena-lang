#define system.
#define system'routines.
#define system'collections.
#define system'text.
#define extensions.
#define extensions'text.

// --- WordList ---

#class WordList
{
    #field theTotal.
    #field theList.

    #constructor new : aList
    [
        theList := aList.
        theTotal := Integer new &int:0.
    ]

    #method append : aWord
    [
        theTotal += 1.

        control ifExist:aWord &in:theList
            ! [ theList append:aWord. ].
    ]

    #method total_words = theTotal.

    #method unique_words = theList length.
    
    #method => theList.
}

// --- Program ---

#symbol Program =
[
    console writeLine:"Enter the text(to stop press enter two times):".

    #var aText := TextBuffer new.
    #var aLine := String new.

    // read until the empty string is not entered
    control do:[ aText << consoleEx readLine:aLine << " ". ] &until:[ aLine length == 0 ].

    #var aList := WordList new:(List new).

    control run:aText &forEachWord: anItem [ aList append:anItem ].

    consoleEx writeLine:"There are ":(aList unique_words):" unique words out of ":(aList total_words).
    (aList total_words > 0)
        ? [ consoleEx writeLine:"the list of unique words:":(aList~indexable sort:ifOrdered). ].

    console readChar. // wait for any key
].
