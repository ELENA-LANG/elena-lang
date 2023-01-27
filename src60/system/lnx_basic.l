
public sealed const struct Handle
{
   embeddable __word value[4];

   constructor(int intValue)
   {
      value := intValue
   }
}