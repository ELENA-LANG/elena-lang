import extensions;

sealed struct OctalNumber
{
    int value;

    int cast() = value;

    constructor(int n)
    {
        value := n;
    }
    
    cast o(string s)
    {
        value := s.toInt(8);
    }

    internal constructor sum(OctalNumber o1, OctalNumber o2)
    {
        int n1 := o1.Value;
        int n2 := o2.Value;

        value := n1 + n2
    }

    internal constructor diff(OctalNumber o1, OctalNumber o2)
    {
        int n1 := o1.Value;
        int n2 := o2.Value;

        value := n1 - n2
    }

    internal constructor prod(OctalNumber o1, OctalNumber o2)
    {
        int n1 := o1.Value;
        int n2 := o2.Value;

        value := n1 * n2
    }

    internal constructor frac(OctalNumber o1, OctalNumber o2)
    {
        int n1 := o1.Value;
        int n2 := o2.Value;

        value := n1 / n2
    }

    int Value = value;  

    string toPrintable()
        = value.toString(8);

    OctalNumber add(OctalNumber n)
        = OctalNumber.sum(self, n);

    OctalNumber subtract(OctalNumber n)
        = OctalNumber.diff(self, n);

    OctalNumber multiply(OctalNumber n)
        = OctalNumber.prod(self, n);

    OctalNumber divide(OctalNumber n)
        = OctalNumber.frac(self, n);
}

public program()
{
    var n := 7o;
    var m := 2o;
    console.printLine(n,"+",m,"=",n + m);

    n := 24o;
    m := 7o;
    console.printLine(n,"+",m,"=",n + m);
    console.printLine(n,"-",m,"=",n - m);
    console.printLine(n,"*",m,"=",n * m);
    console.printLine(n,"/",m,"=",n / m);
}
