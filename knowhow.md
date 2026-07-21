# =============================================
# ELENA Programming Language v 7.0
# =============================================

Content
========

+ [A class method invoke closure](#a-class-method-invoke-closure)

## ----------------------------------------------------------------------------
## A class method invoke closure
## ----------------------------------------------------------------------------

    import extensions;
    
    class A
    {
       Func1 onClick : event;
    
       constructor()
       {
          onClick := &onExit;
       }
    
       onExit(sender)
       {
         console.printLine("onExit was called by ", sender);
       }
    
       invoke(sender)
       {
          onClick(sender)
       }
    }
    
    public program()
    {
       var a := new A();
    
       a.invoke(this self);
    }

## ----------------------------------------------------------------------------
##  A shared code
## ----------------------------------------------------------------------------

    import extensions;
    
    textblock Shared
    {
       foo()
       {
          console.writeLine("foo")
       }
    }
    
    class A : using(Shared)
    {
       bar()
       {
          console.writeLine("bar")
       }
    }
    
    public program()
    {
       var a := new A();
    
       a.foo();
       a.bar();
    }


## ----------------------------------------------------------------------------
##  Preloaded symbols
## ----------------------------------------------------------------------------

    symbol startUp : preloaded = true.then(
    {  
       console.writeLine("Starting up")
    });

    public myFunction()
    {
       console.writeLine("Working");
    }

    public program()
    {
       myFunction()
    }

## ----------------------------------------------------------------------------
##  String interpolation
## ----------------------------------------------------------------------------

    var s := $"a_{ 1 }_b_{ 2 }_c";

## ----------------------------------------------------------------------------
##  user-defined literals
## ----------------------------------------------------------------------------

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
    }

    public program()
    {
       var n := 12o;
    } 

## ----------------------------------------------------------------------------
##  function reference
## ----------------------------------------------------------------------------

    import extensions;
    
    import extensions;
    import system'threading;
    
    myFunction()
    {
       console.writeLine("Hello from the thread");
    }
    
    public program()
    {
       auto myThread := Thread.assign(&myFunction);
    }
    
## ----------------------------------------------------------------------------
##  threads
## ----------------------------------------------------------------------------

-- Without argumnet ---

    import extensions;
    import system'threading;
    
    myFunction()
    {
       console.writeLine("Hello from the thread");
    }
    
    public program()
    {
       auto myThread := Thread.assign(&myFunction);
    
       var priority := myThread.Priority;
    
       console.printLine("Thread priority ", priority);
    
       myThread.start();
    
       console.readChar()
    }

-- With argumnet ---

    import extensions;
    import system'threading;
    
    myFunction(arg)
    {
       console.writeLine($"{arg} from the thread");
    }

    public program()
    {
       auto myThread := Thread.assign(&myFunction);
    
       var priority := myThread.Priority;
    
       console.printLine("Thread priority ", priority);
    
       myThread.start("Good bay");
    
       console.readChar()
    }

## ----------------------------------------------------------------------------
##  Environment
## ----------------------------------------------------------------------------

    import extensions;
    import system'runtime;
    
    public program()
    {
       console.writeLine($"Processor type:{Environment.ProcessorType}");
       console.writeLine($"Processor count:{Environment.ProcessorCount}");
    }

## ----------------------------------------------------------------------------
##  Accessing a variable from the upper scope
## ----------------------------------------------------------------------------

    import extensions;
    
    public program()
    {
       var variable := "Level 0";
       {
          var variable := "Level 1";
    
          console.printLine(super variable);
          console.printLine(variable);
       }
    }

## ----------------------------------------------------------------------------
##  Auto fields 
## ----------------------------------------------------------------------------

## ----------------------------------------------------------------------------
##  Private fields 
## ----------------------------------------------------------------------------

## ----------------------------------------------------------------------------
##  Module info
## ----------------------------------------------------------------------------

    import extensions;
    import extensions'runtime;
    
    public program()
    {
      var o := new MyObject();
      Console.printLine("MyObject info:", o.getFullPackageInfo());
    }

## ----------------------------------------------------------------------------
##  Async program entry
## ----------------------------------------------------------------------------

    import system'threading;
    import extensions'threading;
    
    async public program()
    {
       Task t1 := Task.run({ Console.printLineConcurrent("Enjoy") });
       Task t2 := Task.run({ Console.printLineConcurrent("Rosetta") });
       Task t3 := Task.run({ Console.printLineConcurrent("Code") });
    
       :await Task.whenAllArgs(t1, t2, t3);
    }
    
