[[
   #grammar cf

   #define start          ::= <= root ( public_namespace ( => member+ $eof <= )) =>;
   #define start          ::= $eof;

   #define member         ::= function;

   #define function       ::= 
<=
   singleton (
=>
                              public_prefix? name f_decl
<=
   )
=>;

   #define f_decl         ::=
<=
     function (
=>
                              f_args body 
<=
     )
=>;

   #define f_args         ::= "(" ")";

   #define body           ::= 
<=
     code (
=>
                              "{" statement "}"
<=
     )
=>;

   #define statement      ::= expression;
   #define expression     ::= 
<=
        expression (
=>
                              object
<=
        )
=>;

   #define object         ::= integer;

   #define name           ::= <= nameattr ( identifier = $identifier ) =>; 

   #define integer        ::= <= integer = $numeric =>;

   #define public_prefix  ::= "public";
]]