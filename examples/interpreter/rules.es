[[
   #define start ::= $eof;
   #define start ::= <= &tape [ tape => command <= ] => ;
   #define command ::= <= .append() => "+" command ;
   #define command ::= <= .reduce() => "-" command;
   #define command ::= <= .next() => ">" command;
   #define command ::= <= .previous() => "<" command;
   #define command ::= <= .input() => "," command;
   #define command ::= <= .output() => "." command;
   #define command ::= <= .run( &tape [ tape => "[" command ;
   #define command ::= <= ]) => "]" command;
   #define command ::= $any command;
   #define command ::= $eof; 
   #mode symbolic;
]]                                
