#import system.

// --- StringBookmark ---

#class StringBookmark :: BaseValue
{
    #field theValue.
    #field theIndex.
    #field theStart.
    
    #constructor new
    [
        theValue := emptyLiteralValue.
        theIndex := 0.
        theStart := 0.
    ]

    #constructor new : aText
    [
        theValue := aText.
        theIndex := 0.
        theStart := 0.
    ]

    #method equal &int:aValue
        => theIndex.

    #method equal &literal:aValue
        => theValue.

    #method equal &wide:aValue
        => theValue.

    #method find : anObject
        = anObject cast:%eval &to:
        {
            eval:aBookmark &int : anIndex = aBookmark find &index:anIndex.
            
            eval:aBookmark &literal : aLiteral = aBookmark find &value:aLiteral.
            
            eval:aBookmark &wide : aLiteral = aBookmark find &value:aLiteral.
        } &with:self.

    #method append : anObject
        = anObject cast:%eval &to:
        {
            eval:aBookmark &int : anIndex = aBookmark append &index:anIndex.
            
            eval:aBookmark &literal : aLiteral = aBookmark append &value:aLiteral.
            
            eval:aBookmark &wide : aLiteral = aBookmark append &value:aLiteral.
            
        } &with:self.

    #method insert : anObject
        = anObject cast:%eval &to:
        {
            eval:aBookmark &literal : aLiteral = aBookmark insert &value:aLiteral.
            
            eval:aBookmark &wide : aLiteral = aBookmark insert &value:aLiteral.
            
        } &with:self.

    #method delete : anObject
        = anObject cast:%eval &to:
        {
            eval:aBookmark &int : aValue = aBookmark delete &length:aValue.
            
            eval:aBookmark &literal : aLiteral = aBookmark delete &length:(aLiteral length).
            
            eval:aBookmark &wide : aLiteral = aBookmark delete &length:(aLiteral length).
            
        } &with:self.

    #method find &index:anIndex
    [
        theIndex := anIndex.
    ]

    #method find &value:aValue
    [
        theIndex := theValue indexOf:aValue &at:theIndex.
        
        ^ (-1 != theIndex).
    ]
    
    #method mark
    [
        (-1 == theIndex)
            ? [
                $self clear.
            ]
            ! [
                theIndex := 1 + theIndex.
                theStart := theIndex.
            ].
        
    ]
    
    #method append &value:aValue 
    [
        theValue := theValue + aValue.
    ]
    
    #method append &index:aValue 
    [
        theIndex := theIndex + aValue.
    ]
    
    #method insert &value:aValue
    [
        theValue := theValue insert:aValue &at:theIndex.
        
        theIndex := theIndex + aValue length.
    ]
    
    #method delete &length:aLength
    [
        theValue := theValue delete:aLength &at:theIndex.
    ]

    #method clear
    [
        theValue := emptyLiteralValue.
        theIndex := 0.
        theStart := 0.
    ]

    #method Marked
        = (-1 == theIndex)
            ? [ theValue Substring:theStart ]
            ! [ theValue Substring:(theIndex - theStart) &at:theStart ].
    
    #method literal => theValue.
    
    #method wide => theValue.
    
    #method length => theValue.
    
    #method index = theIndex int.
}

// --- StringWriter ---

#class StringWriter
{    
    #field(literal) theValue.
    
    #constructor new
    [
        theValue := emptyLiteralValue.
    ]
    
//    #constructor new : aText
//    [
//        theValue := aText.
//    ]
    
    #method write : aText
    [
        theValue := theValue add:aText literal.
    ]
    
    #method writeLine : aText
    [
        $self write:aText write:'newLine.
    ]
   
    #method writeLine
    [
        $self write:'newLine.
    ]    

    #method set : aText
    [
        theValue := aText literal.
    ]

    #method get = theValue.

    #method clear
    [
        theValue := emptyLiteralValue.
    ]

    #method wide => theValue.

    /// Dispatches the verb to the target object
    #method cast : aVerb &to:aTarget => theValue.
    
    /// Dispatches the verb to the target object
    #method cast : aVerb &to:aTarget &with:aParam => theValue.

    #method => theValue.
}

/// obsolete
#symbol String = StringWriter.
