#define system.
#define system'collections.
#define system'text.
#define extensions'text.
#define extensions'io.

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

    #method Enumerator => theList.

    #method total_words = theTotal.

    #method unique_words = theList Count.
    
    #method <= &&type'enumerable:$self.
}

// --- Program ---

#symbol Program =
[
    consoleEx writeLine:"Enter the text(to stop press enter two times):".

    #var aText := TextBuffer new.
    #var aLine := String new.

    // read until the empty string is not entered
    control do:[ aText += console readLine:aLine += " " ] &until:[ aLine Length == 0 ].

    #var aList := WordList new:(List new).

    enumControl forEachWord:aText &do: &&:anItem [ aList append:anItem ].

    consoleEx writeLine:"There are ":(aList unique_words):" unique words out of ":(aList total_words).
    (aList total_words > 0)
        ? [ consoleEx writeLine:"the list of unique words:":aList. ].

    consoleEx readChar. // wait for any key
].
