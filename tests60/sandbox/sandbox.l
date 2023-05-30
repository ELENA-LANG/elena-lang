import extensions;

public program()
{
   uint m := 80000000H;
   uint n := 10H;
   uint k := m / n;
   Assert.ifTrue(k == 8000000H);

}