#define system.
#define system'dynamic.
#define extensions'dynamic.

#symbol ReflectionTester =
[
    #var anObject := String new:"abd".

    #var aPresentation := anObject~ETextSerializer Serialized.
].

#symbol TapeTest =
[
   #var aTape := Tape(
       "Hello World", 
       'program'Output,
       %(eval&writeLine),
       'program'Input,
       %(readChar)).

    aTape eval.
].

#symbol Program =
[   
    ReflectionTester eval.
    TapeTest eval.
].