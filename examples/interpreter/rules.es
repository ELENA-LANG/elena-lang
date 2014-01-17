[[
   #define command ::= %append(0) "+" command; 
   #define command ::= %reduce(0) "-" command; 
   #define command ::= %next(0) ">" command; 
   #define command ::= %previous(0) "<" command; 
   #define command ::= %input(0) "," command; 
   #define command ::= %output(0) "." command; 
   #define command ::= { 1 system'dynamic'tapeControl %read(1,) "[" command;
   #define command ::= } system'dynamic'Tape 1 system'dynamic'tapeControl %read(1,) %run(1) "]" command;
   #define command ::= $eof; 
   #define command ::= $any command;
   #define start ::= { 1 system'dynamic'tapeControl %read(1,) command } system'dynamic'Tape;
]]                                
@@
