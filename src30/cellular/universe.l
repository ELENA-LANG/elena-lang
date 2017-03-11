#subject cellfunc = CellFunction.
#subject space = BaseSpace.

limited dispatchable(cellfunc) CellFunction
{
    stacksafe action eval &space:space &int:x &int:y
    [
    ]
    
    stacksafe action eval:target &space:space &int:x &int:y
    [
        $self eval &space:space &int:x &int:y.
    ]
}

limited dispatchable(space) class BaseSpace
{
    stacksafe read &int:x &int:y &vint:retVal
    [        
    ]
    
    embeddable stacksafe int getAt &int:x &int:y
    [
        int retVal := 0.
        $self read &int:x &int:y &vint:retVal.
        
        ^ retVal.
    ]
    
    stacksafe setAt &int:x &int:y &int:cell
    [
    ]

    /// Returns the space actual number of rows    
    stacksafe outnprop(rows) readRows &vint:retVal
    [
    ]
    
    /// Returns the space actual number of columns
    stacksafe outnprop(columns) readColumns &vint:retVal
    [
    ]
    
    scan:target &cellfunc:func
    [
    ]        
        
    scan &cellfunc:func
    [
        $self scan:$nil &cellfunc:func.
    ]
}

class IntMatrixSpace :: BaseSpace
{
    intmatrix theMatrix.
    int       theN.
    int       theM.
    
    constructor new &int:n &int:m
    [
        theN := n.
        theM := m.
        
        theMatrix := IntMatrix new &int:n &int:m.
        
        $self scan &cellfunc:(&space:space &int:i &int:j) [ space setAt &int:i &int:j &int:0. ].
    ]

    constructor new &int:n &int:m &nnfunc:initFunc    
        <= new &int:n &int:m;
    [
        $self scan:initFunc &cellfunc:
            (:target &space:space &int:i &int:j)
                [ space setAt &int:i &int:j &int:(target nnfunc eval &int:i &int:j). ].
    ]

    constructor new &intmatrix:matrix
    [
        matrix readLengths &vint:theN &vint:theM.
        
        theMatrix := matrix.
    ]

    stacksafe read &int:y &int:x &vint:retVal
    [
        int i := x.
        (i > theN)?
        [
            i := i mod &int:theN.
        ].
        int j := y.
        (j > theM)?
        [
            j := j mod &int:theM.
        ].
        
        theMatrix read &int:i &int:j &vint:retVal.
    ]
    
    stacksafe setAt &int:y &int:x &int:cell
    [
        int i := y.
        (i > theN)?
        [
            i := i mod &int:theN.
        ].
        int j := x.
        (j > theM)?
        [
            j := j mod &int:theM.
        ].
        
        theMatrix write &int:i &int:j &int:cell.
    ]
    
    readRows &vint:retVal
    [
        retVal := theN.
    ]
    
    readColumns &vint:retVal
    [
        retVal := theM.
    ]
    
    scan:target &cellfunc:func
    [
        int i := 0.
        int j := 0.
        while (i < theN)
        [
            j := 0.            
            while(j < theM)
            [
                func eval:target &space:$self &int:i &int:j.
                
                j += 1.
            ].
            
            i += 1.
        ].
    ]
}
