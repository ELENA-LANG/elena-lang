public singleton InsertionAscendSorting<T>
{
    sort(T[] a, int start, int end)
    {
        for(int i := start + 1, i <= end, i += 1)
        {
            T entry := a[i];
            int j := i;
 
            while (j > start && a[j - 1] > entry)
            {
                a[j] := a[j - 1];
 
                j -= 1
            };
 
            a[j] := entry
        }
    }    
    
    sort(T[] a)
    {
        int length := a.Length;
        int last := length - 1;
        self.sort(a, 0, last);
    }    
}
