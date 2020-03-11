public interface<T>
{
    T cast() = new T
    {
        embeddable dispatch() => self;
    };
}

#property prop<T, __name>
{
    field;
    
    get()
    {
        ^ __name
    }
    
    set(T value)
    {
        __name := value
    }
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

#property rprop<T, __name>
{
    field;
    
    get()
    {
        ^ __name
    }    
}

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

// --- ifnot statement ---

#declare ifnot<expr>(if_block)
{
    cast bool(expr) ! if_block
}

// --- ifnot-else statement ---

#declare ifnot:else<expr>(ifnot_block,else_block)
{
    cast bool(expr) ? else_block : ifnot_block
}

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

#declare until<step_expr, cond_expr>(loop_body)
{
    step_expr;
    
    __loop: cast bool(cond_expr) ! { loop_body; __ignore_duplicates step_expr }
}
    
// --- for statement ---

#declare for<step_expr, cond_expr>(loop_body)
{
    step_expr;
    
    __loop: cast bool(cond_expr) ? { loop_body; __ignore_duplicates step_expr }
}

#declare for<init_expr, cond_expr, step_expr>(loop_body)
{
    init_expr;
    
    __loop: cast bool(cond_expr) ? { loop_body; step_expr }
}

// --- for statement ---

#declare for<init_expr, cond_expr, step_expr1, step_expr2>(loop_body)
{
    init_expr;
    
    __loop: cast bool(cond_expr) ? { loop_body; step1_expr; step2_expr }
}

// --- doUntil statement ---

#declare do:until(loop_body,cond_expr)
{
    loop_body;
    
    __loop: cast bool(cond_expr) ! { loop_body }
}

#declare do:while(loop_body,cond_expr)
{
    loop_body;
    
    __loop: cast bool(cond_expr) ? { loop_body }
}

// -- try-catch statement ---
#declare try:catch(try_block,catch_block)
{
    try_block | on(catch_block);
}

//public try<try_block,catch_block1,catch_block2>:catch,catch()
//{
//    try_block | on(catch_block1,catch_block2);
//}

// -- try-finally-catch statement ---
#declare try:finally:catch(try_block,finally_block,catch_block)
{
    try_block $fnl finally_block | on(catch_block);
}

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

// -- finally statement ---

#declare try:finally(try_block,finally_block)
{
    try_block $fnl finally_block | raise();
}

// --- using statement ---

#declare using<expr>(block)
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
#declare doIfNotNil<identifier>(expr)
{
    if (nil != identifier)
    {
        expr        
    }
}

// .\
#declare tryOrReturn<identifier>(expr)
{
    expr \ back:identifier;
}
