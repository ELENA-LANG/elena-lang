//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the language common constants
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef LANGCOMMON_H
#define LANGCOMMON_H

namespace elena_lang
{
   // default settings
   constexpr bool DEFAULT_CONDITIONAL_BOXING = true;
   constexpr bool DEFAULT_EVALUATE_OP = true;

   enum MetaHint : int
   {
      mhNone           = 0,
      mhStandart       = 1,
      mhNoValidation   = 2,
   };

   enum class MethodHint : ref_t
   {
      Mask                 = 0x0000000F,

      None                 = 0x00000000,
      Normal               = 0x00000001,
      Sealed               = 0x00000003,
      Fixed                = 0x00000005,
      Dispatcher           = 0x00000007,

      Async                = 0x00000010,
      Interpolator         = 0x00000020,
      Nillable             = 0x00000040,
      Function             = 0x00000080,
      Generic              = 0x00000100,
      RetOverload          = 0x00000200,
      Constructor          = 0x00000400,
      Conversion           = 0x00000800,
      Multimethod          = 0x00001000,
      TargetSelf           = 0x00002000,
      Static               = 0x00004000,
      GetAccessor          = 0x00008000,
      Mixin                = 0x00010000,
      Abstract             = 0x00020000,
      Predefined           = 0x00040000, // virtual class declaration
      Stacksafe            = 0x00080000,
      SetAccessor          = 0x00100000,
      Indexed              = 0x00200000,
      InterfaceDispatcher  = 0x00400000,
      Constant             = 0x00800000,
      VirtualReturn        = 0x01000000,  // used for MutliRet with non-embeddable return type
      Extension            = 0x02000000,
      Initializer          = 0x04000000,
      Autogenerated        = 0x08000000,
      Yieldable            = 0x10000000,

      Protected            = 0x40000000,
      Internal             = 0x80000000,
      Private              = 0xC0000000,

      VisibilityMask       = 0xC0000000,
   };

   // --- MethodInfo ---
   struct MethodInfo
   {
      bool   inherited;
      ref_t  hints;
      ref_t  outputRef;
      mssg_t multiMethod;
      mssg_t byRefHandler;
      int    nillableArgs;

      MethodInfo()
      {
         inherited = false;
         hints = 0;
         outputRef = 0;
         multiMethod = 0;
         byRefHandler = 0;
         nillableArgs = 0;
      }
      MethodInfo(bool inherited, ref_t hints, ref_t outputRef, mssg_t multiMethod, mssg_t byRefHandler, int nillableArgs) :
         inherited(inherited),
         hints(hints),
         outputRef(outputRef),
         multiMethod(multiMethod),
         byRefHandler(byRefHandler),
         nillableArgs(nillableArgs)
      {
      }

      static bool checkVisibility(MethodInfo& info, MethodHint hint)
      {
         return (info.hints & (ref_t)MethodHint::VisibilityMask) == (ref_t)hint;
      }

      static bool checkVisibility(MethodInfo& info, MethodHint hint1, MethodHint hint2)
      {
         ref_t mask = info.hints & (ref_t)MethodHint::VisibilityMask;

         return mask == (ref_t)hint1 || mask == (ref_t)hint2;
      }
   };

   // --- SymbolInfo ---
   enum class SymbolType : int
   {
      Symbol = 0,
      Singleton,
      Constant,
      ConstantArray,
   };

   struct SymbolInfo
   {
      SymbolType symbolType;
      ref_t      valueRef;
      ref_t      typeRef;
      bool       loadableInRuntime;

      SymbolInfo()
      {
         symbolType = SymbolType::Symbol;
         valueRef = typeRef = 0;
         loadableInRuntime = false;
      }
      SymbolInfo(SymbolType symbolType, ref_t valueRef, ref_t typeRef, bool loadableInRuntime)
      {
         this->symbolType = symbolType;
         this->valueRef = valueRef;
         this->typeRef = typeRef;
         this->loadableInRuntime = loadableInRuntime;
      }

      void load(StreamReader* reader)
      {
         symbolType = (SymbolType)reader->getDWord();
         valueRef = reader->getRef();
         typeRef = reader->getRef();
         loadableInRuntime = reader->getBool();
      }

      void save(StreamWriter* writer)
      {
         writer->writeDWord((unsigned int)symbolType);
         writer->writeRef(valueRef);
         writer->writeRef(typeRef);
         writer->writeBool(loadableInRuntime);
      }
   };

   enum class FieldHint : ref_t
   {
      None     = 0x00000000,
      ReadOnly = 0x00000001,
      Private  = 0x00000002,
   };

   // --- FieldInfo ---
   struct FieldInfo
   {
      int      offset;
      TypeInfo typeInfo;
      ref_t    hints;

