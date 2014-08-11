//// --- ETapeControl --
//
//#symbol tapeControl =
//{
//    read :anIndex ::Params
//    [
//        #var n := Params@(anIndex int).
//        
//        ^ n.
//    ]
//    
//    //    = Params@(anIndex int).
//}.

// --- Tape ---

#class(group,dbg:array) Tape
{
    #field(item)theBody.
    
    #constructor => <arr_create>.

    #method => <handle_tape>.
}

// --- Struct ---

#class Struct : Tape
{
    #constructor => <arr_create>.

    #method => <handle_selector>.
}

//// --- Class ---
//
//#class(group) Class
//{
//    #field(type:array)theBody.
//    
//    #method => <handle_class>.
//}
