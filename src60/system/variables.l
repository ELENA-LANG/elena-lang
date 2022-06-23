public abstract class BaseVariable
{
   get abstract Value();

   set abstract Value(o);
}

public sealed ref<T> : BaseVariable
{
   embeddable T value;

   get T Value()
   {
      ^ value
   }

   set Value(T val)
   {
      value := val
   }
}

