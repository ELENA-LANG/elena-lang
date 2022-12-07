import extensions;

public program()
{
   real r := 0.2;

   r := r * 1.0e9;

   int n := intConvertor.convert(r);

   console.writeLine(n);
}
