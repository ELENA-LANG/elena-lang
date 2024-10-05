import extensions;
import extensions'text;

// --- Program ---

public program()
{
    var text := Console.print("Enter the text:").loadLineTo(new StringWriter());
    var searchText := Console.print("Enter the phrase to be found:").readLine();  
    var replaceText := Console.print("Enter the phrase to replace with:").readLine();

    var bm := new StringBookmark(text);

    while (bm.find(searchText))
    {
        bm.delete(searchText).insert(replaceText)
    };

    Console
        .printLine("The resulting text:",bm)
        .readChar() // wait for any key
}
