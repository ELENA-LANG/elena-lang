#define system.

// --- Wrap ---

#class(group,dbg:array) Wrap : BaseArray
{
    #method => <handle_wrap>.
}

// --- Group ---

#class(group,dbg:array) Group : BaseArray
{
    #method => <handle_group>.
}

//// --- Extension ---
//
//#class(group) Extension
//{
//    #field theRole.
//    #field theObject.
//    
//    #method => <handle_extension>.
//}
//
//// --- RoleArray ---
//
//#class(group) RoleArray
//{
//    #field(type:array)theRoles.
//    
//    #method => <handle_roles>.
//}
//
//// --- ArrayExtension ---
//
//#class(group) ArrayExtension
//{
//    #field theRoleArray.
//    #field theObject.
//    
//    #method => <handle_arraygroupextension>.
//}
//
