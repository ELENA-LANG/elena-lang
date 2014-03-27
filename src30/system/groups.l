// --- Wrap ---

#class(group) Wrap
{
    #field theRole.    
    #field theObject.
    
    #method => <handle_wrap>.
}

// --- Roles ---

#class(group) Roles
{
    #field(type:array)theRoles.
    
    #method => <handle_roles>.
}

// --- GroupExtension ---

#class(group) GroupExtension
{
    #field theRole.
    #field theObject.
    
    #method => <handle_extension>.
}

// --- ArrayGroupExtension ---

#class(group) ArrayGroupExtension
{
    #field theRoleArray.
    #field theObject.
    
    #method => <handle_arraygroupextension>.
}

// --- Extension ---

#class(group) Extension
{
    #field theRole.
    #field theObject.
    
    #method => <handle_extension>.
}

//// --- ArrayExtension ---
//
//#class(group) ArrayExtension
//{
//    #field theRoleArray.
//    #field theObject.
//    
//    #method => <handle_arrayextension>.
//}
