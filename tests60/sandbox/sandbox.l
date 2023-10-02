import extensions;
import system'dynamic'expressions;
import system'dynamic;
 
public program()
{
   var code := new SymbolCollection(
      new SymbolInfo(
         "program",
         new DynamicSingleton(
            new ActionMethodExpression(
               new CodeblockExpression(
                  new ExtensionOrMessageCallExpression(
                     new SymbolExpression("system'console"),
                     "writeLine",
                     new IntConstantExpression("2") 
                  )
               )
            )
         )
      )      
   );

   code.eval();
}