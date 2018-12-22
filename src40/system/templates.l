//public interface<type>
//{
//    cast T<type> = type::
//    {
//        embeddable dispatch => self.
//    }.
//}

property prop<T>
{
    T __property;
    
    get T __property()
    {
        ^ __property
    }    
    
    set __property(T value)
    {
        __property := value
    }
}

property prop<T,get_accessor,set_accessor>
{
    get T __property()
    {
        ^ self.get_accessor()
    }
    
    set __property(T value)
    {
        self.set_accessor(value)
    }
}

property vprop<T,get_accessor>
{
    get T __property()
    {
        T retVal := T.Default;
        
        self.get_accessor(ref retVal);
        
        ^ retVal
    }
}

property vprop<T,get_accessor,set_accessor>
{
    get T __property()
    {
        T retVal := T.Default;
        
        self.get_accessor(ref retVal);
        
        ^ retVal
    }
    
    set __property(T val)
    {
        self.set_accessor(val)
    }
}

property rprop<T>
{
    T __property;
    
    get T __property()
    {
        ^ __property
    }    
}
//
//public field dto_prop<prop_type, prop_name>
//{
//    const static V<object> theProperties.
//
//    prop_type field.
//    
//    @theProperties := theProperties + %prop_name.
//    
//    set prop_name(prop_type val)
//    [
//        field := val
//    ]
//    
//    prop_type prop_name = field.
//    
//    static __properties = theProperties.
//}
//
//public field dto_prop<prop_name>
//{
//    const static V<object> theProperties.
//
//    object field.
//    
//    @theProperties := theProperties + %prop_name.
//    
//    set prop_name:val
//    [
//        field := val
//    ]
//    
//    prop_name = field.
//    
//    static __properties = theProperties.
//}
//
//// --- vprop ---
//
//public __accessor __accessor vprop<prop_type,prop_name>
//{
//    embeddable prop_type prop_name
//    [
//        prop_type val := T<prop_type>().
//        self __get(&val).
//
//        ^ val.
//    ]
//    
//    embeddable set prop_name(prop_type val)
//    [
//        self __set(val).
//    ]
//}
//
////// --- vvprop ---
////
////__accessor __accessor template vvprop :: prop_type,prop_name
////{
////    embeddable prop_type prop_name
////    [
////        prop_type val := 0.        
////        self __get(&val).
////
////        ^ val.
////    ]
////    
////    embeddable set prop_name(prop_type val)
////    [
////        self __set(val).
////    ]
////    
////    embeddable append prop_name(prop_type val)
////    [
////        prop_type newVal := 0.        
////        $self $get var<prop_type>:newVal.
////        
////        newVal := newVal + val.
////        
////        $self $set prop_type:newVal.
////    ]
////    
////    stacksafe embeddable reduce prop_name prop_type:val
////    [
////        prop_type newVal := 0.        
////        $self $get var<prop_type>:newVal.
////        
////        newVal := newVal - val.
////        
////        $self $set prop_type:newVal.
////    ]    
////}
//
//////accessor template vprop :: type,prop_name
//////{
//////    stacksafe embeddable type prop_name
//////    [
//////        type val := 0.        
//////        $self $get var<type>:val.
//////
//////        ^ val.
//////    ]
//////}
//
//// --- rvprop ---
//
//public __accessor rvprop<type,prop_name>
//{
//    embeddable type prop_name
//    [
//        type val := T<type>().     
//        self __get(&val).
//
//        ^ val.
//    ]
//}
//
//// --- fnprop ---
//
//public __accessor fnprop<type,template_fn>
//{
//    embeddable type template_fn(type n)
//    [
//        type retVal := T<type>().
//        self __get(n, &retVal).
//        ^ retVal.            
//    ]
//}
//
//// --- fn2prop ---
//
//public __accessor fn2prop<type,template_fn>
//{
//    embeddable type template_fn(type n, type m)
//    [
//        type retVal := T<type>().
//        self __get(n, m, &retVal).
//        ^ retVal.            
//    ]
//}
//
//// --- fn3prop ---
//
//public __accessor fn3prop<type,template_fn>
//{
//    embeddable type template_fn(type n, type m, type o)
//    [
//        type retVal := T<type>().
//        self __get(n, m, o, &retVal).
//        ^ retVal.            
//    ]
//}
//
//// --- while statement ---
//
//public __loop while<expr>
//[
//    T<bool>(expr) ?
//    [
//    ].
//]    

// --- extern statement ---

public extern<extern_block>()
{
    extern extern_block
}

// --- if statement ---

public if<if_block>(expr)
{
    cast bool(expr) ? if_block
}

// --- if-else statement ---

public if<if_block,else_block>:else(expr)
{
    cast bool(expr) ? if_block : else_block
}

//// --- ifnot statement ---
//
//public __block ifnot<expr>
//[
//    T<bool>(expr) !
//    [
//    ].
//]
//
//public __block __block ifnot<expr>
//[
//    T<bool>(expr) inverted ?
//    [
//    ]
//    ! [
//    ].
//]

// --- while statement ---

public while<loop_block>(expr)
{
    __loop cast bool(expr) ? loop_block
}    

// --- until statement ---

public until<loop_block>(expr)
{
    __loop cast bool(expr) ! loop_block
}
    
// --- for statement ---

public for<loop_body>(init_expr, cond_expr, step_expr)
{
    init_expr;
    
    __loop cast bool(cond_expr) ? { loop_body; step_expr }
}

// --- doUntil statement ---

public doUntil<loop_body>(init_expr, cond_expr, step_expr)
{
    init_expr;
    step_expr;
    loop_body;
    
    __loop cast bool(cond_expr) ! { step_expr; loop_body }
}

// -- try-catch statement ---

public try<try_block,catch_block>:catch()
{
    try_block | on(catch_block);
}

//// -- try-finally-catch statement ---
//
//public __block __nested try<expr>
//[
//    expr then:[] | catch([],
//    {
//    }).
//]
//
//// -- finally statement ---
//
//public __block finally<expr>
//[
//    expr then:
//        [
//        ]                 
//        | raise:[].
//]        
//
//public __block __block template finally
//[
//    true doFinally([],[]).
//]
//
//// --- using statement ---
//
//public __block using<expr>
//[
//    expr doAndClose:[].
//]    
