//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		Copmpiler messages 
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

namespace elena_lang
{
   constexpr auto MessageLength = 64;
   const Pair<int, const char*, 0, nullptr> Messages[MessageLength] =
   {
      {errDuplicatedSymbol, errMsgDuplicatedSymbol},
      {errDuplicatedDictionary, errMsgDuplicatedDictionary},
      {errInvalidIntNumber, errMsgInvalidIntNumber},
      {errCannotEval, errMsgCannotEval},
      {errUnresovableLink, errMsgUnresovableLink},
      {errCannotCreate, errMsgCannotCreate},
      {errInvalidModule, errMsgInvalidModule},
      {errUnknownModule, errMsgUnknownModule},
      {errInvalidFile, errMsgInvalidFile},
      {errInvalidModuleVersion, errMsgInvalidModuleVersion},
      {errInvalidSyntax, errMsgInvalidSyntax},
      {errEmptyTarget, errMsgEmptyTarget},
      {errFatalError, errMsgFatalError},
      {errFatalLinker, errMsgFatalLinkerError},
      {errNotImplemented, errMsgNotImplemented},
      {wrnSyntaxFileNotFound, wrnMsgSyntaxFileNotFound},
      {wrnInvalidConfig, wrnMsgInvalidConfig},
      {errParserNotInitialized, errMsgParserNotInitialized},
      {errProjectAlreadyLoaded, errMsgProjectAlreadyLoaded},
      {errCommandSetAbsent, errMsgCommandSetAbsent},
      {errReadOnlyModule, errMsgReadOnlyModule},
      {errReferenceOverflow, errMsgReferenceOverflow},
      {errUnknownObject, errMsgUnknownObject},
      {wrnUnknownHint, wrnMsgUnknownHint},
      {wrnInvalidHint, wrnMsgInvalidHint},
      {errInvalidHint, errMsgInvalidHint},
      {errInvalidOperation, errMsgInvalidOperation},
      {errNotDefinedBaseClass, errMsgNotDefinedBaseClass},
      {errUnknownBaseClass, errMsgUnknownBaseClass},
      {errIllegalMethod, errMsgIllegalMethod},
      {errDuplicatedMethod, errMsgDuplicatedMethod},
      {errNoBodyMethod, errMsgNoBodyMethod},
      {errUnknownTemplate, errMsgUnknownTemplate},
      {errDuplicatedDefinition, errMsgDuplicatedDefinition},
      {errNoDispatcher, errMsgNoDispatcher},
      {errSealedParent, errMsgSealedParent},
      {errDuplicatedLocal, errMsgDuplicatedLocal},
      {errTooManyParameters, errMsgTooManyParameters},
      {errCorruptedVMT, errMsgCorruptedVMT},
      {errIllegalField, errMsgIllegalField},
      {errDuplicatedField, errMsgDuplicatedField},
      {errEmptyStructure, errMsgEmptyStructure},
      {errUnknownClass, errMsgUnknownClass},
      {wrnUnknownModule, wrnMsgUnknownModule},
      {wrnDuplicateInclude, wrnMsgDuplicateInclude},
      {errUnknownVariableType, errMsgUnknownVariableType},
      {errDupPublicMethod, errMsgDupPublicMethod},
      {errDupPrivateMethod, errMsgDupPrivateMethod},
      {errClosedMethod, errMsgClosedMethod},
      {errDupProtectedMethod, errMsgDupProtectedMethod},
      {errDupInternalMethod, errMsgDupInternalMethod},
      {infoNewMethod, infoMsgNewMethod},
      {errClosedParent, errMsgClosedParent},
      {errDispatcherInInterface, errMsgDispatcherInInterface},
      {errIllegalConstructorAbstract, errMsgIllegalConstructorAbstract},
      {errAbstractMethodCode, errMsgAbstractMethodCode},
      {errNotAbstractClass, errMsgNotAbstractClass},
      {errIllegalOperation, errMsgIllegalOperation},
      {errUnknownDefConstructor, errMsgUnknownDefConstructor},
      {wrnUnknownMessage, wrnMsgUnknownMessage},
      {wrnUnknownDefConstructor, wrnMsgUnknownDefConstructor},
      {errUnknownMessage, errMsgUnknownMessage},
      {errInvalidConstAttr, errMsgInvalidConstAttr},
      {wrnUnknownFunction, wrnMsgUnknownFunction}
   };

}
