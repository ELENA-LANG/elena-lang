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
    #field theRole.
    #field theObject.
    
    #method => <handle_extension>.
}

// --- RoleArray ---

#class(group) RoleArray
{
    #field(type:array)theRoles.
    
    #method => <handle_roles>.
}

// --- ArrayExtension ---

#class(group) ArrayExtension
{
    #field theRoleArray.
    #field theObject.
    
    #method => <handle_arraygroupextension>.
}

