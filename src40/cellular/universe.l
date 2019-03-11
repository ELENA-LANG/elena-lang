
public interface Space
{
    fn3prop int LiveCell
    {
        calc(int k, int l, int state, ref int retVal)
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
            
            retVal := counter
        }        
    }
    
    vprop int Rows
    {
        /// Returns the space actual number of rows
        abstract readTo(ref int retVal) {}
    }
    
    vprop int Columns
    {
        /// Returns the space actual number of columns
        abstract readTo(ref int retVal) {}
    }
    
    abstract update(RuleSet rs) {}
    
    abstract read(int i, int j, ref int retVal) {}

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
    
    constructor(int n, int m)
    {
        theN := n;
        theM := m;
        
        theMatrix := new IntMatrix(n, m);

        for (int i := 0, i < theN, i+=1)
        {
            for (int j := 0, j < theM, j+=1)
            {
                theMatrix.write(i, j, 0)
            }
        }
    }

//    constructor new(int n, int m, RuleSet rs)
//        <= new(n, m);
//    [
//        self internalUpdate(rs, theMatrix).
//    ]
//
//    private internalUpdate(RuleSet rs, IntMatrix matrix)
//    [
//        int i := 0.
//        int j := 0.
//        while (i < theN)
//        [
//            j := 0.            
//            while(j < theM)
//            [
//                int cell := 0.
//                rs proceed(self, i, j, &cell).
//                    
//                matrix write(i, j, cell).
//                
//                j := j + 1
//            ].
//            
//            i := i + 1
//        ].
//    ]

    update(RuleSet rs)
    {
        IntMatrix newMatrix := new IntMatrix(theN, theM);
        
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
    
    Rows
    {
        readTo(ref int retVal)
        {
            retVal := theN
        }
    }
    
    Columns
    {
        readTo(ref int retVal)
        {
            retVal := theM
        }
    }
}
