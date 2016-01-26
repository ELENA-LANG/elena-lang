#import system.
#import system'math.

#symbol program =
[
    console writeLine:"Radians:".
    console writeLine:("sin(π/3) = " + ((pi_value/3) sin) literal).
    console writeLine:("cos(π/3) = " + ((pi_value/3) cos) literal).
    console writeLine:("tan(π/3) = " + ((pi_value/3) tan) literal).
    console writeLine:("arcsin(1/2) = " + (0.5r arcsin literal)).
    console writeLine:("arccos(1/2) = " + (0.5r arccos literal)).
    console writeLine:("arctan(1/2) = " + (0.5r arctan literal)).
    console writeLine:"".
    
    console writeLine:"Degrees:".
    console writeLine:("sin(60º) = " + ((60.0r radian) sin) literal).
    console writeLine:("cos(60º) = " + ((60.0r radian) cos) literal).
    console writeLine:("tan(60º) = " + ((60.0r radian) tan) literal).
    console writeLine:("arcsin(1/2) = " + ((0.5r arcsin) degree) literal).
    console writeLine:("arccos(1/2) = " + ((0.5r arccos) degree literal)).
    console writeLine:("arctan(1/2) = " + ((0.5r arctan) degree literal)).
].