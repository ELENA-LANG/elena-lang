// === statement templates ===

#new __symbol statements;

// --- if ---

public template if<expr>(truePart) : __included(statements)
{
   (cast bool(expr))
     ? { truePart }
}

// --- if:else ---

public template if::else<expr>(truePart,falsePart) : __included(statements)
{
   (cast bool(expr))
     ? { truePart } 
     ! { falsePart }
}

// --- ifnot ---

public template if::not<expr>(falsePart) : __included(statements)
{
   (cast bool(expr))
     ! { falsePart }
}

// --- ifnot:else ---

public template if::not::else<expr>(falsePart,truePart) : __included(statements)
{
   (cast bool(expr))
     ? { truePart } 
     ! { falsePart }
}

// --- if:nil ---

public template if::nil<expr>(truePart) : __included(statements)
{
   (cast bool(expr == nil))
     ? { truePart }
}

// --- if:else ---

public template if::nil::else<expr>(truePart,falsePart) : __included(statements)
{
   (cast bool(expr == nil))
     ? { truePart } 
     ! { falsePart }
}

// --- if:not:nil ---

public template if::not::nil<expr>(falsePart) : __included(statements)
{
   (cast bool(expr == nil))
     ! { falsePart }
}

// --- ifnot:else ---

public template if::not::nil::else<expr>(falsePart,truePart) : __included(statements)
{
   (cast bool(expr == nil))
     ? { truePart } 
     ! { falsePart }
}

// --- while ---

public template while<expr>(loopBody) : __included(statements)
{
   $rep (cast bool(expr))
     ? { loopBody }
}

// --- until ---

public template until<expr>(loopBody) : __included(statements)
{
   $rep (cast bool(expr))
     ! { loopBody }
}

public template until<step_expr, cond_expr>(loop_body) : __included(statements)
{
    step_expr;

    $rep (cast bool(cond_expr))
      ! { loop_body; __ignore_duplicates step_expr }
}

public template for::until<step_expr, cond_expr>(loop_body) : __included(statements)
{
    step_expr;
    
    $rep (cast bool(cond_expr))
      ! { loop_body; __ignore_duplicates step_expr }
}

// --- do:until ---

public template do::until(loop_body, cond_expr) : __included(statements)
{
   loop_body;

   $rep (cast bool(cond_expr))
     ! { loop_body }
}

// --- do:while ---

public template do::while(loop_body, cond_expr) : __included(statements)
{
   loop_body;

   $rep (cast bool(cond_expr))
      ? { loop_body }
}

// --- for ---

public template for<stepExpr, condExpr>(loopBody) : __included(statements)
{
   stepExpr;

   $rep (cast bool(condExpr))
     ? { loopBody; __ignore_duplicates stepExpr; }
}

public template for<initExpr,condExpr,stepExpr>(loopBody) : __included(statements)
{
   initExpr;

   $rep (cast bool(condExpr)) ? { loopBody; stepExpr; }
}

public template for<init_expr,condExpr,step_expr1, step_expr2>(loopBody) : __included(statements)
{
   init_expr;

   $rep (cast bool(condExpr)) ? { loopBody; step_expr1; step_expr2; }
}

public template for::and<init_expr1, init_expr2,condExpr,step_expr1, step_expr2>(loopBody) : __included(statements)
{
   init_expr1;
   init_expr2;

   $rep (cast bool(condExpr)) ? { loopBody; step_expr1; step_expr2; }
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
   __intermediate v := expr;
    
   block $fnl (v.close());
}

// --- lock statement ---

public template lock<expr>(block) : __included(statements)
{
   __intermediate v := expr;

   v.enterCriticalSection();
    
   block $fnl (v.leaveCriticalSection());
}

// --- operator templates ---

// :break

public template operator::break() : __included(statements)
{
    $break;
}

// :continue

public template operator::continue() : __included(statements)
{
    $continue;
}

// :yield

public template operator::yield(expr) : __included(statements)
{
    $yield expr;
}

// :await

public template operator::await(expr) : __included(statements)
    = $await expr;

// :lazy

public template operator::lazy(expr) : __included(statements)
    = $lazy expr;

// shl:

public template operator::shl(expr, arg) : __included(statements)
   = expr $shl arg;

// shr:

public template operator::shr(expr, arg) : __included(statements)
   = expr $shr arg;

// :sizeof

public template operator::sizeof(expr) : __included(statements)
   = $size expr;
