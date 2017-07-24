import extensions.

// --- Basic types ---

type space   :: BaseSpace.
type ruleset :: BaseRuleSet.

// --- Rule Set ---

limited class BaseRuleSet :: dispatchable(ruleset)
{
    stacksafe bool validate space:s int:i int:j []
}

class randomSet = BaseRuleSet::
{
    validate space:s int:i int:j
        = 1 == randomGenerator eval:0:2.
}.

class LoadedSet :: BaseRuleSet
{
    intmatrix theSet.

    constructor load intmatrix:m
    [
        theSet := m.
    ]
    
    constructor load:list
        <= load intmatrix:(list toIntMatrix).
    
    validate space:s int:i int:j    
    [
        int n := 0.
        int m := 0.        
        theSet readLengthsTo vint:n vint:m.
        
        if ((i >= n) || (j >= m))
            [ ^ false ].
        
        int cell := theSet getAt int:i int:j.
        
        ^ cell != 0
    ]
}
