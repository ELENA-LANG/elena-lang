#define system.
#define system'routines.
#import system'math.
#import extensions'routines.
 
#class(extension)op
{
    #method compose : f : g
        = (self::g eval)::f eval.
}
 
#symbol program =
[
   #var fs := (%(mathOp get&sin)(0), %(mathOp get&cos)(0), =[ self power:3.0r ]).
   #var gs := (%(mathOp get&arcsin)(0), %(mathOp get&arccos)(0), =[ self power:(1.0r / 3) ]).
 
   fs zip:gs &into: (:f:g)[ 0.5r compose:f:g ]
        run &each:printingLn.
].