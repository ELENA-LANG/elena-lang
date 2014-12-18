#define system.
#define system'routines.
#define system'io.

#type(type:func1)forEachWord.
#type(type:func1)forEachLine.

// --- ReaderEnumerator ---

#class ReaderEnumerator : Enumerator
{
    #field(type:textreader) theReader.
    #field theLine.

    #constructor new
    [
    ]

    #constructor new &stream:aStream
        <= (%new)
    [
        theReader := StreamReader new &stream:aStream.
    ]        
                        
    #constructor new &path:aPath
        <= (%new)
    [
        theReader := StreamReader new &path:aPath.
    ]
    
    #method reset 
    [
        theReader := StreamReader new &stream::(theReader source clone).
    ]
    
    #method next
    [
        theLine := theReader readLine.
        
        ^ nil != theLine.
    ]
    
    #method get = theLine.
}

//// --- textFileControl ---
//
//#symbol textFileControl =
//{
//    forEachLine:aPath &do:anAction
//        = anAction~functionOp run &enumerator:(TextFileEnumerator new &path:aPath).
//}.

// --- textControlEx ---

#class(extension:control) textControlEx 
{
    #method run:aText &forEachWord:anAction
        = anAction run &enumerator:(WordEnumerator new &literal::aText).
        
    #method run &path:aPath &forEachLine:anAction
        = anAction run &enumerator:(ReaderEnumerator new &path:aPath).
}
