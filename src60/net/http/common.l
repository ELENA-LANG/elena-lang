public sealed class Uri
{   
   private constructor() {}

   constructor(string url)
      = Uri.parse(url);

   constructor parse(url)
   {
   }
}