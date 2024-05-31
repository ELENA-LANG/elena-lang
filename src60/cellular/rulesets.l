import extensions;

// --- Rule Set ---

public interface RuleSet
{
   abstract int proceed(Space s, int i, int j);
}

public singleton randomSet : RuleSet
{
   int proceed(Space s, int i, int j)
   {
      ^ randomGenerator.nextInt(0,2)
   }
}

public class LoadedSet : RuleSet
{
   IntMatrix _set;

   constructor load(IntMatrix m)
   {
      _set := m
   }
    
   constructor load(Array list)
      <= load(list.toIntMatrix());
    
   int proceed(Space s, int i, int j)
   {
      _set.readLengthsTo(out int n, out int m);
        
      if (i >= n || j >= m)
      {
         ^ 0
      };

      int cell := _set[i][j];
      ^ cell
   }
}
