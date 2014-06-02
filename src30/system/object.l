
// --- super object ---

#class Object
{
    #method => <redirect>.
    
    #method equal : anObject
        => &equal &object:anObject.

    #method equal &object:anObject
        <object_equal>.

    #method <= &object:$self.
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
    clone = $self.
    
    <= &nil:$self.
}.
