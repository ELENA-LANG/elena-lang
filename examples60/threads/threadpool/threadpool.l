import extensions;

public program()
{
   int solution := program_arguments.getAtOrDefault(1, "2").toInt();

   solution =>
      1 { sample1() }
      2 { sample2() }
}