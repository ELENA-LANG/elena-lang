// === statement templates ===

#new __symbol statementTemplates;

// --- if ---

public template if<expr>(truePart) : __included(statementTemplates)
{
   cast bool(expr) ? { truePart }
}

// --- if:else ---

public template if::else<expr>(truePart,falsePart) : __included(statementTemplates)
{
   cast bool(expr) ? { truePart } : { falsePart }
}

// --- ifnot ---

public template ifnot<expr>(falsePart) : __included(statementTemplates)
{
   cast bool(expr) ! { falsePart }
}

// --- ifnot:else ---

public template ifnot::else<expr>(falsePart,truePart) : __included(statementTemplates)
{
   cast bool(expr) ? { truePart } : { falsePart }
}

// --- while ---

public template while<expr>(loopBody) : __included(statementTemplates)
{
   $rep cast bool(expr) ? { loopBody }
}

// --- until ---

public template until<expr>(loopBody) : __included(statementTemplates)
{
   $rep cast bool(expr) ! { loopBody }
}

public template until<step_expr, cond_expr>(loop_body) : __included(statementTemplates)
{
    step_expr;
    
    $rep cast bool(cond_expr) ! { loop_body; __ignore_duplicates step_expr }
}

// --- do:until ---

public template do::until(loop_body, cond_expr) : __included(statementTemplates)
{
   loop_body;

   $rep cast bool(cond_expr) ! { loop_body }
}

// --- do:while ---

public template do::while(loop_body, cond_expr) : __included(statementTemplates)
{
   loop_body;

   $rep cast bool(cond_expr) ? { loop_body }
}

// --- for ---

public template for<stepExpr, condExpr>(loopBody) : __included(statementTemplates)
{
   stepExpr;

   $rep cast bool(condExpr) ? { loopBody; __ignore_duplicates stepExpr; }
}

public template for<initExpr,condExpr,stepExpr>(loopBody) : __included(statementTemplates)
{
   initExpr;

   $rep cast bool(condExpr) ? { loopBody; stepExpr; }
}

public template for<init_expr1, init_expr2,condExpr,step_expr1, step_expr2>(loopBody) : __included(statementTemplates)
{
   init_expr1;
   init_expr2;

   $rep cast bool(condExpr) ? { loopBody; step_expr1; step_expr2; }
}

// --- try:catch ---

public template try::catch(try_block,catch_block) : __included(statementTemplates)
{
   try_block \\ on(catch_block);
}

// --- try:catch:finally ---

public template try::catch::finally(try_block,catch_block,finally_block) : __included(statementTemplates)
{
   try_block \\ on(catch_block) $fnl finally_block;
}

// --- try:finally ---

public template try::finally(try_block,finally_block) : __included(statementTemplates)
{
   try_block $fnl finally_block;
}

// --- extern ---

public template extern(body) : __included(statementTemplates)
{
   $ext body
}

// --- using statement ---

public template using<expr>(block) : __included(statementTemplates)
{
   auto v := expr;
    
   block $fnl (v.close());
}

// --- lock statement ---

public template lock<expr>(block) : __included(statementTemplates)
{
   auto v := expr;

   v.enterCriticalSection();
    
   block $fnl (v.leaveCriticalSection());
}

// --- operator templates ---

// .\
public template operator::alt<identifier>(expr) : __included(statementTemplates)
{
   expr \ back:identifier;
}

// .!
public template operator::else<identifier>(expr) : __included(statementTemplates)
{
    (nil == identifier) ?
    {
        identifier := new __typeof identifier();
    };
    
    expr
}

// .?
public template operator::ifnil<identifier>(expr) : __included(statementTemplates)
{
    cast bool(nil != identifier) ?
      { expr } : { nil };
}
