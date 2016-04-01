//// ==== ver 3.4.1 ===
//
//#import system.
//
//// --- Wrap ---
//
//#class(group) Wrap :: BaseArray
//{
//    #method => system'core_routines'handle_wrap.
//}
//
//// --- Extension ---
//
//#class(group) Extension :: BaseArray
//{
//    #method => system'core_routines'handle_extension.
//}
//
//// --- Group ---
//
//#class(group) Group :: BaseArray
//{
//    #method => system'core_routines'handle_group.
//}
//
//// --- extensions ---
//
//#class(extension) dynamicOp
//{
//    #method mix &into:role
//        = Extension(role, self).
//}