import system'routines;
import system'collections;
import extensions;
import extensions'routines;
import extensions'text;

// --- WordList ---

class WordList
{
   int       _total;
   ArrayList _list;

   constructor()
   {
      _list := new ArrayList();
      _total := 0;
   }

   append(word)
   {
      _total := _total + 1;

         ifnot(_list.ifExists(word))
            { _list.append(word) }
   }

   total_words = _total;

   unique_words = _list.Length;
        
   dispatch() => _list;
}

// --- Program ---

public program()
{
   console.writeLine:"Enter the text(to stop press enter two times):";

   // read until the empty string is not entered
   var text := new StringWriter();
   until(string line := console.readLine(), line.isEmpty())
   {
      text.writeLine(line)
   };
                
   var list := new WordList();

   text.forEachWord:(word){ list.append:word };

   console.printLine("There are ",list.unique_words," unique words out of ",list.total_words);
   if (list.total_words > 0)
      { console.printLine("the list of unique words:",list.ascendant()) };

   console.readChar() // wait for any key
}
