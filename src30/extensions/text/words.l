#define system.
#define system'text.

// --- WordEnumerator ---

#symbol CSpaceMode =
{
    $nextChar
        => %$nextSpace.
}.

#symbol CWordMode =
{
    $nextChar
        => %$nextWord.
}.

#class WordEnumerator
{
    #field theIndexer.
    #field theRole.
    #field theWord.
    
    #constructor new : aLiteral
    [
        theIndexer := aLiteral Enumerator write &Index:0.
        theWord := TextBuffer new.
    ]    
    
    #method $nextSpace
        = (theIndexer get == 32)
            ? [ theIndexer next ]
            ! [ theRole := CWordMode. ^ true. ].
    
    #method $nextWord
    [
        #var aChar := theIndexer get.
        
        ^ (aChar == 32)
            ? [ false ]
            ! [ theWord append &type'widechar:aChar. ^ theIndexer next. ].
    ]
    
    #method next
    [
        theWord clear.

        (theIndexer EOF)        
            ! [
                theRole := CSpaceMode.
        
                control while:[ $self~theRole $nextChar ] &do:[].
            ].
        
        ^ 0 != theWord Length.
    ]
    
    #method get = theWord Literal.
}
