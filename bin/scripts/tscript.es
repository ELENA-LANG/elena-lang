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
                              object operation?
<=
        )
=>;

   #define object         ::= integer;
   #define object         ::= literal;
   #define object         ::= reference;
   #define object         ::= identifier;

   #define operation      ::= "." message m_args;

   #define m_args         ::= "(" ")";
   #define m_args         ::= "(" expression { "," expression }* ")";

   #define name           ::= <= nameattr ( identifier = $identifier ) =>; 

   #define message        ::= <= message = $identifier =>;
   #define identifier     ::= <= identifier = $identifier =>;
   #define integer        ::= <= integer = $numeric =>;
   #define literal        ::= <= literal = "$literal" =>;
   #define reference      ::= <= reference = $reference =>;

   #define public_prefix  ::= "public";
]]