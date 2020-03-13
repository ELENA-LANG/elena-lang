import system'math;
import extensions;

//function altcol(x)
//clc, close all
altcol(x)
{
    //% Load auxilliary anonymous functions
    //hR=@(x,y,z)(x-z)*y+z;
    var hR := (x,y,z => (x-z)*y + z);
    
    //hL=@(x,y,z,w)(1-y)*(x-z) + w;
    var hL := (x,y,z,w => (1-y)*(x-z) + w );
    
    //Id=@(x)mod(x,2);
    var Id := (x => x.mod:2 );

    //f=@(x)x/2;
    var f := (x => x / 2);
    
    //g=@(x)3*x+1;
    var g := (x => 3*x + 1);
    
    //% Simple branching
    //disp('Execute branches independently w. half morphism')
    console.printLine:"Execute branches independently w. half morphism";
    
    //y = f( hR(x, 1-Id(x), 0) );
    var y := f(hR(x,1-Id(x),0));
    
    //disp(['1st branch: mod: ', num2str(Id(x)),' out: ', num2str(y)])
    console.printLine("1st branch: mod: ",Id(x)," out: ",y);
    
    //y = g( hR(x, Id(x), -1/3) );
    y := g(hR(x, Id(x), -1.realDiv:3));
    
    //disp(['2nd branch: mod: ',num2str(Id(x)),' out: ', num2str(y)])
    console.printLine("2st branch: mod: ",Id(x)," out: ",y);
    
    //disp '  '
    //% Branching w. safety value return (restore original)
    //disp('Execute branches independently w. full morphism')
    console.printLine:"Execute branches independently w. full morphism";
    
    //y = hL( x, 1-Id(x), 0, f( hR(x, 1-Id(x), 0) ) );
    y := hL(x, 1-Id(x), 0, f(hR(x, 1-Id(x), 0)));
    
    //disp(['1st branch: mod:  ', num2str(Id(x)),' out: ', num2str(y)])
    console.printLine("1st branch: mod: ",Id(x)," out: ",y);
    
    //y = hL( x, Id(x), 0, g( hR(x, Id(x), -1/3) ) );
    y := hL(x, Id(x), 0, g(hR(x, Id(x), -1.realDiv:3)));
    
    //disp(['2nd branch: mod: ', num2str(Id(x)),' out: ',  num2str(y)])
    console.printLine("2st branch: mod: ", Id(x), " out: ", y);
    
    //disp '  '
    //% All together
    //disp('Execute combined morphism')
    console.printLine:"Execute combined morphism";
    
    //y = hL( x, 1-Id(x), 0, f( hR(x, 1-Id(x), 0) ) );
    y := hL(x, 1 - Id(x), 0, f(hR(x, 1- Id(x), 0)));
    
    //z = hL( y, Id(x), 0, g( hR(y, Id(x), -1/3) ) );
    var z := hL(y, Id(x), 0, g(hR(y, Id(x), -1.realDiv:3))).toInt();

    //disp(['Final out: ', num2str(z), ' Correct should be ', num2str( col(x) )])
    console.printLine("Final out: ",z," Correct should be ", col(x));
    
//end
}

col(x)
    = x.isEven().iif((lazy:x / 2), (lazy:3*x + 1));

//function x = col(x)
//if ~mod(x,2)
//	x = x/2;
//else
//	x = 3*x+1;
// end
//end

//function altcolsingle(x)
//clc, close all
altcolsingle(x)
{
    //% Load auxilliary anonymous functions
    //h=@(x,y,z,w)y*(x-z) + w;
    var h := (x,y,z,w => y*(x-z) + w );
    
    //Id=@(x)mod(x,2);
    var Id := (x => x.mod:2 );
    
    //f=@(x)x/2;
    var f := (x => x / 2 );
    
    //g=@(x)3*x+1;
    var g := (x => 3*x + 1 );
    
    //% Simple branching
    //disp('Execute branches independently w. half morphism')
    console.printLine:"Execute branches independently w. half morphism";
    
    //y = f( h(x, 1-Id(x), 0, 0) );
    var y := f(h(x, 1 - Id(x), 0, 0));
    
    //disp(['1st branch: mod: ', num2str(Id(x)),' out: ', num2str(y)])
    console.printLine("1st branch: mod: ",Id(x)," out: ",y);
    
    //y = g( h(x, Id(x), -1/3, -1/3) );
    y := g(h(x,Id(x),-1.realDiv:3, -1.realDiv:3));
    
    //disp(['2nd branch: mod: ',num2str(Id(x)),' out: ', num2str(y)])
    console.printLine("2st branch: mod: ",Id(x)," out: ",y);
    
    //disp '  '
    //% Branching w. safety value return (restore original)
    //disp('Execute branches independently w. full morphism')
    console.printLine:"Execute branches independently w. full morphism";
    
    //y = h( x, Id(x), 0, f( h(x, 1-Id(x), 0, 0) ) );
    y := h(x, Id(x), 0, f(h(x, 1 - Id(x), 0, 0))).toInt();
    
    //disp(['1st branch: mod:  ', num2str(Id(x)),' out: ', num2str(y)])
    console.printLine("1st branch: mod: ", Id(x), " out: ", y);
    
    //y = h( x, 1-Id(x), 0, g( h(x, Id(x), -1/3, -1/3) ) );
    y := h(x, 1 - Id(x),0, g(h(x, Id(x),-1.realDiv:3, -1.realDiv:3))).toInt();
    
    //disp(['2nd branch: mod: ', num2str(Id(x)),' out: ',  num2str(y)])
    console.printLine("2st branch: mod: ", Id(x)," out: ", y);
    
    //disp '  '
    //% All together
    //disp('Execute combined morphism')
    console.printLine:"Execute combined morphism";
    
    //y = h( x, Id(x), 0, f( h(x, 1-Id(x), 0, 0) ) );
    y := h(x, Id(x), 0, f(h(x, 1 - Id(x),0,0)));
    
    //z = h( y, 1-Id(x), 0, g( h(y, Id(x), -1/3, -1/3) ) );
    var z := h(y, 1 - Id(x),0, g(h(y, Id(x),-1.realDiv:3, -1.realDiv:3))).toInt();
    
    //disp(['Final out: ', num2str(z), ' Correct should be ', num2str( col(x) )])
    console.printLine("Final out: ",z," Correct should be ",col(x));
}
//end

public program()
{
    altcol(7);
    altcol(44);
    
    altcolsingle(7);
    altcolsingle(44);
}
