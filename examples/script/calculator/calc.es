[[
    #grammar cf

    #define start      ::= <= #import "extensions'convertor" as convertor;#import "extensions'math'mathControl" as math; States = { => rule start_r <= }; =>;

    #define start_r    ::= rule start_r;
    #define start_r    ::= $eof;

    #define rule       ::= in_state ":" rule_r rule_rs <= }; =>;
    #define rule_r     ::= state rule_body <= }; =>;
    #define rule_rs    ::= ":" rule_r rule_rs;
    #define rule_rs    ::= $eps;

    #define rule_body  ::= "=>" new_leaf add_cont next_state;
    #define rule_body  ::= "=>" new_node next_state;
    #define rule_body  ::= "=>" add_cont next_state;
    #define rule_body  ::= "=>" next_state;

    #define new_leaf   ::= <= this.newLeaf({ => "(" body ")" <= }); =>;
    #define new_leaf   ::= <= this.newFunction({ => "<" body ">" <= }); =>;
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
  : digit => ( "order : 5; evalToken: function(s) { return convertor.toReal(s); };" ) += = state2
  : minus => ( "order : 5; evalToken: function(s) { return convertor.toReal(s); };" ) += = state2
  : letter => < "order : 6; sqrt : function(v) { return math.sqrt(v); }; min : function(l,r) { return math.min(l,r); };" > += = state3
  : opening => = state0
  : comma => = state0
  : whitespace => = state0

state1
  : plus => [ "order : 1; evalNode: function(left,right){ return left.add(right); }; " ] = state0
  : minus => [ "order : 1; evalNode: function(left,right){ return left.subtract(right); }; " ] = state0
  : star => [ "order : 2; evalNode: function(left,right){ return left.multiply(right); }; " ] = state0
  : slash => [ "order : 2; evalNode: function(left,right){ return left.divide(right); }; " ] = state0
  : closing => = state1

state2
  : digit => += = state2
  : plus => [ "order : 1; evalNode: function(left,right){ return left.add(right); }; " ] = state0
  : minus => [ "order : 1; evalNode: function(left,right){ return left.subtract(right); }; " ] = state0
  : star => [ "order : 2; evalNode: function(left,right){ return left.multiply(right); }; " ] = state0
  : slash => [ "order : 2; evalNode: function(left,right){ return left.divide(right); }; " ] = state0
  : closing => = state1
  : dot => += =state2
  : comma => = state0
  : whitespace => = state2

state3
  : letter => += = state3
  : opening => = state0
