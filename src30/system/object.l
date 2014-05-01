
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

// --- EException ---

#class EException
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
