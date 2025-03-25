import extensions;

public program()
{
    real pi := 0.0;
    int i := 0;

    pi += -1.0.power(i) / (i*2+1) * 4;
    i++;
    pi += -1.0.power(i) / (i*2+1) * 4;

    Console.printLine(pi);
}