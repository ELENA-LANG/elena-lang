#define system.
#define system'collections.
#define system'text.
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

    #method Enumerator => theList.

    #method total_words = theTotal.

    #method unique_words = theList Count.
}

// --- Program ---

#symbol Program =
[
    console writeLine:"Enter the text(to stop press enter two times):".

    #var aText := TextBuffer new.
    #var aLine := String new.

    // read until the empty string is not entered
    control do:[ aText += console readLine:aLine += " " ] &until:[ aLine Length == 0 ].

    #var aList := WordList new:(List new).

    enumControl forEachWord:aText &do: &&:anItem [ aList append:anItem ].

    console write:"There are " write:(aList unique_words) write:" unique words out of " writeLine:(aList total_words).
    (aList total_words > 0)
        ? [ console write:"the list of unique words:" writeLine:(ListPresenter new:aList) ].

    console readChar. // wait for any key
].
