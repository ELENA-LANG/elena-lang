[[
    #grammar cf

    #define start      ::= 
<= 
    States = {
=>
                             $eof
<=
    };
=>;

]]

State0  : 
  digit => { $machine.State1; };

