import system'collections;
import system'routines;

public singleton IfOrdered<T> : Func<T,T,bool>
{
   bool function(T f, T l)
      = f < l;
}

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

public extension quickSortOp<T> : Array<T>
{
   quickSort(Func<T,T,bool> compf)
   {
      QuickSortAlgorithm<T>.sort(new ArraySortingAgent<T>(self), 0, self.Length, compf);
   }

   quickSort()
   {
      QuickSortAlgorithm<T>.sort(new ArraySortingAgent<T>(self), 0, self.Length, ::IfOrdered<T>);
   }
}

public extension insertSortOp : Indexable
{
   insertSort(int length, Func2 compf)
   {
      InsertSortAlgorithm.sort(new IndexerSortingAgent(self), 0, length, compf);
   }

   insertSort(int length)
      = self.quickSort(length, ifOrdered);
}

public extension isnertSortOp2 : ArrayList
{
   insertSort(Func2 compf)
   {
      InsertSortAlgorithm.sort(new IndexerSortingAgent(self), 0, self.Length, compf);
   }

   insertSort()
   {
      InsertSortAlgorithm.sort(new IndexerSortingAgent(self), 0, self.Length, ifOrdered);
   }
}

public extension insertSortOp3
{
   insertSort(Func2 compf)
   {
      InsertSortAlgorithm.sort(new IndexerSortingAgent(self), 0, self.Length, compf);
   }

   insertSort()
   {
      InsertSortAlgorithm.sort(new IndexerSortingAgent(self), 0, self.Length, ifOrdered);
   }
}

public extension insertSortOp<T> : Array<T>
{
   insertSort(Func<T,T,bool> compf)
   {
      InsertSortAlgorithm<T>.sort(new ArraySortingAgent<T>(self), 0, self.Length, compf);
   }

   insertSort()
   {
      InsertSortAlgorithm<T>.sort(new ArraySortingAgent<T>(self), 0, self.Length, ::IfOrdered<T>);
   }
}