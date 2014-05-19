[[
   #define command ::= < .append() > "+" command; 
   #define command ::= < .reduce() > "-" command; 
   #define command ::= < .next() > ">" command; 
   #define command ::= < .previous() > "<" command; 
   #define command ::= < .input() > "," command; 
   #define command ::= < .output() > "." command; 
   #define command ::= < { eval : &tape [ tape > "[" command;
   #define command ::= < ] > "]" command;
   #define command ::= $eof; 
   #define command ::= $any command;
   #define start ::= < { 1 system'dynamic'tapeControl %read(1,) > command < } system'dynamic'Tape >;
]]                                
@@
