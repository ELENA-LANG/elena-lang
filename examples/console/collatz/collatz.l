#import system.
#import system'math.
#import extensions.

//function altcol(x)
//clc, close all
#symbol altcol = (:x)
[
    //% Load auxilliary anonymous functions
    //hR=@(x,y,z)(x-z)*y+z;
    #var hR := (:x:y:z) [ (x-z)*y + z ].
    
    //hL=@(x,y,z,w)(1-y)*(x-z) + w;
    #var hL := (:x:y:z:w) [ (1-y)*(x-z) + w ].
    
    //Id=@(x)mod(x,2);
    #var Id := (:x) [ x mod:2 ].

    //f=@(x)x/2;
    #var f := (:x) [ x / 2 ].
    
    //g=@(x)3*x+1;
    #var g := (:x) [ 3*x + 1 ].
    
    //% Simple branching
    //disp('Execute branches independently w. half morphism')
    console writeLine:"Execute branches independently w. half morphism".
    
    //y = f( hR(x, 1-Id(x), 0) );
    #var y := f eval:(hR eval:x:(1 - Id eval:x):0).
    
    //disp(['1st branch: mod: ', num2str(Id(x)),' out: ', num2str(y)])
    console writeLine:"1st branch: mod: ":(Id eval:x):" out: ":y.
    
    //y = g( hR(x, Id(x), -1/3) );
    y := g eval:(hR eval:x:(Id eval:x):(-1 realDiv:3)).
    
    //disp(['2nd branch: mod: ',num2str(Id(x)),' out: ', num2str(y)])
    console writeLine:"2st branch: mod: ":(Id eval:x):" out: ":y.
    
    //disp '  '
    //% Branching w. safety value return (restore original)
    //disp('Execute branches independently w. full morphism')
    console writeLine:"Execute branches independently w. full morphism".
    
    //y = hL( x, 1-Id(x), 0, f( hR(x, 1-Id(x), 0) ) );
    y := hL eval:x:(1 - Id eval:x):0:(f eval:(hR eval:x:(1-Id eval:x):0)).
    
    //disp(['1st branch: mod:  ', num2str(Id(x)),' out: ', num2str(y)])
    console writeLine:"1st branch: mod: ":(Id eval:x):" out: ":y.
    
    //y = hL( x, Id(x), 0, g( hR(x, Id(x), -1/3) ) );
    y := hL eval:x:(Id eval:x):0:(g eval:(hR eval:x:(Id eval:x):(-1 realDiv:3))).
    
    //disp(['2nd branch: mod: ', num2str(Id(x)),' out: ',  num2str(y)])
    console writeLine:"2st branch: mod: ":(Id eval:x):" out: ":y.
    
    //disp '  '
    //% All together
    //disp('Execute combined morphism')
    console writeLine:"Execute combined morphism".
    
    //y = hL( x, 1-Id(x), 0, f( hR(x, 1-Id(x), 0) ) );
    y := hL eval:x:(1 - Id eval:x):0:(f eval:(hR eval:x:(1- Id eval:x):0)).
    
    //z = hL( y, Id(x), 0, g( hR(y, Id(x), -1/3) ) );
    #var z := hL eval:y:(Id eval:x):0:(g eval:(hR eval:y:(Id eval:x):(-1 realDiv:3))) toInt.
    
    //disp(['Final out: ', num2str(z), ' Correct should be ', num2str( col(x) )])
    console writeLine:"Final out: ":z:" Correct should be ":(col eval:x).
    
//end
].

#symbol col = (:x)
    [ x int is &even iif:[ x / 2 ]:[ 3*x + 1 ] ].

//function x = col(x)
//if ~mod(x,2)
//	x = x/2;
//else
//	x = 3*x+1;
// end
//end

//function altcolsingle(x)
//clc, close all
#symbol altcolsingle = (:x)
[
    //% Load auxilliary anonymous functions
    //h=@(x,y,z,w)y*(x-z) + w;
    #var h := (:x:y:z:w) [ y*(x-z) + w ].
    
    //Id=@(x)mod(x,2);
    #var Id := (:x) [ x mod:2 ].
    
    //f=@(x)x/2;
    #var f := (:x) [ x / 2 ].
    
    //g=@(x)3*x+1;
    #var g := (:x) [ 3*x + 1 ].
    
    //% Simple branching
    //disp('Execute branches independently w. half morphism')
    console writeLine:"Execute branches independently w. half morphism".
    
    //y = f( h(x, 1-Id(x), 0, 0) );
    #var y := f eval:(h eval:x:(1 - Id eval:x):0:0).
    
    //disp(['1st branch: mod: ', num2str(Id(x)),' out: ', num2str(y)])
    console writeLine:"1st branch: mod: ":(Id eval:x):" out: ":y.
    
    //y = g( h(x, Id(x), -1/3, -1/3) );
    y := g eval:(h eval:x:(Id eval:x):(-1 realDiv:3):(-1 realDiv:3)).
    
    //disp(['2nd branch: mod: ',num2str(Id(x)),' out: ', num2str(y)])
    console writeLine:"2st branch: mod: ":(Id eval:x):" out: ":y.
    
    //disp '  '
    //% Branching w. safety value return (restore original)
    //disp('Execute branches independently w. full morphism')
    console writeLine:"Execute branches independently w. full morphism".
    
    //y = h( x, Id(x), 0, f( h(x, 1-Id(x), 0, 0) ) );
    y := h eval:x:(Id eval:x):0:(f eval:(h eval:x:(1 - Id eval:x):0:0)) toInt.
    
    //disp(['1st branch: mod:  ', num2str(Id(x)),' out: ', num2str(y)])
    console writeLine:"1st branch: mod: ":(Id eval:x):" out: ":y.
    
    //y = h( x, 1-Id(x), 0, g( h(x, Id(x), -1/3, -1/3) ) );
    y := h eval:x:(1 - Id eval:x):0:(g eval:(h eval:x:(Id eval:x):(-1 realDiv:3):(-1 realDiv:3))) toInt.
    
    //disp(['2nd branch: mod: ', num2str(Id(x)),' out: ',  num2str(y)])
    console writeLine:"2st branch: mod: ":(Id eval:x):" out: ":y.
    
    //disp '  '
    //% All together
    //disp('Execute combined morphism')
    console writeLine:"Execute combined morphism".
    
    //y = h( x, Id(x), 0, f( h(x, 1-Id(x), 0, 0) ) );
    y := h eval:x:(Id eval:x):0:(f eval:(h eval:x:(1 - Id eval:x):0:0)).
    
    //z = h( y, 1-Id(x), 0, g( h(y, Id(x), -1/3, -1/3) ) );
    #var z := h eval:y:(1 - Id eval:x):0:(g eval:(h eval:x:(Id eval:x):(-1 realDiv:3):(-1 realDiv:3))) toInt.
    
    //disp(['Final out: ', num2str(z), ' Correct should be ', num2str( col(x) )])
    console writeLine:"Final out: ":z:" Correct should be ":(col eval:x).
].
//end

#symbol program =
[
    altcol eval:7.
    altcol eval:44.
    
    altcolsingle eval:7.
    altcolsingle eval:44.
].
