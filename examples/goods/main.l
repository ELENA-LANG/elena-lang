#define system.
#define system'io.
#define system'routines.
#define system'collections.
#define extensions.
#define extensions'routines.
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
        ^ StringWriter new writeLiteral:"Name:":theName:" Price:":thePrice literal.
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
                theBM find:";".
                
                theBM Marked convertTo:aVariable.
                
                theBM mark.
                
                ^ true.
            ].
}

// --- Program --- 

#symbol Program = 
[
    ('program'arguments length == 1)?
        [  console << "Please provide the path to the file to view". #throw AbortException new. ].

    #var aList := List new.
    #var anAdapter := Adapter new.

    File new &path:('program'arguments@1) run &eachLine: aLine
    [
        anAdapter write:aLine.

        #var aName := String new.
        #var aPrice := Real new.
                        
        #loop (anAdapter read:aName)?
        [
            anAdapter read:aPrice.
        
            aList append:(Record new &Price:(aPrice value) &Name:(aName value)).
        ].
    ].

    aList run &each:printingLn.

    console readChar.
].
