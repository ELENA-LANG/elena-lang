#define system.
#define system'routines.
#define system'collections.

// --- Arguments ---

#static arguments = control do:
[
    #var aList := List new.
    
    #var anEnumerator := extensions'text'WordEnumerator new:CommandLine.
    // !! temporal : extra space are reserved
    control while:[ anEnumerator next ] &do: 
    [ 
        #var anArg := anEnumerator get.
    
        aList append:(LiteralValue new &type'length:(anArg Length + 4) &type'widestr:anArg).
    ].

    ^ Array new &type'length:(aList Count) &function: &&:anIndex [ aList~EList getAt:anIndex ].
].

// --- ProgramLoop ---

#class $ProgramLoop
{
    #field theActive.
    #field theControl.
    
    #constructor new : aControl
    [
        theActive := true.
        theControl := aControl.
    ]
    
    #method stop [ theActive := false. ]
    
    #method eval
    [
        control while:theActive &do:
          [ 'program'output << (theControl >> 'program'input). ].        
    ]
}

#static programLoop = $ProgramLoop new:'program'control.




