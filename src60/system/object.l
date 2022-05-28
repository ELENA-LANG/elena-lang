// === statement templates ===

#new __meta statementTemplates;

// --- if ---

public if<expr>(truePart) : __included<statementTemplates>
{
   cast bool(expr) ? truePart
}

#reload statementTemplates;

// === classes ===

// --- super object ---

public class Object
{
    dispatch() : external<system'core_routines'__object_dispatcher>;
}

// --- basic extensions ---

public extension commonOp
{
   bool equalReference(o) : external<system'core_routines'__object_equal>;
}

