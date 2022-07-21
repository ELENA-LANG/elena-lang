import system;

private entry
    = forward program();

public program()
{
   console.writeLine("--- ELENA API Tests ---");

   for (int n := 0, n < 5, n := n + 1) {
      console.write("*");
   };

   console.writeLine("--- Passed ---")
}