## ----------------------------------------------------------------------------
##  Evaluating a script without compilation
## ----------------------------------------------------------------------------

    import extensions;
    import extensions'scripting;
       
    public program()
    {
        Console.print("Evaluating:");
    
        var t := new ScriptEngine()
                .loadScript("[[ #grammar build ]]")
                .loadPath("~\scripts\grammar60.es")
                .loadPath("~\scripts\lscript60.es");
                
        var o := t.buildScript("import extensions; public program() { Console.printLine(""Hello "", ""World"") }");
        
        o.eval();
    }

## ----------------------------------------------------------------------------
##  Using a callback function declared in ELENA
## ----------------------------------------------------------------------------

    const int CTRL_C_EVENT = 0;
    const int CTRL_BREAK_EVENT = 1;
    
    extern int myFunction(uint fdwCtrlType)
    {
       fdwCtrlType =>
          CTRL_C_EVENT : { Console.writeLine("Ctrl-C event"); ^ -1 }
          CTRL_BREAK_EVENT : { Console.writeLine("Ctrl-Break event"); ^ 0 };
          
       ^ 0;      
    }

    public Program()
    {
       extern KERNEL32.SetConsoleCtrlHandler(&myFunction, -1);
       
       Console.writeLine("The Control Handler is installed.");
       Console.writeLine("-- Now try pressing Ctrl+C or Ctrl+Break");
       
       while(true) {}
    }

## ----------------------------------------------------------------------------
##  Enumerations
## ----------------------------------------------------------------------------

    public const struct Color : enum<int>(Red = 1,Green = 2,Blue = 3);
    
    public program()
    {
        Color color := Color.Red; 
    }
    
## ----------------------------------------------------------------------------
##  Type short-cuts
## ----------------------------------------------------------------------------

    use extern KERNEL32;
    use system'text'StringBuilder; 
    
    public program()
    {
       StringBuilder sb := new StringBuilder(); // allow to use the class proper name without importing the whole namespace
    
       pointer commandLinePtr := KERNEL32.GetCommandLineW(); // skipping extern attribute
    }

## ----------------------------------------------------------------------------
##  Providing an external library name
## ----------------------------------------------------------------------------

    use extern libcxx : importing("libc++");
    
    public program()
    {
       libcxx.do();
    }

## ----------------------------------------------------------------------------
##  Conditional compilation
## ----------------------------------------------------------------------------

    #if (__project["_Win32"])
    
    class Win32A
    {
    }
    
    #elif (__project["_Win64"])
    
    class Win64A
    {
    }
    
    #endif

## ----------------------------------------------------------------------------
##  Calling extension template directly
## ----------------------------------------------------------------------------

    extension testOp<T> : T
    {
       testMe(Func<T,T> f)
          = f(self);
    }
    
    public program()
    {
       var r := 3::testMe<int>((int x => x + 1));
       Console.writeLine("3+1=",r);
    }

## ----------------------------------------------------------------------------
##  Specify the lambda function returning value
## ----------------------------------------------------------------------------

    var f := ([real]int x => x + 1);
    o.testMe::([object]int x => x + 1);

## ----------------------------------------------------------------------------
##  Declaring a parametrized template
## ----------------------------------------------------------------------------

    class TemplateWithDefaults<T>(defValue)::
    {
       T X := defValue;
    }
    
    MyClass : TemplateWithDefaults<int>(2)
    {
    }

## ----------------------------------------------------------------------------
##  Calling property with message constant
## ----------------------------------------------------------------------------

    singleton A { X = 2; }
    
    public program()
    {
        var o := get mssg X(A);
    }

## ----------------------------------------------------------------------------
##  Declaring a record
## ----------------------------------------------------------------------------

    MyRecord : record(string FirstName,string LastName,int Age);
    
    public program()
    {
       MyRecord r := new MyRecord("Ivan", "Ivanov", 22);
       MyRecord r2 := new MyRecord("Ivan", "Ivanov", 22);
    
       Assert.ifTrue(r == r2);
    }

## ----------------------------------------------------------------------------
##  Not nil operation
## ----------------------------------------------------------------------------

    A
    {
       testMe() {}
    }
    
    public program()
    {
       A? a := nil;
       if:nil(a)
          a := new A();
    
       a!.testMe(); 
    }

## ----------------------------------------------------------------------------
##  Platform attribute
## ----------------------------------------------------------------------------

In the project file:

        <module name="winforms" platform="windows">
            <include>winforms\win_common.l</include>
            <include>winforms\win32_exceptions.l</include>
            <include>winforms\win_handles.l</include>
            <include>winforms\win_controls.l</include>
            <include>winforms\win_windows.l</include>
            <include>winforms\win_dialogs.l</include>
            <include>winforms\win_app.l</include>
        </module>

In the project collection file:

    <project platform="windows">forms\forms.prj</project>

## ----------------------------------------------------------------------------
##    __nonboxable attribute
## ----------------------------------------------------------------------------

Raise an error if the expression is not memory allocated

