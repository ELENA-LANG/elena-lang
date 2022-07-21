import system;

private entry
    = forward program();

public program()
{
   for (int n := 0, n < 5, n := n + 1) {
      console.write("*");
   }
}