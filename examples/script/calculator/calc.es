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
    #define object     ::= integer;

    #define operation  ::= <= . => "." property;
    #define operation  ::= <= . => "." message m_args;

    #define m_args     ::= <= ( => "(" ")" <= ) =>;
    #define m_args     ::= <= ( => "(" parameter next_param* ")" <= ) =>;

    #define next_param ::= <= , => "," parameter;
    #define parameter  ::= object;

    #define identifier ::= <= $identifier =>;
    #define property   ::= <= $identifier =>;
    #define message    ::= <= $identifier =>;
    #define integer    ::= <= $numeric =>;

]]

State0  : 
  digit => { $machine.newLeaf(3); $machine.appendContent($input); $machine.State1; }
  minus => { $machine.newLeaf(3); $machine.appendContent($input); $machine.State1; };

State1 :
  digit => { $machine.appendContent($input); }
  dot   => { $machine.appendContent($input); }
  plus  => { $machine.newNode(1, $input); $machine.State0; }
  minus => { $machine.newNode(1, $input); $machine.State0; }
  star  => { $machine.newNode(2, $input); $machine.State0; }
  slash => { $machine.newNode(2, $input); $machine.State0; };
