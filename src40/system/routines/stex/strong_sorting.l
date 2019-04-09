public interface SortingAgent<T>
{
    prop int Length
    {
        abstract int get() {}
    }    
    
    abstract T at(int index) {}
    
    abstract exchange(int i, int j) {}
}

public sealed ArraySortingAgent<T> : SortingAgent<T>
{
    T[] a;
    
    int Length
    {
        int get()
        {
            int len := a.Length;
            
            ^ len
        }
    }
        
    constructor(T[] a)
    {
        this a := a
    }
    
    T at(int index)
        = a[index];

    exchange(int i, int j)
    {
        T tmp := a[i];
        a[i]:= a[j];
        a[j] := tmp
    }    
}

public singleton QuickSorting<T>
{
    private int partion(SortingAgent<T> a, int start, int end, int pivot, Func<T, T, bool> compf)
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

    private int median(SortingAgent<T> a, int i1, int i2, int i3, Func<T, T, bool> compf)
    {
        T t1 := a[i1];
        T t2 := a[i2];
        T t3 := a[i3];
        
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
                ^ (compf(t2, t3)) ? i3 : i2
            }
        }
    }
            
    sort(SortingAgent<T> a, int start, int end, Func<T, T, bool> compf)
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
    
    sort(SortingAgent<T> a, Func<T, T, bool> compf)
    {
        int length := a.Length;
        int last := length - 1;
        self.sort(a, 0, last, compf);
    }
}