      static bool checkHint(FieldInfo& info, FieldHint hint)
      {
         return test(info.hints, (ref_t)hint);
      }

      FieldInfo()
         : offset(0), typeInfo({}), hints(0)
      {

      }
      FieldInfo(int offset, TypeInfo typeInfo)
         : offset(offset), typeInfo(typeInfo), hints(0)
      {
      }
      FieldInfo(int offset, TypeInfo typeInfo, bool readOnly, bool privateOne)
         : offset(offset), typeInfo(typeInfo), hints(0)
      {
         hints |= readOnly ? (ref_t)FieldHint::ReadOnly : 0;
         hints |= privateOne ? (ref_t)FieldHint::Private : 0;
      }
   };

   // --- ClassInfo ---
   struct ClassInfo
   {
      typedef MemoryMap<mssg_t, MethodInfo, Map_StoreUInt, Map_GetUInt> MethodMap;
      typedef MemoryMap<ustr_t, FieldInfo, Map_StoreUStr, Map_GetUStr> FieldMap;
      typedef MemoryMap<ustr_t, StaticFieldInfo, Map_StoreUStr, Map_GetUStr> StaticFieldMap;

      pos_t           inheritLevel;
      ClassHeader     header;
      pos_t           size;           // Object size
      MethodMap       methods;
      FieldMap        fields;
      StaticFieldMap  statics;
      ClassAttributes attributes;

      static void loadStaticFields(StreamReader* reader, StaticFieldMap& statics)
      {
         pos_t statCount = reader->getPos();
         for (pos_t i = 0; i < statCount; i++) {
            IdentifierString fieldName;
            reader->readString(fieldName);
            StaticFieldInfo fieldInfo;
            reader->read(&fieldInfo, sizeof(fieldInfo));

            statics.add(*fieldName, fieldInfo);
         }
      }

      static void saveStaticFields(StreamWriter* writer, StaticFieldMap& statics)
      {
         writer->writePos(statics.count());
         statics.forEach<StreamWriter*>(writer, [](StreamWriter* writer, ustr_t name, StaticFieldInfo info)
            {
               writer->writeString(name);
               writer->write(&info, sizeof(info));
            });
      }

      void save(StreamWriter* writer, bool headerAndSizeOnly = false)
      {
         writer->write(&header, sizeof(ClassHeader));
         writer->writeDWord(size);
         if (!headerAndSizeOnly) {
            writer->writeDWord(inheritLevel);
            writer->writePos(fields.count());
            fields.forEach<StreamWriter*>(writer, [](StreamWriter* writer, ustr_t name, FieldInfo info)
               {
                  writer->writeString(name);
                  writer->write(&info, sizeof(info));
               });

            writer->writePos(methods.count());
            methods.forEach<StreamWriter*>(writer, [](StreamWriter* writer, mssg_t message, MethodInfo info)
               {
                  writer->writeDWord(message);
                  writer->write(&info, sizeof(info));
               });

            writer->writePos(attributes.count());
            attributes.forEach<StreamWriter*>(writer, [](StreamWriter* writer, ClassAttributeKey key, ref_t reference)
               {
                  writer->write(&key, sizeof(key));
                  writer->writeRef(reference);
               });

            saveStaticFields(writer, statics);
         }
      }

      void load(StreamReader* reader, bool headerAndSizeOnly = false, bool fieldsOnly = false)
      {
         reader->read(&header, sizeof(ClassHeader));
         size = reader->getDWord();
         if (!headerAndSizeOnly) {
            inheritLevel = reader->getDWord();
            pos_t fieldCount = reader->getPos();
            for (pos_t i = 0; i < fieldCount; i++) {
               IdentifierString fieldName;
               reader->readString(fieldName);
               FieldInfo fieldInfo = {};
               reader->read(&fieldInfo, sizeof(fieldInfo));

               fields.add(*fieldName, fieldInfo);
            }

            if (!fieldsOnly) {
               pos_t methodsCount = reader->getPos();
               for (pos_t i = 0; i < methodsCount; i++) {
                  mssg_t message = reader->getDWord();
                  MethodInfo methodInfo;
                  reader->read(&methodInfo, sizeof(MethodInfo));

                  methods.add(message, methodInfo);
               }
               pos_t attrCount = reader->getPos();
               for (pos_t i = 0; i < attrCount; i++) {
                  ClassAttributeKey key;
                  reader->read(&key, sizeof(key));

                  ref_t reference = reader->getRef();

                  attributes.add(key, reference);
               }

               loadStaticFields(reader, statics);
            }
         }
      }

      ClassInfo() :
         inheritLevel(0),
         header({}),
         size(0),
         methods({}),
         fields({ -1, {} }),
         statics({ -1 }),
         attributes(0)
      {
      }
   };

