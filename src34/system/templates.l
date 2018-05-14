public interface<type>
{
    cast T<type> = type::
    {
        embeddable dispatch => self.
    }.
}

public field prop<prop_name>
{
    object field.
    
    set prop_name:val
    [
        field := val
    ]
    
    prop_name = field.
}

public field prop<prop_type, prop_name>
{
    prop_type field.
    
    set prop_name (prop_type val)
    [
        field := val
    ]
    
    prop_type prop_name = field.
}

public field rprop<prop_type, prop_name>
{
    prop_type field.
    
    prop_type prop_name = field.
}

//field template dto_prop :: prop_type, prop_name
//{
//    const static object<> theProperties.
//
//    prop_type $field.
//    
//    @theProperties := theProperties + %prop_name.
//    
//    set prop_name prop_type:val
//    [
//        $field := val
//    ]
//    
//    prop_type prop_name = $field.
//    
//    static __properties = theProperties.
//}
//
//field template dto_prop :: prop_name
//{
//    const static object<> theProperties.
//
//    object $field.
//    
//    @theProperties := theProperties + %prop_name.
//    
//    set prop_name:val
//    [
//        $field := val
//    ]
//    
//    prop_name = $field.
//    
//    static __properties = theProperties.
//}

// --- vprop ---

public __accessor __accessor vprop<prop_type,prop_name>
{
    embeddable prop_type prop_name
    [
        prop_type val := 0.        
        self __get(&val).

        ^ val.
    ]
    
    embeddable set prop_name(prop_type val)
    [
        self __set(val).
    ]
}

//// --- vvprop ---
//
//__accessor __accessor template vvprop :: prop_type,prop_name
//{
//    embeddable prop_type prop_name
//    [
//        prop_type val := 0.        
//        self __get(&val).
//
//        ^ val.
//    ]
//    
//    embeddable set prop_name(prop_type val)
//    [
//        self __set(val).
//    ]
//    
//    embeddable append prop_name(prop_type val)
//    [
//        prop_type newVal := 0.        
//        $self $get var<prop_type>:newVal.
//        
//        newVal := newVal + val.
//        
//        $self $set prop_type:newVal.
//    ]
//    
//    stacksafe embeddable reduce prop_name prop_type:val
//    [
//        prop_type newVal := 0.        
//        $self $get var<prop_type>:newVal.
//        
//        newVal := newVal - val.
//        
//        $self $set prop_type:newVal.
//    ]    
//}

////accessor template vprop :: type,prop_name
////{
////    stacksafe embeddable type prop_name
////    [
////        type val := 0.        
////        $self $get var<type>:val.
////
////        ^ val.
////    ]
////}

// --- rvprop ---

public __accessor rvprop<type,prop_name>
{
    stacksafe embeddable type prop_name
    [
        type val := 0.        
        self __get(&val).

        ^ val.
    ]
}

// --- fnprop ---

public __accessor fnprop<type,template_fn>
{
    stacksafe embeddable type template_fn(type n)
    [
        type retVal := 0.
        self __get(n, &retVal).
        ^ retVal.            
    ]
}

//// --- fn2prop ---
//
//__accessor template fn2prop :: type,template_fn
//{
//    stacksafe embeddable type template_fn type:n type:m
//    [
//        type retVal := 0.
//        $self $get type:n type:m var<type>:retVal.
//        ^ retVal.            
//    ]
//}
//
//// --- fn3prop ---
//
//__accessor template fn3prop :: type,template_fn
//{
//    stacksafe embeddable type template_fn type:n type:m type:o
//    [
//        type retVal := 0.
//        $self $get type:n type:m type:o var<type>:retVal.
//        ^ retVal.            
//    ]
//}

// --- while statement ---

public __loop while<expr>
[
    T<bool>(expr) ?
    [
    ].
]    

// --- until statement ---

public __loop until<expr>
[
    T<bool>(expr) !
    [
    ].
]
    
// --- extern statement ---

public __extern template extern
[
    [
    ].
]

// --- if statement ---

public __block if<expr>
[
    T<bool>(expr) ?
    [
    ].
]

// --- ifnot statement ---

public __block ifnot<expr>
[
    T<bool>(expr) !
    [
    ].
]

// --- if-else statement ---

public __block __block if<expr>
[
    T<bool>(expr) ?
    [
    ]
    ! [
    ].    
]


public __block __block ifnot<expr>
[
    T<bool>(expr) inverted ?
    [
    ]
    ! [
    ].
]

// -- try-catch statement ---

public __nested try<expr>
[
    expr | catch:
    {
    }.
]

//// -- try-finally-catch statement ---
////
////__nested __block template try :: expr =
////    expr then:[] | catch([],
////    {
////    }).
//
//// -- finally statement ---
//
//__block template finally :: expr
//[
//    expr then:
//        [
//        ]                 
//        | raise:[].
//]        
//
//__block __block template finally
//[
//    true doFinally([],[]).
//]
//
//
//// --- using statement ---
//
//__block template using :: expr
//[
//    expr doAndClose:[].
//]    
