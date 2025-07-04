import system'routines;

// --- Strategy ---

interface BaseStrategy
{
   abstract at(Indexer[] indexers, int length);
    
   abstract BaseStrategy eval(Indexer[] indexers, int length);
    
   get abstract bool Result();
}

// --- Combination strategies ---

singleton combinationEnd : BaseStrategy
{
   at(Indexer[] indexers, int length) = nil;
    
   BaseStrategy eval(Indexer[] indexers, int length) = self;
    
   bool Result = false;
}

singleton sequentialNext : BaseStrategy
{
   bool Result = true;
    
   BaseStrategy eval(Indexer[] indexers, int length)
   {
      if(nil == (indexers.seekEach::(indexer)
         {
            auto it := cast Indexer(indexer); 
                    
            if:not(it.appendIndex(1).Available)
               { it.Index := 0; ^ false };
                        
            ^ true
         }))
            { ^ combinationEnd };
            
        ^ self          
   }
        
   at(Indexer[] indexers, int length)
   {
      ^ Array.allocate(length).populate::(int i => indexers.at(i).Value)
   }
}

singleton sequentialStart : BaseStrategy
{
   bool Result = false;
    
   at(Indexer[] indexers, int length) = nil;
    
   BaseStrategy eval(Indexer[] indexers, int length)
   {
      indexers.forEach::(indexer){ indexer.Index := 0 };
        
      ^ sequentialNext
   }
}

// --- Parallel strategies

singleton parallelNext : BaseStrategy
{
   bool Result = true;
    
   BaseStrategy eval(Indexer[] indexers, int length)
   {
      if(nil != (indexers.seekEach::(indexer)
               {
                  auto it := cast Indexer(indexer); 
                    
                  ^ it.appendIndex(1).Available.Inverted
               }))
         { ^ combinationEnd };
            
      ^ self          
   }
        
   at(Indexer[] indexers, int length)
   {
      ^ Array.allocate(length).populate::(int i => indexers.at(i).Value)
   }
}

singleton parallelStart : BaseStrategy
{
   bool Result = false;
    
   at(Indexer[] indexers, int length) = nil;
    
   BaseStrategy eval(Indexer[] indexers, int length)
   {
      indexers.forEach::(indexer){ indexer.Index := 0 };
        
      ^ parallelNext
   }
}

// --- combination strategy ---

singleton combinationNext : BaseStrategy
{
   bool Result = true;

   at(Indexer[] indexers, int length)
   {
      ^ Array.allocate(length).populate::(int i => indexers.at(i).Value)
   }

   BaseStrategy eval(Indexer[] indexers, int length)
   {        
      int k := indexers.Length;
      int i := k - 1;
        
      Indexer currentIndexer := indexers[i];
        
      if(currentIndexer.appendIndex(1).Available)
         { ^ self };
             
      while (i > 0 && (indexers[i - 1].Index + k - i + 1) >= length)
         { i := i - 1 };
        
      if(i == 0)
         { ^ combinationEnd };
        
      indexers[i - 1].appendIndex(1);
        
      for (int j := i; j <= k - 1; j += 1) {
         indexers[j].Index := indexers[j - 1].Index + 1
      };
            
      ^ self          
   }
}

singleton combinationStart : BaseStrategy
{
   bool Result = false;

   at(Indexer[] indexers, int length) = nil;
   
   BaseStrategy eval(Indexer[] indexers, int length)
   {
      int len := indexers.Length;
         
      for (int i := 0; i < len; i+=1) {
         indexers[i].Index := i
      };
        
      ^ combinationNext
   }
}

// --- BaseRowEnumerator ---

public abstract class BaseRowEnumerator : Enumerator
{
   Indexer[]    _indexers;
   int          _length;  
   BaseStrategy _strategy;

   bool next()
   {
      _strategy := _strategy.eval(_indexers, _length);

      ^ _strategy.Result
   }
    
   get Value()
      = _strategy.at(_indexers, _length);
        
   enumerable() = _indexers;
}

// --- SequentialEnumerator ---

public class SequentialEnumerator : BaseRowEnumerator
{
   constructor new(object[] arguments)
   {
      _length := arguments.Length;
      _indexers := new Indexer[](_length);
        
      for (int i := 0; i < _length; i += 1)
      {
         _indexers[i] := arguments[i].indexer()
      };
        
      self.reset()
   }
        
   constructor load(params object[] arguments)
      <= new(cast object[](arguments));

   reset()
   {
      _strategy := sequentialStart;
   }
}

// --- ParallelEnumerator ---

public class ParallelEnumerator : BaseRowEnumerator
{
   constructor new(object[] arguments)
   {
      _length := arguments.Length;
      _indexers := new Indexer[](_length);
        
      for (int i := 0; i < _length; i += 1)
      {
         Indexer it := arguments[i].indexer();
            
         _indexers[i] := /*arguments[i].indexer()*/it
      };
        
      self.reset()        
   }
        
   constructor new(Array a)
      <= new(a.Value);    
        
   reset()
   {
      _strategy := parallelStart
   }
}

// --- Combinator ---

public class Combinator : BaseRowEnumerator
{
   int _totalLength;
    
   constructor new(arguments)
   {
      _length := arguments.Length;
        
      _indexers := new Indexer[](_length);
        
      for (int i := 0; i < _length; i += 1)
      {
         Indexer it := arguments[i].indexer();
            
         _indexers[i] := /*arguments[i].indexer()*/it
      };
        
      self.reset()
   }
        
   constructor new(int subLength, set)
      <= new(Array.allocate(subLength).populate::(int n => set))
   {
      _totalLength := set.Length
   }
    
   bool next()
   {
      _strategy := _strategy.eval(_indexers, _totalLength);

      ^ _strategy.Result
   }
    
   reset()
   {
      _strategy := combinationStart
   }
}
