import system'io;
import extensions;
import ltests;

public streamSegmentReadTest() : testCase()
{
   byte[] source := new byte[](8);
   for (int i := 0; i < source.Length; i += 1) {
      source[i] := i + 1
   };

   Stream stream := MemoryStream.load(source);
   byte[] target := new byte[](9);
   for (int i := 0; i < target.Length; i += 1) {
      target[i] := 99
   };

   int read := stream.read(target, 2, 5);

   Assert.ifEqual(read, 5);
   Assert.ifEqual(stream.Index, 5);
   Assert.ifEqual(target[1], 99);
   Assert.ifEqual(target[2], 1);
   Assert.ifEqual(target[6], 5);
   Assert.ifEqual(target[7], 99);

   read := stream.read(target, 7, 2);
   Assert.ifEqual(read, 2);
   Assert.ifEqual(stream.Index, 7);
   Assert.ifEqual(target[7], 6);
   Assert.ifEqual(target[8], 7);

   read := stream.read(target, 0, 2);
   Assert.ifEqual(read, 1);
   Assert.ifEqual(stream.Index, 8);
   Assert.ifEqual(target[0], 8);

   Console.write(".")
}

public streamSegmentWriteTest() : testCase()
{
   byte[] source := new byte[](8);
   for (int i := 0; i < source.Length; i += 1) {
      source[i] := i + 1
   };

   MemoryStream stream := new MemoryStream();
   Stream output := stream;

   output.write(source, 2, 4);

   byte[] written := new byte[](4);
   Assert.ifEqual(stream.read(written, 4), 4);
   Assert.ifEqual(written[0], 3);
   Assert.ifEqual(written[3], 6);

   Console.write(".")
}

public streamSegmentRangeTest() : testCase()
{
   byte[] source := new byte[](4);
   Stream stream := MemoryStream.load(source);

   bool readFailed := false;
   try {
      stream.read(source, 3, 2)
   }
   catch (OutOfRangeException e) {
      readFailed := true
   };

   bool writeFailed := false;
   try {
      stream.write(source, -1, 1)
   }
   catch (OutOfRangeException e) {
      writeFailed := true
   };

   Assert.ifTrue(readFailed);
   Assert.ifTrue(writeFailed);

   Console.write(".")
}

public fileStreamSegmentTest() : testCase()
{
   string path := "stream_segment_test.tmp";
   try {
      byte[] source := new byte[](2054);
      for (int i := 0; i < source.Length; i += 1) {
         source[i] := i & 255
      };

      using(Stream output := FileStream.openForReWrite(path)) {
         output.write(source, 3, 2048)
      };

      byte[] target := new byte[](2052);
      for (int i := 0; i < target.Length; i += 1) {
         target[i] := 99
      };
      Assert.ifEqual(target[2050], 99);

      using(Stream input := FileStream.openForRead(path)) {
         int read := input.read(target, 2, 2048);

         Assert.ifEqual(read, 2048);
         Assert.ifEqual(target[1], 99);
         Assert.ifEqual(target[2], 3);
         Assert.ifEqual(target[1025], 2);
         Assert.ifEqual(target[2049], 2);
         Assert.ifEqual(target[2050], 99)
      }
   }
   finally {
      File.assign(path).delete()
   };

   Console.write(".")
}
