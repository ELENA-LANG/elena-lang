#define system.
#define system'io.

// --- TextFileEnumerator ---

#class TextFileEnumerator
{
    #field thePath.
    #field theReader.
    #field theLine.
        
    #constructor new &path:aPath
    [
        thePath := aPath literal.        
    ]
    
    #method next
    [
        (nil == theReader) 
            ? [
                theReader := StreamReader new &path:thePath.
                
                theLine := theReader readLine.
            ]
            ! [
                theLine := theReader readLine.
            ].
            
        ^ theReader available.
    ]
    
    #method get = theLine.
}

// --- textFileControl ---

#symbol textFileControl =
{
    forEachLine:aPath &do:anAction
        = anAction~functionOp run &enumerator:(TextFileEnumerator new &path:aPath).
}.

// --- textControl ---

#symbol textControl =
{
    forEachWord:aText &do:anAction
        = anAction~functionOp run &enumerator:(WordEnumerator new:(aText literal)).
}.