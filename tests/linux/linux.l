import extensions;

public program()
{
   console.printLine("Start")//.fail();

           int ch := extern libc.getchar();

   console.write(".");

           byte retVal := byteConvertor.convert(ch);

   console.write(".");
}