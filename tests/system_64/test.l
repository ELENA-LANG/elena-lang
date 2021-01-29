import system'routines;
import system'collections;
import extensions;
import extensions'routines;
import extensions'text;

class WordList
{
    int       theTotal;
    ArrayList theList;

    constructor()
    {
        theList := new ArrayList();
        theTotal := 0;
    }

    append(word)
    {
        theTotal := theTotal + 1;

        ifnot(theList.ifExists(word))
            { theList.append(word) }
    }

    total_words = theTotal;

    unique_words = theList.Length;
        
    dispatch() => theList;
}

public program()
{
    var text := "a b c";
    
    var list := new WordList();

    text.forEachWord:(word){ list.append:word };
    
    console.printLine("There are ",list.unique_words," unique words out of ",list.total_words);
    if (list.total_words > 0)
        { console.printLine("the list of unique words:",list.ascendant()) };
    
}
