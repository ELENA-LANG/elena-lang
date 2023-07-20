namespace culture
{
   interface ILocale
   {
      abstract string toUppercase(string s);

      abstract string toLowercase(string s);
   }

   public extension stringCultureOp : String
   {
      string toUpper(ILocale locale)
         = locale.toUppercase(self);
   
      string toUpper()
         = currentLocale.toUppercase(self);

      string toLower(ILocale locale)
         = locale.toLowercase(self);
   
      string toLower()
         = currentLocale.toLowercase(self);
   }
}