e.g.

    int n := 0;
    var o := __nonboxable n; // !! must raise an error : "Heap allocated object is expected"

## ----------------------------------------------------------------------------
##    lambda function without arguments
## ----------------------------------------------------------------------------

    public program()
    {
       var f1 := { Console.writeLine("The traditional lambda function without arguments"); };
       var f2 := (){ Console.writeLine("The new lambda function without arguments to be in line with syntax like c(x){ ... }"); };
       var f3 := ([] => "Hello");
    
       f1();
       f2();
    }

## ----------------------------------------------------------------------------
##    short-cut syntax for array
## ----------------------------------------------------------------------------

    public program()
    {
      string[] dirNames := new []{ ".", ".." };
    }
	
## ----------------------------------------------------------------------------
##    syntax for a constant array
## ----------------------------------------------------------------------------

    public program()
    {
      const string[] dirNames := new const string []{ ".", ".." };
    }
	
## ----------------------------------------------------------------------------
##    readonly fields
## ----------------------------------------------------------------------------

    class B;
    
    class A
    {
       readonly B b;
    
       constructor new(B b)
       {
          this b := b
       }
    
       testReadOnly(B arg)
       {
          b := arg // !! raises an error 116 : Read-only field cannot be changed
       }   
    }

    public program()
    {
       A a := A.new(new B());
       a.testReadOnly(new B());
    }
	
## ----------------------------------------------------------------------------
##    without template reusing
## ----------------------------------------------------------------------------

It is possible to force the compiler to generate the templates without
reusing ones declared in the previous modules. The special module hint 
must be provided

     <module>
        <include>common.l</include>
        <include>mbedtlssocket.l</include>
        <include>mbedtlslistener.l</include>
     </module>
     <module name="client" hints="8">                                <!-- all required templates will be generated again, so no extra link to the previous module -->
        <include>client\registration\registration.l</include>
     </module>
     <module name="server" hints="8">                                <!-- all required templates will be generated again, so no extra link to the previous module -->
       <include>server\registration\registration.l</include>
     </module>

## ------------------------------ 
##   clearing lexical information
## ------------------------------

    <lexicals>
      <clear/>
    </lexicals>

## ------------------------------ 
##   Checks if the object reacts to the strong typed message
## ------------------------------

    import extensions;
    import system'dynamic;
       
    public interface IFunction
    {
       abstract real calculate(real arg);
    }
       
    public class LinearFunction : IFunction
    {
       real a;
       real b;
   
       constructor(real a, real b)
       {
          this a := a;
          this b := b;
       }
   
       real calculate(real x)
          = x*a + b;
    }
       
    public Program()
    {
       IFunction f := new LinearFunction(2.3, 3.3);
    
       auto mssg := new StrongMessage("calculate<system'RealNumber>[2]");
    
       var found := f.__getClass().respondTo(mssg);
    
       Console.printLine(f, found ? " responds to " : " does not respond to ", mssg);
    }

## ----------------------------------------------------------------------------
##  Read a binary file page-by-page and print its content
## ----------------------------------------------------------------------------

    import system'io;
    import extensions;
    
    public Program()
    {
       byte buffer[512];
       
       using(auto reader := new BinaryStreamReader(FileStream.openForRead("sandbox.l"))) {
          int len := reader.read(buffer, 512);
          
          Console.printLine(buffer.asEnumerable());
       };
    }

## ----------------------------------------------------------------------------
##  Checking a method result type
## ----------------------------------------------------------------------------

    import extensions;
    import system'dynamic;

    public interface IFunction
    {
       abstract real calculate(real arg);
    }
    
    public class LinearFunction : IFunction
    {
       real a;
       real b;
    
       constructor(real a, real b)
       {
          this a := a;
          this b := b;
       }
    
       real calculate(real x)
          = x*a + b;
    }

    public Program()
    {
       IFunction f := new LinearFunction(2.3, 3.3);
    
       auto mssg := new StrongMessage("calculate<system'RealNumber>[2]");
    
       var found := f.__getClass().respondTo(mssg);
    
       Console.printLine(f, found ? " responds to " : " does not respond to ", mssg);   
    
       if (found)
          Console.printLine(mssg, " returns an instance of ", f.__getClass().__getMethodOutput(mssg).getTypeUnsafe());
    }

## ----------------------------------------------------------------------------
##  Converting an object to a type obtained in run-time
## ----------------------------------------------------------------------------
    
    A
    {
       string _origin;
   
       constructor()
       {
          _origin := "created directly"
       }
       
       constructor(B b)
       {
          _origin := "created from B"
       }
       
       string toPrintable()
          = $"A - {_origin}";
    }
    
    B
    {
       A cast() = new A(self);
    }
    
    // convert object to a type of type_instance in run-time
    dynamic_typecast(type_instance,object)
    {
       var type := type_instance.__getClass();
    
       ^ type.__typecast(object);
    }
    
    public Program()
    {
       var o1 := new A(); // the target type
       var o2 := new B(); // the source object
    
       Console.printLine("An instance of ", o1.__getClass(), " ", dynamic_typecast(o1, o2))       
    }

