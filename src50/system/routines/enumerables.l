import system'collections;

// === actions ===

public ifOrdered
    = (f,l => f < l);

public ifDecrescent
    = (f,l => f > l);

// --- literalSplitOp ---

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

// --- wideListOp ---

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
        
    Array splitBy(wide value)
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

//    forEach(Func2 f)
//        = self.forEach:(t => f(t.Item1,t.Item2));
//
//    forEach(ExtensionMessage extMessage)
//        = self.forEach:(n => extMessage(n));
        
    forEach(fn)
        = self.forEach:(n){ fn(n) };

    selectBy(selector)
        = Selector.new(selector, cast Enumerator(self.enumerator()));

//    forEachPair(object object, ExtensionMessage extMessage)
//        = self.forEachPair(object, (o,n => extMessage(o,n)));
//    
//    forEachPair(object object, Func2 f)
//      = f.run(object, cast Enumerator(self.enumerator()));
//    
//    forEachPair(object,f)
//        = self.forEachPair(object, (o,n){ f(o,n) });

    seekEach(Func1 f)
        = f.retrieve(cast Enumerator(self.enumerator()));

    seekEach(ExtensionMessage extMessage)
        = self.seekEach:(n => extMessage(n));
        
    seekEach(fn)
        = self.seekEach:(n => fn(n));

    seekEach(object param, Func2 f)
        = f.retrieve(param, cast Enumerator(self.enumerator()));

    safeSeek(val)
        = (x => val.safeEqual(x)).retrieve(cast Enumerator(self.enumerator()));

    seekFor(val)
        = (x => val.equal(x)).retrieve(cast Enumerator(self.enumerator()));

    bool ifExists(val)
        = nil != self.seekFor(val);

//    bool sequenceEqual(Enumerable list)
//    {
//        Enumerator it1 := self.enumerator();
//        Enumerator it2 := list.enumerator();
//        
//        while (it1.next())
//        {
//            ifnot(it2.next())
//                { ^ false };
//            
//            ifnot (it1.get().equal(it2.get()))
//                { ^ false }
//        };
//        
//        if(it2.next())
//            { ^ false };
//        
//        ^ true
//    }
//             
//    get FirstMember()
//    {
//        auto en := cast Enumerator(self.enumerator());
//        
//        if (en.next()) 
//        {
//            ^ en.get()
//        }
//        else
//        {
//            ^ nil
//        }
//    }
//
//    get MaximalMember()
//    {
//        auto en := cast Enumerator(self.enumerator());
//        
//        object maximal := nil;
//        while (en.next())
//        {
//            var item := en.get();
//            if (nil == maximal)
//            {
//                maximal := item
//            }
//            else if (maximal < item)
//            {
//                maximal := item
//            }
//        };
//        
//        ^ maximal   
//    }
//    
//    get MinimalMember()
//    {
//        auto en := cast Enumerator(self.enumerator());
//        
//        object maximal := nil;
//        while (en.next())
//        {
//            var item := en.get();
//            if (nil == maximal)
//            {
//                maximal := item
//            }
//            else if (maximal > item)
//            {
//                maximal := item
//            }
//        };
//        
//        ^ maximal   
//    }
    
    get LastMember()
    {
        auto   en := cast Enumerator(self.enumerator());
        object lastItem := nil;
        
        while (en.next())
            { lastItem := *en };
            
        ^ lastItem
    }
    
