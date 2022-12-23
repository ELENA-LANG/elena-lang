// === statement templates ===

#new __symbol statementTemplates;

// --- if ---

public if<expr>(truePart) : __included<statementTemplates>
{
   cast bool(expr) ? { truePart }
}

// --- if:else ---

public if<expr>:else(truePart, falsePart) : __included<statementTemplates>
{
   cast bool(expr) ? { truePart } : { falsePart }
}

// --- ifnot ---

public ifnot<expr>(falsePart) : __included<statementTemplates>
{
   cast bool(expr) ! { falsePart }
}

// --- while ---

public while<expr>(loopBody) : __included<statementTemplates>
{
   $rep cast bool(expr) ? { loopBody }
}

// --- until ---

public until<expr>(loopBody) : __included<statementTemplates>
{
   $rep cast bool(expr) ! { loopBody }
}

// --- do:until ---

public do:until(loop_body,cond_expr) : __included<statementTemplates>
{
   loop_body;

   $rep cast bool(cond_expr) ? { loop_body }
}

// --- for ---

public for<initExpr,condExpr,stepExpr>(loopBody) : __included<statementTemplates>
{
   initExpr;

   $rep cast bool(condExpr) ? { loopBody; stepExpr; }
}

// --- try:catch ---

public try:catch(try_block,catch_block) : __included<statementTemplates>
{
   try_block \\ on(catch_block);
}

// --- extern ---

public template extern(body) : __included<statementTemplates>
{
   $ext body
}
