#define system.

// --- controller ---

#symbol control =
{
    prompt = ">".
    
    proceed : aLine
    [
        ("-q" == aLine)
            ? [ 'program stop. ^emptyLiteralValue. ]
            ! [
                ^ aLine evaluated.
            ].
    ]    
}.