   // === ELENA Error codes ===
   constexpr auto errInvalidSyntax           = 4;
   constexpr auto errCBrExpectedSyntax       = 9;

   constexpr auto errDuplicatedSymbol        = 102;
   constexpr auto errDuplicatedMethod        = 103;
   constexpr auto errUnknownClass            = 104;
   constexpr auto errDuplicatedLocal         = 105;
   constexpr auto errUnknownObject           = 106;
   constexpr auto errInvalidOperation        = 107;
   constexpr auto errDuplicatedDictionary    = 108;
   constexpr auto errDuplicatedField         = 109;
   constexpr auto errUnknownVariableType     = 110;
   constexpr auto errIllegalField            = 111;
   constexpr auto errNoInitializer           = 112;
   constexpr auto errTooManyParameters       = 113;
   constexpr auto errMixedUpVariadicMessage  = 114;
   constexpr auto errRedirectToItself        = 115;
   constexpr auto errAssigningRealOnly       = 116;
   constexpr auto errDuplicatedDefinition    = 119;
   constexpr auto errInvalidIntNumber        = 130;
   constexpr auto errCannotEval              = 140;
   constexpr auto errSealedParent            = 141;
   constexpr auto errClosedParent            = 142;
   constexpr auto errInvalidHint             = 147;
   constexpr auto errIllegalConstructor      = 149;
   constexpr auto errClosedMethod            = 150;
   constexpr auto errIllegalStaticMethod     = 151;
   constexpr auto errIllegalMethod           = 152;
   constexpr auto errIllegalOperation        = 153;
   constexpr auto errNotCompatibleMulti      = 157;
   constexpr auto errTypeAlreadyDeclared     = 158;
   constexpr auto errAbstractMethods         = 159;
   constexpr auto errDispatcherInInterface   = 160;
   constexpr auto errAbstractMethodCode      = 161;
   constexpr auto errNotAbstractClass        = 164;
   constexpr auto errIllegalPrivate          = 166;
   constexpr auto errDupPublicMethod         = 167;
   constexpr auto errEmptyStructure          = 169;
   constexpr auto errInvalidType             = 172;
   constexpr auto errDupInternalMethod       = 173;
   constexpr auto errInvalidConstAttr        = 174;
   constexpr auto errIllegalConstructorAbstract = 177;
   constexpr auto errNoBodyMethod            = 180;
   constexpr auto errUnknownTemplate         = 181;
   constexpr auto errDupPrivateMethod        = 182;
   constexpr auto errDupProtectedMethod      = 183;
   constexpr auto errUnknownDefConstructor   = 184;
   constexpr auto errUnknownMessage          = 185;
   constexpr auto errAssigningToSelf         = 186;

   constexpr auto errUnknownModule           = 201;
   constexpr auto errUnresovableLink         = 202;
   constexpr auto errInvalidModule           = 203;
   constexpr auto errCannotCreate            = 204;
   constexpr auto errInvalidFile             = 205;
   constexpr auto errInvalidParserTarget     = 206;
   constexpr auto errInvalidParserTargetType = 207;
   constexpr auto errTLSIsNotAllowed         = 208;
   constexpr auto errInvalidModuleVersion    = 210;
   constexpr auto errEmptyTarget             = 212;

   constexpr auto errParserNotInitialized    = 300;
   constexpr auto errProjectAlreadyLoaded    = 301;

   constexpr auto wrnUnknownHint             = 404;
   constexpr auto wrnInvalidHint             = 406;
   constexpr auto wrnUnknownMessage          = 407;
   constexpr auto wrnUnknownFunction         = 408;
   constexpr auto wrnUnknownDefConstructor   = 409;
   constexpr auto wrnCallingItself           = 410;
   constexpr auto wrnAssigningNillable       = 411;
   constexpr auto wrnReturningNillable       = 412;
   constexpr auto wrnUnknownModule           = 413;
   constexpr auto wrnTypeInherited           = 420;
   constexpr auto wrnDuplicateInclude        = 425;
   constexpr auto wrnUnknownTypecast         = 426;
   constexpr auto wrnUnsupportedOperator     = 427;
   constexpr auto wrnUnassignedVariable      = 428;
   constexpr auto wrnLessAccessible          = 429;

   constexpr auto wrnSyntaxFileNotFound      = 500;
   constexpr auto wrnInvalidConfig           = 501;
   constexpr auto wrnInvalidPrjCollection    = 502;

   constexpr auto errCommandSetAbsent        = 600;
   constexpr auto errReadOnlyModule          = 601;
   constexpr auto errNotDefinedBaseClass     = 602;
   constexpr auto errReferenceOverflow       = 603;
   constexpr auto errUnknownBaseClass        = 604;
   constexpr auto errNoDispatcher            = 605;
   constexpr auto errClosureError            = 606;

