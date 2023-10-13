import system;
import system'collections;

// === actions ===

public ifOrdered
    = (f,l => f < l);

public ifDecrescent
    = (f,l => f > l);

public extension stringListOp : String
{
   Array split()
      = self.splitBy($32);
    
   Array splitBy(char ch)
   {
      ArrayList list := new ArrayList();
      int       start := 0;
      int       index := 0;
      int       chLen := ch.Length;

      while (index >= 0)
      {
         index := self.indexOf(start, ch);
         if (-1 != index)
         {
            list.append(self.Substring(start,index - start));
            start := index + chLen
         }
         else
         {
            list.append(self.Substring(start))
         }
      };

      ^ list
   }
    
   Array splitBy(string value)
   {
      ArrayList list := new ArrayList();
      int       start := 0;
      int       index := 0;
      int       valueLen := value.Length;
      int       literalLen := self.Length;
        
      while (index >= 0)
      {
         index := self.indexOf(start, value);
         if (-1 != index)
         {
            list.append(self.Substring(start, index - start));
            start := index + valueLen
         }
         else
         {
            list.append(self.Substring(start, literalLen - start))
         }
      };
        
      ^ list
   }

}

public extension wideListOp : WideString
{
   Array split()
      = self.splitBy($32);
    
   Array splitBy(char ch)
   {
      ArrayList list := new ArrayList();
      int       start := 0;
      int       index := 0;
      int       chLen := ch.Length;

      while (index >= 0)
      {
         index := self.indexOf(start, ch);
         if (-1 != index)
         {
            list.append(self.Substring(start,index - start));
            start := index + chLen
         }
         else
         {
            list.append(self.Substring(start))
         }
      };

      ^ list
   }
}

// --- enumeratorOp ---

public extension enumeratorOp : Enumerator
{
   forEach(Func1 f)
      = f.run(self);

   selectBy(selector)
      = Selector.new(selector, self);
}

// --- enumerableOp ---

public extension enumerableOp
{
   forEach(Func1 f)
      = f.run(cast Enumerator(self.enumerator()));

   forEach(Func2 f)
      = self.forEach:(t => f(t.Item1,t.Item2));

   forEach(fn)
      = self.forEach:(n){ fn(n) };

   seekEach(Func1 f)
      = f.retrieve(cast Enumerator(self.enumerator()));

   seekEach(ExtensionMessage extMessage)
      = self.seekEach:(n => extMessage(n));
        
   seekEach(fn)
      = self.seekEach:(n => fn(n));

   seekEach(object param, Func2 f)
      = f.retrieve(param, cast Enumerator(self.enumerator()));

   filterBy(Func1 selector)
      = Filter.new(selector, cast Enumerator(self.enumerator()));

   selectBy(selector)
      = Selector.new(selector, cast Enumerator(self.enumerator()));

   groupBy(groupFunc)
      = GroupingEnumerator.new(groupFunc, cast Enumerator(self.enumerator()));

   orderBy(orderFunc)
      = OrderingEnumerator.new(orderFunc, cast Enumerator(self.enumerator()));

   top(counter)
      = TopFilter.new(cast int(counter), cast Enumerator(self.enumerator()));

   summarize(var)
   {
      new Summing(var).run(cast Enumerator(self.enumerator()));

      ^ var.Value
   }

   summarize()
      = self.summarize(new Variable());

   seekFor(val)
      = (x => val.equal(x)).retrieve(cast Enumerator(self.enumerator()));

   bool ifExists(val)
      = nil != self.seekFor(val);

   bool sequenceEqual(Enumerable list)
   {
      Enumerator it1 := self.enumerator();
      Enumerator it2 := list.enumerator();
        
      while (it1.next())
      {
            ifnot(it2.next())
                { ^ false };
            
            ifnot ((*it1).equal(*it2))
                { ^ false }
      };
        
      if(it2.next())
         { ^ false };
        
      ^ true
   }

   get FirstMember()
   {
      auto en := cast Enumerator(self.enumerator());

      if (en.next()) 
      {
         ^ *en
      }
      else
      {
         ^ nil
      }
   }

   get LastMember()
   {
      auto   en := cast Enumerator(self.enumerator());
      object lastItem := nil;
        
      while (en.next())
         { lastItem := *en };
            
      ^ lastItem
   }

   repeatTill(n)
      = RangeEnumerator.new(self, n - 1, 1);

