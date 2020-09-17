
public interface Space
{
    int LiveCell(int k, int l, int state)
    {
        int j := -1;
        int counter := 0;
        for (int i := -1, i < 2, i += 1)
        {
            for (int j := -1, j < 2, j += 1)
            {
                int cell := self.at(k + i, l + j);
                if (cell == state)
                    { counter := counter + 1 }
            }
        };
        
        ^ counter
    }        
    
    abstract int Rows
    {
        /// Returns the space actual number of rows
        get();
    }
    
    abstract int Columns
    {
        /// Returns the space actual number of columns
        get();
    }
    
    abstract update(RuleSet rs);
    
    abstract read(int i, int j, ref int retVal);

    embeddable int at(int i, int j)
    {
        int retVal := 0;
        self.read(i, j, ref retVal);
        
        ^ retVal
    }
}

public sealed class IntMatrixSpace : Space
{
    IntMatrix theMatrix;
    int       theN;
    int       theM;
    
    constructor allocate(int n, int m)
    {
        theN := n;
        theM := m;
        
        theMatrix := IntMatrix.allocate(n, m);

        for (int i := 0, i < theN, i+=1)
        {
            for (int j := 0, j < theM, j+=1)
            {
                theMatrix.setAt(i, j, 0)
            }
        }
    }

    constructor allocate(int n, int m, RuleSet rs)
        <= allocate(n, m)
    {
        self.internalUpdate(rs, theMatrix)
    }

    private internalUpdate(RuleSet rs, IntMatrix matrix)
    {
        for (int i := 0, i < theN, i += 1)
        {
            for(int j := 0, j < theM, j += 1)            
            {
                int cell := 0;
                rs.proceed(self, i, j, ref cell);
                    
                matrix.setAt(i, j, cell)
            }
        }
    }

    update(RuleSet rs)
    {
        IntMatrix newMatrix := IntMatrix.allocate(theN, theM);
        
        self.internalUpdate(rs, newMatrix);
        
        theMatrix := newMatrix
    }

    read(int k, int l, ref int retVal)
    {
        int i := k;
        if(i < 0)
        {
            i := theN + i
        };
        
        if(i >= theN)
        {
            i := i.mod:theN
        };
        
        int j := l;
        if(j < 0)
        {
            j := theM + j
        };
        if(j >= theM)
        {
            j := j.mod:theM
        };
        
        theMatrix.read(i, j, ref retVal)
    }
    
    int Rows
    {
        get()
        {
            ^ theN
        }
    }
    
    int Columns
    {
        get()
        {
            ^ theM
        }
    }
}