## ----------------------------------------------------------------------------
##  Checking the method output type in run-time
## ----------------------------------------------------------------------------
    
To be able to check the method output type, the project must be compiled with "-xo" option:

    elena-cli -xo example.l

NOTE : The output type can be retrieve only for public methods / public classes

The code is:

    import extensions;
    import system'dynamic;
    
    public class A
    {
       real getValue()
         = 2.0;
    }
    
    public class B
    {
       int getValue()
         = 2;
    }
    
    public class C
    {
       getValue()
         = "Any";
    }
    
    extension op
    {
       checkOutput(string messageName)
       {
          auto mssg := new Message(messageName);
    
          var outputType := self.__getClass().__getMethodOutput(mssg)?.getTypeUnsafe();
          if:not:nil(outputType) {
             Console.printLine("The output type of ",self,".",messageName," is ", outputType);
          }
          else Console.printLine(self,".",messageName," has no declared output type");
       }
    }
    
    public Program()
    {
       var a := new A();
       var b := new B();
       var c := new C();
    
       a.checkOutput("getValue[1]");
       b.checkOutput("getValue[1]");
       c.checkOutput("getValue[1]");
    }

## ----------------------------------------------------------------------------
##  Mocking an interface
## ----------------------------------------------------------------------------
    
To be able to check the method output type, the project must be compiled with "-xo" option:

    elena-cli -xo example.l

The code is:

    import extensions;
    import system'dynamic;
    
    public interface IFunction
    {
       abstract real calculate(real arg);
    }
    
    public class Mockup
    {
       field object;
    
       constructor(object)
       {
          this object := object;
       }
    
       generic cast()
       {
          var type := __received.__getFirstSignatureMember();
    
          var proxy := object.mockInferface(type);
       
          ^ __received(proxy);
       }
    }
    
    public singleton Actor
    {
       invoke(IFunction function)
       {
          Console.printLine("Calculating f(2) = ", function.calculate(2));
       }
    }
    
    public Program()
    {
       var mockup := new Mockup(::{ real calculate(real x) = x * x; });
    
       Actor.invoke(mockup);
    }

## ----------------------------------------------------------------------------
##  Primitive Value operation
## ----------------------------------------------------------------------------
    
Value operator "\*" can be replaced with direct access to the corresponding field.

NOTE : the getter method budy must be an expression (not a method body) and contains only the field. It must be sealed as well

    class IntWrapper 
    {
       int _value;
    
       constructor(int v) { _value := v }
    
       sealed int Value // NOTE : the method or a class must be sealed
          = _value;
    }

So in the following code the Value method call will be replaced with returning a field directly:

    public Program()
    {
       auto o := new IntWrapper(3);
    
       int n := *o; // o.Value call is replaced with a direct reference to _value field
      
       Console.printLine("o.Value=", n)
    }


The generated byte code will be:

    >@function Program.function:#invoke
           xflush       sp:0
           open           :7, :4
           store        fp:1
           xstore       sp:0, intconst:3
           set       class:sandbox'$private'IntWrapper
           mov        mssg:function:#constructor<system'IntNumber>[1]
           call       mssg:function:#constructor<system'IntNumber>[1], class:sandbox'$private'IntWrapper#class
           store        fp:2
        // ; getting a field directly
           set          dp:-4
           store        sp:0
           peek         fp:2
           get           i:0
           xwrite     offs:0, :4
        // ; <...>
    @end

## ----------------------------------------------------------------------------
##  Safe typecasting operation
## ----------------------------------------------------------------------------
    
Safe typecasting operation can be done using "if is" operation:

    import extensions;
    
    public Program()
    {
       var r := Range.for(0, 10);
       if(r.enumerator(); is Enumerator en) {
          Console.printLine("The typecasting was successful - ", en);
       };    
    }

The statement tries to typecast the first argument to the second one *Enumerator*. If the typecasting was successful the code in 
the code brackets is executed and a variable *en* contains the converted object.

## ----------------------------------------------------------------------------
##  Nested named classes
## ----------------------------------------------------------------------------

It is now possible to declared a nested private class inside another class.
The nested class is available only inside the owner one

    singleton A
    {
       class B
       {
          testMe()
          {
             Console.writeLine("A::B testMe")
          }
       }
    
       testB()
       {
          B b := new B();
    
          b.testMe();
       }
    }
    
    public Program()
    {
       A.testB()
    }