   constexpr auto infoNewMethod              = 701;
   constexpr auto infoCurrentMethod          = 702;
   constexpr auto infoCurrentClass           = 703;
   constexpr auto infoAbstractMetod          = 704;
   constexpr auto infoMixedUpVariadic        = 705;
   constexpr auto infoUnknownMessage         = 706;
   constexpr auto infoTargetClass            = 707;
   constexpr auto infoScopeMethod            = 708;
   constexpr auto infoExptectedType          = 709;

   constexpr auto errVMBroken                = 800;
   constexpr auto errVMNotInitialized        = 801;
   constexpr auto errVMNotExecuted           = 802;
   constexpr auto errVMReferenceNotFound     = 803;

   constexpr auto errFatalError       = -1;
   constexpr auto errFatalLinker      = -2;
   constexpr auto errCorruptedVMT     = -4;

   // --- Project warning levels
   constexpr int WARNING_LEVEL_1          = 1;
   constexpr int WARNING_LEVEL_2          = 2;
   constexpr int WARNING_LEVEL_3          = 4;

   constexpr int WARNING_MASK_0           = 0;
   constexpr int WARNING_MASK_1           = 1;
   constexpr int WARNING_MASK_2           = 3;
   constexpr int WARNING_MASK_3           = 7;

   // === Attributes / Predefined Types ===

   /// scope_accessors? modificator? visibility? scope_prefix? scope? type?
   constexpr auto V_CATEGORY_MASK         = 0x7FFFFF00u;
   constexpr auto V_CATEGORY_MAX          = 0x0000F000u;

   /// modificator
   constexpr auto V_IGNOREDUPLICATE       = 0x80006001u;
   constexpr auto V_SCRIPTSELFMODE        = 0x80006002u;

   /// accessors:
   constexpr auto V_GETACCESSOR           = 0x80005001u;
   constexpr auto V_SETACCESSOR           = 0x80005002u;
   constexpr auto V_YIELDABLE             = 0x80005003u;
   constexpr auto V_ASYNC                 = 0x80005004u;

   /// visibility:
   constexpr auto V_PUBLIC                = 0x80004001u;
   constexpr auto V_PRIVATE               = 0x80004002u;
   constexpr auto V_INTERNAL              = 0x80004003u;
   constexpr auto V_PROTECTED             = 0x80004004u;

   /// property:
   constexpr auto V_SEALED                = 0x80003001u;
   constexpr auto V_ABSTRACT              = 0x80003002u;
   constexpr auto V_CLOSED                = 0x80003003u;
   constexpr auto V_PREDEFINED            = 0x80003005u;
   constexpr auto V_OVERRIDE              = 0x80003006u;

   /// scope_prefix:
   constexpr auto V_CONST                 = 0x80002001u;
   constexpr auto V_EMBEDDABLE            = 0x80002002u;
   constexpr auto V_WRAPPER               = 0x80002003u;
   constexpr auto V_READONLY              = 0x80002004u;
   constexpr auto V_OUTWRAPPER            = 0x80002005u;
   constexpr auto V_OVERLOADRET           = 0x8000200Au;
   constexpr auto V_VARIADIC              = 0x8000200Bu;

   /// scope:
   constexpr auto V_CLASS                 = 0x80001001u;
   constexpr auto V_STRUCT                = 0x80001002u;
   constexpr auto V_SYMBOLEXPR            = 0x80001003u;
   constexpr auto V_CONSTRUCTOR           = 0x80001004u;
   constexpr auto V_EXTENSION             = 0x80001005u;
   constexpr auto V_SINGLETON             = 0x80001006u;
   constexpr auto V_INTERFACE             = 0x80001007u;
   constexpr auto V_METHOD                = 0x80001008u;
   constexpr auto V_FIELD                 = 0x80001009u;
   constexpr auto V_NONESTRUCT            = 0x8000100Au;
   constexpr auto V_GENERIC               = 0x8000100Bu;
   constexpr auto V_FUNCTION              = 0x8000100Cu;     // a closure attribute
   constexpr auto V_VARIABLE              = 0x8000100Du;
   constexpr auto V_MEMBER                = 0x8000100Eu;
   constexpr auto V_STATIC                = 0x8000100Fu;
   constexpr auto V_ENUMERATION           = 0x80001010u;
   constexpr auto V_CONVERSION            = 0x80001011u;
   constexpr auto V_NEWOP                 = 0x80001012u;
   constexpr auto V_DISPATCHER            = 0x80001013u;
   constexpr auto V_TEXTBLOCK             = 0x80001014u;
   constexpr auto V_EXTERN                = 0x80001015u;
   constexpr auto V_INTERN                = 0x80001016u;
   constexpr auto V_FORWARD               = 0x80001017u;
   constexpr auto V_IMPORT                = 0x80001018u;
   constexpr auto V_MIXIN                 = 0x80001019u;
   constexpr auto V_DISTRIBUTED_FORWARD   = 0x8000101Au;
   constexpr auto V_AUTO                  = 0x8000101Cu;
   constexpr auto V_NAMESPACE             = 0x80001021u;
   constexpr auto V_SUPERIOR              = 0x80001024u;
   constexpr auto V_INLINE                = 0x80001025u;
   constexpr auto V_PROBEMODE             = 0x80001026u;
   constexpr auto V_TEMPLATE              = 0x80001027u;
   constexpr auto V_TEMPLATEBASED         = 0x80001028u;
   constexpr auto V_WEAK                  = 0x80001029u;
   constexpr auto V_INTERFACE_DISPATCHER  = 0x8000102Au;
   constexpr auto V_PACKED_STRUCT         = 0x8000102Cu;
   constexpr auto V_THREADVAR             = 0x8000102Du;

