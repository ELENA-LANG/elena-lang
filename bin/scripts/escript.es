[[
   #grammar cf

   #define start          ::= $eof;
   #define start          ::= 
<=
root
(
   namespace
   (
=>
       scope $eof
<=
   ) 
)
=>;

   #define scope          ::= 
<= 
   preloaded_symbol
   (
       expression
       (
          expression
          (
             nested
             (
=>
                code
<=
             ) 
          )
          message = eval
       ) 
   )
=>;

   #define code           ::= 
<= 
                method
                (
                   nameattr ( identifier = eval )

                   code
                   (
=>
                   statement* last_statement?
<= 
                   )
                )
=>;

   #define statement      ::= expression ";" ;

   #define last_statement ::= 
 
<=
             returning
             (
=>
                 "^" expression
<=
	     )
=>;

   #define expression     ::= 
<=
             expression
             (
=>
                 object operations?
<=
	     )
=>;

   #define nested     ::= 
<=
             nested
             (
=>
                 method*
<=
	     )
=>;

   #define method     ::= 
<=
             method
             (
=>
                 m_name m_params? m_body
<=
	     )
=>;

   #define m_body         ::=
<= 
                 code
                 (
=>
                   "{" statement* last_statement? "}"
<= 
                 )
=>;

   #define operations     ::= operation next_operation*;

   #define operation      ::= "." message message_body?;

   #define m_params       ::= "(" m_param next_mparam* ")";

   #define next_mparam    ::= "," m_param;

   #define message_body   ::= "(" parameters? ")";

   #define parameters     ::= expression next_parameter? ;

   #define next_parameter ::= "," expression;
   #define next_operation ::= <= ; => operation;

   #define m_name         ::= <= nameattr ( => identifier  <= ) =>;
   #define m_param        ::= <= parameter ( nameattr ( => identifier <= ) ) =>;

   #define object         ::= "{" nested "}" ;
   #define object         ::= <= reference = $reference =>;
   #define object         ::= <= literal = "$literal" =>;
   #define object         ::= identifier;
   #define object         ::= <= numeric = $numeric =>;
   #define message        ::= <= message = $identifier =>;
   #define identifier     ::= <= identifier = $identifier =>;
]]
