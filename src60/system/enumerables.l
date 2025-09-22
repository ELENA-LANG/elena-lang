// === Basic prototypes ===

// --- Enumerator ---

public interface Enumerator
   : info("A generic enumerator interface")
{
   get abstract Value()
      : info("Returns the current object");

   abstract bool next()
      : info("Goes to the next enumeration member and returns true if it is not the last member");

   abstract reset()
      : info("Resets the enumerator");

   abstract enumerable()
      : info("returns the enumerator owner");

   Enumerator enumerator() : info("returns itself")
      = self;
}

// --- Enumerator<T> ---

public interface Enumerator<T>
   : info("A strong-typed enumerator template")
{
   abstract bool next()
      : info("Goes to the next enumeration member and returns true if it is not the last member");

   get abstract T Value()
      : info("Returns the current object");

   abstract reset()
      : info("Resets the enumerator");

   abstract enumerable()
      : info("returns the enumerator owner");

   abstract Enumerator cast()
      : info("converts the class to the generic enumerator");

   Enumerator<T> enumerator() = self;
}

// --- Enumerable ---

public abstract Enumerable
   : info("An abstract generic enumerable class<br/>Must be overridden")
{
   abstract Enumerator enumerator()
      : info("Returns the generic enumerator of the class");
}

public abstract Enumerable<T>
   : info("An abstract strong-typed enumerable class<br/>Must be overridden")
{
   abstract Enumerator<T> enumerator()
     : info("Returns the strong-typed enumerator of the class");
}

// --- Indexer ---

public interface Indexer
   : info("A generic indexer interface<br/>The indexer is a class which allow to access a collection elements by an index")
{
   get abstract bool Available()
      : info("Returns true if there is an element with a given index in the corresponding collection<br/>Otherwise returns false");

   get abstract Value()
      : info("Returns an element the given index");

   set abstract Value(value)
      : info("Set an element at the given index");

   abstract int Index : info("Index property")
   {
      get();

      set(value);
   }

   abstract appendIndex(int index)
     : info("Increases the current index by parameter **index**");

   abstract int Length
     : info("Returns the length of the corresponding collection")
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

// --- BaseEnumerator ---

public abstract class BaseEnumerator : Enumerator
{
   Reference<int> index;
   int            length;

   bool next()
   {
      index.append(1);
        
      int n := *index;  

      ^ n < length
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
      this index := 0;
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
