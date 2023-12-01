import extensions;

interface I
{
   get abstract int Reserved();
}

public sealed class C : interface<I>
{
   int Reserved = 2;
}

public program()
{
   auto c := new C();

   I i := c;
   
   console.printLine(i.Reserved);
}