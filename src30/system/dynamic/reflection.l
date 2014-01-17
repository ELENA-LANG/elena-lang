#define system.

#symbol classControl =
{
    readLength &object:anObject &out'int:aLength
        <class_getLen>.
        
    read &index:anIndex &object:anObject
        <class_get>.
}.

// --- classOp ---

#class(role) classOp
{
    #method Name
        <class_getName>.

    #method length
    [
        #var(type:int)aCount := classControl readLength &object:self.
        
        ^ IntNumber new &int:aCount.
    ]

    #method getAt : anIndex
        = classControl read &index::anIndex &object:self.
}