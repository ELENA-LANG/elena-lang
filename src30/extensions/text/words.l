#define system.
#define system'text.

// --- WordEnumerator ---

#class WordEnumerator
{
    #field theIndexer.
    #field theRole.
    #field theWord.
    
    #constructor new : aLiteral
    [
        theIndexer := aLiteral enumerator set &index:0.
        theWord := TextBuffer new.
    ]    
    
    #method next &space
        = (theIndexer get == 32)
            ? [ theIndexer next ]
            ! [ theRole := %word. ^ true. ].
    
    #method next &word
    [
        #var aChar := theIndexer get.
        
        ^ (aChar == 32)
            ? [ false ]
            ! [ theWord write &char:aChar. ^ theIndexer next. ].
    ]
    
    #method next
    [
        theWord clear.

        (theIndexer available)
            ? [
                theRole := %space.
        
                #loop ($self~theRole next) ? [].
            ].

        ^ 0 != theWord length.
    ]
    
    #method get = theWord literal.
}

