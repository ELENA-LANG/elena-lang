// --- Range ---

public sealed class Range : Enumerable<int>
{
//   int start;
//   int end;
//    
//   constructor(int start, int count)
//   {
//      this start := start;
//      this end := start + count
//   }
//    
//   constructor new(int start, int count)
//   {
//      this start := start;
//      this end := start + count
//   }
//    
//   constructor for(int start, int end)
//      <= new(start, end - start);
    
   int Length
   {
      get()
      {
         int l := /*end - start*/2;
            
         ^ l
      }
   }
    
   Enumerator<int> enumerator() = nil;
    
   Enumerable cast() = nil;
}

public program()
{
}