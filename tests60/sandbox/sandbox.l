import extensions;
////import extensions'scripting;
////
////public program()
////{
////    lscript.interpretPath("sample.ls");
////}
//
//public program()
//{
////   var f := new system'dynamic'expressions'SymbolCollection ( 
////             new system'dynamic'expressions'SymbolInfo ( 
////              "program", 
////              new system'dynamic'expressions'DynamicSingleton ( 
////               new system'dynamic'expressions'ActionMethodExpression ( 
////                new system'dynamic'expressions'CodeblockExpression ( 
////                 new system'dynamic'expressions'DeclaringAndAssigningExpression ( 
////                  "number", 
////                  new system'dynamic'expressions'NewExpression ( 
////                   new system'dynamic'expressions'ClassIdentifierExpression ( "Integer" ) 
////                  ) 
////                 ) 
////                ) 
////               ) 
////              ) 
////             ) 
////            );
////
////   var p := f.eval();
//
//
////   var symbol := Symbol.tryLoad("system", "Integer");
////   if (symbol != default)
////   {
////      console.writeLine("Loaded");
////   }
//
//   Symbol s1 := default;
//   Symbol s2 := default;
//
//   bool r := s1 == s2;
//   
//}

public sealed const struct XSymbol
{
   __ptr pointer;

   constructor(pointer ptr)
   {
   }

   static XSymbol Default
      = default;
}

public program()
{
   XSymbol s := XSymbol.Default;
}
