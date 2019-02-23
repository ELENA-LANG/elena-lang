////import extensions.

// --- StringBookmark ---

public class StringBookmark : BaseValue
{
    string theValue;
    int    theIndex;
    int    theStart;
    
    constructor()
    {
        theValue := emptyString;
        theIndex := 0;
        theStart := 0
    }

    constructor(text)
    {
        theValue := text;
        theIndex := 0;
        theStart := 0
    }

    bool equal(int value)
        => theIndex;

    bool equal(string value)
        => theValue;

    bool equal(wide value)
        => theValue;

    find(object)
        <= find(cast int(object));

    append(object)
        <= append(cast int(object));

    insert(object)
        <= insert(object.Printable);

    delete(object)
        <= delete(cast int(object));

    find(int n)
    {
        theIndex := n
    }

    find(string s)
    {
        theIndex := theValue.indexOf(theIndex, s);
        
        ^ (-1 != theIndex)
    }
    
    find(wide s)
    {
        theIndex := theValue.indexOf(theIndex, s);
        
        ^ (-1 != theIndex)
    }
    
    mark()
    {
        if(-1 == theIndex)
        {
            self.clear()
        }
        else
        {
            theIndex := 1 + theIndex;
            theStart := theIndex
        }        
    }
    
    append(string s)
    {
        theValue := theValue + s
    }
    
    append(wide s)
    {
        theValue := theValue + s
    }
    
    append(int n)
    {
        theIndex := theIndex + n
    }
    
    insert(string s)
    {
        theValue := theValue.insert(theIndex, s);
        
        theIndex := theIndex + s.Length
    }
    
    insert(wide s)
    {
        theValue := theValue.insert(theIndex, s);
        
        theIndex := theIndex + s.Length
    }
    
    delete(int len)
    {
        theValue := theValue.delete(theIndex, len)
    }

    delete(string s)
        <= delete(s.Length);

    delete(wide s)
        <= delete(s.Length);

    clear()
    {
        theValue := emptyString;
        theIndex := 0;
        theStart := 0
    }

    get Marked()
    {
        if(-1 == theIndex)
        {
            ^ theValue.Substring:theStart
        }
        else
        {
            ^ theValue.Substring(theStart, theIndex - theStart)           
        }
    }
    
    get string Printable() => theValue;
    
    string cast() = cast string(theValue);
    
    wide cast() = cast wide(theValue);
    
    int cast() = cast int(theIndex);
    
    get Length() => theValue;
    
    get Index() = theIndex;
}

// --- StringWriter ---

public sealed class StringWriter : BaseVariable
{    
    string theValue;
    string theNewLine;
    
    constructor()
    {
        theValue := emptyString;
        theNewLine := forward newLine;
    }
    
//    constructor new : aText
//    [
//        theValue := aText.
//    ]
//
//    constructor new(literal s, int count)
//    [
//        theValue := emptyLiteral.
//        
//        0 till:count do(:i)[ self append(s) ].
//    ]
    
    write(string s)
    {
        theValue := theValue.add:s
    }
    
    write(char ch)
    {
        theValue := theValue.add:ch
    }
    
    writeLine(string s)
    {
        self.write(s).write(theNewLine)
    }
   
    writeLine()
    {
        self.write(theNewLine)
    }    

    append(string s)
        <= write(s); 
    
    append(char ch)
        <= write(ch); 
    
//    insert:s at:index
//    [
//        theValue := theValue insert:s at:index.
//    ]
//
//    replace(literal s, literal s2)
//    [
//        theValue := theValue replace(s, s2).
//    ]
//    
//    replace(object s, object s2)
//    [
//        theValue := theValue replace(s literal, s2 literal).
//    ]
    
    clear()
    {
        theValue := emptyString
    }

    string Printable = theValue;

    string cast() = theValue;

    prop string Value
    {
        get() = theValue;
        
        set(string val)
        {
            theValue := val
        }
    }

//    cast T<wide> = theValue.

    dispatch() => theValue;
}
