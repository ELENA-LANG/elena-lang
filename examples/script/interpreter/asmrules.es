[[
   #grammar transform
   #grammar cf

   #define start      ::= <= ( > => commands <= " * system'dynamic'ClosureTape= " # ) =>;

   #define commands   ::= command commands;
   #define commands   ::= comment commands;
   #define commands   ::= $eof;

   #define command    ::= <= += " %""output[0]"" system'dynamic'MessageClosure ^""new[1]"" " => ".";
   #define command    ::= <= += " %""input[0]"" system'dynamic'MessageClosure ^""new[1]"" " => ",";
   #define command    ::= <= += " %""previous[0]"" system'dynamic'MessageClosure ^""new[1]"" " => "<";
   #define command    ::= <= += " %""next[0]"" system'dynamic'MessageClosure ^""new[1]"" " => ">";
   #define command    ::= <= += " %""increase[0]"" system'dynamic'MessageClosure ^""new[1]"" " => "+";
   #define command    ::= <= += " %""decrease[0]"" system'dynamic'MessageClosure ^""new[1]"" " => "-";
   #define command    ::= <= += " %""open[0]"" system'dynamic'MessageClosure ^""new[1]"" " => "[";
   #define command    ::= <= += " %""close[0]"" system'dynamic'MessageClosure ^""new[1]"" " => "]";

   #define comment    ::= " " comments;
   #define comment    ::= "'" comments;
   #define comment    ::= "!" comments;
   #define comment    ::= $eol;

   #define comments   ::= $chr comments;
   #define comments   ::= $eps;

   #mode symbolic;
]]
