// --- PrimitiveOperations ---

internal singleton PrimitiveOperations
{
   copy(byte[] dest, int index, int size, byte[] sour)
   {
      for (int i := 0, i < size, i := i + 1) {
         dest[i] := sour[index + i];
      }
   }
}
