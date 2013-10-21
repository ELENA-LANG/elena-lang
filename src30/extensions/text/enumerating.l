#define system.
#define system'text.

// --- TextFileEnumerator ---

#class TextFileEnumerator
{
    #field thePath.
    #field theReader.
    #field theLine.
        
    #constructor new &path:aPath
    [
        thePath := aPath type'widestr.        
    ]
    
    #method next
    [
        (nil == theReader) 
            ? [
                theReader := TextReader new &path:thePath.
                
                theLine := theReader readLine.
            ]
            ! [
                theLine := theReader readLine.
            ].
            
        ^ (nil != theLine).
    ]
    
    #method get = theLine.
}

// --- textFileControl ---

#symbol textFileControl =
{
    forEachLine:aPath &do:anAction
        = anAction~EAction iterate &each:(TextFileEnumerator new &path:aPath).
}.

// --- enumControl ---

#symbol enumControl =
{
    forEachWord:aText &do:anAction
        = anAction~EAction iterate &each:(WordEnumerator new:(aText Literal)).
}.