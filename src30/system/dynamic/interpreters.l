// --- ETapeControl --

#symbol tapeControl =
{
    // NOTE: open argument list first argument is the index, better to have a normal argument and an open argument list
    getAt :: Params
//        = Params@(() type'int add:1).
    [
        #var anIndex := Params@0 + 1.
        
        ^ Params@(anIndex type'int).
    ]    
    
    getFirst :: Params
        = Params@1.
    
//    get : aParam
//        = aParam.
//        
//    get :aDummy:aParam
//        = aParam.
//        
//    get :aDummy1:aDummy2:aParam
//        = aParam.
//        
//    get :aDummy1:aDummy2:aDummy3:aParam
//        = aParam.
//        
//    get :aDummy1:aDummy2:aDummy3:aDummy4:aParam
//        = aParam.
}.

// --- Tape ---

#class(group) Tape
{
    #field(type:array)theBody.
    
    #method => <handle_tape>.
}

//// --- Struct ---
//
//#class Struct : Tape
//{
//    #method => #inline handle_selector.
//}
//
//// --- Class ---
//
//#class(group) Class
//{
//    #field(type:array)theBody.
//    
//    #method => #inline handle_class.
//}
