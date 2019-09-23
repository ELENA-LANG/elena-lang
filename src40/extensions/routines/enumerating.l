import system'io;
//import system'text.
import extensions;
import extensions'text;

// --- ReaderEnumerator ---

public class ReaderEnumerator : Enumerator
{
    TextReader theReader;
    object     theLine;

//    constructor()
//    { 
//    }

    constructor new(source)
  //      <= new
    {
        theReader := source.textreader();
    }

//    constructor new(Stream aStream, Encoder anEncoder)
//        <= new;
//    [
//        theReader := StreamReader new(aStream, anEncoder).
//    ]        
//                        
//    constructor new(literal aPath)
//        <= new;
//    [
//        theReader := StreamReader new:aPath.
//    ]
//    
//    constructor new(literal aPath, Encoder anEncoder)
//        <= new;
//    [
//        theReader := StreamReader new(aPath, anEncoder)
//    ]
    
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
            
    get() = theLine;
}

// --- fileExOp ---

public extension fileExOp : File
{
    forEachLine(f)
        = f.run(ReaderEnumerator.new(self));
        
//    run for:anEncoder eachLine:anAction
//        = anAction run enumerator:(ReaderEnumerator new path:self encoder:anEncoder).
}

// --- stringExOp ---

public extension stringExOp
{
    forEachWord(Func1 f)
        = f.run(cast Enumerator(TokenEnumerator.new(self)));
        
////    run token_dfa:aStatemachine each:aFunction
////        = aFunction run enumerator:(TokenEnumerator new literal:self token_dfa:aStatemachine).
}

// --- printingLn ---

public symbol printingLn = (aLine){ forward program'output.printLine:aLine };

// --- func1Op ---

public extension func1Op : Func1
{
    asComparator()
        = new::{ bool equal(o) = self(o); };
}
