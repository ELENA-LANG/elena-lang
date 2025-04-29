import extensions;

extern pointer environ
  = libc;

public program()
{
   pointer ptr := environ;

   Console.writeLine("Hello");
}