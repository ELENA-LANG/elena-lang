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

    #method Class
        <class_getClass>.

    #method respondsTo &message:aMessage
        <class_indexOfMsg>.

    #method length
    [
        #var(type:int)aCount.
        classControl readLength &object:self &out'int:aCount.
        
        ^ IntNumber new &int:aCount.
    ]

    #method getAt : anIndex
        = classControl read &index::anIndex &object:self.
}