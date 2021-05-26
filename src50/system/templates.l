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
    
    set(value)
    {
        __name := value
    }
}

#property prop<__name>
{
    field;
    
    get()
    {
        ^ __name
    }    
    
    set(value)
    {
        __name := value
    }
}

#property rprop<T, __name>
{
    field;
    
    get()
    {
        ^ __name
    }    
}

#property rprop<__name>
{
    field;
    
    get()
    {
        ^ __name
    }    
}

//#inline dto
//{
//    this __dto_properties += @subject;
//}
//
//#property dto_prop<__name>
//{
//    field;
//
//    [dto]    
//    get()
//    {
//        ^ __name
//    }
//    
//    set(value)
//    {
//        __name := value
//    }
//}
//
//#property dto_prop<T, __name>
//{
//    field;
//
//    [dto]    
//    get()
//    {
//        ^ __name
//    }
//    
//    set(T value)
//    {
//        __name := value
//    }
//}

// --- extern statement ---

#declare extern(extern_block)
{
    extern extern_block
}

// --- if statement ---

#declare if<expr>(if_block)
{
    cast bool(expr) ? if_block
}

// --- if-else statement ---

#declare if<expr>:else(if_block,else_block)
{
    cast bool(expr) ? if_block : else_block
}

// --- ifnot statement ---

#declare ifnot<expr>(if_block)
{
    cast bool(expr) ! if_block
}

// --- ifnot-else statement ---

#declare ifnot<expr>:else(ifnot_block,else_block)
{
    cast bool(expr) ? else_block : ifnot_block
}

// --- while statement ---

#declare while<expr>(loop_block)
{
    __loop cast bool(expr) ? loop_block
}    

// --- until statement ---

#declare until<expr>(loop_block)
{
    __loop cast bool(expr) ! loop_block
}

#declare until<step_expr, cond_expr>(loop_body)
{
    step_expr;
    
    __loop cast bool(cond_expr) ! { loop_body; __ignore_duplicates step_expr }
}
    
// --- for statement ---

//#declare for<step_expr, cond_expr>(loop_body)
//{
//    step_expr;
//    
//    __loop: cast bool(cond_expr) ? { loop_body; __ignore_duplicates step_expr }
//}

#declare for<init_expr, cond_expr, step_expr>(loop_body)
{
    init_expr;
    
    __loop cast bool(cond_expr) ? { loop_body; step_expr }
}

// --- for statement ---

#declare for<init_expr1, init_expr2, cond_expr, step_expr1, step_expr2>(loop_body)
{
    init_expr1;
    init_expr2;
    
    __loop cast bool(cond_expr) ? { loop_body; step_expr1; step_expr2 }
}

// --- doUntil statement ---

#declare do:until(loop_body,cond_expr)
{
    loop_body;
    
    __loop cast bool(cond_expr) ! { loop_body }
}

#declare do:while(loop_body,cond_expr)
{
    loop_body;
    
    __loop cast bool(cond_expr) ? { loop_body }
}

// -- try-catch statement ---
#declare try:catch(try_block,catch_block)
{
    try_block \\ on(catch_block);
}

// -- try-finally-catch statement ---
#declare try:finally:catch(try_block,finally_block,catch_block)
{
    try_block $fnl finally_block \\ on(catch_block);
}

// -- finally statement ---

#declare try:finally(try_block,finally_block)
{
    try_block $fnl finally_block \\ raise();
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

// === inline operator templates ===

// .?
#declare __inlineif<identifier>(expr)
{
    if (nil != identifier)
    {
        expr        
    }
}

// .!
#declare __inlineifnot<identifier>(expr)
{
    if (nil == identifier)
    {
        identifier := new __typeof identifier();
    };
    
    expr
}

// .\
#declare __inlinealt<identifier>(expr)
{
    expr \ back:identifier;
}
