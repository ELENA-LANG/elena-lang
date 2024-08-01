import extensions;

stringInterpolation()
{
   var s := $"a_{ 1 }_b_{ 2 }_c";

   Assert.ifEqual(s,"a_1_b_2_c");
}

public program()
{
   stringInterpolation()
}
