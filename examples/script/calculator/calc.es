[[
    #grammar cf

    #define start      ::= 
<= 
    States = {
=>
                            state* $eof
<=
    };
=>;

    #define state      ::= identifier ":" body ";";
    #define body       ::= 
<= 
       = { 
=> 
                           rule* 
<= 
       }; 
=>;

    #define rule       ::= identifier "=>" rul_body;
    #define rul_body   ::= 
<=
           ( current )
           {
=>
                           "{" statement* "}"
<=
           }
=>;

    #define statement  ::= object operation ";" <= ; =>;
    #define object     ::= <= self => "$machine";

    #define operation  ::= <= . => "." property;

    #define identifier ::= <= $identifier =>;
    #define property   ::= <= $identifier =>;

]]

State0  : 
  digit => { $machine.State0; };
