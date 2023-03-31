namespace io
{
   // --- IOException ---
   public class IOException : Exception
   {
      /// Creates the exception
      constructor new()
         <= new("I/O error");
            
      constructor new(string message)
         <= super new(message);
   }

}