import extensions;

 
public program()
{
   var tape :=  new system'dynamic'expressions'SymbolCollection ( 
     new system'dynamic'expressions'SymbolInfo ( 
       "program", 
        new system'dynamic'expressions'DynamicSingleton ( 
          new system'dynamic'expressions'ActionMethodExpression ( 
            new system'dynamic'expressions'CodeblockExpression ( 
              new system'dynamic'expressions'ExtensionOrMessageCallExpression ( 
                 new system'dynamic'expressions'IntConstantExpression ( "2" ),
                 "printMe" 

//                new system'dynamic'expressions'SymbolExpression ( "system'console" ) ,
//                "printLine" ,
//                new system'dynamic'expressions'IntConstantExpression ( "2" ) 
              ) 
            ) 
          ) 
        ) 
      ) 
   ); 

   tape.eval();
}