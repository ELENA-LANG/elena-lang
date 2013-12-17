#define system.
#define extensions.
#define extensions'routines.

#symbol M = 3.
#symbol N = 5. 

// --- Numbers ---

#symbol numbers = &&:anN
[
    Array new &type'length:anN &function: &&:anIndex [ Integer new:(anIndex + 1) ]
].

// --- Program ---

#symbol program =
[
    #var aNumbers := numbers:N.    
    controlEx for:(Combinator new:(Array new &type'length:M &function: &&:i [ aNumbers ])) &do: &&:aRow
    [
        consoleEx writeLine:aRow.
    ].
].