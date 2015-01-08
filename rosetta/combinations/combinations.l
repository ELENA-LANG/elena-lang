#define system.
#define system'routines.
#define extensions.
#define extensions'routines.

#symbol(const)M = 3.
#symbol(const)N = 5. 

// --- Numbers ---

#symbol numbers = (:anN)
[
    Array new &length:(anN int) set &every: (&index:n) [ Integer new &int:n ]
].

// --- Program ---

#symbol program =
[
    #var aNumbers := numbers:N.    
    control run:(Combinator new &k:M &of:aNumbers) &forEach: aRow
    [
        consoleEx writeLine:aRow.
    ].
].