import system'routines;
import extensions;
import extensions'text;

const string Target = "METHINKS IT IS LIKE A WEASEL";
const string AllowedCharacters = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";

const int C = 100;
const real P = 0.05r;

rnd = randomGenerator;

randomChar 
    = AllowedCharacters[rnd.nextInt(AllowedCharacters.Length)];

extension evoHelper
{
    randomString()
        = 0.repeatTill(self).selectBy::(x => randomChar).summarize(new StringWriter());
        
    fitnessOf(s)
        = self.zipBy(s, (a,b => (a==b) ? 1 : 0)).summarize(new Integer()).toInt();
        
    mutate(p)
        = self.selectBy::(ch => (rnd.nextReal() <= p) ? randomChar : ch).summarize(new StringWriter());
}

class EvoAlgorithm : Enumerator
{
    object _target;
    object _current;
    object _variantCount;

    constructor new(s,count)
    {
        _target := s;
        _variantCount := count.toInt();
    }

    get Value() = _current;

    bool next()
    {
        if (nil == _current)
            { _current := _target.Length.randomString(); ^ true };
            
        if (_target == _current)
            { ^ false };
            
        auto variants := Array.allocate(_variantCount).populate::(x => _current.mutate(P) );
        
        _current := variants.sort::(a,b => a.fitnessOf(Target) > b.fitnessOf(Target) ).at(0);
        
        ^ true
    } 
    
    reset()
    {
        _current := nil
    }
    
    enumerable() => _target;                          
}

public program()
{
    var attempt := new Integer();
    EvoAlgorithm.new(Target,C).forEach::(current)
    {
        Console 
            .printPaddingLeft(10,"#",attempt.append(1))
            .printLine(" ",current," fitness: ",current.fitnessOf(Target))
    };
    
    Console.readChar()
}