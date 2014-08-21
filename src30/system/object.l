// --- super object ---

#class Object
{
    #method equal : anObject
        <= %equal &reference:anObject.

    #method equal &reference:anObject
        <object_equal>.
}

// --- nil constant ---

#symbol(type:nil) nil =
{
    clone = $self.
}.
