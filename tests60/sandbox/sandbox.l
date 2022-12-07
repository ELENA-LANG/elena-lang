import extensions;

public program()
{
   real r := 3.2;

   real remainder := r.Integer;

   remainder := r - remainder;

   //remainder := remainder * 1.0e9;

   console.writeLine(remainder);
}
