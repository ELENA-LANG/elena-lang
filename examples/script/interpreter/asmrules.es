[[
   #grammar build
   #grammar cf

   #define start      ::= <= system'dynamic'ClosureTape ( => command commands <= ) =>;
   #define start      ::= $eof;

   #define commands   ::= command commands;
   #define commands   ::= comment commands;
   #define commands   ::= $eof;

   #define command    ::= <= system'dynamic'MessageClosure ( "output[1]"   ) => ".";
   #define command    ::= <= system'dynamic'MessageClosure ( "input[1]"    ) => ",";
   #define command    ::= <= system'dynamic'MessageClosure ( "previous[1]" ) => "<";
   #define command    ::= <= system'dynamic'MessageClosure ( "next[1]"     ) => ">";
   #define command    ::= <= system'dynamic'MessageClosure ( "increase[1]" ) => "+";
   #define command    ::= <= system'dynamic'MessageClosure ( "decrease[1]" ) => "-";
   #define command    ::= <= system'dynamic'MessageClosure ( "open[1]"     ) => "[";
   #define command    ::= <= system'dynamic'MessageClosure ( "close[1]"    ) => "]";

   #define comment    ::= " " comments;
   #define comment    ::= "'" comments;
   #define comment    ::= "!" comments;
   #define comment    ::= $eol;

   #define comments   ::= $chr comments;
   #define comments   ::= $eps;

   #mode symbolic;
]]
