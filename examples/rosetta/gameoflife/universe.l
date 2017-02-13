#import system.

#subject space = BaseSpace.
#class(limited) BaseSpace
{
    #method(stacksafe) read &int:x &int:y &vint:retVal
    [        
    ]
    
    #method(int,w3,embeddable,stacksafe)getAt &int:x &int:y
    [
        #var(int)retVal.
        $self read &int:x &int:y &vint:retVal.
        
        ^ retVal.
    ]
    
    #method(stacksafe) setAt &int:x &int:y &int:cell
    [
    ]
}

#class IntMatrixSpace :: BaseSpace
{
    #field(intmatrix)theMatrix.
    #field(int)theN.
    #field(int)theM.
    
    #constructor new &int:n &int:m
    [
        theN := n.
        theM := m.
        
        theMatrix := IntMatrix new &int:n &int:m.
    ]

    #method(stacksafe) read &int:x &int:y &vint:retVal
    [
        #var(int)i := x.
        (i > theN)?
        [
            i := i mod &int:theN.
        ].
        #var(int)j := y.
        (j > theM)?
        [
            j := j mod &int:theM.
        ].
    ]
    
    #method(stacksafe) setAt &int:x &int:y &int:cell
    [
    ]
}
