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
      int n := 0;
      int m := 0;        
      _set.readLengthsTo(ref n, ref m);
        
      if (i >= n || j >= m)
      {
         ^ 0
      };

      int cell := _set[i][j];
      ^ cell
   }
}
