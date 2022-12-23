// === Basic prototypes ===

// --- Enumerator ---

public interface Enumerator
   : info<"An enumerator prototype">
{
   get abstract Value()
      : info<"Returns the current object">;

   abstract bool next()
      : info<"Goes to the next enumeration member and returns true if it is not the last member">;

   abstract reset()
      : info<"Resets the enumerator">;

   abstract enumerable()
      : info<"returns the enumerator owner">;

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
