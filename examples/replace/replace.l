#define system.
#define extensions.
#define extensions'text.

// --- Program ---

#symbol Program =
[
    #var aText := console write:"Enter the text:" readLine:(String new).
    #var aSearchText := console write:"Enter the phrase to be found:" readLine.  
    #var aReplaceText := console write:"Enter the phrase to replace with:" readLine.

    #var aBM := StringBookmark new:aText.

    #loop (aBM find:aSearchText)?
    [
        aBM delete:aSearchText insert:aReplaceText.
    ].

    console
        writeLine:"The resulting text:":aBM
        readChar. // wait for any key
].