   zipBy(enumerable, zipper)
      = ZipEnumerator.new(zipper, cast Enumerator(self.enumerator()), cast Enumerator(enumerable.enumerator()));

   zipForEach(enumerable, func2)
      = self.zipBy(enumerable, func2).forEach:idleFunc1;

   accumulate(variable, operation)
   {
      Accumulating.new(variable, operation).run(cast Enumerator(self.enumerator()));

      ^ variable.Value
   }

   bool allMatchedBy(Func1 f)
   {
      auto en := cast Enumerator(self.enumerator());
      while (en.next())
      {
         ifnot (f(*en))
         {
            ^ false
         }
      };
     
      ^ true
   }

   get MaximalMember()
   {
      auto en := cast Enumerator(self.enumerator());
        
      object maximal := nil;
      while (en.next())
      {
            var item := *en;
            if (nil == maximal)
            {
                maximal := item
            }
            else if (maximal < item)
            {
                maximal := item
            }
      };
        
      ^ maximal   
   }
    
   get MinimalMember()
   {
      auto en := cast Enumerator(self.enumerator());
        
      object maximal := nil;
      while (en.next())
      {
         var item := *en;
         if (nil == maximal)
         {
            maximal := item
         }
         else if (maximal > item)
         {
            maximal := item
         }
      };
        
      ^ maximal   
   }

}

// --- indexableOp ---

public extension indexableOp<T>
{
   populate(Func<int,T> f)
   {
      Indexer it := self.indexer();

      while (it.Available)
      {
         int i := it.Index;
            
         it.Value := f(i);
            
         it.appendIndex:1
      };
        
      ^ self
   }

   forEvery(Func<int,T> fn)
   {
      Indexer it := self.indexer();
        
      while (it.Available)
      {
         int i := it.Index;
            
         fn(i);
            
         it.appendIndex:1
      }
   }    
}

public extension indexableOp
{
   exchange(int i1, int i2)
   {
      Indexer indexer := self.indexer();
        
      indexer.Index := i1;
        
      var o1 := *indexer;
        
      indexer.Index := i2;
      var o2 := *indexer;
        
      indexer.Value := o1;
        
      indexer.Index := i1;
      indexer.Value := o2
   }

   sort(Func2 compareFunc)
   {
      QuickSorting.sort(new IndexerSortingAgent(self), compareFunc);
   }

   populate(Func1 f)
   {
      Indexer it := self.indexer();

      while (it.Available)
      {
         int i := it.Index;
            
         it.Value := f(i);
            
         it.appendIndex:1
      };
   }    

   populate(action)
      = self.populate:(int n){ ^action(n) };

   ascendant()
      = self.clone().sort(ifOrdered);

   bool sequenceEqual(array)
   {
      if (self.Length != array.Length)
         { ^ false };
            
      Indexer a := self.indexer();
      Indexer b := array.indexer();
        
      while (a.Available)
      {
         ifnot ((*a) == (*b))
            { ^ false };
                
         a.appendIndex:1;
         b.appendIndex:1
      };
          
      ^ true                
   }

   sequenceReverse()
      = self.sequenceReverse(0, self.Length);
            
   sequenceReverse(int index, int counter)
   {
      Indexer it := self.indexer();
        
      int      i := index;
      int      j := counter;
      j := j + i;
      j := j - 1;
        
      while (i < j)
      {
            it.Index := i;
            var temp1 := *it;
            it.Index := j;
            var temp2 := *it;
            it.Value := temp1;
            it.Index := i;
            it.Value := temp2;
            
            i := i + 1;
            j := j - 1
      };
        
      ^ self
   }    

   int indexOfElement(object element)
   {
      Indexer it := self.indexer();
        
      while (it.Available)
      {
         if (element == (*it))
            { ^ it.Index };
            
         it.appendIndex:1;
      };
        
      ^ -1
   }

   bool isSorted(Func2 compareFunc)
   {
      auto it := cast Indexer(self.indexer());
      if (it.Available)
      {
         var previous := *it;
         it.appendIndex:1;
         while (it.Available)
         {
            ifnot (compareFunc(previous, *it))
            {
               ^ false
            };
                
            previous := *it;
            it.appendIndex:1
         }
      };
        
      ^ true
   }

   isAscendant()
      = self.isSorted(ifOrdered);
}