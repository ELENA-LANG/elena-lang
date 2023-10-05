import extensions;

public program()
{
   var tape :=  new system'dynamic'expressions'SymbolCollection ( 
   new system'dynamic'expressions'SymbolInfo ( 
   "f" ,
   new system'dynamic'expressions'DynamicSingleton ( 
     new system'dynamic'expressions'ActionMethodExpression ( 
       new system'dynamic'expressions'ScopeIdentifier ( "s1" ) ,
       new system'dynamic'expressions'CodeblockExpression ( 
         new system'dynamic'expressions'ReturnExpression ( 
           new system'dynamic'expressions'IdentifierExpression ( 
             new system'dynamic'expressions'ScopeIdentifier ( 
               "s1" 
             ) 
           ) 
         ) 
       ) 
     ) 
   ) 
  ),
  new system'dynamic'expressions'SymbolInfo ( 
   "program" ,
   new system'dynamic'expressions'DynamicSingleton ( 
     new system'dynamic'expressions'ActionMethodExpression ( 
       new system'dynamic'expressions'CodeblockExpression ( 
         new system'dynamic'expressions'ExtensionOrMessageCallExpression ( 
           new system'dynamic'expressions'SymbolExpression ( "system'console" ),  
           "writeLine", 
           new system'dynamic'expressions'FunctionCallExpression ( 
             new system'dynamic'expressions'IdentifierExpression ( 
               new system'dynamic'expressions'ScopeIdentifier ( 
                 "f" 
               ) 
             ),
             new system'dynamic'expressions'IntConstantExpression ( "2" ) 
           ) 
         ) 
       ) 
     ) 
   ) 
  ) 
); 

   tape.eval();

   tape.f
}