import extensions;
import system'routines;
 
public program()
{
    var text := new string[]{"---------- Ice and Fire ------------",
                  "",
                  "fire, in end will world the say Some",
                  "ice. in say Some",
                  "desire of tasted I've what From",
                  "fire. favor who those with hold I",
                  "",
                  "... elided paragraph last ...",
                  "",
                  "Frost Robert -----------------------"};
 
    text.forEach::(line)
    {
        line.splitBy(" ").sequenceReverse().forEach::(word)
        {
            Console.print(word," ")
        };
        Console.writeLine()
    }
}