//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Script Engine
//
//                                              (C)2023, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef SCRIPTMACHINE_H
#define SCRIPTMACHINE_H

namespace elena_lang
{
   // --- ScriptEngine ---
   class ScriptEngine
   {
      int                _lastId;

   public:
      int newScope();

      ScriptEngine();
      ~ScriptEngine() = default;
   };

}

#endif

