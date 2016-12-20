[[
   #grammar cf

   #define start      ::= <= [ 2 %"system'dynamic'tapeOp.var&args$[]" => commands <= * system'dynamic'Tape ] =>;

   #define commands   ::= command commands;
   #define commands   ::= $eof;

   #define command    ::= <= %"output[0]" => ".";
   #define command    ::= <= %"input[0]" => ",";
   #define command    ::= <= %"previous[0]" => "<";
   #define command    ::= <= %"next[0]" => ">";
   #define command    ::= <= %"append[0]" => "+";
   #define command    ::= <= %"reduce[0]" => "-";
   #define command    ::= " ";

   #define command    ::= <= -2 %"system'dynamic'tapeOp.ptr&args$[]" 1 %"system'dynamic'tapeOp.stack&args$[]" %"push[1]" => "[";
   #define command    ::= <= 0 %"system'dynamic'tapeOp.stack&args$[]" %"check[0]" 1 %"system'dynamic'tapeOp.stack&args$[]" %"pop[0]" %"system'dynamic'tapeOp.jumpif&args$[13]" => "]";

   #mode symbolic;
]]
