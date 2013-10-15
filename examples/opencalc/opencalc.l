
#subject prop1. // ---> #symbol(subj:prop1) prop1 = {}.

#class Prop
{
    #field theValue.

    #method load : anObject
    [
        theValue := anObject get &:self.
    ]
            
    #method new : anObject
    [
        theValue := anObject.
    ]
    
    #annex theValue.
}

#class(subj:prop1) Prop1 (Prop)
{
}

#class TestObject
{
    #field theValue.
    
    #method prop1'set : anObject
    [
        theValue := anObject.
    ]
    
    #method prop1'get = theValue.
}

#symbol GroupObject = TestObject.



