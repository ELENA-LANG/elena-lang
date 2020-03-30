public singleton QuickAscendSorting<T>
{
    private exchange(T[] a, int i, int j)
    {
        T tmp := a[i];
        a[i]:= a[j];
        a[j] := tmp
    }
    
    private int partion(T[] a, int start, int end, int pivot)
    {
        self.exchange(a, pivot, start);
        int i := start + 1;
        for (int j := i, j <= end, j += 1)
        {
            if (a[j] < a[start])
            {
                self.exchange(a, i, j);
                i += 1
            }
        };
        
        self.exchange(a, start, i - 1);
        
        ^ i - 1
    }

    private int median(T[] a, int i1, int i2, int i3)
    {
        T t1 := a[i1];
        T t2 := a[i2];
        T t3 := a[i3];
        
        if (t1 < t2)
        {
            if (t2 < t3)
            {
                ^ i2
            }
            else
            {
                ^ (t1 < t3) ? i3 : i1
            }
        }
        else
        {
            if (t1 < t3)
            {
                ^ i1
            }
            else
            {
                ^ (t2 < t3) ? i3 : i2
            }
        }
    }
            
    sort(T[] a, int start, int end)
    {
        int len := end - start;
        if (end <= start)
        {
            ^ self
        };
        
        int mid := len $shr 1;
        mid += start;
        int pivot := self.median(a, start, mid, end);
        
        pivot := self.partion(a, start, end, pivot);
        
        int end1 := pivot - 1;
        int start2 := pivot + 1;
        self.sort(a, start, end1); 
        self.sort(a, start2, end)
    }
    
    sort(T[] a)
    {
        int length := a.Length;
        int last := length - 1;
        self.sort(a, 0, last);
    }
}
