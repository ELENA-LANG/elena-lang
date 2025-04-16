import system'routines;

public singleton InsertSortAlgorithm
{
   sort(SortingAgent source, int start, int length, Func2 compf)
   {
      int end := start + length;
      
      for(int i := start + 1; i < end; i += 1)
      {
         var entry := source[i];
         int j := i;
 
         while (j > start && compf(entry, source[j - 1]))
         {
            source.move(j, j - 1);
 
            j -= 1
         };
 
         source[j] := entry
      }
   }
}