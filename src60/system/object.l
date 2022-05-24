// === statement templates ===

#new __meta statementTemplates;

// --- if ---

public if<expr>(truePart) : __included<statementTemplates>
{
   expr.if(truePart)
}

#reload statementTemplates;

// === classes ===

// --- super object ---

public class Object
{
    dispatch() : external<system'core_routines'__object_dispatcher>;
}

