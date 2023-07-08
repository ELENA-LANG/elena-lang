import extensions;

public program()
{
    var o := new
    {
        field3 := 3;
        
        innerMost() => new
        {
            field2 := 2;
            
            innerMost() => new
            {
                field1 := 1;
                
                innerMost() = this self;
                
                getField() = field1;
                
                getParentField() = field2;
                
                getParentParentField() = field3;
            };
        };
    };
  
    var n := o.innerMost();
    Assert.ifEqual(n.getField(), 1);
    Assert.ifEqual(n.getParentField(), 2);
    Assert.ifEqual(n.getParentParentField(), 3);
}
