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
    
        aList append:(LiteralValue new &length:(anArg length + 4) &literal::anArg).
    ].

    ^ arrayControl new &length:(aList length) &each: anIndex [ aList @ anIndex ].
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
        control while:[ theActive ] &do:
        [ 
            'program'output write:(theControl prompt:'program'output read:'program'input). 
        ].
    ]
}

#static programLoop = $ProgramLoop new:'program'control.
