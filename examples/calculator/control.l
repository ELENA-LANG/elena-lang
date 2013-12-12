// --- namespaces ---
#define system.

// --- Controller --
#class Controller 
{
    #field theParser.
    
    #method read : anInput
    [
        'program'output << "%n>".
        
        #var aLine := anInput readLine.
        
        ^ ("-q" == aLine)
            ? [ 'program stop. ^emptyLiteralValue. ]
            ! [
                theParser eval::aLine
            ].
    ]

    #constructor new
    [
        'program'output write:"ELENA Calculator. Evaluate the expression. To quit type -q".
        
        theParser := Parser new.
    ]
}

#symbol controller = Controller new.