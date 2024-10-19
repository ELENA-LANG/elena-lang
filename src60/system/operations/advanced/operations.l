import system'operations;

// === advanced statement templates ===

#share __symbol statements;

// --- foreach ---

public template foreach::in<item, collection>(loopBody) : __included(statements)
{
   auto en := collection.enumerator();

   while (en.next())
   {
      item := *en;

      loopBody;
   }
}

// --- if is ---

public template if::is<expr, target>(truePart) : __included(statements)
{
   try
   {
      target := expr;

      truePart;
   }
}

// --- expr as: T ---

public template as<T>(expr)  : __included(statements)
   = cast T(expr);

// --- expr is: T ---

public template is<T>(expr) : __included(statements)
   = expr.instanceOf(T);

