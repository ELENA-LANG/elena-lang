//#subject cellfunc = CellFunction.
#subject space = BaseSpace.

//#class(limited,dispatchable:cellfunc)CellFunction
//{
//    #method(stacksafe,action) eval &space:space &int:x &int:y
//    [
//    ]
//    
//    #method(stacksafe,action) eval:target &space:space &int:x &int:y
//    [
//        $self eval &space:space &int:x &int:y.
//    ]
//}

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
    
//    #method(stacksafe) setAt &int:x &int:y &int:cell
//    [
//    ]
//
//    /// Returns the space actual number of rows    
//    #method(stacksafe,outnprop:rows) readRows &vint:retVal
//    [
//    ]
//    
//    /// Returns the space actual number of columns
//    #method(stacksafe,outnprop:columns) readColumns &vint:retVal
//    [
//    ]
//    
//    #method scan:target &cellfunc:func
//    [
//    ]        
//        
//    #method scan &cellfunc:func
//    [
//        $self scan:$nil &cellfunc:func.
//    ]
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
        
//        $self scan &cellfunc:(&space:space &int:i &int:j) [ space setAt &int:i &int:j &int:0. ].
    ]

//    #constructor new &int:n &int:m &nnfunc:initFunc    
//        <= (new &int:n &int:m)
//    [
//        $self scan:initFunc &cellfunc:(:target&space:space &int:i &int:j)
//            [ space setAt &int:i &int:j &int:(target nnfunc eval &int:i &int:j). ].
//    ]
//
//    #constructor new &intmatrix:matrix
//    [
//        matrix readLengths &vint:theN &vint:theM.
//        
//        theMatrix := matrix.
//    ]
//
//    #method(stacksafe) read &int:y &int:x &vint:retVal
//    [
//        #var(int)i := x.
//        (i > theN)?
//        [
//            i := i mod &int:theN.
//        ].
//        #var(int)j := y.
//        (j > theM)?
//        [
//            j := j mod &int:theM.
//        ].
//        
//        theMatrix read &int:i &int:j &vint:retVal.
//    ]
//    
//    #method(stacksafe) setAt &int:y &int:x &int:cell
//    [
//        #var(int)i := y.
//        (i > theN)?
//        [
//            i := i mod &int:theN.
//        ].
//        #var(int)j := x.
//        (j > theM)?
//        [
//            j := j mod &int:theM.
//        ].
//        
//        theMatrix write &int:i &int:j &int:cell.
//    ]
//    
//    #method readRows &vint:retVal
//    [
//        retVal := theN.
//    ]
//    
//    #method readColumns &vint:retVal
//    [
//        retVal := theM.
//    ]
//    
//    #method scan:target &cellfunc:func
//    [
//        #var(int)i := 0.
//        #var(int)j := 0.
//        #loop (i < theN)?
//        [
//            j := 0.            
//            #loop (j < theM)?
//            [
//                func eval:target &space:$self &int:i &int:j.
//                
//                j += 1.
//            ].
//            
//            i += 1.
//        ].
//    ]
}