   /// primitive type attribute
   constexpr auto V_STRINGOBJ             = 0x80000801u;
   constexpr auto V_FLOATBINARY           = 0x80000802u;
   constexpr auto V_INTBINARY             = 0x80000803u;
   constexpr auto V_UINTBINARY            = 0x80000804u;
   constexpr auto V_WORDBINARY            = 0x80000805u;
   constexpr auto V_MSSGNAME              = 0x80000806u;
   constexpr auto V_SUBJBINARY            = 0x80000807u;
   constexpr auto V_SYMBOL                = 0x80000808u;
   constexpr auto V_MSSGBINARY            = 0x80000809u;
   constexpr auto V_POINTER               = 0x8000080Au;
   constexpr auto V_EXTMESSAGE            = 0x8000080Bu;
   constexpr auto V_TYPEOF                = 0x8000080Cu;
   //constexpr auto V_INLINEARG             = 0x8000080Du;

   /// primitive types
   constexpr auto V_STRING                = 0x80000001u;
   constexpr auto V_INT32                 = 0x80000002u;
   constexpr auto V_DICTIONARY            = 0x80000003u;
   constexpr auto V_NIL                   = 0x80000004u;
   constexpr auto V_OBJARRAY              = 0x80000005u;
   constexpr auto V_OBJECT                = 0x80000006u;
   constexpr auto V_FLAG                  = 0x80000007u;
   constexpr auto V_WORD32                = 0x80000008u;
   constexpr auto V_INT8                  = 0x80000009u;
   constexpr auto V_INT8ARRAY             = 0x8000000Au;
   constexpr auto V_BINARYARRAY           = 0x8000000Bu;
   constexpr auto V_ELEMENT               = 0x8000000Cu;
   constexpr auto V_MESSAGE               = 0x8000000Du;
   constexpr auto V_MESSAGENAME           = 0x8000000Eu;
   constexpr auto V_INT16                 = 0x8000000Fu;
   constexpr auto V_INT16ARRAY            = 0x80000010u;
   constexpr auto V_WIDESTRING            = 0x80000011u;
   constexpr auto V_OBJATTRIBUTES         = 0x80000012u;
   constexpr auto V_CLOSURE               = 0x80000013u;
   constexpr auto V_DECLARATION           = 0x80000014u;
   constexpr auto V_DECLATTRIBUTES        = 0x80000015u;
   constexpr auto V_ARGARRAY              = 0x80000016u;
   constexpr auto V_INT64                 = 0x80000017u;
   constexpr auto V_FLOAT64               = 0x80000018u;
   constexpr auto V_INT32ARRAY            = 0x80000019u;
   constexpr auto V_PTR32                 = 0x8000001Au;
   constexpr auto V_PTR64                 = 0x8000001Bu;
   constexpr auto V_EXTMESSAGE64          = 0x8000001Cu;
   constexpr auto V_EXTMESSAGE128         = 0x8000001Du;
   constexpr auto V_WORD64                = 0x8000001Eu;
   constexpr auto V_UINT32                = 0x8000001Fu;
   constexpr auto V_DEFAULT               = 0x80000020u;
   constexpr auto V_UINT8                 = 0x80000021u;
   constexpr auto V_UINT16                = 0x80000022u;
   constexpr auto V_NILLABLE              = 0x80000023u;
   constexpr auto V_FLOAT64ARRAY          = 0x80000024u;
   constexpr auto V_GETTER                = 0x80000025u;

   /// built-in variables
   constexpr auto V_SELF_VAR              = 0x80000081u;
   constexpr auto V_DECL_VAR              = 0x80000082u;
   constexpr auto V_SUPER_VAR             = 0x80000083u;
   constexpr auto V_RECEIVED_VAR          = 0x80000084u;

