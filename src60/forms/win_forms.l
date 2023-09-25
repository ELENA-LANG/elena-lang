// --- BaseForm ---

public abstract class BaseWinForm : BaseWinContainer
{
}

public closed class SDIForm : BaseWinForm
{
}

// --- SDIDialog ---

public class SDIDialog : SDIForm
{
   constructor new()
   {
   }
}