// --- BaseForm ---

public abstract class BaseWinForm : BaseWinContainer
{
    protected constructor new()
        <= super new()
    {
    }
}

public closed class SDIForm : BaseWinForm
{
   constructor new()
      <= super new()
   {
   }
}

// --- SDIDialog ---

public class SDIDialog : SDIForm
{
   constructor new()
      <= super new()
   {
   }
}