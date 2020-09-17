import extensions;

// --- Rule Set ---

public interface RuleSet
{
    abstract proceed(Space s, int i, int j, ref int newState);
}

public singleton randomSet : RuleSet
{
    proceed(Space s, int i, int j, ref int newState)
    {
        newState := randomGenerator.eval(0,2)
    }
}

public class LoadedSet : RuleSet
{
    IntMatrix theSet;

    constructor load(IntMatrix m)
    {
        theSet := m
    }
    
    constructor load(list)
        <= load(list.toIntMatrix());
    
    proceed(Space s, int i, int j, ref int newState)
    {
        int n := 0;
        int m := 0;        
        theSet.readLengthsTo(ref n, ref m);
        
        if (i >= n || j >= m)
        {
            newState := 0
        }
        else
        {
            int cell := theSet[i][j];
            newState := cell
        }
    }
}
