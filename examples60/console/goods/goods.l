import system'io;
import system'routines;
import system'collections;
import extensions;
import extensions'routines;
import extensions'text;

// --- Record ---
class Record
{
    object _name;
    object _price;
    
    constructor(object price, object name)
    {
        _name := name;
        _price := price
    }

    Name = _name;
    
    Price = _price;

    string toPrintable()
        = new StringWriter().print("Name:",_name," Price:",_price);
}

// --- Adapter ---

class Adapter
{
    object _bm;
    
    constructor()
    {
        _bm := new StringBookmark();
    }

    write(s)
    {
        ifnot(0 == _bm.Length)
           { _bm.append:";" };
        
        _bm.append:s
    }

    read(variable)
    {
        if(0 == _bm.Length)
        { 
            ^false 
        }
        else
        {
            _bm.find:";";
            
            _bm.Marked.convertTo(variable);
            
            _bm.mark();
            
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
