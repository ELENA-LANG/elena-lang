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

//   Enumerator<T> enumerator() = self;
}