#import extensions.

// --- Basic types ---

#subject space   = BaseSpace.
#subject ruleset = BaseRuleSet.

// --- Rule Set ---

limited dispatchable(ruleset) class BaseRuleSet
{
    stacksafe bool validate &space:s &int:i &int:j []
}

class randomSet = BaseRuleSet
{
    validate &space:s &int:i &int:j
        = 1 == randomGenerator eval:0:2.
}.

class LoadedSet :: BaseRuleSet
{
    intmatrix theSet.

    constructor load &intmatrix:m
    [
        theSet := m.
    ]
    
    constructor load:list
        <= load &intmatrix:(list toIntMatrix).
    
    validate &space:s &int:i &int:j    
    [
        int n := 0.
        int m := 0.        
        theSet readLengths &vint:n &vint:m.
        
        (i >= n) || (j >= m)
            ? [ ^ false. ].
        
        int cell := theSet getAt &int:i &int:j.
        
        ^ cell != 0.
    ]
}
