import system'io;
import system'routines;
import system'collections;
import extensions;
import extensions'routines;
import extensions'text;

// --- Record ---
class Record
{
    object theName;
    object thePrice;
    
    constructor(object price, object name)
    {
        theName := name;
        thePrice := price
    }

    Name = theName;
    
    Price = thePrice;

    string toPrintable()
        = new StringWriter().print("Name:",theName," Price:",thePrice);
}

// --- Adapter ---

class Adapter
{
    object theBM;
    
    constructor()
    {
        theBM := new StringBookmark();
    }

    write(s)
    {
        ifnot(0 == theBM.Length)
           { theBM.append:";" };
        
        theBM.append:s
    }

    read(variable)
    {
        if(0 == theBM.Length)
        { 
            ^false 
        }
        else
        {
            theBM.find:";";
            
            theBM.Marked.convertTo(variable);
            
            theBM.mark();
            
            ^ true
        }
    }
}

// --- Program --- 

public program() 
{
    if(program_arguments.Length == 1)
        {  console.printLine("Please provide the path to the file to view"); AbortException.raise() };

    var list := new List();
    var adapter := new Adapter();

    File.assign(program_arguments[1]).forEachLine:(line)
    {
        adapter.write:line;

        var name := new StringWriter();
        var price := Real.new();
                        
        while (adapter.read(name))
        {
            adapter.read(price);
        
            list.append(new Record(price.Value, name.Value))
        }
    };

    list.forEach:printingLn;

    console.readChar()
}
