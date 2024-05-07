// === statement templates ===

#new __symbol statements;

// --- if ---

public template if<expr>(truePart) : __included(statements)
{
   cast bool(expr) 
     ? { truePart }
}

// --- if:else ---

public template if::else<expr>(truePart,falsePart) : __included(statements)
{
   cast bool(expr) 
     ? { truePart } 
     ! { falsePart }
}

// --- ifnot ---

public template ifnot<expr>(falsePart) : __included(statements)
{
   cast bool(expr) 
     ! { falsePart }
}

// --- ifnot:else ---

public template ifnot::else<expr>(falsePart,truePart) : __included(statements)
{
   cast bool(expr) 
     ? { truePart } 
     ! { falsePart }
}

// --- while ---

public template while<expr>(loopBody) : __included(statements)
{
   $rep cast bool(expr) 
     ? { loopBody }
}

// --- until ---

public template until<expr>(loopBody) : __included(statements)
{
   $rep cast bool(expr) 
     ! { loopBody }
}

public template until<step_expr, cond_expr>(loop_body) : __included(statements)
{
    step_expr;

    $rep cast bool(cond_expr) 
      ! { loop_body; __ignore_duplicates step_expr }
}

public template for::until<step_expr, cond_expr>(loop_body) : __included(statements)
{
    step_expr;
    
    $rep cast bool(cond_expr) 
      ! { loop_body; __ignore_duplicates step_expr }
}

// --- do:until ---

public template do::until(loop_body, cond_expr) : __included(statements)
{
   loop_body;

   $rep cast bool(cond_expr) 
     ! { loop_body }
}

// --- do:while ---

public template do::while(loop_body, cond_expr) : __included(statements)
{
   loop_body;

   $rep cast bool(cond_expr) 
      ? { loop_body }
}

// --- for ---

public template for<stepExpr, condExpr>(loopBody) : __included(statements)
{
   stepExpr;

   $rep cast bool(condExpr) 
     ? { loopBody; __ignore_duplicates stepExpr; }
}

public template for<initExpr,condExpr,stepExpr>(loopBody) : __included(statements)
{
   initExpr;

   $rep cast bool(condExpr) ? { loopBody; stepExpr; }
}

public template for<init_expr,condExpr,step_expr1, step_expr2>(loopBody) : __included(statements)
{
   init_expr;

   $rep cast bool(condExpr) ? { loopBody; step_expr1; step_expr2; }
}

public template for::and<init_expr1, init_expr2,condExpr,step_expr1, step_expr2>(loopBody) : __included(statements)
{
   init_expr1;
   init_expr2;

   $rep cast bool(condExpr) ? { loopBody; step_expr1; step_expr2; }
}

// --- try ---

public template try(try_block) : __included(statements)
{
   try_block \ back(nil);
}

// --- try:catch ---

public template try::catch(try_block,catch_block) : __included(statements)
{
   try_block \\ on(catch_block);
}

// --- try:catch:finally ---

public template try::catch::finally(try_block,catch_block,finally_block) : __included(statements)
{
   try_block \\ on(catch_block) $fnl finally_block;
}

// --- try:finally ---

public template try::finally(try_block,finally_block) : __included(statements)
{
   try_block $fnl finally_block;
}

// --- extern ---

public template extern(body) : __included(statements)
{
   $ext body
}

// --- using statement ---

public template using<expr>(block) : __included(statements)
{
   auto v := expr;
    
   block $fnl (v.close());
}

// --- lock statement ---

public template lock<expr>(block) : __included(statements)
{
   auto v := expr;

   v.enterCriticalSection();
    
   block $fnl (v.leaveCriticalSection());
}

// --- operator templates ---

// \.
public template operator::alt<identifier>(expr) : __included(statements)
{
   expr \ back(identifier);
}

// !.
public template operator::else<identifier>(expr) : __included(statements)
{
    (nil == identifier) ?
    {
        identifier := new __typeof identifier();
    };
    
    expr
}

// ?.
public template operator::ifnil<identifier>(expr) : __included(statements)
{
    cast bool(nil != identifier) 
      ? { expr } 
      ! { nil };
}
