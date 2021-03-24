public interface SortingAgent
{
    abstract int Length
    {
        get();
    }    
    
    abstract at(int index);
    
    abstract exchange(int i, int j);
}

public sealed ArraySortingAgent : SortingAgent
{
    field a;
    
    int Length
    {
        get()
        {
            int len := a.Length;
            
            ^ len
        }
    }
        
    constructor(a)
    {
        this a := a
    }
    
    at(int index)
        = a[index];

    exchange(int i, int j)
    {
        var tmp := a[i];
        a[i]:= a[j];
        a[j] := tmp
    }    
}

public sealed IndexerSortingAgent : SortingAgent
{
    Indexer a;
    
    int Length
    {
        get()
        {
            int len := a.Length;
            
            ^ len
        }
    }
        
    constructor(Indexer a)
    {
        this a := a
    }
    
    constructor(a)
        = cast Indexer(a.indexer());
        //<=();
    
    at(int index)
    {
        a.Index := index;
        
        ^ *a
    }

    exchange(int i, int j)
    {
        a.Index := i;
        var vi := *a;

        a.Index := j;
        var vj := *a;
        a.Value := vi;
        
        a.Index := i;
        a.Value := vj
    }    
}

public singleton QuickSorting
{
    private int partion(SortingAgent a, int start, int end, int pivot, Func2 compf)
    {
        a.exchange(pivot, start);
        int i := start + 1;
        for (int j := i, j <= end, j += 1)
        {
            if (compf(a[j], a[start]))
            {
                a.exchange(i, j);
                i += 1
            }
        };
        
        a.exchange(start, i - 1);
        
        ^ i - 1
    }

    private int median(SortingAgent a, int i1, int i2, int i3, Func2 compf)
    {
        var t1 := a[i1];
        var t2 := a[i2];
        var t3 := a[i3];
        
        if (compf(t1, t2))
        {
            if (compf(t2, t3))
            {
                ^ i2
            }
            else
            {
                ^ (compf(t1, t3)) ? i3 : i1
            }
        }
        else
        {
            if (compf(t1, t3))
            {
                ^ i1
            }
            else
            {
                ^ (compf(t2, t3)) ? i3 : i2/*i2 : i3*/
            }
        }
    }
            
    sort(SortingAgent a, int start, int end, Func2 compf)
    {
        int len := end - start;
        if (end <= start)
        {
            ^ self
        };
        
        int mid := len $shr 1;
        mid += start;
        int pivot := self.median(a, start, mid, end, compf);
        
        pivot := self.partion(a, start, end, pivot, compf);
        
        int end1 := pivot - 1;
        int start2 := pivot + 1;
        self.sort(a, start, end1, compf); 
        self.sort(a, start2, end, compf)
    }
    
    sort(SortingAgent a, Func2 compf)
    {
        int length := a.Length;
        int last := length - 1;
        self.sort(a, 0, last, compf);
    }
}
