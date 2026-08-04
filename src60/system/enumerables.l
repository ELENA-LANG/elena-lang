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

// --- Indexable ---

public interface Indexable
   : info("An abstract generic indexable class<br/>Must be overridden")
{
   abstract int Length
     : info("Returns the length of the corresponding collection")
   {
      get();
   }
   
   abstract at(int index)
      : info("Returns an element at the given <b>index</b>");

   abstract setAt(int index, object element)
      : info("Set an element at the given <b>index</b>");   
}

public interface Indexable<T>
   : info("An abstract strong-typed indexable class<br/>Must be overridden")
{
   abstract int Length
     : info("Returns the length of the corresponding collection")
   {
      get();
   }
   
   abstract T at(int index)
      : info("Returns an element at the given <b>index</b>");

   abstract setAt(int index, T element)
      : info("Set an element at the given <b>index</b>");   
}

// --- BaseEnumerator<type> ---

public abstract class BaseEnumerator<T> : Enumerator<T>,
   info("A base enumerator template for a collection or an array<br/>Property getter <b>Value<b> must be implemented")
{
   Reference<int> index;
   int            length;

   bool next()
      : info("Increments the field <b>index</b> by one. If the value is smaller then the field <b>length</b> it returns true; returns false otherwise")
   {
      index.append(1);
        
      int n := *index;  

      ^ n < length
   }
    
   Enumerator cast() : info("Returns a wrapper implementing a generic enumerator")
      = new Enumerator { embeddable dispatch() => self; };
}

// --- BaseEnumerator ---

public abstract class BaseEnumerator : Enumerator,
   info("A base generic enumerator for a collection or an array<br/>Property getter <b>Value<b> must be implemented")
{
   Reference<int> index;
   int            length;

   bool next()
      : info("Increments the field <b>index</b> by one. If the value is smaller then the field <b>length</b> it returns true; returns false otherwise")
   {
      index.append(1);
        
      int n := *index;  

      ^ n < length
   }
}

// --- Range ---

public sealed class Range : Enumerable<int>,
   info("represents a range that has start and end indexes.<br>The class support Enumerable interface")
{
   int start;
   int end;
    
   constructor(int start, int count)
      : info("creates a range with the specified <b>start</b> position. <b>count</b> contains the length of the range")
   {
      this start := start;
      this end := start + count
   }
    
   constructor new(int start, int count)
      : info("creates a range with the specified <b>start</b> position. <b>count</b> contains the length of the range")
   {
      this start := start;
      this end := start + count
   }
    
   constructor for(int start, int end)
        : info("creates a range with the specified <b>start</b> and <b>end</b> positions")
      <= new(start, end - start);
    
   int Length
       : info("returns the length of the range")
   {
      get()
      {
         int l := end - start;
            
         ^ l
      }
   }
    
   Enumerator<int> enumerator() : info("returns the range enumerator")
      = new IntRangeEnumerator(start, end);
    
   Enumerable cast() : info("Returns a wrapper implementing a generic enumerator")
      = new Enumerable { Enumerator enumerator() => self; };
}

// --- IntRangeEnumerator --

class IntRangeEnumerator : Enumerator<int>,
   info("implements a numeric range enumerator")
{
   Reference<int> index;
   int start;
   int end;

   constructor(int start, int end)
        : info("creates a range enumerator with the specified <b>start</b> and <b>end</b> positions")
   {
      this index := 0;
      this start := start;
      this end := end;
      
      self.reset();
   }

   get int Value() : info("Returns the current range enumeration value")
      = *index;
            
   bool next()
      : info("Returns false if the index out of range")
   {
      index.append(1);

      int n := *index;

      ^ n < end
   }
    
   reset()
      : info("Resets the enumeration to start again")
   {
      index.Value := start - 1;
   } 
    
   Enumerator cast() : info("Returns a wrapper implementing a generic enumerator")
      = new Enumerator { embeddable dispatch() => self; };
    
   enumerable() : info("returns the enumeration range")
      = new Range(start, end - start);
}
