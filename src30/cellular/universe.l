
limited dispatchable(space) class BaseSpace
{
    update &ruleset:rs
    [
    ]
    
    stacksafe read &int:i &int:j &vint:retVal
    [        
    ]
    
    embeddable stacksafe int getAt &int:i &int:j
    [
        int retVal := 0.
        $self read &int:i &int:j &vint:retVal.
        
        ^ retVal.
    ]
    
    stacksafe nnfn(getLiveCell) calcLiveCells &int:k &int:l &vint:retVal
    [
        int i := -1.
        int j := -1.
        int counter := 0.
        while (i < 2)
        [
            j := -1.
            while (j < 2)
            [
                int cell := $self getAt &int:(k + i) &int:(l + j).
                (cell == 1)
                    ? [ counter += 1. ].
                
                j += 1.
            ].
            i += 1.
        ].
        
        retVal := counter.
    ]
    
    /// Returns the space actual number of rows    
    stacksafe outnprop(rows) readRows &vint:retVal
    [
    ]
    
    /// Returns the space actual number of columns
    stacksafe outnprop(columns) readColumns &vint:retVal
    [
    ]
}

sealed class IntMatrixSpace :: BaseSpace
{
    intmatrix theMatrix.
    int       theN.
    int       theM.
    
    constructor new &int:n &int:m
    [
        theN := n.
        theM := m.
        
        theMatrix := IntMatrix new &int:n &int:m.

        int i := 0.
        int j := 0.
        while (i < theN)
        [
            j := 0.            
            while(j < theM)
            [
                theMatrix write &int:i &int:j &int:0.
                
                j += 1.
            ].
            
            i += 1.
        ].                
    ]

    constructor new &int:n &int:m &ruleset:rs
        <= new &int:n &int:m;
    [
        $self $update &ruleset:rs &intmatrix:theMatrix.
    ]

    sealed $update &ruleset:rs &intmatrix:matrix
    [
        int i := 0.
        int j := 0.
        while (i < theN)
        [
            j := 0.            
            while(j < theM)
            [
                int cell := 0.
                rs validate &space:$self &int:i &int:j 
                    ? [ cell := 1. ].
                    
                matrix write &int:i &int:j &int:cell.
                
                j += 1.
            ].
            
            i += 1.
        ].
    ]

    update &ruleset:rs
    [
        intmatrix newMatrix := IntMatrix new &int:theN &int:theM.
        
        $self $update &ruleset:rs &intmatrix:newMatrix.
        
        theMatrix := newMatrix.
    ]

    stacksafe read &int:k &int:l &vint:retVal
    [
        int i := k.
        (i < 0)?
        [
            i := theN + i.
        ].
        
        (i >= theN)?
        [
            i := i mod &int:theN.
        ].
        
        int j := l.
        (j < 0)?
        [
            j := theM + j.
        ].
        (j >= theM)?
        [
            j := j mod &int:theM.
        ].
        
        theMatrix read &int:i &int:j &vint:retVal.
    ]
    
    readRows &vint:retVal
    [
        retVal := theN.
    ]
    
    readColumns &vint:retVal
    [
        retVal := theM.
    ]
}
