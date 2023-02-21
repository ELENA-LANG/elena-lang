import system;
import system'collections;

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
}

// --- indexableOp ---

public extension indexableOp
{
   sort(Func2 compareFunc)
   {
      QuickSorting.sort(new IndexerSortingAgent(self), compareFunc);
   }

}