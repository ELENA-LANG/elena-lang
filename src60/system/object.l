// === classes ===

// --- super object ---

public class Object
{
    const string __className := $name __decl;

    bool equal(o)
       = self.equalReference(o);

    bool notequal(o)
       = !(self.equal(o));

    // virtual method declarations : forcing the output type                            
    predefined bool less(o);
    predefined bool greater(o);
    predefined bool notless(o);
    predefined bool notgreater(o);

    string toPrintable() = __className;

    dispatch() : external<system'core_routines'__object_dispatcher>;
}

// --- basic extensions ---

public extension commonOp
{
   bool equalReference(o) : external<system'core_routines'__object_equal>;
}

