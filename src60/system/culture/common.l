namespace culture
{
   interface ILocale
   {
      abstract string toUppercase(string s);
   }

   public extension stringCultureOp : String
   {
      string toUpper(ILocale locale)
         = locale.toUppercase(self);
   
      string toUpper()
         = currentLocale.toUppercase(self);
   }
}