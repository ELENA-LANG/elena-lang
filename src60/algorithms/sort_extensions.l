import system'collections;
import system'routines;

public extension quickSortOp : Indexable
{
   quickSort(int length, Func2 compf)
   {
      QuickSortAlgorithm.sort(new IndexerSortingAgent(self), 0, length, compf);
   }

   quickSort(int length)
      = self.quickSort(length, ifOrdered);
}

public extension quickSortOp2 : ArrayList
{
   quickSort(Func2 compf)
   {
      QuickSortAlgorithm.sort(new IndexerSortingAgent(self), 0, self.Length, compf);
   }

   quickSort()
   {
      QuickSortAlgorithm.sort(new IndexerSortingAgent(self), 0, self.Length, ifOrdered);
   }
}

public extension quickSortOp3
{
   quickSort(Func2 compf)
   {
      QuickSortAlgorithm.sort(new IndexerSortingAgent(self), 0, self.Length, compf);
   }

   quickSort()
   {
      QuickSortAlgorithm.sort(new IndexerSortingAgent(self), 0, self.Length, ifOrdered);
   }
}