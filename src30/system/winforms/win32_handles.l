// --- HInstance ---

#class(sealed,integer:4) HInstance
{
    #constructor new &handle:aHandle
        => system'internal'int_copy.

    #method hinstance = $self.
}