   // === Operators ===
   constexpr auto OPERATOR_MAKS              = 0x1840;
   constexpr auto INDEX_OPERATOR_ID          = 0x0001;
   constexpr auto SET_OPERATOR_ID            = 0x0002;
   constexpr auto ADD_ASSIGN_OPERATOR_ID     = 0x0003;
   constexpr auto ADD_OPERATOR_ID            = 0x0004;
   constexpr auto SUB_OPERATOR_ID            = 0x0005;
   constexpr auto LEN_OPERATOR_ID            = 0x0006;
   constexpr auto IF_OPERATOR_ID             = 0x0007;
   constexpr auto LESS_OPERATOR_ID           = 0x0008;
   constexpr auto NAME_OPERATOR_ID           = 0x0009;
   constexpr auto EQUAL_OPERATOR_ID          = 0x000A;
   constexpr auto NOT_OPERATOR_ID            = 0x000B;
   constexpr auto NOTEQUAL_OPERATOR_ID       = 0x000C;
   constexpr auto ELSE_OPERATOR_ID           = 0x000E;
   constexpr auto IF_ELSE_OPERATOR_ID        = 0x000F;
   constexpr auto MUL_OPERATOR_ID            = 0x0010;
   constexpr auto DIV_OPERATOR_ID            = 0x0011;
   constexpr auto NOTLESS_OPERATOR_ID        = 0x0012;
   constexpr auto GREATER_OPERATOR_ID        = 0x0013;
   constexpr auto NOTGREATER_OPERATOR_ID     = 0x0014;
   constexpr auto NEGATE_OPERATOR_ID         = 0x0016;
   constexpr auto VALUE_OPERATOR_ID          = 0x0017;
   constexpr auto BAND_OPERATOR_ID           = 0x0018;
   constexpr auto BOR_OPERATOR_ID            = 0x0019;
   constexpr auto BXOR_OPERATOR_ID           = 0x001A;
   constexpr auto BNOT_OPERATOR_ID           = 0x001B;
   constexpr auto SHL_OPERATOR_ID            = 0x001C;
   constexpr auto SHR_OPERATOR_ID            = 0x001D;
   constexpr auto SUB_ASSIGN_OPERATOR_ID     = 0x001E;
   constexpr auto MUL_ASSIGN_OPERATOR_ID     = 0x001F;
   constexpr auto DIV_ASSIGN_OPERATOR_ID     = 0x0020;
   constexpr auto AND_OPERATOR_ID            = 0x0021;
   constexpr auto OR_OPERATOR_ID             = 0x0022;
   constexpr auto XOR_OPERATOR_ID            = 0x0023;
   constexpr auto BREAK_OPERATOR_ID          = 0x0024;
   constexpr auto CONTINUE_OPERATOR_ID       = 0x0027;
   constexpr auto YIELD_OPERATOR_ID          = 0x0028;
   constexpr auto REFERENCE_OPERATOR_ID      = 0x002B;
   constexpr auto INC_OPERATOR_ID            = 0x002C;
   constexpr auto DEC_OPERATOR_ID            = 0x002D;

   constexpr int MAX_OPERATOR_ID             = 0x002D;

   constexpr auto ISNIL_OPERATOR_ID          = 0x003E;
   constexpr auto CLASS_OPERATOR_ID          = 0x003F;
   constexpr auto SET_INDEXER_OPERATOR_ID    = 0x0201;

   // === Conversion Routines ===
   constexpr auto INT32_64_CONVERSION        = 0x001;
   constexpr auto INT32_FLOAT64_CONVERSION   = 0x002;
   constexpr auto INT16_32_CONVERSION        = 0x003;
   constexpr auto INT8_32_CONVERSION         = 0x004;

   // === Global Attributes ===
   constexpr auto GA_SYMBOL_NAME             = 0x0001;
   constexpr auto GA_CLASS_NAME              = 0x0002;
   constexpr auto GA_EXT_OVERLOAD_LIST       = 0x0003;

   // === VM Command ===
   constexpr pos_t VM_STR_COMMAND_MASK       = 0x100;
   constexpr pos_t VM_INT_COMMAND_MASK       = 0x200;

   constexpr pos_t VM_ENDOFTAPE_CMD          = 0x001;
   constexpr pos_t VM_CALLSYMBOL_CMD         = 0x102;
   constexpr pos_t VM_SETNAMESPACE_CMD       = 0x103;
   constexpr pos_t VM_SETPACKAGEPATH_CMD     = 0x104;
   constexpr pos_t VM_INIT_CMD               = 0x005;
   constexpr pos_t VM_FORWARD_CMD            = 0x106;
   constexpr pos_t VM_PACKAGE_CMD            = 0x107;
   constexpr pos_t VM_PRELOADED_CMD          = 0x108;
   constexpr pos_t VM_CALLCLASS_CMD          = 0x109;
   constexpr pos_t VM_TERMINAL_CMD           = 0x00A;
   constexpr pos_t VM_STRING_CMD             = 0x10B;
   constexpr pos_t VM_ALLOC_CMD              = 0x20C;
   constexpr pos_t VM_SET_ARG_CMD            = 0x20D;
   constexpr pos_t VM_ARG_TAPE_CMD           = 0x20E;
   constexpr pos_t VM_NEW_CMD                = 0x10F;
   constexpr pos_t VM_CONFIG_CMD             = 0x110;
   constexpr pos_t VM_FREE_CMD               = 0x211;
   constexpr pos_t VM_SEND_MESSAGE_CMD       = 0x112;
   constexpr pos_t VM_TRYCALLSYMBOL_CMD      = 0x113;

