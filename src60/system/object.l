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

