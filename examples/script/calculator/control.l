import extensions;
import extensions'scripting;

// --- testCase ---

preloaded subjects = new object[]{ subjconst States,subjconst State0,subjconst State1,subjconst State2,subjconst State3 };

preloaded functions = new object[]{ extensions'math'sqrt,extensions'math'min };

public static dynamicRules = 
    lscript.evalPath("calc.es").States;
    
public symbol staticRules =
    calc'rules'States;

extension testOp
{
    testEvaluation()
    {
        var result := self.evaluatedWith(dynamicRules);
        
        console.printLine(self," evaluated as ",result)
    }
}

extension staticTestOp
{
    testStaticEvaluation()
    {
        var result := self.evaluatedWith(staticRules);
        
        console.printLine(self," evaluated as ",result)
    }
}

public testCases()
{    
    "22.4".testEvaluation();

    "2+3-1".testEvaluation();
            
    "-2+3".testEvaluation();

    "2+3*4".testEvaluation();

    "(2+3)*4".testEvaluation();
    
    "sqrt(2*2)".testEvaluation();
    
    "min(2,3)".testEvaluation();
}

public staticTestCases()
{
    "22.4".testStaticEvaluation();

    "2+3-1".testStaticEvaluation();
            
    "-2+3".testStaticEvaluation();

    "2+3*4".testStaticEvaluation();

    "(2+3)*4".testStaticEvaluation();
    
    "sqrt(2*2)".testStaticEvaluation();
    
    "min(2,3)".testStaticEvaluation();
}

// --- controller ---

public singleton control
{
    greeting() = "Enter the expression to evaluate (e.g. 2+2) or -q to exit";
    
    Prompt = ">";
    
    proceed(line)
    {
        if ("-q" == line)
        {
            forward program.stop(); ^emptyString
        }
        else
        {
            ^ line.evaluatedWith(forward rules)
        }
    }    
}
