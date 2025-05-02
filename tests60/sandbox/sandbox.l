import extensions;
import system'text;
import system'io;

public program()
{
   byte map[10];
   byte buffer[4];

   buffer[0] := 12;
   map[0] := 1;
   map[1] := 2;
   map[2] := 3;
   map[3] := 4;
   map[4] := 5;
   map[5] := 6;
   map[6] := 7;
   map[7] := 8;
   map[8] := 9;
   map[9] := 10;

   byte i := 4;
   byte b := map[buffer[0] shr:2];

//   Console.writeLine(b);

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