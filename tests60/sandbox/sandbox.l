import extensions;

public program()
{
    int i := 1;

    real a :=  -1.0.power(i);
    real b := (i*2+1);
    real c := a / b;
    real d := c * 4;

    Console.printLine(a);
    Console.printLine(b);
    Console.printLine(c);
    Console.printLine(d);
    Console.printLine(a / (i*2+1) * 4);
    Console.printLine(-1.0.power(i) / (i*2+1) * 4);
}