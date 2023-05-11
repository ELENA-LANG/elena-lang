import extensions;

class Record
{
    // an initialization expression can be a constant
    int x := 0;

    // it can be a result of operations  
    string y := "My" + "String";

    // or class creation  
    z := new Object(); 
}

class MySpecialRecord : Record
{
    // we have to use this prefix to initialize already existing fields
    this x := 2; 
} 

public program()
{
    var p := new Record();
}