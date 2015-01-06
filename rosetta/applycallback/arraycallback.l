#define system.
#define extensions.

#symbol PrintSecondPower =
    (:n) [ console writeLine:(n * n) ].

#symbol program =
[
    #var anArray := (1, 2, 3, 4, 5, 6, 7, 8, 9, 10).
    
    control run:anArray &forEach:PrintSecondPower.
].
