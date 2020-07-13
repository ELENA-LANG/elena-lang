import extensions;
import extensions'text;

// --- Program ---

public program()
{
    var text := console.print:"Enter the text:".loadLineTo(new StringWriter());
    var searchText := console.print:"Enter the phrase to be found:".readLine();  
    var replaceText := console.print:"Enter the phrase to replace with:".readLine();

    var bm := new StringBookmark(text);

    while (bm.find(searchText))
    {
        bm.delete(searchText).insert(replaceText)
    };

    console
        .printLine("The resulting text:",bm)
        .readChar() // wait for any key
}
