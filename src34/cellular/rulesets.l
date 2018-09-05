import extensions.

// --- Rule Set ---

public interface class RuleSet
{
    abstract stacksafe proceed(Space s, int i, int j, ref<int> newState) []
}

public singleton randomSet :: RuleSet
{
    proceed(Space s, int i, int j, ref<int> newState)
    [
        newState value := randomGenerator eval(0,2).
    ]
}

public class LoadedSet :: RuleSet
{
    T<IntMatrix> theSet.

    constructor load(IntMatrix m)
    [
        theSet := m.
    ]
    
    constructor load:list
        <= load(list toIntMatrix).
    
    proceed(Space s, int i, int j, ref<int> newState)
    [
        int n := 0.
        int m := 0.        
        theSet readLengthsTo(&n, &m).
        
        if ((i >= n) || (j >= m))
        [ 
            newState value := 0
        ];
        [
            int cell := theSet getAt(i, j).
            newState value := cell
        ]
    ]
}
