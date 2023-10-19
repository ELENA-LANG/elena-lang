// --- Handle ---

public sealed const struct Handle
{
   embeddable __ptr _value;

   private clear() : external(system'core_routines'__clearPtr); 

   get constructor Default()
      = new Handle();   

   constructor()
   {
      self.clear();
   }

   constructor(pointer ptr)
   {
      _value := ptr
   }

   bool isUnassigned()
   {
      ^ _value == 0;
   }
}
