import extensions;

public singleton HeapSortAlgorithm
{
   extension heapSort<T>(T[] array)
      = HeapSortAlgorithm::heapSort<T>(array, 0, array.Length, (l, r => l < r));
      
   extension heapSort<T>(T[] array, int offset, int length, Func<T, T, bool> comparison)
   {
      // build binary heap from all items
      for (int i := 0; i < length; i++) {
         int index := i;
         T item := array[offset + i]; // use next item
         
         // and move it on top, if greater than parent
         while (index > 0 &&
            comparison(array[offset + (index - 1) / 2], item))
         {
            int top := (index - 1) / 2;
            array[offset + index] := array[offset + top];
            index := top;
         };
         array[offset + index] := item;
      };
      
      for (int i := length - 1; i > 0; i--) {
         // delete max and place it as last
         T last := array[offset + i];
         array[offset + i] := array[offset];

         int index := 0;
         // the last one positioned in the heap
         while (index * 2 + 1 < i) {
            int left := index * 2 + 1;
            int right := left + 1;

            if (right < i && comparison(array[offset + left], array[offset + right]))
            {
               if:not (comparison(last, array[offset + right])) :break;

               array[offset + index] := array[offset + right];
               index := right;
            }
            else
            {
               if:not (comparison(last, array[offset + left])) :break;

               array[offset + index] := array[offset + left];
               index := left;
            }
         };
         array[offset + index] := last;
      }
   }      
}

public Program()
{
   byte[] r := new []{5, 4, 1, 2};

   HeapSortAlgorithm::heapSort<byte>(r);
   
   Console.printLine(r.asEnumerable())
}


