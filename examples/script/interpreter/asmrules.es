[[
   #grammar build
   #grammar cf

   #define start      ::= <= system'dynamic'ClosureTape ( => commands <= ) =>;

   #define commands   ::= command commands;
   #define commands   ::= comment commands;
   #define commands   ::= $eof;

   #define command    ::= <= system'dynamic'MessageClosure ( "output[0]"   ) => ".";
   #define command    ::= <= system'dynamic'MessageClosure ( "input[0]"    ) => ",";
   #define command    ::= <= system'dynamic'MessageClosure ( "previous[0]" ) => "<";
   #define command    ::= <= system'dynamic'MessageClosure ( "next[0]"     ) => ">";
   #define command    ::= <= system'dynamic'MessageClosure ( "increase[0]" ) => "+";
   #define command    ::= <= system'dynamic'MessageClosure ( "decrease[0]" ) => "-";
   #define command    ::= <= system'dynamic'MessageClosure ( "open[0]"     ) => "[";
   #define command    ::= <= system'dynamic'MessageClosure ( "close[0]"    ) => "]";

   #define comment    ::= " " comments;
   #define comment    ::= "'" comments;
   #define comment    ::= "!" comments;
   #define comment    ::= $eol;

   #define comments   ::= $chr comments;
   #define comments   ::= $eps;

   #mode symbolic;
]]
