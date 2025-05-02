import extensions;
import system'text;
import system'io;

public program()
{
   int k := 2;
//   int n := k shl:3;
   int m := (k + 1) shl:3;

   Console.writeLine(m);

/*   auto source := "Standard comparison operations are defined for all the primitive numeric types.";
   auto expected := "U3RhbmRhcmQgY29tcGFyaXNvbiBvcGVyYXRpb25zIGFyZSBkZWZpbmVkIGZvciBhbGwgdGhlIHByaW1pdGl2ZSBudW1lcmljIHR5cGVzLg==";
   
   auto buffer := MemoryBuffer.allocate();
   UTF8Encoding.fromString(source, buffer);
   var dest := Base64Encoding.toString(0, buffer.Length, *buffer);
   
   Assert.ifEqual(dest, expected);
   
   buffer.clear();
   Base64Encoding.fromString(dest, buffer);
   
   var decoded := UTF8Encoding.toString(buffer);*/
}