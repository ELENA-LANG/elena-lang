import system'collections;
import system'routines;
import system'dynamic'expressions;
import extensions;
import extensions'scripting;

symbol used : preloaded= new object[]{mssg get, mssg set};

class Person
{
    string FirstName : prop;
    
    string LastName : prop;
    
    int Age : prop;

    // override Age property accessor to support a string value as well   
    set Age(string value)
    {
        self.Age := value.toInt()
    }
}

// Select a solution

const int Solution = 5; // put 1,2,3,4 or 5

// Solution 1

getProperty(object target, string name)
{
   auto prop := new MessageName(name);
   
   ^ prop.getPropertyValue(target)
}

setProperty(object target, string name, value)
{
    auto prop := new MessageName(name);
    
    prop.setPropertyValue(target, value);
    
    ^ target
}

static verbs = new Map<string,object>()
                    .setAt("get",getProperty)
                    .setAt("set",setProperty);

// Solution 2

getPropertyTree(target,name)
{
    auto prop := new MessageName(name);
    
    ^ Expression.GetProperty(prop, 
        Expression.Constant(target)).eval()
}

setPropertyTree(target,name,value)
{
    auto prop := new MessageName(name);
    
    ^ Expression.SetProperty(prop, 
        Expression.Constant(target),
        Expression.Constant(value)).eval()
}

static nodes = new Map<string,object>()
                    .setAt("get",getPropertyTree)
                    .setAt("set",setPropertyTree);

// Solution 3

static nodes2 = new Map<string,object>()
                    .setAt("set",Expression.Closure(
                                    Expression.CodeBlock(
                                        Expression.SetDynamicProperty(
                                            Expression.Variable("name"),
                                            Expression.Variable("target"),
                                            Expression.Variable("value")
                                        )
                                    ),
                                    ScopeIdentifier.Variable("target"),
                                    ScopeIdentifier.Variable("name"),
                                    ScopeIdentifier.Variable("value")
                    ))
                    .setAt("get",Expression.Closure(
                                    Expression.CodeBlock(
                                        Expression.Return(
                                            Expression.GetDynamicProperty(
                                                Expression.Variable("name"),
                                                Expression.Variable("target")
                                            )
                                        )
                                    ),
                                    ScopeIdentifier.Variable("target"),
                                    ScopeIdentifier.Variable("name")                                             
                    ));
                    
// Solution 4

static classTree = DynamicSingleton.new(
        Expression.Method(
            "get",            
            Expression.CodeBlock(
                Expression.Return(
                    Expression.GetDynamicProperty(
                        Expression.Variable("name"),
                        Expression.Variable("target")
                    )
                )
            ),                     
            ScopeIdentifier.Variable("target"),
            ScopeIdentifier.Variable("name")
        ),
        Expression.Method(
            "set",
            Expression.CodeBlock(
                Expression.SetDynamicProperty(
                    Expression.Variable("name"),
                    Expression.Variable("target"),
                    Expression.Variable("value")
                )
            ),
            ScopeIdentifier.Variable("target"),
            ScopeIdentifier.Variable("name"),
            ScopeIdentifier.Variable("value")                     
        )
    ).compile();

// Solution 5

static classTreeFromScript
    = new ScriptEngine().buildScriptFile("exprtree2.es").compile();

// Common code

closureSolution(select) = (r,words)
{
    var f := select(words[0]);
    (words.Length) =>
        2 {
            console.printLine(f(r, words[1]));
        }
        3 {
            f(r, words[1], words[2]);
        };        
};
   
classSolution(class) = (r,words)
{
    auto prop := new MessageName(words[0]);
    
    (words.Length) =>
        2 {            
            console.printLine(prop(class, r, words[1]))
        }
        3 {
            var c := class;
            
            prop(class, r, words[1], words[2])
        };
};
   
public program()
{
    var proceed := nil;
    
    Solution =>
        1 {
            proceed := closureSolution((word => verbs[word]))
        }
        2 {
            proceed := closureSolution((word => nodes[word]))
        }
        3 {
            proceed := closureSolution((word => nodes2[word].compile(new RootExpressionScope())))
        }
        4 {
            proceed := classSolution(classTree)
        }
        5 {
            proceed := classSolution(classTreeFromScript)            
        };
    
    var r := new Person();
    
    console.printLine(
"Enter ""verb property value"" or press enter to stop
set FirstName Alex
get FirstName");

    until(string line := console.write(">").readLine(), line == "")
    {
        proceed(r, line.split());        
    }
}