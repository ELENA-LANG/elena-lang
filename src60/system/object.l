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

// --- for ---

public for<initExpr,condExpr,stepExpr>(loopBody) : __included<statementTemplates>
{
   initExpr;

   $rep cast bool(condExpr) ? { loopBody; stepExpr; }
}

#reload statementTemplates;

// === classes ===

// --- super object ---

public class Object
{
    const string __className := $name __decl;

    bool equal(o)
       = self.equalReference(o);

    bool notequal(o)
       = !(self.equal(o));

    predefined bool less(o);

    string toPrintable() = __className;

    dispatch() : external<system'core_routines'__object_dispatcher>;
}

// --- basic extensions ---

public extension commonOp
{
   bool equalReference(o) : external<system'core_routines'__object_equal>;
}

