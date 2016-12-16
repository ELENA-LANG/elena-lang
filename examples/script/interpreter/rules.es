[[
   #grammar cf

   #define start      ::= <= [ ( 2 %"system'dynamic'tapeOp.var&args$[]" => commands <=  ) * system'dynamic'Tape ] =>;

   #define commands   ::= command commands;
   #define commands   ::= $eof;

   #define command    ::= <= %"input[0]" => ".";
   #define command    ::= <= %"output[0]" => ",";
]]                                
