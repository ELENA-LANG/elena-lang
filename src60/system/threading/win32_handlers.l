import system;

// --- ThreadHandle ---
public sealed struct ThreadHandle
{
   embeddable __word value[4];

   constructor(Func f)
   {
   }
}
