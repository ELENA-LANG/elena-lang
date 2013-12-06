// --- Wrap ---

#class(group) Wrap
{
    #field theRole.    
    #field theObject.
    
    #method => <handle_wrap>.
}

// --- Extension ---

#class(group) Extension
{
    #field theExtender.
    #field theObject.
    
    #method => <handle_pair>.
}

// --- RoleList ---

#class(group) RoleList
{
    #field(type:array)theRoles.
    
    #method => <handle_rolebatch>.
}