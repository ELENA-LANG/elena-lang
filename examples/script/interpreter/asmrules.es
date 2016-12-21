[[
   #grammar cf

   #define start      ::= <= [ 2 %"system'dynamic'tapeOp.var&args$[]" => commands <= %"get[0]" * system'dynamic'Tape ] =>;

   #define commands   ::= command commands;
   #define commands   ::= comment commands;
   #define commands   ::= $eof;

   #define command    ::= <= %"output[0]" => ".";
   #define command    ::= <= %"input[0]" => ",";
   #define command    ::= <= %"previous[0]" => "<";
   #define command    ::= <= %"next[0]" => ">";
   #define command    ::= <= %"increase[0]" => "+";
   #define command    ::= <= %"decrease[0]" => "-";
   #define command    ::= <= %"open[0]" => "[";
   #define command    ::= <= %"close[0]" => "]";

   #define comment    ::= " " comments;
   #define comment    ::= "'" comments;
   #define comment    ::= "!" comments;
   #define comment    ::= $eol;

   #define comments   ::= $chr comments;
   #define comments   ::= $eps;

   #mode symbolic;
]]
