
// --- super object ---

#class Object
{
    #method equal &type'object : anObject
        <object_equal>.

    #method equal : anObject
        = $self equal &type'object:anObject.

    #method notequal : anObject
        = $self equal:anObject not. 
    
    #method <= &&type'object:$self.

    #method => <redirect>.
}

// --- EException ---

#class(role) EException
{
    #method => <handle_exception>.
}

// --- Action wrapper ---

#class Action
{
    #method <= &&action:$self.
}

// --- Function wrapper ---

#class Function
{
    #method <= &&function:$self.
}

// --- nil constant ---

#symbol nil =
{
    equal : anObject = 
        anObject equal &type'object:$self.
}.
