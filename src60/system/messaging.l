
// --- Message ---
public sealed const struct Message //: BaseValue
{
   embeddable __mssg theValue[4];

   dispatch() : external<system'core_routines'__message_dispatcher>;
}