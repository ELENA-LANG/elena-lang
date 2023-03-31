// === Basic prototypes ===

// --- Enumerator ---

public interface Enumerator
   : info("An enumerator prototype")
{
   get abstract Value()
      : info("Returns the current object");

   abstract bool next()
      : info("Goes to the next enumeration member and returns true if it is not the last member");

   abstract reset()
      : info("Resets the enumerator");

   abstract enumerable()
      : info("returns the enumerator owner");

   Enumerator enumerator() = self;
}

// --- Enumerator<T> ---

public interface Enumerator<T>
{
   abstract bool next();

   get abstract T Value();

   abstract reset();

   abstract enumerable();

   abstract Enumerator cast();

   Enumerator<T> enumerator() = self;
}

// --- Enumerable ---

public abstract Enumerable
{
   abstract Enumerator enumerator();
}

public abstract Enumerable<T>
{
   abstract Enumerator<T> enumerator();
}

// --- Indexer ---

public interface Indexer
{
   get abstract bool Available();

   get abstract Value();

   set abstract Value(value);

   abstract int Index
   {
      get();

      set(value);
   }

   abstract appendIndex(int index);

   abstract int Length
   {
      get();
   }
}

public interface Indexer<T>
{
   get abstract bool Available();

   get abstract T Value();

   set abstract Value(T value);

   abstract int Index
   {
      get();

      set(value);
   }

   abstract appendIndex(int index);

   abstract int Length
   {
      get();
   }

   abstract Indexer cast();
}

// --- Indexable ---

public abstract Indexable
{
   abstract Indexer indexer();
}

public abstract Indexable<T>
{
   abstract Indexer<T> indexer();
}

// --- BaseEnumerator<type> ---

public abstract class BaseEnumerator<T> : Enumerator<T>
{
   Reference<int> index;
   int            length;

   bool next()
   {
      index.append(1);
        
      int n := *index;  

      ^ n < length
   }
    
   Enumerator cast() = new Enumerator { embeddable dispatch() => self; };
}

public abstract BaseIndexer<T> : Indexer<T>
{
   Reference<int> index;
   int length;
    
   /// Returns false if the index out of range
   bool Available = index.Value < length;

   int Index
   {    
      /// Reads the current i-index into an output parameter
      get()
      {
         ^ *index
      }    
        
      /// Sets the i-index by specified value
      set(index)
      {
         this index.Value := index;
      }
   } 
    
   appendIndex(int offs)
   { 
      index.append(offs);
   }     
    
   int Length
   {
      get()
      {
         ^ length;
      }
   }
    
   Indexer cast() = new Indexer { embeddable dispatch() => self; };
}

// --- BaseIndexer ---

public abstract class BaseIndexer : Indexer
{
   Reference<int> index;
   int            length;
    
   get bool Available()
   {
      int n := *index;

      ^ n < length;
   }

   int Index
   {
      /// Reads the current i-index into an output parameter
      get()
      {
         ^ *index
      }
        
      /// Sets the i-index by specified value
      set(value)
      { 
         index.Value := value
      }
   }    
    
   appendIndex(int disp)
   { 
      index.append(disp)
   }
    
   int Length
   {
      get()
      {
         ^ length
      }
   }
}

// --- Range ---

public sealed class Range : Enumerable<int>
{
   int start;
   int end;
    
   constructor(int start, int count)
   {
      this start := start;
      this end := start + count
   }
    
   constructor new(int start, int count)
   {
      this start := start;
      this end := start + count
   }
    
   constructor for(int start, int end)
      <= new(start, end - start);
    
   int Length
   {
      get()
      {
         int l := end - start;
            
         ^ l
      }
   }
    
   Enumerator<int> enumerator() = new IntRangeEnumerator(start, end);
    
   Enumerable cast() = new Enumerable { Enumerator enumerator() => self; };
}

// --- IntRangeEnumerator --

class IntRangeEnumerator : Enumerator<int>
{
   Reference<int> index;
   int start;
   int end;

   constructor(int start, int end)
   {
      this start := start;
      this end := end;
      
      self.reset();
   }

   get int Value() = *index;
            
   bool next()
      : info("Returns false if the index out of range")
   {
      index.append(1);

      int n := *index;

      ^ n < end
   }
    
   reset()
      : info("Sets the i-index by specified value")
   {
      index.Value := start - 1;
   } 
    
   Enumerator cast() = new Enumerator { embeddable dispatch() => self; };
    
   enumerable() = new Range(start, end - start);
}
