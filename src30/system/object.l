
// --- super object ---

#class Object
{
    #method equal : anObject
        <object_equal>.

    #method notequal : anObject
        = $self equal:anObject not.
                        
    #method equal &object:anObject
        <object_equal>.
        
    #method <= &object:$self.

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
    #method <= &action:$self.
}

// --- Function wrapper ---

#class Function
{
    #method <= &function:$self.
}

// --- nil constant ---

#symbol nil =
{
    <= &nil:$self.
}.
