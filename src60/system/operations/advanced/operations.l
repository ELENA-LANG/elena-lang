import system'operations;

// === advanced statement templates ===

#new __symbol statementTemplates;

// --- foreach ---

public template foreach::in<item, collection>(loopBody) : __included(statementTemplates)
{
   auto en := collection.enumerator();

   while (en.next())
   {
      item := *en;

      loopBody;
   }
}

// --- if is ---

public template if::is<expr, target>(truePart) : __included(statementTemplates)
{
   try
   {
      target := expr;

      truePart;
   }
}
