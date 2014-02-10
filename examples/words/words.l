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
        theTotal := Integer new::0.
    ]

    #method append : aWord
    [
        theTotal += 1.

        listControl ifExist:aWord &in:theList
            ! [ theList append:aWord. ].
    ]

    #method enumerator => theList.

    #method total_words = theTotal.

    #method unique_words = theList length.
    
    #method <= &enumerable:$self.
}

// --- Program ---

#symbol Program =
[
    consoleEx writeLine:"Enter the text(to stop press enter two times):".

    #var aText := TextBuffer new.
    #var aLine := String new.

    // read until the empty string is not entered
    control do:[ aText << consoleEx readLine:aLine << " " ] &until:[ aLine length == 0 ].

    #var aList := WordList new:(List new).

    enumControl forEachWord:aText &do: anItem [ aList append:anItem ].

    consoleEx writeLine:"There are ":(aList unique_words):" unique words out of ":(aList total_words).
    (aList total_words > 0)
        ? [ consoleEx writeLine:"the list of unique words:":(listControl sort:aList). ].

    consoleEx readChar. // wait for any key
].
