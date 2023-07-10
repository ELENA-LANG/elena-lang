import system'routines;
import system'dynamic'expressions;
import extensions;

//duplicateStructBoxing()
//{
//    console.write("duplicateStructBoxing..");
//    
//    int mode := -1;
//    var value := false;
//    value ? { mode := 0 } : { mode := 1 }  ;
//    
//    Assert.ifTrue(mode == 1);
//    
//    console.writeLine("done")
//}


public stackSafeExtensions()
{
    console.write("stackSafeExtensions..");
    
    int i := 5;
    var m := 5;
   
    int j := 2;
    var n := 2;
                         
    int r1 := i.mod(j);
    int r2 := i.mod(n);
    int r3 := m.mod(j);
    int r4 := m.mod(n);
    
    Assert.ifTrue(r1 == 1);
    Assert.ifTrue(r2 == 1);
    Assert.ifTrue(r3 == 1);
    Assert.ifTrue(r4 == 1);
    
    console.writeLine("done")
}
A;


public extension etOp
{
    testExtension(obj)
    {
        Assert.ifTrue(false)
    }
}
      
public extension etOp<T> 
{
    testExtension(A obj)
    {
        Assert.ifTrue(true)
    }
}
      
templateExtensionTest()
{
    console.write("templateExtensionTest..");
    
    auto a := new A();
    Assert.testExtension(a);
    
    console.writeLine("done")    
}  

