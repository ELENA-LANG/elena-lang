#import "extensions'convertor" as convertor;

States = 
{
   state0 : 
   {
      digit : function(n)
      {
         this.newLeaf({ level : 1; evalToken: function(s) { return convertor.toReal(s); }; });

         this.seek<%state0>();
      };
   };
};
