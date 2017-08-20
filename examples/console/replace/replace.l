import extensions.
import extensions'text.

// --- Program ---

Program =
[
    var aText := console print:"Enter the text:"; readLineTo(String new).
    var aSearchText := console print:"Enter the phrase to be found:"; readLine.  
    var aReplaceText := console print:"Enter the phrase to replace with:"; readLine.

    var aBM := StringBookmark new:aText.

    while (aBM find:aSearchText)
    [
        aBM delete:aSearchText; insert:aReplaceText.
    ].

    console
        printLine("The resulting text:",aBM);
        readChar. // wait for any key
].
