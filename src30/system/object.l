
// --- super object ---

#class Object
{
    #method equal &type'object : anObject
        = system'external'CoreAPI equal &type'object:$self &type'object:anObject.
                                                            
    #method equal : anObject
        = $self equal &type'object:anObject.
        
    #method notequal : anObject
        = $self equal:anObject not. 

    #method => #inline redirect.    
    
    #method <= &&type'object:$self.
}

// --- EException ---

#class EException
{
    #method => #inline handle_exception.
}

#class Action
{
    #method <= &&action:$self.
}

// --- nil constant ---

#symbol nil =
{
    equal : anObject = 
        anObject equal &type'object:$self.
}.
