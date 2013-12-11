#define system.
#define system'text.
#define extensions.

// --- Program ---

#symbol Program =
[
    #var aText := consoleEx write:"Enter the text:" readLine:(String new).
    #var aSearchText := consoleEx write:"Enter the phrase to be found:" readLine.  
    #var aReplaceText := consoleEx write:"Enter the phrase to replace with:" readLine.

    #var aBM := Bookmark new::aText.

    control while:[ aBM find::aSearchText ] &do:
    [
        aBM delete &type'length::aSearchText insert::aReplaceText.
    ].

    console write:"The resulting text:" writeLine:(aBM Literal).

    console readChar. // wait for any key
].
