public interface<T>
{
    T cast() = new T:
    {
        embeddable dispatch() => self;
    };
}

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

// --- ifnot statement ---

public ifnot<if_block>(expr)
{
    cast bool(expr) ! if_block
}

// --- ifnot-else statement ---

public ifnot<ifnot_block,else_block>:else(expr)
{
    cast bool(expr) ? else_block : ifnot_block
}

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

// --- for statement ---

public for<loop_body>(init_expr, cond_expr, step1_expr, step2_expr)
{
    init_expr;
    
    __loop cast bool(cond_expr) ? { loop_body; step1_expr; step2_expr }
}

// --- doUntil statement ---

public doUntil<loop_body>(init_expr, cond_expr, step_expr)
{
    init_expr;
    step_expr;
    loop_body;
    
    __loop cast bool(cond_expr) ! { step_expr; loop_body }
}

public doUntil<loop_body>(cond_expr)
{
    loop_body;
    
    __loop cast bool(cond_expr) ! { loop_body }
}

public doWhile<loop_body>(cond_expr)
{
    loop_body;
    
    __loop cast bool(cond_expr) ? { loop_body }
}


// -- try-catch statement ---

public try<try_block,catch_block>:catch()
{
    try_block | on(catch_block);
}

public try<try_block,catch_block1,catch_block2>:catch,catch()
{
    try_block | on(catch_block1,catch_block2);
}

// -- try-finally-catch statement ---

public try<try_block,finally_block, catch_block>:finally,catch()
{
    try_block $fnl finally_block | on(catch_block);
}

// -- try-finally-catch-catch statement ---

public try<try_block,finally_block,catch_block1,catch_block2>:finally,catch,catch()
{
    try_block $fnl finally_block | on(catch_block1,catch_block2);
}

// -- finally statement ---

public try<try_block,finally_block>:finally()
{
    try_block $fnl finally_block | raise();
}

// --- using statement ---

public using<block>(expr)
{
    auto v := expr;
    
    try
    {
        block;
    }
    finally
    {
        v.close()
    }
}

// === operator templates ===

// .?
public doIfNotNil<expr>(identifier)
{
    if (nil != identifier)
    {
        expr        
    }
}

// .\
public tryOrReturn<expr>(identifier)
{
    expr \ back:identifier;
}

