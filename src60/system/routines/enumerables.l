import system'collections;

// === actions ===

public ifOrdered
    = (f,l => f < l);

public ifDecrescent
    = (f,l => f > l);

public extension stringListOp : String
{
   string[] split()
      = self.splitBy($32);
    
   string[] splitBy(char ch)
   {
      List<string> list := new List<string>();
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

      ^ *list
   }
    
   string[] splitBy(string value)
   {
      List<string> list := new List<string>();
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
        
      ^ *list
   }

   string[] splitByNewLine()
   {
      List<string> list := new List<string>();
      int       start := 0;
      int       index := 0;
      int       valueLen := NewLineConstant.Length;
      int       literalLen := self.Length;
        
      while (index >= 0)
      {
         index := self.indexOf(start, NewLineConstant);
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
        
      ^ *list
   }
}

public extension wideListOp : WideString
{
   wide[] split()
      = self.splitBy($32);
    
   wide[] splitBy(char ch)
   {
      auto      list := new List<wide>();
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

      ^ *list
   }
    
   wide[] splitBy(wide value)
   {
      auto      list := new List<wide>();
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
        
      ^ *list
   }

   wide[] splitByNewLine()
   {
      auto      list := new List<wide>();
      int       start := 0;
      int       index := 0;
      int       literalLen := self.Length;
      wide      nlConst := NewLineConstant;
      int       valueLen := nlConst.Length;
        
      while (index >= 0)
      {
         index := self.indexOf(start, nlConst);
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
        
      ^ *list
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
      = self.forEach::(t => f(t.Item1,t.Item2));

   forEach(fn)
      = self.forEach::(n){ fn(n) };

   forEachPair(object object, Func2 f)
      = f.run(object, cast Enumerator(self.enumerator()));

   forEachPair(object,f)
      = self.forEachPair(object, (o,n){ f(o,n) });

   seekEach(Func1 f)
      = f.retrieve(cast Enumerator(self.enumerator()));

   seekEach(ExtensionMessage extMessage)
      = self.seekEach::(n => extMessage(n));
        
   seekEach(fn)
      = self.seekEach::(n => fn(n));

   seekEach(object param, Func2 f)
      = f.retrieve(param, cast Enumerator(self.enumerator()));

   filterBy(Func1 selector)
      = Filter.new(selector, cast Enumerator(self.enumerator()));

   filterBy(Func2 selector)
      = PatternFilter2.new(selector, cast Enumerator(self.enumerator()));
        
   filterBy(Func3 selector)
      = PatternFilter3.new(selector, cast Enumerator(self.enumerator()));
        
   filterBy(Func4 selector)
      = PatternFilter4.new(selector, cast Enumerator(self.enumerator()));
        
   filterBy(Func5 selector)
      = PatternFilter5.new(selector, cast Enumerator(self.enumerator()));
        
   filterBy(Func6 selector)
      = PatternFilter6.new(selector, cast Enumerator(self.enumerator()));

   selectBy(selector)
      = Selector.new(selector, cast Enumerator(self.enumerator()));

   groupBy(groupFunc)
      = GroupingEnumerator.new(groupFunc, cast Enumerator(self.enumerator()));

   orderBy(orderFunc)
      = OrderingEnumerator.new(orderFunc, cast Enumerator(self.enumerator()));

   top(counter)
      = TopFilter.new(cast int(counter), cast Enumerator(self.enumerator()));

   coundDown()
      = CountDownEnumerator.new(self);

   joinBy(object list, Func2 filter_f, Func2 select_f)
      = JoinEnumerator.new(filter_f, select_f, cast Enumerator(self.enumerator()), cast Enumerator(list.enumerator()));        

   repeatTo(n)
      = RangeEnumerator.new(self,n, 1);

   repeatTill(n)
      = RangeEnumerator.new(self, n - 1, 1);

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
            if:not(it2.next())
                { ^ false };
            
            if:not ((*it1).equal(*it2))
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
      auto en := cast Enumerator(self.enumerator());
      var  lastItem := nil;
        
      while (en.next())
         { lastItem := *en };
            
      ^ lastItem
   }

   zipBy(enumerable, zipper)
      = ZipEnumerator.new(zipper, cast Enumerator(self.enumerator()), cast Enumerator(enumerable.enumerator()));

   zipForEach(enumerable, func2)
      = self.zipBy(enumerable, func2).forEach(idleFunc1);

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
         if:not (f(*en))
         {
            ^ false
         }
      };
     
      ^ true
   }

   allMatchedBy(ExtensionMessage extMessage)
      = self.allMatchedBy::(n => extMessage(n));

   int countMembers()
   {
      auto   en := cast Enumerator(self.enumerator());
      int    counter := 0;
      while (en.next())
      {
         counter += 1
      };
        
      ^ counter
   }

   get MaximalMember()
   {
      auto en := cast Enumerator(self.enumerator());
        
      var maximal := nil;
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
        
      var maximal := nil;
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
      Indexable it := self;
      int len := it.Length;
      for (int i := 0; i < len; i++) {
         it[i] := f(i);
      };
        
      ^ self
   }

   forEvery(Action<int> fn)
   {
      Indexable it := self;
      int len := it.Length;
      for (int i := 0; i < len; i++) {
         fn(it[i])
      };
   }    
}

public extension indexableOp
{
   exchange(int i1, int i2)
   {
      var temp := self[i1];
      self[i1] := self[i2];
      self[i2] := temp; 
   }

   sort(Func2 compareFunc)
   {
      QuickSorting.sort(new IndexableSortingAgent(self), compareFunc);
   }

   populate(Func1 f)
   {
      Indexable it := self;
      
      int len := it.Length;
      for (int i := 0; i < len; i++) {
         it[i] := f(i);
      };
   }    

   populate(action)
      = self.populate::(int n){ ^action(n) };

   populate(ExtensionMessage ext_mssg)
      = self.populate::(n => ext_mssg(n));

   deepClone()
   {
      Indexable it := self;
      
      var copy := self.clone();

      int len := it.Length;
      for (int i := 0; i < len; i++) {
         copy[i] := it[i].clone();
      };
        
      ^ copy
   }

   ascendant()
      = self.clone().sort(ifOrdered);

   bool sequenceEqual(Indexable array)
   {
      Indexable it := self;
      
      if (it.Length != array.Length)
         { ^ false };

      int len := it.Length;
      for (int i := 0; i < len; i++) {
         if:not (it[i] == array[i])
            { ^ false };         
      };
          
      ^ true                
   }

   sequenceReverse()
      = self.sequenceReverse(0, self.Length);
            
   sequenceReverse(int index, int counter)
   {
      Indexable it := self;
        
      int      i := index;
      int      j := counter;
      j := j + i;
      j := j - 1;
        
      while (i < j)
      {
         var temp1 := it[i];
         var temp2 := it[j];

         it[j] := temp1;
         it[i] := temp2;
            
         i := i + 1;
         j := j - 1
      };
        
      ^ self
   }    

   int indexOfElement(object element)
   {
      Indexable it := self;
        
      int len := it.Length;
      for (int i := 0; i < len; i++) {
         if (element == it[i])
            { ^ i };         
      };
        
      ^ -1
   }

   bool isSorted(Func2 compareFunc)
   {
      Indexable it := self;
      int len := it.Length;
      if (len == 0)
         { ^ true };
      
      var previous := it[0];
      for (int i := 1; i < len; i++) {
         if:not (compareFunc(previous, it[i]))
         {
            ^ false
         };
         
         previous := it[i];
      };
        
      ^ true
   }

   isAscendant()
      = self.isSorted(ifOrdered);
}