public interface<T>
{
    T cast() = new T::
    {
        embeddable dispatch() => self;
    };
}

//property prop<__name>
//{
//    __name;
//    
//    get __name()
//    {
//        ^ __name
//    }    
//    
//    set __name(value)
//    {
//        __name := value
//    }
//}
//
//property prop<T, __name>
//{
//    T __name;
//    
//    get T __name()
//    {
//        ^ __name
//    }    
//    
//    set __name(T value)
//    {
//        __name := value
//    }
//}
//
//property rprop<__name>
//{
//    __name;
//    
//    get __name()
//    {
//        ^ __name
//    }    
//}
//
//property rprop<T, __name>
//{
//    T __name;
//    
//    get T __name()
//    {
//        ^ __name
//    }    
//}
//
//inline dto<__name>
//{
//    this __dto_properties += subjconst __name;
//}

// --- extern statement ---

#declare extern(extern_block)
{
    extern extern_block
}

// --- if statement ---

// --- if statement ---

#declare if<expr>(if_block)
{
    cast bool(expr) ? if_block
}

// --- if-else statement ---

#declare if:else<expr>(if_block,else_block)
{
    cast bool(expr) ? if_block : else_block
}

//// --- ifnot statement ---
//
//public ifnot<if_block>(expr)
//{
//    cast bool(expr) ! if_block
//}
//
//// --- ifnot-else statement ---
//
//public ifnot<ifnot_block,else_block>:else(expr)
//{
//    cast bool(expr) ? else_block : ifnot_block
//}

// --- while statement ---

#declare while<expr>(loop_block)
{
    __loop: cast bool(expr) ? loop_block
}    

// --- until statement ---

#declare until<expr>(loop_block)
{
    __loop: cast bool(expr) ! loop_block
}

//public until<loop_body>(step_expr, cond_expr)
//{
//    step_expr;
//    
//    (__loop: cast bool(cond_expr) ! { loop_body; __ignore_duplicates step_expr })
//}
    
// --- for statement ---

//public for<loop_body>(step_expr, cond_expr)
//{
//    step_expr;
//    
//    (__loop: cast bool(cond_expr) ? { loop_body; __ignore_duplicates step_expr })
//}

#declare for<init_expr, cond_expr, step_expr>(loop_body)
{
    init_expr;
    
    __loop: cast bool(cond_expr) ? { loop_body; step_expr }
}

//// --- for statement ---
//
//public for<loop_body>(init_expr, cond_expr, step1_expr, step2_expr)
//{
//    init_expr;
//    
//    (__loop: cast bool(cond_expr) ? { loop_body; step1_expr; step2_expr })
//}
//
//// --- doUntil statement ---
//
//public do<loop_body,cond_expr>:until()
//{
//    loop_body;
//    
//    (__loop: cast bool(cond_expr) ! { loop_body })
//}
//
//
//public do<loop_body,cond_expr>:while()
//{
//    loop_body;
//    
//    (__loop: cast bool(cond_expr) ? { loop_body })
//}

// -- try-catch statement ---
#declare try:catch(try_block,catch_block)
{
    try_block | on(catch_block);
}

//public try<try_block,catch_block1,catch_block2>:catch,catch()
//{
//    try_block | on(catch_block1,catch_block2);
//}
//
//// -- try-finally-catch statement ---
//
//public try<try_block,finally_block, catch_block>:finally,catch()
//{
//    try_block $fnl finally_block | on(catch_block);
//}
//
//// -- try-finally-catch-catch statement ---
//
//public try<try_block,finally_block,catch_block1,catch_block2>:finally,catch,catch()
//{
//    try_block $fnl finally_block | on(catch_block1,catch_block2);
//}
//
//// -- finally statement ---
//
//public try<try_block,finally_block>:finally()
//{
//    try_block $fnl finally_block | raise();
//}
//
//// --- using statement ---
//
//public using<block>(expr)
//{
//    auto v := expr;
//    
//    try
//    {
//        block;
//    }
//    finally
//    {
//        v.close()
//    }
//}
//
//// === operator templates ===
//
//// .?
//public doIfNotNil<expr>(identifier)
//{
//    if (nil != identifier)
//    {
//        expr        
//    }
//}
//
//// .\
//public tryOrReturn<expr>(identifier)
//{
//    expr \ back:identifier;
//}
//
