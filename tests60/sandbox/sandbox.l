import extensions;

//function(acc)
//    = (n => acc.append:n);

//accumulator(n)
//    = function(new Variable(n));

singleton Tester
{
   test(arg)
   {
      int n := 2;

      ^ arg
   }
}


public program()
{
   var o := self;

   real n := 2.1;
//    real m := 3.2;

   Tester.test(n);

   console.writeLine(n);
   
//   real r := n + m;
//   console.writeLine(r);
  
//    console.writeLine(n == m);            

//    Assert.ifTrue(n != m);            
//    Assert.ifTrue(n < k);            
//    Assert.ifFalse(n > m);            
//    Assert.ifFalse(n >= k);            
//    Assert.ifFalse(n >= m);            
//    Assert.ifTrue(n >= n);            
//    Assert.ifTrue(n <= n);            
//    Assert.ifTrue(n < m);            
//    Assert.ifTrue(m > n);            




//    var x := accumulator(1);
    
//    x(5);
    
//    var y := accumulator(3);
    
//    console.write(x(2.3r))
}

