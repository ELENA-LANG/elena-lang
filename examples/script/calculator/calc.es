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
           ( input )
           {
=>
                           "{" statement* "}"
<=
           }
=>;

    #define statement  ::= object operation ";" <= ; =>;
    #define object     ::= <= self => "$machine";
    #define object     ::= <= input => "$input";

    #define operation  ::= <= . => "." property;
    #define operation  ::= <= . => "." message m_args;

    #define m_args     ::= <= ( => "(" ")" <= ) =>;
    #define m_args     ::= <= ( => "(" parameter ")" <= ) =>;

    #define parameter  ::= object;

    #define identifier ::= <= $identifier =>;
    #define property   ::= <= $identifier =>;
    #define message    ::= <= $identifier =>;

]]

State0  : 
  digit => { $machine.newLeaf(); $machine.appendContent($input); $machine.State1; };

State1 :
  digit => { $machine.appendContent($input); };
