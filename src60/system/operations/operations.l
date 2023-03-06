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

   $rep cast bool(cond_expr) ? { loop_body }
}

// --- for ---

public template for<initExpr,condExpr,stepExpr>(loopBody) : __included(statementTemplates)
{
   initExpr;

   $rep cast bool(condExpr) ? { loopBody; stepExpr; }
}

// --- try:catch ---

public template try::catch(try_block,catch_block) : __included(statementTemplates)
{
   try_block \\ on(catch_block);
}

// --- extern ---

public template extern(body) : __included(statementTemplates)
{
   $ext body
}
