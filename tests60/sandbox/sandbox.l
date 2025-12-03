import extensions;

internal singleton UTFOperations
{
   int convertUTF8_2_32(byte[] sour, int index, int length, ref int retVal)
   {
      int ch := sour[index];

      ^ 1
   }
}

public const struct String
{
   __string byte[] array;

   char at(int index)
      : info("Returns a character at the specified position <b>index</b>")
   {
      int len := self.Length;

      UTFOperations.convertUTF8_2_32(array, index, len, out int retVal);

      char ch := retVal;

      ^ ch;
   }
}

public program()
{
//   const byte[] arr := new const byte[] { 1, 3, 4 };

//   arr[0] := 3;
}
