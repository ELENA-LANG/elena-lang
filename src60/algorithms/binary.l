public singleton BinarySearchAlgorithm<T>
{
   int search(SortingAgent<T> source, T value)
   {
      int low := 0;
      int high := source.Length - 1;
      while (low <= high) {
         int mid := (low + high) / 2;
         if (source[mid] > value) {
            high := mid - 1;
         }
         else if (source[mid] < value) {
            low := mid + 1;
         }
         else {
            ^ mid
         }
      };

      ^ -1 // indicate not found       
   }
}