//    int countMembers()
//    {
//        auto   en := cast Enumerator(self.enumerator());
//        int    counter := 0;
//        while (en.next())
//        {
//            counter += 1
//        };
//        
//        ^ counter
//    }
//    
//    allMatchedBy(Func1 f)
//    {
//        auto   en := cast Enumerator(self.enumerator());
//        while (en.next())
//        {
//            ifnot (f(en.get()))
//            {
//                ^ false
//            }
//        };
//        
//        ^ true
//    }
//    
//    allMatchedBy(ExtensionMessage extMessage)
//        = self.allMatchedBy:(n => extMessage(n));
//
//    summarize(var)
//    {
//        new Summing(var).run(cast Enumerator(self.enumerator()));
//        
//        ^ var.Value
//    }
//    
//    summarize()
//        = self.summarize(new Variable());
//        
//    filterBy(Func1 selector)
//        = Filter.new(selector, cast Enumerator(self.enumerator()));
//        
//    filterBy(Func2 selector)
//        = PatternFilter2.new(selector, cast Enumerator(self.enumerator()));
//        
//    filterBy(Func3 selector)
//        = PatternFilter3.new(selector, cast Enumerator(self.enumerator()));
//        
//    filterBy(Func4 selector)
//        = PatternFilter4.new(selector, cast Enumerator(self.enumerator()));
//        
//    filterBy(Func5 selector)
//        = PatternFilter5.new(selector, cast Enumerator(self.enumerator()));
//        
//    filterBy(Func6 selector)
//        = PatternFilter6.new(selector, cast Enumerator(self.enumerator()));
//        
//    repeatTill(n)
//        = RangeEnumerator.new(self, n - 1, 1);
//
//    repeatTo(n)
//        = RangeEnumerator.new(self,n, 1);
//
//    top(counter)
//        = TopFilter.new(cast int(counter), cast Enumerator(self.enumerator()));
//        
//    accumulate(variable, operation)
//    {
//        Accumulating.new(variable, operation).run(cast Enumerator(self.enumerator()));
//        
//        ^ variable.Value
//    }
//    
//    zipBy(enumerable, zipper)
//        = ZipEnumerator.new(zipper, cast Enumerator(self.enumerator()), cast Enumerator(enumerable.enumerator()));
//
//    zipForEach(enumerable, func2)
//        = self.zipBy(enumerable, func2).forEach:idleFunc1;
//        
//    orderBy(orderFunc)
//        = OrderingEnumerator.new(orderFunc, cast Enumerator(self.enumerator()));
//        
//    groupBy(groupFunc)
//        = GroupingEnumerator.new(groupFunc, cast Enumerator(self.enumerator()));
//        
//    joinBy(object list, Func2 filter_f, Func2 select_f)
//        = JoinEnumerator.new(filter_f, select_f, cast Enumerator(self.enumerator()), cast Enumerator(list.enumerator()));        
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
        
//    forEvery(Func<int,T> fn)
//    {
//        Indexer it := self.indexer();
//        
//        while (it.Available)
//        {
//            int i := it.Index;
//            
//            fn(i);
//            
//            it.appendIndex:1
//        }
//    }    
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
    
    int indexOfElement(element)
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
    
//    deepClone()
//    {
//        var copy := self.clone();
//        
//        Indexer indexer := copy.indexer();
//        
//        while(indexer.Available)
//        {
//            indexer.set(indexer.get().clone());
//            
//            indexer.appendIndex(1)
//        };
//        
//        ^ copy
//    }
//        
//    populate(ExtensionMessage ext_mssg)
//        = self.populate:(n => ext_mssg(n));

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

    sort(Func2 compareFunc)
    {
        QuickSorting.sort(new IndexerSortingAgent(self), compareFunc);
    }
    
    ascendant()
        = self.clone().sort(ifOrdered);

    isAscendant()
        = self.isSorted(ifOrdered);

//    Subarray(int index, int length)
//    {
//        auto it := cast Indexer(self.indexer());
//        
//        ^ Array.allocate(length).populate:(int i){ it.Index := index + i; ^ it.get()}
//    }
//    
//    bool sequenceEqual(array)
//    {
//        if (self.Length != array.Length)
//            { ^ false };
//            
//        Indexer a := self.indexer();
//        Indexer b := array.indexer();
//        
//        while (a.Available)
//        {
//            ifnot (a.get() == b.get())
//                { ^ false };
//                
//            a.appendIndex:1;
//            b.appendIndex:1
//        };
//          
//        ^ true                
//    }
//            
//    sequenceReverse()
//        = self.sequenceReverse(0, self.Length);
//            
//    sequenceReverse(int index, int counter)
//    {
//        Indexer it := self.indexer();
//        
//        int      i := index;
//        int      j := counter;
//        j := j + i;
//        j := j - 1;
//        
//        while (i < j)
//        {
//            it.Index := i;
//            var temp1 := it.get();
//            it.Index := j;
//            var temp2 := it.get();
//            it.set:temp1;
//            it.Index := i;
//            it.set:temp2;
//            
//            i := i + 1;
//            j := j - 1
//        };
//        
//        ^ self
//    }    
}
