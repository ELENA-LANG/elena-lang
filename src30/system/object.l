// --- super object ---

#class Object
{
    #constructor new []

    #method equal : anObject <object_equal>.
        
    #method => <dispatch>.
}

// --- common methods ---

#class(extension)objectOp
{
    #method equal &reference:anObject
        <object_equal>.
}

// --- nil constant ---

#symbol nil =
{
    clone = $self.
}.
