#define system.
#define system'routines.
#define extensions.

// --- Sum ---

#class MeanAction :: BasePattern
{
    #field theValue.
    #field theCount.
    
    #constructor new
    [
        theValue := Real new.
        theCount := Integer new.
    ]

    #method evaluate : aValue
    [
        theCount += 1.
        
        theValue += aValue.
        
        theResult := theValue / theCount.
    ]
}

// --- Program ---

#symbol program =
[
    #var anArray := (1, 2, 3, 4, 5, 6, 7, 8).
    consoleEx writeLine:"Arithmetic mean of {":anArray:"} is ":(control run:anArray &forEach:(MeanAction new)).
]. 