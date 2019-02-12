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

    #define new_leaf   ::= <= __target.newLeaf({ => "(" body ")" <= }); =>;
    #define new_leaf   ::= <= __target.newFunction({ => "<" body ">" <= }); =>;
    #define new_node   ::= <= __target.newNode({ => "[" body "]" <= }); =>;
    #define add_cont   ::= "+=" <= __target.addContent(c); =>;

    #define level        ::= integer; 
    #define next_state   ::= <= __target. => "=" out_state <= (); =>;

    #define in_state     ::= ident <= : { =>;
    #define state        ::= ident <= : function(c) { => ;
    #define out_state    ::= ident;
    #define body         ::= <= $literal =>;
    #define ident        ::= <= $identifier =>;
]]

State0
  : digit => ( "order : 5; evalToken: function(s) { return convertor.toReal(s); };" ) += = State2
  : minus => ( "order : 5; evalToken: function(s) { return convertor.toReal(s); };" ) += = State2
  : letter => < "order : 6; sqrt : function(v) { return math.sqrt(v); }; min : function(l,r) { return math.min(l,r); };" > += = State3
  : opening => = State0
  : comma => = State0
  : whitespace => = State0

State1
  : plus => [ "order : 1; evalNode: function(left,right){ return left.add(right); }; " ] = State0
  : minus => [ "order : 1; evalNode: function(left,right){ return left.subtract(right); }; " ] = State0
  : star => [ "order : 2; evalNode: function(left,right){ return left.multiply(right); }; " ] = State0
  : slash => [ "order : 2; evalNode: function(left,right){ return left.divide(right); }; " ] = State0
  : closing => = State1

State2
  : digit => += = State2
  : plus => [ "order : 1; evalNode: function(left,right){ return left.add(right); }; " ] = State0
  : minus => [ "order : 1; evalNode: function(left,right){ return left.subtract(right); }; " ] = State0
  : star => [ "order : 2; evalNode: function(left,right){ return left.multiply(right); }; " ] = State0
  : slash => [ "order : 2; evalNode: function(left,right){ return left.divide(right); }; " ] = State0
  : closing => = State1
  : dot => += =State2
  : comma => = State0
  : whitespace => = State2

State3
  : letter => += = State3
  : opening => = State0
