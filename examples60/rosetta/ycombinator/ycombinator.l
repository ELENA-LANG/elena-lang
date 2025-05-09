import extensions;
 
singleton YCombinator
{
    fix(func)
        = (f){(x){ x(x) }((g){ f((x){ (g(g))(x) })})}(func);
}
 
public program()
{
    var fib := YCombinator.fix::(f => (i => (i <= 1) ? i : (f(i-1) + f(i-2)) ));
    var fact := YCombinator.fix::(f => (i => (i == 0) ? 1 : (f(i-1) * i) ));
 
    Console.printLine("fib(10)=",fib(10));
    Console.printLine("fact(10)=",fact(10));
}