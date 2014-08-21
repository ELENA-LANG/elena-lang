#define system.
#define extensions.
#define extensions'text.

// --- Program ---

#symbol Program =
[
    #var aText := consoleEx write:"Enter the text:" readLine:(String new:"1223").
    #var aSearchText := consoleEx write:"Enter the phrase to be found:" readLine.  
    #var aReplaceText := consoleEx write:"Enter the phrase to replace with:" readLine.

    #var aBM := Bookmark new::aText.

    control while:[ aBM find::aSearchText ] &do:
    [
        aBM delete &length::aSearchText insert::aReplaceText.
    ].

    consoleEx 
        writeLine:"The resulting text:":aBM
        readChar. // wait for any key
].
