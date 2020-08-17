[[
   #grammar cf

   #define start          ::= <= root ( public_namespace ( => member+ $eof <= )) =>;
   #define start          ::= $eof;

   #define member         ::= import;
   #define member         ::= function;

   #define import         ::= 
<=
    import (
=>
                             "import" identifier ";"
<=
    )
=>;

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
                              "{" statement* last_statement? "}"
<=
     )
=>;

   #define statement      ::= "var" decl_variable ";";
   #define statement      ::= expression ";";
   #define last_statement ::= expression;

   #define decl_variable  ::= 
<=
        expression 
        (
=>
                              a_new_variable ":=" expression
<=
        )
=>;

   #define a_new_variable ::= new_variable
<=
           assign = 0
=>;

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

   #define new_variable   ::= <= variable_identifier = $identifier =>; 

   #define message        ::= <= message = $identifier =>;
   #define identifier     ::= <= identifier = $identifier =>;
   #define integer        ::= <= integer = $numeric =>;
   #define literal        ::= <= literal = "$literal" =>;
   #define reference      ::= <= reference = $reference =>;

   #define public_prefix  ::= "public";
]]