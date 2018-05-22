//import extensions.
//
//// --- Basic types ---
//
//type space   = BaseSpace.
//type ruleset = BaseRuleSet.

// --- Rule Set ---

public interface class RuleSet
{
    abstract stacksafe proceed(Space s, int i, int j, ref<int> newState) []
}

//class randomSet = BaseRuleSet::
//{
//    proceed space:s int:i int:j vint:newState
//    [
//        newState int := randomGenerator eval(0,2).
//    ]
//}.
//
//class LoadedSet :: BaseRuleSet
//{
//    intmatrix theSet.
//
//    constructor load intmatrix:m
//    [
//        theSet := m.
//    ]
//    
//    constructor load:list
//        <= load intmatrix:(list toIntMatrix).
//    
//    proceed space:s int:i int:j vint:newState
//    [
//        int n := 0.
//        int m := 0.        
//        theSet readLengthsTo vint:n vint:m.
//        
//        if ((i >= n) || (j >= m))
//        [ 
//            newState int := 0
//        ];
//        [
//            int cell := theSet getAt int:i int:j.
//            newState int := cell
//        ]
//    ]
//}
