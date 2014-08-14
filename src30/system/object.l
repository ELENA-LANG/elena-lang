// --- super object ---

#class(type:object) Object
{
    #method => <dispatch>.

    #method <= <typecast>.

    #method equal : anObject
        => &equal &object:anObject.

    #method equal &object:anObject
        <object_equal>.
}

// --- nil constant ---

#symbol(type:nil) nil =
{
    clone = $self.
}.
