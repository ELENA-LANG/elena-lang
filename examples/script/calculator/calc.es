[[
    #grammar cf

    #define start      ::= <= #import "extensions'convertor" as convertor; States = { => rule start_r <= }; =>;

    #define start_r    ::= rule start_r;
    #define start_r    ::= $eof;

    #define rule       ::= in_state ":" rule_r rule_rs <= }; =>;
    #define rule_r     ::= state rule_body <= }; =>;
    #define rule_rs    ::= ":" rule_r rule_rs;
    #define rule_rs    ::= $eps;

    #define rule_body  ::= "=>" new_leaf add_cont next_state;
    #define rule_body  ::= "=>" new_node next_state;
    #define rule_body  ::= "=>" add_cont next_state;

    #define new_leaf   ::= <= this.newLeaf({ => "(" body ")" <= }); =>;
    #define new_node   ::= <= this.newNode({ => "[" body "]" <= }); =>;
    #define add_cont   ::= "+=" <= this.addContent(c); =>;

    #define level        ::= integer; 
    #define next_state   ::= <= this.seek => "=" out_state <= (); =>;

    #define in_state     ::= ident <= : { =>;
    #define state        ::= ident <= : function(c) { => ;
    #define out_state    ::= <= < % => ident <= > =>;
    #define body         ::= <= $literal =>;
    #define ident        ::= <= $identifier =>;
]]

state0
  : digit => ( "level : 9; evalToken: function(s) { return convertor.toReal(s); };" ) += = digit

digit
  : digit => += = digit
  : plus => [ "level : 1; evalNode: function(left,right){ return left.add(right); }; " ] = state0
  : minus => [ "level : 1; evalNode: function(left,right){ return left.subtract(right); }; " ] = state0
