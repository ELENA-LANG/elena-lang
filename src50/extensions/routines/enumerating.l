import system'io;
import system'text;
import extensions;
import extensions'text;

// --- ReaderEnumerator ---

public class ReaderEnumerator : Enumerator
{
    TextReader theReader;
    object     theLine;

    constructor new(source)
    {
        theReader := source.textreader();
    }

    constructor new(Stream stream, Encoder encoder)
    {
        theReader := StreamReader.new(stream, encoder)
    }        
                        
    constructor load(string path)
    {
        theReader := StreamReader.new(path)
    }
    
    constructor load(string path, Encoder encoder)
    {
        theReader := StreamReader.new(path, encoder)
    }
    
    reset() 
    {
        NotSupportedException.raise()
    }
    
    bool next()
    {
        if(theReader.Available)
        {
            theLine := theReader.readLine();
            
            ^ true
        };        
        
        theReader.close();
        
        ^ false
    }

    enumerable() = theReader;
            
    get Value() = theLine;
}

// --- fileExOp ---

public extension fileExOp : File
{
    forEachLine(f)
        = f.run(ReaderEnumerator.new(self));
}

// --- stringExOp ---

public extension stringExOp
{
    forEachWord(Func1 f)
        = f.run(cast Enumerator(TokenEnumerator.new(self)));
}

// --- printingLn ---

public symbol printingLn = (line){ forward program'output.printLine:line };

//// --- func1Op ---
//
//public extension func1Op : Func1
//{
//    asComparator()
//        = new { bool equal(o) = self(o); };
//}