   // --- ELENA standard forwards
   constexpr auto INLINE_CLASSNAME           = "$inline";         // nested class generic name

   constexpr auto OPERATION_MAP_KEY          = "statements";
   constexpr auto PREDEFINED_MAP_KEY         = "defaults";
   constexpr auto ATTRIBUTES_MAP_KEY         = "attributes";
   constexpr auto ALIASES_MAP_KEY            = "aliases";

   constexpr auto OPERATION_MAP              = "system@operations@statements";
   constexpr auto PREDEFINED_MAP             = "system@predefined@defaults";
   constexpr auto ATTRIBUTES_MAP             = "system@attributes@attributes";
   constexpr auto ALIASES_MAP                = "system@predefined@aliases";

   constexpr auto STARTUP_ENTRY              = "$auto'startUpSymbol";

   constexpr auto VM_TAPE                    = "$elena'meta$startUpTape";

   constexpr auto PROGRAM_ENTRY              = "$forwards'program";         // used by the linker to define the debug entry

   constexpr auto SYSTEM_FORWARD             = "$system_entry";   // the system entry
   constexpr auto SUPER_FORWARD              = "$super";          // the common class predecessor
   constexpr auto NILVALUE_FORWARD           = "$nil";            // the nil value
   constexpr auto INTLITERAL_FORWARD         = "$int";            // the int literal
   constexpr auto LONGLITERAL_FORWARD        = "$long";           // the long literal
   constexpr auto REALLITERAL_FORWARD        = "$real";           // the real literal
   constexpr auto INT8LITERAL_FORWARD        = "$int8";           // the int literal
   constexpr auto UINT8LITERAL_FORWARD       = "$uint8";          // the int literal
   constexpr auto INT16LITERAL_FORWARD       = "$short";          // the int literal
   constexpr auto UINT16LITERAL_FORWARD      = "$ushort";         // the int literal
   constexpr auto LITERAL_FORWARD            = "$string";         // the string literal
   constexpr auto WIDELITERAL_FORWARD        = "$wide";           // the wide string literal
   constexpr auto CHAR_FORWARD               = "$char";           // the char literal
   constexpr auto BOOL_FORWARD               = "$boolean";        // the boolean class
   constexpr auto TRUE_FORWARD               = "$true";           // the true boolean value
   constexpr auto FALSE_FORWARD              = "$false";          // the false boolean value
   constexpr auto WRAPPER_FORWARD            = "$ref";            // the wrapper template
   constexpr auto ARRAY_FORWARD              = "$array";          // the array template
   constexpr auto VARIADIC_ARRAY_FORWARD     = "$varray";         // the array template 
   constexpr auto MESSAGE_FORWARD            = "$message";        // the message class
   constexpr auto MESSAGE_NAME_FORWARD       = "$subject";        // the message class
   constexpr auto EXT_MESSAGE_FORWARD        = "$ext_message";    // the extension message class
   constexpr auto CLOSURE_FORWARD            = "$closure";        // the closure template class
   constexpr auto TUPLE_FORWARD              = "$tuple";          // the tuple template class
   constexpr auto YIELDIT_FORWARD            = "$yieldit";        // the yield state machine iterator template class
   constexpr auto ASYNCIT_FORWARD            = "$taskit";         // the async state machine iterator template class
   constexpr auto UINT_FORWARD               = "$uint";           // the uint wrapper
   constexpr auto PTR_FORWARD                = "$ptr";            // the ptr wrapper
   constexpr auto TASK_FORWARD               = "$task";           // the ptr wrapper
   constexpr auto LAZY_FORWARD               = "$lazy";
   constexpr auto NULLABLE_FORWARD           = "$nullable";
   constexpr auto PRELOADED_FORWARD          = "system@preloadedSymbols";
   constexpr auto START_FORWARD              = "$symbol_entry";

   // --- Configuration xpaths ---
   constexpr auto WIN_X86_KEY       = "Win_x86";
   constexpr auto WIN_X86_64_KEY    = "Win_x64";
   constexpr auto LINUX_X86_KEY     = "Linux_I386";
   constexpr auto LINUX_X86_64_KEY  = "Linux_AMD64";
   constexpr auto LINUX_PPC64le_KEY = "Linux_PPC64le";
   constexpr auto LINUX_ARM64_KEY   = "Linux_ARM64";

