// --- super object ---

#class Object
{
    #method equal : anObject
        <object_equal>.
}

// --- common methods ---

#class(extension)objectOp
{
    #method equal &reference:anObject
        <object_equal>.
}

// --- nil constant ---

#symbol(type:nil) nil =
{
    clone = $self.
}.
