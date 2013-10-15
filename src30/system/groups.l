// --- Wrap ---

#class(group) Wrap
{
    #field theRole.    
    #field theObject.
    
    #method => #inline handle_wrap.
}

// --- Pair ---

#class(group) Pair
{
    #field theExtender.
    #field theObject.
    
    #method => #inline handle_pair.
}

// --- RoleList ---

#class(group) RoleList
{
    #field(type:array)theRoles.
    
    #method => #inline handle_rolebatch.
}