   constexpr auto LIBRARY_KEY       = "Library";
   constexpr auto CONSOLE_KEY       = "STA Console";
   constexpr auto GUI_KEY           = "STA GUI";
   constexpr auto MT_CONSOLE_KEY    = "MTA Console";
   constexpr auto VM_CONSOLE_KEY    = "VM STA Console";
   constexpr auto VM_GUI_KEY        = "VM STA GUI";

   constexpr auto CONFIG_ROOT             = "configuration";
   constexpr auto PLATFORM_CATEGORY       = "configuration/platform";

   constexpr auto COLLECTION_CATEGORY     = "configuration/collection/*";

   constexpr auto TEMPLATE_CATEGORY       = "templates/*";
   constexpr auto PRIMITIVE_CATEGORY      = "primitives/*";
   constexpr auto FORWARD_CATEGORY        = "forwards/*";
   constexpr auto EXTERNAL_CATEGORY       = "externals/*";
   constexpr auto WINAPI_CATEGORY         = "winapi/*";
   constexpr auto REFERENCE_CATEGORY      = "references/*";
   constexpr auto MODULE_CATEGORY         = "files/*";
   constexpr auto FILE_CATEGORY           = "include/*";
   constexpr auto PARSER_TARGET_CATEGORY  = "targets/*";
   constexpr auto PROFILE_CATEGORY        = "/profile";

   constexpr auto LIB_PATH                = "project/libpath";
   constexpr auto OUTPUT_PATH             = "project/output";
   constexpr auto TARGET_PATH             = "project/executable";
   constexpr auto PROJECT_TEMPLATE        = "project/template";
   constexpr auto NAMESPACE_KEY           = "project/namespace";
   constexpr auto DEBUGMODE_PATH          = "project/debuginfo";
   constexpr auto FILE_PROLOG             = "project/prolog";
   constexpr auto FILE_EPILOG             = "project/epilog";
   constexpr auto MODULE_PROLOG           = "project/moduleProlog";
   constexpr auto AUTOEXTENSION_PATH      = "project/autoextension";

   constexpr auto PLATFORMTYPE_KEY        = "system/platform";

   constexpr auto MGSIZE_PATH             = "linker/mgsize";
   constexpr auto YGSIZE_PATH             = "linker/ygsize";
   constexpr auto THREAD_COUNTER          = "linker/threadcounter";

   constexpr auto MANIFEST_NAME           = "manifest/name";
   constexpr auto MANIFEST_VERSION        = "manifest/version";
   constexpr auto MANIFEST_AUTHOR         = "manifest/author";

   constexpr auto RETVAL_ARG              = "$retVal";
   constexpr auto PARENT_VAR              = "$parent";
   constexpr auto OWNER_VAR               = "$owner";

   inline ustr_t getPlatformName(PlatformType type)
   {
      switch (type) {
         case PlatformType::Win_x86:
            return WIN_X86_KEY;
         case PlatformType::Win_x86_64:
            return WIN_X86_64_KEY;
         case PlatformType::Linux_x86:
            return LINUX_X86_KEY;
         case PlatformType::Linux_x86_64:
            return LINUX_X86_64_KEY;
         case PlatformType::Linux_PPC64le:
            return LINUX_PPC64le_KEY;
         case PlatformType::Linux_ARM64:
            return LINUX_ARM64_KEY;
         default:
            return nullptr;
      }
   }

   typedef Map<ustr_t, ref_t, allocUStr, freeUStr> AttributeMap;

   class LangHelper
   {
   public:
      static void loadAttributes(AttributeMap& map)
      {
         map.add("singleton", V_SINGLETON);
         map.add("public_singleton", V_SINGLETON);
         map.add("public_singleton", V_PUBLIC);
         //map.add("preloaded_symbol", V_PRELOADED);
         map.add("function", V_FUNCTION);
         map.add("get_method", V_GETACCESSOR);
         map.add("script_method", V_SCRIPTSELFMODE);
         map.add("public_namespace", V_PUBLIC);
         map.add("public_class", V_PUBLIC);
         map.add("public_textblock", V_TEXTBLOCK);
         map.add("public_textblock", V_PUBLIC);
         map.add("script_function", V_SCRIPTSELFMODE);
         map.add("script_function", V_FUNCTION);
         map.add("public_symbol", V_PUBLIC);
         map.add("new_variable", V_VARIABLE);
         map.add("new_identifier", V_NEWOP);
         map.add("super_identifier", V_SUPERIOR);
         //map.add("loop_expression", V_LOOP);
      }
   };
}

#endif
