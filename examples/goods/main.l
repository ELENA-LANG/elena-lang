#define system.
#define system'routines.
#define system'collections.
#define system'text.
#define extensions.
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

    #method literal
    [
        ^ ("Name:" + theName + " Price:" + thePrice literal) literal.
    ]
}

// --- Adapter ---

#class Adapter
{
    #field theBM.
    
    #constructor new
    [
        theBM := StringBookmark new.
    ]

    #method write : aLiteral
    [
        (0 == theBM length)
            ! [ theBM += ";". ].
        
        theBM += aLiteral.
    ]

    #method read : aVariable
        = (0 == theBM length)
            ? [ false ]
            ! [
                theBM find &literal:";".
                
                convertor save:(theBM Marked) &to:aVariable.
                
                theBM mark.
                
                ^ true.
            ].
}

// --- Program --- 

#symbol Program = 
[
    ('program'arguments length == 1)?
        [  console << "Please provide the path to the file to view". #throw AbortException new. ].

    #var aFileName := 'program'arguments@1.

    #var aList := List new.
    #var anAdapter := Adapter new.

    control run &path:(aFileName literal) &forEachLine: aLine
    [
        anAdapter write:aLine.

        #var aName := String new.
        #var aPrice := Real new.
                        
        control while:[ anAdapter read:aName ] &do:
        [
            anAdapter read:aPrice.
        
            aList append:(Record new &Price:(aPrice value) &Name:(aName value)).
        ].
    ].

    control run:aList &forEach:printingLn.

    console readChar.
].
