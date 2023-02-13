// --- variables ---

public sealed class Integer : BaseVariable, Reference<int>
{
   constructor(int n) 
   {
      value := n
   }

   constructor()
   {
      value := 0
   }

   constructor new()
   {
   }

   append(int n)
   {
      int v := value;
      v := v + n;

      value := v
   }

   reduce(int n)
   {
      int v := value;
      v := v - n;

      value := v
   }

   dispatch() => value;
}
