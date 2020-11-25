[[
   #grammar build
   #grammar cf

   #define start      ::= <= system'dynamic'DynamicTape ( => command commands <= ) =>;
   #define start      ::= <= system'dynamic'DynamicTape ( => comment commands <= ) =>;
   #define start      ::= $eof;

   #define commands   ::= command commands;
   #define commands   ::= comment commands;
   #define commands   ::= $eof;

   #define command    ::= <= system'dynamic'MessageFunction ( "output[1]"   ) => ".";
   #define command    ::= <= system'dynamic'MessageFunction ( "input[1]"    ) => ",";
   #define command    ::= <= system'dynamic'MessageFunction ( "previous[1]" ) => "<";
   #define command    ::= <= system'dynamic'MessageFunction ( "next[1]"     ) => ">";
   #define command    ::= <= system'dynamic'MessageFunction ( "increase[1]" ) => "+";
   #define command    ::= <= system'dynamic'MessageFunction ( "decrease[1]" ) => "-";
   #define command    ::= <= system'dynamic'MessageFunction ( "open[1]"     ) => "[";
   #define command    ::= <= system'dynamic'MessageFunction ( "close[1]"    ) => "]";

   #define comment    ::= " " comments;
   #define comment    ::= "'" comments;
   #define comment    ::= "!" comments;
   #define comment    ::= "*" comments;
   #define comment    ::= $eol;

   #define comments   ::= $chr comments;
   #define comments   ::= $eps;

   #mode symbolic;
]]
