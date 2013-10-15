// --- default namespaces ---
#define system.
#define system'collections.
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

    #method Name = theName Literal.
    
    #method set &Name:aName
    [
        theName << aName.
    ]
    
    #method Age = theAge Number.
    
    #method set &Age:anAge
    [
        theAge << anAge.
    ]
    
    #method Phone = thePhone Literal.
    
    #method set &Phone:aPhone
    [
        thePhone << aPhone.
    ]

    #method Literal
        = String new 
            append:"Name:"
            append:theName
            append:"%n"
            append:"Age:"
            append:(theAge Literal)
            append:"%n"
            append:"Phone:"
            append:thePhone
            append:"%n"
            Literal.
}

// --- Program ---

#symbol Program = 
[
    ('program'arguments Count == 1)?
        [  console << "Please provide the path to the file to view". #throw nil. ].

    #var aFileName := 'program'arguments@1.

    #var aDB := List new += Record new.
    textFileControl forEachLine:aFileName &do: &&:aLine
    [
        (aLine Length == 0)
            ? [ aDB += Record new. ]
            ! [
                // split record line                    
                #var aKeyValue := stringControl split:aLine &by:":".
                #var aName  := aKeyValue@0.
                #var aValue := aKeyValue@1.

                // update field
                aDB~EList Last ~(aName~EField Unserialized) set:(Variant new:aValue).
            ].
    ].
    
    // print the list
    aDB~EList run:printingLn.

    console readChar.
].