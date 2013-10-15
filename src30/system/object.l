#define(const)true = system'true.
#define(const)false = system'false.

// --- super object ---

#class Object
{
    #method equal : anObject
        = $self equal &type'object:anObject.
        
    #method notequal : anObject
        = $self equal:anObject ? [ false ] ! [ true ]. 

    #method equal &type'object : anObject
        = system'external'CoreAPI equal &type'object:$self &type'object:anObject.
                    
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
        
    notequal : anObject = 
        anObject equal &type'object:$self ? [ false ] ! [ true ].
}.


