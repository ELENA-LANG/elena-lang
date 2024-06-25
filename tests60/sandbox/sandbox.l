import extensions'scripting;
import system'dynamic'expressions;

symbol used : preloaded = new object[]{Expression, DynamicSingleton,DynamicExpressionSymbol,SymbolCollection,LazySymbolExpression};

test()
{
   var s := new system'dynamic'expressions'SymbolCollection ( 
      new system'dynamic'expressions'SymbolInfo ( 
         "program",
         new system'dynamic'expressions'DynamicSingleton ( 
            new system'dynamic'expressions'ActionMethodExpression ( 
               new system'dynamic'expressions'CodeblockExpression ( 
                  new system'dynamic'expressions'ExtensionOrMessageCallExpression ( 
                     new system'dynamic'expressions'LazySymbolExpression ( 
                        "system'console" 
                     ), 
                     "writeLine", 
                     new system'dynamic'expressions'IntConstantExpression ( "2" ) ) ) ) ) ) ) ;

      s.safeEval()
}

public program()
{
//   var tape := lscript.eval("public program() { system'console.writeLine(2); }");
//   var program := tape.program;

   test();

//   program();
}