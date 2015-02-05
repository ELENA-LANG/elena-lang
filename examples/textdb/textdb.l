// --- default namespaces ---
#define system.
#define system'routines.
#define system'collections.
#define extensions.
#define extensions'text.

// --- EField ---

#symbol EField =
{
    Unserialized
    [
        ^ self =>
            "Name" ? [ %Name ]
            "Age" ? [ %Age ]
            "Phone" ? [ %Phone ].
    ]
}.

// --- Record ---
#class Record
{
    #field theName.
    #field theAge.
    #field thePhone.
    
    #constructor new
    [
        theName := String new.
        theAge := Integer new.
        thePhone := String new.
    ]

    #method Name = theName literal.
    
    #method set &Name:aName
    [
        convertor save:aName &to:theName.
    ]
    
    #method Age = theAge Number.
    
    #method set &Age:anAge
    [
        convertor save:anAge &to:theAge.
    ]
    
    #method Phone = thePhone literal.
    
    #method set &Phone:aPhone
    [
        thePhone << aPhone.
    ]

    #method literal
        = String new 
            append:"Name:"
            append:theName
            append:'newLine
            append:"Age:"
            append:(theAge literal)
            append:'newLine
            append:"Phone:"
            append:thePhone
            append:'newLine
            literal.
}

// --- Program ---

#symbol Program = 
[
    ('program'arguments length == 1)?
        [  console << "Please provide the path to the file to view". #throw nil. ].

    #var aFileName := 'program'arguments@1.

    #var aDB := List new += Record new.
    control run &path:(aFileName literal) &forEachLine: aLine
    [
        (aLine length == 0)
            ? [ aDB += Record new. ]
            ! [
                // split record line                    
                #var aKeyValue := aLine~literal split &by:":".
                #var aName  := aKeyValue@0.
                #var aValue := aKeyValue@1.

                // update field
                aDB~enumerable last :: (aName::EField Unserialized) set:aValue.
            ].
    ].
    
    // print the list
    control run:aDB &forEach:printingLn.

    console readChar.
].