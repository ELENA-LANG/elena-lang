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
}