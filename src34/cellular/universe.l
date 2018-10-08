
public interface class Space
{
    int fn3prop getLiveCell :: calcLiveCells.
    
    int rvprop rows         :: readRowsTo.
    int rvprop columns      :: readColumnsTo.
    
    abstract update(RuleSet rs)
    [
    ]
    
    abstract read(int i, int j, ref<int> retVal)
    [        
    ]
    
    embeddable int getAt(int i, int j)
    [
        int retVal := 0.
        self read(i, j, &retVal).
        
        ^ retVal
    ]
    
    calcLiveCells(int k, int l, int state, ref<int> retVal)
    [
        int i := -1.
        int j := -1.
        int counter := 0.
        while (i < 2)
        [
            j := -1.
            while (j < 2)
            [
                int cell := self getAt(k + i, l + j).
                if (cell == state)
                    [ counter := counter + 1 ].
                
                j := j + 1
            ].
            i := i + 1
        ].
        
        retVal value := counter
    ]
    
    /// Returns the space actual number of rows    
    abstract readRowsTo(ref<int> retVal)
    [
    ]
    
    /// Returns the space actual number of columns
    abstract readColumnsTo(ref<int> retVal)
    [
    ]
}

public sealed class IntMatrixSpace :: Space
{
    T<IntMatrix> theMatrix.
    int          theN.
    int          theM.
    
    constructor new(int n, int m)
    [
        theN := n.
        theM := m.
        
        theMatrix := IntMatrix new(n, m).

        int i := 0.
        int j := 0.
        while (i < theN)
        [
            j := 0.            
            while(j < theM)
            [
                theMatrix write(i, j, 0).
                
                j := j + 1
            ].
            
            i := i + 1
        ]         
    ]

    constructor new(int n, int m, RuleSet rs)
        <= new(n, m);
    [
        self internalUpdate(rs, theMatrix).
    ]

    private internalUpdate(RuleSet rs, IntMatrix matrix)
    [
        int i := 0.
        int j := 0.
        while (i < theN)
        [
            j := 0.            
            while(j < theM)
            [
                int cell := 0.
                rs proceed(self, i, j, &cell).
                    
                matrix write(i, j, cell).
                
                j := j + 1
            ].
            
            i := i + 1
        ].
    ]

    update(RuleSet rs)
    [
        T<IntMatrix> newMatrix := IntMatrix new(theN, theM).
        
        self internalUpdate(rs, newMatrix).
        
        theMatrix := newMatrix.
    ]

    read(int k, int l, ref<int> retVal)
    [
        int i := k.
        (i < 0)?
        [
            i := theN + i.
        ].
        
        (i >= theN)?
        [
            i := i mod(theN).
        ].
        
        int j := l.
        (j < 0)?
        [
            j := theM + j.
        ].
        (j >= theM)?
        [
            j := j mod(theM).
        ].
        
        theMatrix read(i, j, retVal).
    ]
    
    readRowsTo(ref<int> retVal)
    [
        retVal value := theN.
    ]
    
    readColumnsTo(ref<int> retVal)
    [
        retVal value := theM.
    ]
}
