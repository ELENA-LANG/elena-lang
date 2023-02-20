import system'collections;
import extensions;

extension op
{
   bool safeEqualX(o)
      = self.equal:o \ back:false;
}

public program()
{
   var o := new Object();

   var b := o.safeEqualX(2);

//   var countMap := Dictionary.new(0);
//   countMap[1] := 11;

//   var n := countMap[1];
}