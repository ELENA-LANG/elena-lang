import extensions;

X
{
   real convert(byte b)
   {
      real r := b

      ^ r;
   }
}

A;

B
{
   A get()
   {
      if (true) {
         ^ new A()
      }
      else {
         ^ new A()
      }
   }
}

public program()
{

}
