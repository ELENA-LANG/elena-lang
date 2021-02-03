import extensions;

Record
{
    prop Name;
}

public program()
{
    var r := new Record();
    
    auto m := new MessageName("Name");
    
    m.setPropertyValue(r,"Alex");      
}