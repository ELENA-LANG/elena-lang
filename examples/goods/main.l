#define system.
#define system'collections.
#define system'text.
#define extensions'text.

// --- Record ---
#class Record
{
    #field theName.
    #field thePrice.

    #constructor new &Price:aPrice &Name:aName
    [
        theName := aName.
        thePrice := aPrice.
    ]

    #method Name = theName.
    
    #method Price = thePrice.

    #method Literal
    [
        ^ "Name:" + theName + " Price:" + thePrice Literal.
    ]
}

// --- Adapter ---

#class Adapter
{
    #field theBM.
    
    #constructor new
    [
        theBM := Bookmark new.
    ]

    #method write : aLiteral
    [
        (0 == theBM Length)
            ! [ theBM += ";". ].
        
        theBM += aLiteral.
    ]

    #method read : aVariable
        = (0 == theBM Length)
            ? [ false ]
            ! [
                theBM find &type'widestr:";".
                
                aVariable << Variant new:(theBM Marked).
                
                theBM mark.
                
                ^ true.
            ].
}

// --- Program --- 

#symbol Program = 
[
    ('program'arguments Count == 1)?
        [  console << "Please provide the path to the file to view". #throw BreakException new. ].

    #var aFileName := 'program'arguments@1.

    #var aList := List new.
    #var anAdapter := Adapter new.

    textFileControl forEachLine:aFileName &do: &&:aLine
    [
        anAdapter write:aLine.

        #var aName := String new.
        #var aPrice := Real new.
                        
        control while:(anAdapter read:aName) &do:
        [
            anAdapter read:aPrice.
        
            aList append:(Record new &Price:(aPrice Number) &Name:(aName Literal)).
        ].
    ].

    aList~EList run:printingLn.

    console readChar.
].
