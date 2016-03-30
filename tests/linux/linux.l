#import system.
#import system'calendar. 

#symbol program =
[
   #var d1 := Date new &year:2015 &month:1 &day:1 &hour:0&minute:0 &second:0.
   #var d2 := Date new &year:2015 &month:1 &day:11 &hour:1 &minute:0 &second:0.

   #var s := d2 - d1.
   #var msec := s milliseconds.

   console writeLine:d1.
   console writeLine:d2.
   console writeLine:msec.
   console writeLine:s.

   console readChar.
].