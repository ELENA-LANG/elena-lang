import extensions;
import system'math;
import extensions'math;

Pi = RealNumber.Pi;

public program()
{
    console
        .printLine("sin(π/3) = ",(Pi / 3).sin())
        .printLine("cos(π/3) = ",cos(Pi / 3))
        .printLine("tan(π/3) = ",tan(Pi / 3))
}