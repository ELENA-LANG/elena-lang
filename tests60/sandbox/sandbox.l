    class MyFirstClass
    { 
       field myFirstField;
       
       method doMyFirstOperation()
       {
          console.writeLine("My first method is invoked"); 
       }
    }

    function invokeMyClass(myObject)
    {
       myObject.doMyFirstOperation();
    }

    public program()
    {
       invokeMyClass(new MyFirstClass());  
    }


