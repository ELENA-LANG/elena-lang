import extensions;

public program()
{
    real pi := 0.0;
    int i := 0;

    pi += -1.0.power(i) / (i*2+1) * 4;
    i++;
    real a :=  -1.0.power(i);
    real b := (i*2+1) * 4;
    real r := a / b;
    pi += r;

    Console.printLine(a);
    Console.printLine(b);
    Console.printLine(r);
    Console.printLine(pi);
}