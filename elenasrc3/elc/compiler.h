//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class.
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef COMPILER_H
#define COMPILER_H

#include "clicommon.h"
#include "buildtree.h"
#include "syntaxtree.h"
#include "compilerlogic.h"

namespace elena_lang
{
   enum class ObjectKind
   {
      Unknown = 0,

      AttributeDictionary, // meta symbols
      StringDictionary,
      TypeDictionary,
      TypeList,
      DistributedTypeList,

      MetaConstant,
      StringLiteral,
      WideStringLiteral,
      CharacterLiteral,
      IntLiteral,
      LongLiteral,
      Float64Literal,
      ConstantLiteral,
      MssgNameLiteral,
      MssgLiteral,
      ExtMssgLiteral,
      Template,
      Nil,
      Default,
      Terminator,
      Symbol,
      Class,
      ClassSelf,
      // NOTE : used for the constructor resend operation
      // it is a message self argument
      ConstructorSelf,
      Method,
      Object,
      Singleton,
      InternalProcedure,
      Param,
      ParamReference,
      VArgParam,
      ParamAddress,
      ByRefParam,
      ByRefParamAddress,
      OutParam,
      OutParamAddress,
      Local,
      LocalReference,
      RefLocal,
      TempLocal,
      SelfLocal,
      SuperLocal,
      ReadOnlySelfLocal,
      LocalAddress,
      TempLocalAddress,
      SelfBoxableLocal, // the argument can be stack allocated
      Extern,
      FloatExtern,
      NewVariable,
      ReadOnlyFieldAddress,
      FieldAddress,
      ReadOnlyField,
      Field,
      Outer,
      OuterField,
      OuterSelf,
      Closure,
      Extension,
      ConstantRole,
      ClassConstant,
      Constant,
      ConstArray,
      SelfName,
      SelfPackage,
      MethodName,
      FieldName,
      StaticField,
      StaticThreadField,
      StaticConstField,
      ClassStaticConstField,
      Wrapper,
      ContextInfo,
      MemberInfo,
      LocalField,
      ConstGetter,  // key = value constant
   };

   enum TargetMode
   {
      None,
      Probe,
      External,
      WinApi,
      CreatingArray,
      Creating,
      Casting,
      UnboxingRequired,
      RefUnboxingRequired,
      LocalUnboxingRequired,
      ArrayContent,
      UnboxingVarArgument,
      UnboxingAndTypecastingVarArgument,
      BoxingPtr,
      Conditional,
      ConditionalUnboxingRequired,
      Weak,
   };

   enum DeclResult : int
   {
      Success = 0,
      Duplicate = 1,
      Illegal = 2
   };

   struct ObjectInfo
   {
      ObjectKind kind;
      TypeInfo   typeInfo;
      union
      {
         ref_t   reference;
         int     argument;
      };
      int        extra;
      TargetMode mode;

      bool operator ==(ObjectInfo& val) const
      {
         return (this->kind == val.kind && this->reference == val.reference && this->typeInfo.typeRef == val.typeInfo.typeRef
            && this->typeInfo.elementRef == val.typeInfo.elementRef);
      }

      bool operator !=(ObjectInfo& val) const
      {
         return !(*this == val);
      }

      ObjectInfo()
      {
         kind = ObjectKind::Unknown;
         typeInfo = {};
         reference = 0;
         extra = 0;
         mode = TargetMode::None;
      }
      ObjectInfo(ObjectKind kind)
      {
         this->kind = kind;
         typeInfo = {};
         this->reference = 0;
         this->extra = 0;
         mode = TargetMode::None;
      }
      ObjectInfo(ObjectKind kind, ref_t reference)
      {
         this->kind = kind;
         this->typeInfo = {};
         this->reference = reference;
         this->extra = 0;
         mode = TargetMode::None;
      }
      ObjectInfo(ObjectKind kind, TypeInfo typeInfo, ref_t reference)
      {
         this->kind = kind;
         this->typeInfo = typeInfo;
         this->reference = reference;
         this->extra = 0;
         mode = TargetMode::None;
      }
      ObjectInfo(ObjectKind kind, TypeInfo typeInfo, int value)
      {
         this->kind = kind;
         this->typeInfo = typeInfo;
         this->argument = value;
         this->extra = 0;
         mode = TargetMode::None;
      }
      ObjectInfo(ObjectKind kind, TypeInfo typeInfo, ref_t reference, ref_t extra)
      {
         this->kind = kind;
         this->typeInfo = typeInfo;
         this->reference = reference;
         this->extra = (int)extra;
         mode = TargetMode::None;
      }
      ObjectInfo(ObjectKind kind, TypeInfo typeInfo, int argument, ref_t extra)
      {
         this->kind = kind;
         this->typeInfo = typeInfo;
         this->reference = argument;
         this->extra = (int)extra;
         mode = TargetMode::None;
      }
      ObjectInfo(ObjectKind kind, TypeInfo typeInfo, ref_t reference, int extra)
      {
         this->kind = kind;
         this->typeInfo = typeInfo;
         this->reference = reference;
         this->extra = extra;
         mode = TargetMode::None;
      }
      ObjectInfo(ObjectKind kind, TypeInfo typeInfo, int argument, int extra)
      {
         this->kind = kind;
         this->typeInfo = typeInfo;
         this->argument = argument;
         this->extra = extra;
         mode = TargetMode::None;
      }
      ObjectInfo(ObjectKind kind, TypeInfo typeInfo, int argument, TargetMode mode)
      {
         this->kind = kind;
         this->typeInfo = typeInfo;
         this->argument = argument;
         this->extra = 0;
         this->mode = mode;
      }
      ObjectInfo(ObjectKind kind, TypeInfo typeInfo, ref_t reference, TargetMode mode)
      {
         this->kind = kind;
         this->typeInfo = typeInfo;
         this->reference = reference;
         this->extra = 0;
         this->mode = mode;
      }
      ObjectInfo(ObjectKind kind, TypeInfo typeInfo, ref_t reference, int extra, TargetMode mode)
      {
         this->kind = kind;
         this->typeInfo = typeInfo;
         this->reference = reference;
         this->extra = extra;
         this->mode = mode;
      }
   };

   typedef Pair<ObjectKind, ref_t, ObjectKind::Unknown, 0>                                   ObjectKey;
   typedef MemoryMap<ObjectKey, ObjectInfo, Map_StoreKey<ObjectKey>, Map_GetKey<ObjectKey>>  ObjectKeys;
   typedef CachedList<ref_t, 4>                                                              TemplateTypeList;

   struct Parameter
   {
      int      offset;
      TypeInfo typeInfo;
      int      size;
      bool     unassigned;

      Parameter()
      {
         offset = -1;
         typeInfo = {};
         size = 0;
         unassigned = false;
      }
      Parameter(int offset)
      {
         this->offset = offset;
         typeInfo = {};
         this->size = 0;
         this->unassigned = false;
      }
      Parameter(int offset, TypeInfo typeInfo)
      {
         this->offset = offset;
         this->typeInfo = typeInfo;
         this->size = 0;
         this->unassigned = false;
      }
      Parameter(int offset, TypeInfo typeInfo, int size)
      {
         this->offset = offset;
         this->typeInfo = typeInfo;
         this->size = size;
         this->unassigned = false;
      }
      Parameter(int offset, TypeInfo typeInfo, int size, bool unassigned)
      {
         this->offset = offset;
         this->typeInfo = typeInfo;
         this->size = size;
         this->unassigned = unassigned;
      }
   };

   typedef CachedList<ObjectInfo, 5> ArgumentsInfo;
   typedef CachedList<ref_t, 5>      TypeList;

   // --- Interpreter ---
   class Interpreter
   {
      ModuleScopeBase* _scope;
      CompilerLogic*   _logic;

      void setAttributeMapValue(ref_t dictionaryRef, ustr_t key, int value);
      void setAttributeMapValue(ref_t dictionaryRef, ustr_t key, ustr_t value);
      void setTypeMapValue(ref_t dictionaryRef, ustr_t key, ref_t reference);

      void addTypeListItem(ref_t dictionaryRef, ref_t symbolRef, ref_t mask);
      void addConstArrayItem(ref_t dictionaryRef, ref_t item, ref_t mask);
      void addIntArrayItem(ref_t dictionaryRef, int value);
      void addLongArrayItem(ref_t dictionaryRef, long long value);
      void addFloatArrayItem(ref_t dictionaryRef, double value);
      void addMssgNameArrayItem(ref_t dictionaryRef, ref_t constRef);

      bool evalDictionaryOp(ref_t operator_id, ArgumentsInfo& args);

      bool evalObjArrayOp(ref_t operator_id, ArgumentsInfo& args);
      bool evalDeclOp(ref_t operator_id, ArgumentsInfo& args, ObjectInfo& retVal);
      bool evalIntOp(ref_t operator_id, ArgumentsInfo& args, ObjectInfo& retVal);
      bool evalRealOp(ref_t operator_id, ArgumentsInfo& args, ObjectInfo& retVal);

   public:
      ObjectInfo mapStringConstant(ustr_t s);
      ObjectInfo mapWideStringConstant(ustr_t s);

      bool eval(BuildKey key, ref_t operator_id, ArgumentsInfo& args, ObjectInfo& retVal);

      ObjectInfo createConstCollection(ref_t arrayRef, ref_t typeRef, ArgumentsInfo& args, bool byValue);

      void copyConstCollection(ref_t sourRef, ref_t destRef, bool byValue);

      Interpreter(ModuleScopeBase* scope, CompilerLogic* logic);
   };

   // --- Compiler ---
   class Compiler : public CompilerBase
   {
   public:
      typedef MemoryMap<ustr_t, Parameter, Map_StoreUStr, Map_GetUStr>  LocalMap;

      // InheritResult
      enum class InheritResult
      {
         irNone = 0,
         irSuccessfull,
         irUnsuccessfull,
         irSealed,
         //irInvalid,
   //      irObsolete
      };

      struct Scope
      {
         enum class ScopeLevel
         {
            Namespace,
            Template,
            Symbol,
            Class,
            OwnerClass,
            ClassClass,
            Statemachine,
            Method,
            Field,
            Code,
            Expr
         };

         ModuleBase*      module;
         ModuleScopeBase* moduleScope;
         Scope*           parent;
         CompilerLogic*   compilerLogic;

         virtual Scope* getScope(ScopeLevel level)
         {
            if (parent) {
               return parent->getScope(level);
            }
            else {
               return nullptr;
            }
         }

         virtual void markAsAssigned(ObjectInfo object) {}

         virtual void raiseError(int message, SyntaxNode terminal)
         {
            parent->raiseError(message, terminal);
         }
         virtual void raiseWarning(int level, int message, SyntaxNode terminal)
         {
            parent->raiseWarning(level, message, terminal);
         }

         virtual ref_t mapNewIdentifier(ustr_t identifier, Visibility visibility)
         {
            return parent->mapNewIdentifier(identifier, visibility);
         }

         virtual ObjectInfo mapIdentifier(ustr_t identifier, bool referenceOne, ExpressionAttribute attr)
         {
            if (parent) {
               return parent->mapIdentifier(identifier, referenceOne, attr);
            }
            else return {};
         }

         virtual ObjectInfo mapMember(ustr_t identifier)
         {
            return {};
         }
         virtual ObjectInfo mapGlobal(ustr_t globalReference)
         {
            return {};
         }

         virtual ObjectInfo mapDictionary(ustr_t identifier, bool referenceOne, ExpressionAttribute mode)
         {
            if (parent) {
               return parent->mapDictionary(identifier, referenceOne, mode);
            }
            else return {};
         }

         virtual bool resolveAutoType(ObjectInfo& info, TypeInfo typeInfo, int size, int extra)
         {
            if (parent) {
               return parent->resolveAutoType(info, typeInfo, size, extra);
            }
            else return false;
         }

         virtual bool resolveAutoOutput(TypeInfo typeInfo)
         {
            if (parent) {
               return parent->resolveAutoOutput(typeInfo);
            }
            else return false;
         }

         template<class T> static T* getScope(Scope& scope, ScopeLevel level)
         {
            T* targetScope = (T*)scope.getScope(level);

            return targetScope;
         }

         Scope(Scope* parent)
         {
            this->parent = parent;
            if (parent) {
               this->module = parent->module;
               this->moduleScope = parent->moduleScope;
               this->compilerLogic = parent->compilerLogic;
            }
            else {
               this->module = nullptr;
               this->moduleScope = nullptr;
               this->compilerLogic = nullptr;
            }
         }
      };

      struct NamespaceScope : Scope
      {
         ReferenceName        nsName;
         IdentifierString     sourcePath;

         // forward declarations
         ForwardMap           forwards;
         // imported namespaces
         IdentifierList       importedNs;
         // extensions
         ExtensionMap         extensions;
         ExtensionTemplateMap extensionTemplates;
         ResolvedMap          extensionTargets;
         ResolvedMap          extensionDispatchers;
         ExtensionMap         declaredExtensions;

         // COMPILER MAGIC : used for extension template compilation
         ExtensionMap*        outerExtensionList;

         Map<ref_t, int>      intConstants;

         Visibility           defaultVisibility;

         ErrorProcessor*      errorProcessor;

         Scope* getScope(ScopeLevel level) override
         {
            if (level == ScopeLevel::Namespace) {
               return this;
            }
            else return Scope::getScope(level);
         }

         void addExtension(mssg_t message, ref_t extRef, mssg_t strongMessage);

         ref_t resolveExtensionTarget(ref_t reference);

         void raiseError(int message, SyntaxNode terminal) override;
         void raiseWarning(int level, int message, SyntaxNode terminal) override;

         ObjectInfo defineObjectInfo(ref_t reference, ExpressionAttribute mode, bool checkMode);
         ObjectInfo definePredefined(ref_t reference, ExpressionAttribute mode);
         ObjectInfo defineConstant(SymbolInfo info);

         ref_t resolveImplicitIdentifier(ustr_t name, bool referenceOne, bool innnerMost);

         ref_t mapNewIdentifier(ustr_t identifier, Visibility visibility) override;

         ObjectInfo mapIdentifier(ustr_t identifier, bool referenceOne, ExpressionAttribute mode) override;
         ObjectInfo mapGlobal(ustr_t identifier, ExpressionAttribute mode);
         ObjectInfo mapWeakReference(ustr_t identifier, bool directResolved);
         ObjectInfo mapDictionary(ustr_t identifier, bool referenceOne, ExpressionAttribute mode) override;

         void defineIntConstant(ref_t reference, int value)
         {
            intConstants.add(reference, value);
         }

         NamespaceScope(ModuleScopeBase* moduleScope, ErrorProcessor* errorProcessor, CompilerLogic* compilerLogic, ExtensionMap* outerExtensionList) :
            Scope(nullptr),
            forwards(0),
            importedNs(nullptr),
            extensions({}),
            extensionTemplates(nullptr),
            extensionTargets(INVALID_REF),
            extensionDispatchers(INVALID_REF),
            declaredExtensions({}),
            intConstants(0)
         {
            this->moduleScope = moduleScope;
            this->module = moduleScope->module;
            // by default - private visibility
            this->defaultVisibility = Visibility::Private;
            this->errorProcessor = errorProcessor;
            this->compilerLogic = compilerLogic;

            this->outerExtensionList = outerExtensionList;
         }
         NamespaceScope(NamespaceScope* parent);
      };

      struct FieldScope : Scope
      {
         ustr_t fieldName;

         Scope* getScope(ScopeLevel level) override
         {
            if (level == ScopeLevel::Field) {
               return this;
            }
            else return Scope::getScope(level);
         }

         FieldScope(Scope* parent, ustr_t fieldName);
      };

      struct SourceScope : Scope
      {
         Visibility visibility;
         ref_t      reference;

         SourceScope(Scope* parent, ref_t reference, Visibility visibility);
      };

      struct TemplateScope : SourceScope
      {
         TemplateType type;

         Scope* getScope(ScopeLevel level) override
         {
            if (level == ScopeLevel::Template) {
               return this;
            }
            else return Scope::getScope(level);
         }

         TemplateScope(Scope* parent, ref_t reference, Visibility visibility);
      };

      struct SymbolScope : SourceScope
      {
         SymbolInfo   info;
         SymbolKind   type;

         pos_t        reserved1;             // defines managed frame size
         pos_t        reserved2;             // defines unmanaged frame size (excluded from GC frame chain)
         pos_t        reservedArgs;          // contains the maximal argument list

         Scope* getScope(ScopeLevel level) override
         {
            if (level == ScopeLevel::Symbol) {
               return this;
            }
            else return Scope::getScope(level);
         }

         int allocLocalAddress(int size)
         {
            int retVal = reserved2;

            reserved2 += align(size, moduleScope->rawStackAlingment);

            return retVal;
         }

         void save();
         void load();

         SymbolScope(NamespaceScope* ns, ref_t reference, Visibility visibility);
      };

      struct ClassScope : SourceScope
      {
         ClassInfo   info;
         ref_t       extensionClassRef;
         bool        abstractMode;
         bool        abstractBasedMode;
         bool        extensionDispatcher;
         bool        withPrivateField;
         bool        withStaticConstructor;

         Scope* getScope(ScopeLevel level) override
         {
            if (level == ScopeLevel::Class || level == ScopeLevel::OwnerClass) {
               return this;
            }
            else return Scope::getScope(level);
         }

         void addMssgAttribute(mssg_t message, ClassAttribute attribute, mssg_t value)
         {
            ClassAttributeKey key = { message, attribute };
            info.attributes.exclude(key);
            info.attributes.add(key, value);
         }

         void addRefAttribute(mssg_t message, ClassAttribute attribute, ref_t value)
         {
            ClassAttributeKey key = { message, attribute };
            info.attributes.exclude(key);
            info.attributes.add(key, value);
         }

         void addAttribute(ClassAttribute attribute, ref_t value)
         {
            ClassAttributeKey key = { 0, attribute };
            info.attributes.exclude(key);
            info.attributes.add(key, value);
         }

         mssg_t getMssgAttribute(mssg_t message, ClassAttribute attribute)
         {
            return info.attributes.get({ message, attribute });
         }
         ref_t getAttribute(ClassAttribute attribute)
         {
            return info.attributes.get({ 0, attribute });
         }

         bool isClassClass()
         {
            return test(info.header.flags, elClassClass);
         }

         bool isAbstract()
         {
            return test(info.header.flags, elAbstract);
         }

         ObjectInfo mapMember(ustr_t identifier) override;

         virtual ObjectInfo mapField(ustr_t identifier, ExpressionAttribute attr);
         ObjectInfo mapPrivateField(ustr_t identifier, ExpressionAttribute attr);

         ObjectInfo mapIdentifier(ustr_t identifier, bool referenceOne, ExpressionAttribute attr) override;

         ObjectInfo mapDictionary(ustr_t identifier, bool referenceOne, ExpressionAttribute mode) override;

         void save();

         ClassScope(Scope* parent, ref_t reference, Visibility visibility);
      };

      class ClassClassScope : public ClassScope
      {
         // the reference to the proper class info (used for resolving class constants)
         ClassInfo* classInfo;
         ref_t      classInfoRef;

      public:
         Scope* getScope(ScopeLevel level) override
         {
            if (level == ScopeLevel::ClassClass) {
               return this;
            }
            else return ClassScope::getScope(level);
         }

         ref_t getProperClassRef() { return classInfoRef; }

         ObjectInfo mapField(ustr_t identifier, ExpressionAttribute attr) override;

         ClassClassScope(Scope* parent, ref_t reference, Visibility visibility, ClassInfo* classInfo, ref_t classInfoRef);
      };

      struct MethodScope : Scope
      {
         mssg_t       message;
         LocalMap     parameters;

         int          selfLocal;
         int          messageLocalAddress;

         MethodInfo   info;
         pos_t        reserved1;             // defines managed frame size
         pos_t        reserved2;             // defines unmanaged frame size (excluded from GC frame chain)
         pos_t        reservedArgs;          // contains the maximal argument list

         bool         functionMode;
         bool         closureMode;
         bool         nestedMode;
         bool         targetSelfMode;
         bool         constructorMode;
         bool         isEmbeddable;
         bool         byRefReturnMode;
         bool         isExtension;

         Scope* getScope(ScopeLevel level) override
         {
            if (level == ScopeLevel::Method) {
               return this;
            }
            else return Scope::getScope(level);
         }

         mssg_t getAttribute(ClassAttribute attribute, bool ownerClass = true)
         {
            ClassScope* classScope = Scope::getScope<ClassScope>(*this, ownerClass ? ScopeLevel::OwnerClass : ScopeLevel::Class);

            return classScope->getMssgAttribute(message, attribute);
         }

         mssg_t getAttribute(mssg_t attrMessage, ClassAttribute attribute, bool ownerClass = true)
         {
            ClassScope* classScope = Scope::getScope<ClassScope>(*this, ownerClass ? ScopeLevel::OwnerClass : ScopeLevel::Class);

            return classScope->getMssgAttribute(attrMessage, attribute);
         }

         ObjectInfo mapIdentifier(ustr_t identifier, bool referenceOne, ExpressionAttribute attr) override;
         ObjectInfo mapParameter(ustr_t identifier, ExpressionAttribute attr);
         ObjectInfo mapSelf(bool memberMode = true, bool ownerClass = false);
         ObjectInfo mapSuper();

         void markAsAssigned(ObjectInfo object) override;

         bool isPrivate() const
         {
            return test(message, STATIC_MESSAGE);
         }

         bool isProtected() const
         {
            return test(info.hints, (ref_t)MethodHint::Protected);
         }

         static bool checkHint(MethodInfo& methodInfo, MethodHint hint)
         {
            return test(methodInfo.hints, (ref_t)hint);
         }
         static bool checkAnyHint(MethodInfo& methodInfo, MethodHint hint1, MethodHint hint2)
         {
            return testany(methodInfo.hints, (ref_t)hint1 | (ref_t)hint2);
         }
         bool checkHint(MethodHint hint)
         {
            return test(info.hints, (ref_t)hint);
         }

         bool checkType(MethodHint type);

         bool isGeneric()
         {
            return checkHint(MethodHint::Generic);
         }
         bool isYieldable()
         {
            return checkHint(MethodHint::Yieldable);
         }
         bool isAsync()
         {
            return checkHint(MethodHint::Async);
         }

         ref_t getClassRef(bool ownerClass = true)
         {
            ClassScope* scope = Scope::getScope<ClassScope>(*this, ownerClass ? ScopeLevel::OwnerClass : ScopeLevel::Class);

            return scope ? scope->reference : 0;
         }

         ref_t getClassFlags(bool ownerClass = true)
         {
            ClassScope* scope = Scope::getScope<ClassScope>(*this, ownerClass ? ScopeLevel::OwnerClass : ScopeLevel::Class);

            return scope ? scope->info.header.flags : 0;
         }

         Visibility getClassVisibility(bool ownerClass = true)
         {
            ClassScope* scope = Scope::getScope<ClassScope>(*this, ownerClass ? ScopeLevel::OwnerClass : ScopeLevel::Class);

            return scope ? scope->visibility : Visibility::Public;
         }

         bool resolveAutoOutput(TypeInfo typeInfo) override
         {
            if (info.outputRef == V_AUTO) {
               if (!typeInfo.typeRef)
                  typeInfo.typeRef = moduleScope->buildins.superReference;

               info.outputRef = typeInfo.typeRef;

               return true;
            }
            else return Scope::resolveAutoOutput(typeInfo);
         }

         MethodScope(ClassScope* classScope);
      };

      typedef Map<int, SyntaxNode> NodeMap;

      struct CodeScope : Scope
      {
         // scope local variables
         LocalMap locals;
         NodeMap  localNodes;

         pos_t    allocated1, reserved1;       // defines managed frame size
         pos_t    allocated2, reserved2;       // defines unmanaged frame size

         bool     withRetStatement;

         Scope* getScope(ScopeLevel level) override
         {
            if (level == ScopeLevel::Code) {
               return this;
            }
            else return Scope::getScope(level);
         }

         ref_t getClassFlags(bool ownerClass = true)
         {
            ClassScope* scope = Scope::getScope<ClassScope>(*this, ownerClass ? ScopeLevel::OwnerClass : ScopeLevel::Class);

            return scope ? scope->info.header.flags : 0;
         }

         mssg_t getMessageID()
         {
            MethodScope* scope = Scope::getScope<MethodScope>(*this, ScopeLevel::Method);

            return scope ? scope->message : 0;
         }
         bool isSealedMethod()
         {
            MethodScope* scope = Scope::getScope<MethodScope>(*this, ScopeLevel::Method);

            return scope->checkType(MethodHint::Sealed);
         }

         TypeInfo getOutputInfo()
         {
            MethodScope* scope = Scope::getScope<MethodScope>(*this, ScopeLevel::Method);
            if (scope) {
               return { scope->info.outputRef, 0, scope->checkHint(MethodHint::Nillable) };
            }
            return {};
         }

         bool isByRefHandler()
         {
            MethodScope* scope = Scope::getScope<MethodScope>(*this, ScopeLevel::Method);

            return scope ? scope->byRefReturnMode : false;
         }

         bool isExtension()
         {
            MethodScope* scope = Scope::getScope<MethodScope>(*this, ScopeLevel::Method);

            return scope ? scope->isExtension : false;
         }

         bool resolveAutoType(ObjectInfo& info, TypeInfo typeInfo, int size, int extra) override;

         void markAsAssigned(ObjectInfo object) override;

         ObjectInfo mapIdentifier(ustr_t identifier, bool referenceOne, ExpressionAttribute attr) override;

         ObjectInfo mapByRefReturnArg();

         int newLocal()
         {
            allocated1++;
            if (allocated1 > reserved1)
               reserved1 = allocated1;

            return allocated1;
         }
         int allocLocalAddress(int size)
         {
            int retVal = allocated2;

            allocated2 += align(size, moduleScope->rawStackAlingment);
            if (allocated2 > reserved2)
               reserved2 = allocated2;

            return retVal;
         }

         ObjectInfo mapLocal(ustr_t identifier);

         void mapNewLocal(ustr_t local, int level)
         {
            locals.add(local, Parameter(level));
         }
         void mapNewLocal(ustr_t local, int level, TypeInfo typeInfo)
         {
            locals.add(local, Parameter(level, typeInfo));
         }
         void mapNewLocal(ustr_t local, int level, TypeInfo typeInfo, int size)
         {
            locals.add(local, Parameter(level, typeInfo, size));
         }
         void mapNewLocal(ustr_t local, int level, TypeInfo typeInfo, int size, bool unassigned)
         {
            locals.add(local, Parameter(level, typeInfo, size, unassigned));
         }

         void syncStack(MethodScope* methodScope);
         void syncStack(CodeScope* parentScope);

         CodeScope(MethodScope* scope);
         CodeScope(CodeScope* scope);
      };

      struct MetaScope : Scope
      {
         ScopeLevel scopeLevel;

         ObjectInfo mapDecl();

         ObjectInfo mapIdentifier(ustr_t identifier, bool referenceOne, ExpressionAttribute attr) override;

         MetaScope(Scope* parent, ScopeLevel scopeLevel);
      };

      struct ExprScope : Scope
      {
         ObjectKeys tempLocals;

         pos_t allocatedArgs;
         pos_t tempAllocated1;
         pos_t tempAllocated2;

         Scope* getScope(ScopeLevel level) override
         {
            if (level == ScopeLevel::Expr) {
               return this;
            }
            else return Scope::getScope(level);
         }

         ref_t getClassRef(bool ownerClass = true)
         {
            ClassScope* scope = Scope::getScope<ClassScope>(*this, ownerClass ? ScopeLevel::OwnerClass : ScopeLevel::Class);

            return scope ? scope->reference : 0;
         }
         ref_t getClassFlags(bool ownerClass = true)
         {
            ClassScope* scope = Scope::getScope<ClassScope>(*this, ownerClass ? ScopeLevel::OwnerClass : ScopeLevel::Class);

            return scope ? scope->info.header.flags : 0;
         }
         ref_t isSealed(bool ownerClass = true)
         {
            ClassScope* scope = Scope::getScope<ClassScope>(*this, ownerClass ? ScopeLevel::OwnerClass : ScopeLevel::Class);

            return scope ? test(scope->info.header.flags, elSealed) : false;
         }
         ref_t isExtension(bool ownerClass = true)
         {
            ClassScope* scope = Scope::getScope<ClassScope>(*this, ownerClass ? ScopeLevel::OwnerClass : ScopeLevel::Class);

            return scope ? (scope->extensionClassRef != 0) : false;
         }

         ObjectInfo mapSelf(bool ownerClass = false)
         {
            MethodScope* scope = Scope::getScope<MethodScope>(*this, ScopeLevel::Method);
            if (scope) {
               return scope->mapSelf(!scope->isExtension, ownerClass);
            }
            else return {};
         }

         ObjectInfo mapMember(ustr_t identifier) override;
         ObjectInfo mapGlobal(ustr_t globalReference) override;

         void markAsAssigned(ObjectInfo object) override
         {
            parent->markAsAssigned(object);
         }

         int newTempLocal();

         void reserveArgs(pos_t argsCount)
         {
            allocatedArgs = _max(allocatedArgs, argsCount);
         }

         void syncStack();

         ExprScope(SourceScope* parent);
         ExprScope(CodeScope* parent);
      };

      struct InlineClassScope : ClassScope
      {
         struct Outer
         {
            ref_t       reference;
            bool        updated;
            ObjectInfo  outerObject;

            Outer()
               : reference(INVALID_REF), updated(false), outerObject({})
            {
            }
         };

         Map<ustr_t, Outer, allocUStr, freeUStr> outers;

         ref_t expectedRef;

         Outer mapParent();
         Outer mapOwner();
         Outer mapSelf();

         ObjectInfo mapMember(ustr_t identifier) override;

         Scope* getScope(ScopeLevel level) override
         {
            if (level == ScopeLevel::Class) {
               return this;
            }
            else return Scope::getScope(level);
         }

         ObjectInfo mapIdentifier(ustr_t identifier, bool referenceOne, ExpressionAttribute attr) override;

         bool markAsPresaved(ObjectInfo object);

         InlineClassScope(ExprScope* owner, ref_t reference);
      };

      struct StatemachineClassScope : InlineClassScope
      {
         pos_t contextSize;
         ref_t typeRef;
         ref_t resultRef;
         bool  asyncMode;

         ObjectInfo mapContextField()
         {
            return { ObjectKind::Field };
         }
         ObjectInfo mapCurrentField();

         Scope* getScope(ScopeLevel level) override
         {
            if (level == ScopeLevel::Statemachine) {
               return this;
            }
            else return Scope::getScope(level);
         }

         StatemachineClassScope(ExprScope* owner, ref_t reference, bool asyncMode);
      };

      struct MessageResolution
      {
         bool   resolved;
         mssg_t message;
         ref_t  extensionRef;
         int    stackSafeAttr;
         mssg_t byRefHandler;

         MessageResolution()
         {
            this->resolved = false;
            this->message = 0;
            this->extensionRef = 0;
            this->stackSafeAttr = 0;
            this->byRefHandler = 0;
         }
         MessageResolution(bool resolved, mssg_t message)
         {
            this->resolved = resolved;
            this->message = message;
            this->extensionRef = 0;
            this->stackSafeAttr = 0;
            this->byRefHandler = 0;
         }
         MessageResolution(mssg_t message)
         {
            this->resolved = false;
            this->message = message;
            this->extensionRef = 0;
            this->stackSafeAttr = 0;
            this->byRefHandler = 0;
         }
      };

      struct MessageCallContext
      {
         mssg_t weakMessage;
         ref_t  implicitSignatureRef;
         pos_t  templateArgCount;
         ref_t* templateArgs;

         MessageCallContext()
            : weakMessage(0), implicitSignatureRef(0), templateArgCount(0), templateArgs(nullptr)
         {

         }
         MessageCallContext(mssg_t weakMessage, ref_t implicitSignatureRef)
            : weakMessage(weakMessage), implicitSignatureRef(implicitSignatureRef), templateArgCount(0), templateArgs(nullptr)
         {

         }
      };

      struct TerminalAttributes
      {
         bool variableMode;
         bool forwardMode;
         bool refOp;
         bool outRefOp;
         bool mssgOp;
         bool memberMode;

         bool isAnySet()
         {
            return forwardMode || variableMode || refOp || outRefOp || mssgOp || memberMode;
         }
      };

      class CommonHelper
      {
      protected:
         Compiler* compiler;

      public:
         CommonHelper(Compiler* compiler)
            : compiler(compiler)
         {

         }
      };

      class Namespace : public CommonHelper
      {
         friend class Compiler;

         void declareNamespace(SyntaxNode node, bool ignoreImport = false, bool ignoreExtensions = false);
         void declareMemberIdentifiers(SyntaxNode node);
         bool declareMembers(SyntaxNode node, bool& repeatMode, bool forced);

      public:
         NamespaceScope scope;

         void declare(SyntaxNode node, bool withMembers);

         Namespace(Compiler* compiler, ModuleScopeBase* moduleScope, ErrorProcessor* errorProcessor, CompilerLogic* compilerLogic,
            ExtensionMap* outerExtensionList);
         Namespace(Compiler* compiler, NamespaceScope* parent);
      };

      class MetaExpression : public CommonHelper
      {
         friend class Compiler;

         Interpreter*   interpreter;
         Scope*         scope;

         void generateObject(SyntaxTreeWriter& writer, SyntaxNode node);
         void generateNameOperation(SyntaxTreeWriter& writer, SyntaxNode node);
         void generateExpression(SyntaxTreeWriter& writer, SyntaxNode node);
         void generateMethod(SyntaxTreeWriter& writer, SyntaxNode node);

      public:
         ObjectInfo generateNestedConstant(SyntaxNode node);

         MetaExpression(Compiler* compiler, Scope* scope, Interpreter* interpreter);
      };

      class Symbol : public CommonHelper
      {
         friend class Compiler;

      public:
         SymbolScope scope;

         bool isDeclared()
         {
            return scope.moduleScope->isSymbolDeclared(scope.reference);
         }

         Symbol(Namespace& ns, ref_t reference, Visibility visibility);
         Symbol(Compiler* compiler, NamespaceScope* parent, ref_t reference, Visibility visibility);
      };

      class Class : public CommonHelper
      {
         friend class Compiler;

         ClassScope        scope;

         void resolveClassPostfixes(SyntaxNode node, bool extensionMode);

         void declareClassClass(ClassScope& classClassScope, SyntaxNode node, ref_t parentRef);

      public:
         bool isParentDeclared(SyntaxNode node);

         bool isDeclared()
         {
            return scope.moduleScope->isDeclared(scope.reference);
         }

         void declare(SyntaxNode node);

         void load();

         Class(Compiler* compiler, Scope* parent, ref_t reference, Visibility visibility);
         Class(Namespace& ns, ref_t reference, Visibility visibility);
      };

      class ClassClass : public CommonHelper
      {
         friend class Compiler;

         ClassClassScope   scope;

      public:
         void load();

         ClassClass(Class& classHelper);
      };

      class Method : public CommonHelper
      {
         friend class Compiler;

         MethodScope scope;

         void compileConstructor(BuildTreeWriter& writer, SyntaxNode current, ClassScope& classClassScope);

      public:
         void compile(BuildTreeWriter& writer, SyntaxNode current);
         void compileConstructor(BuildTreeWriter& writer, SyntaxNode current, ClassClass& classClassHelper);

         Method(Class& cls);
         Method(Compiler* compiler, ClassScope& classScope);
      };

      class Code : public CommonHelper
      {
         friend class Compiler;

         CodeScope scope;

      public:
         Code(Method& method);
      };

      class Expression : public CommonHelper
      {
         friend class Compiler;

         enum class ArgumentListType : int
         {
            Normal = 0,
            VariadicArgList = 1,
            VariadicArgListWithTypecasting = 2,
         };

         ExprScope         scope;
         BuildTreeWriter*  writer;

         bool isDirectMethodCall(SyntaxNode& node);

         bool checkValidity(ObjectInfo target, CheckMethodResult& result, bool allowPrivateCall);
         bool checkValidity(ObjectInfo target, MessageResolution& resolution, bool allowPrivateCall);

         ObjectInfo compileLookAhead(SyntaxNode node,
            ref_t targetRef, ExpressionAttribute attrs);

         ObjectInfo compileMessageOperation(SyntaxNode node, ref_t targetRef, ExpressionAttribute attrs);
         ObjectInfo compilePropertyOperation(SyntaxNode node, ref_t targetRef, ExpressionAttribute attrs);

         ObjectInfo compileOperation(SyntaxNode node, int operatorId, ref_t expectedRef, ExpressionAttribute mode);
         ObjectInfo compileEvalOnlySpecialOperation(SyntaxNode node);
         ObjectInfo compileSpecialOperation(SyntaxNode node, int operatorId, ref_t expectedRef);
         ObjectInfo compileAssignOperation(SyntaxNode node, int operatorId, ref_t expectedRef);
         ObjectInfo compileIndexAssignOperation(SyntaxNode lnode, SyntaxNode rnode, int operatorId, ref_t expectedRef);
         ObjectInfo compileBoolOperation(SyntaxNode node, int operatorId);
         ObjectInfo compileIndexerOperation(SyntaxNode node, int operatorId, ref_t expectedRef);
         ObjectInfo compileBranchingOperation(SyntaxNode node, int operatorId, bool retValExpected, bool withoutDebugInfo);
         ObjectInfo compileCatchOperation(SyntaxNode node);
         ObjectInfo compileFinalOperation(SyntaxNode node);
         ObjectInfo compileAltOperation(SyntaxNode node);
         ObjectInfo compileIsNilOperation(SyntaxNode node);
         ObjectInfo compileTupleAssigning(SyntaxNode node);

         ObjectInfo compileAssigning(SyntaxNode loperand, SyntaxNode roperand, ExpressionAttribute mode);

         ObjectInfo compileMessageOperationR(ObjectInfo target, SyntaxNode node, bool propertyMode);
         ObjectInfo compileMessageOperationR(SyntaxNode node, SyntaxNode messageNode, ObjectInfo source, ArgumentsInfo& arguments,
            ArgumentsInfo* updatedOuterArgs, ref_t expectedRef, bool propertyMode, bool probeMode, bool ignoreVariadics, ExpressionAttribute attrs);

         ObjectInfo compileLoop(SyntaxNode node, ExpressionAttribute mode);
         ObjectInfo compileExtern(SyntaxNode node, ExpressionAttribute mode);

         ObjectInfo compileNested(InlineClassScope& classCcope, ExpressionAttribute mode, ArgumentsInfo* updatedOuterArgs);

         ObjectInfo compileNested(SyntaxNode node, ExpressionAttribute mode, ArgumentsInfo* updatedOuterArgs);
         ObjectInfo compileClosure(SyntaxNode node, ref_t targetRef, ExpressionAttribute mode, ArgumentsInfo* updatedOuterArgs);
         ObjectInfo compileWeakOperation(SyntaxNode node, ref_t* arguments, pos_t argLen,
            ObjectInfo& loperand, ArgumentsInfo& messageArguments, mssg_t message, ref_t expectedRef, ArgumentsInfo* updatedOuterArgs);

         ObjectInfo compileNewOp(SyntaxNode node, ObjectInfo source, ref_t signRef, ArgumentsInfo& arguments);

         ObjectInfo typecastObject(SyntaxNode node, ObjectInfo source, ref_t targetRef, bool nillable);

         ObjectInfo validateObject(SyntaxNode node, ObjectInfo retVal,
            ref_t targetRef, bool noPrimitives, bool paramMode, bool dynamicRequired, bool nillable);

         ObjectInfo compileExternalOp(SyntaxNode node, ref_t externalRef, bool stdCall,
            ArgumentsInfo& arguments, ref_t expectedRef);

         ObjectInfo compileNewArrayOp(SyntaxNode node, ObjectInfo source, ref_t targetRef, ArgumentsInfo& arguments);

         ObjectInfo convertObject(SyntaxNode node, ObjectInfo source, ref_t targetRef, bool dynamicRequired,
            bool withoutBoxing, bool nillable, bool directConversion);

         void handleUnsupportedMessageCall(SyntaxNode node, mssg_t message, ref_t targetRef, bool weakTarget, bool strongResolved);

         ObjectInfo compileMessageCall(SyntaxNode node, ObjectInfo target, MessageCallContext& context, MessageResolution resolution,
            ArgumentsInfo& arguments, ExpressionAttributes mode, ArgumentsInfo* updatedOuterArgs);
         ObjectInfo compileOperation(SyntaxNode loperand, SyntaxNode roperand,
            int operatorId, ref_t expectedRef);
         ObjectInfo compileOperation(SyntaxNode node, ArgumentsInfo& messageArguments,
            int operatorId, ref_t expectedRef, ArgumentsInfo* updatedOuterArgs);

         ObjectInfo compileBranchingOperation(SyntaxNode node, ObjectInfo loperand, SyntaxNode rnode,
            SyntaxNode r2node, int operatorId, ArgumentsInfo* updatedOuterArgs, bool retValExpected, bool withoutDebugInfo);

         ref_t compileMessageArguments(SyntaxNode current, ArgumentsInfo& arguments, ref_t expectedSignRef, ExpressionAttribute mode,
            ArgumentsInfo* updatedOuterArgs, ArgumentListType& argListType, int nillableArgs);

         MessageResolution resolveByRefHandler(ObjectInfo source, ref_t expectedRef, MessageCallContext& context, bool noExtensions);
         MessageResolution resolveMessageAtCompileTime(ObjectInfo target, MessageCallContext& messageContext, bool ignoreExtensions,
            bool ignoreVariadics, bool checkByRefHandler = false);

         ObjectInfo declareTempLocal(ref_t typeRef, bool dynamicOnly = true);
         ObjectInfo declareTempStructure(SizeInfo sizeInfo);

         ObjectInfo boxArgument(ObjectInfo info, bool stackSafe, bool boxInPlace, bool allowingRefArg, ref_t targetRef = 0);
         ObjectInfo boxArgumentLocally(ObjectInfo info, bool stackSafe, bool forced);
         ObjectInfo boxLocally(ObjectInfo info, bool stackSafe);
         ObjectInfo boxPtrLocally(ObjectInfo info);
         ObjectInfo boxArgumentInPlace(ObjectInfo info, ref_t targetRef = 0);
         ObjectInfo boxRefArgumentInPlace(ObjectInfo info, ref_t targetRef = 0);
         ObjectInfo boxVariadicArgument(ObjectInfo info);

         ObjectInfo unboxArguments(ObjectInfo retVal, ArgumentsInfo* updatedOuterArgs);
         void unboxArgumentLocaly(ObjectInfo tempLocal, ObjectKey targetKey);
         void unboxOuterArgs(ArgumentsInfo* updatedOuterArgs);

         ObjectInfo saveToTempLocal(ObjectInfo object);

         ObjectInfo compileBranchingOperands(SyntaxNode rnode, SyntaxNode r2node, bool retValExpected, bool withoutDebugInfo);
         ObjectInfo compileTernaryOperands(SyntaxNode rnode, SyntaxNode r2node, BuildNode& opNode, bool withoutDebugInfo);

         ObjectInfo compileNativeConversion(SyntaxNode node, ObjectInfo source, ref_t operationKey);

         ObjectInfo allocateResult(ref_t resultRef);

         void compileNestedInitializing(InlineClassScope& classScope, ref_t nestedRef, int& preservedContext,
            ArgumentsInfo* updatedOuterArgs);

         void compileYieldOperation(SyntaxNode node);
         void compileAsyncOperation(SyntaxNode node, bool valueExpected);
         void compileSwitchOperation(SyntaxNode node);

         bool compileAssigningOp(ObjectInfo target, ObjectInfo source, bool& nillableOp);

         bool validateShortCircle(mssg_t message, ObjectInfo target);

         ref_t mapNested(ExpressionAttribute mode);

         bool resolveAutoType(ObjectInfo source, ObjectInfo& target);

         void showContextInfo(mssg_t message, ref_t targetRef);

         void writeMessageArguments(ObjectInfo& target, mssg_t message, ArgumentsInfo& arguments, ObjectInfo& lenLocal,
            int& stackSafeAttr, bool targetOverridden, bool found, ArgumentListType argType, bool stackSafe);

         void convertIntLiteralForOperation(SyntaxNode node, int operatorId, ArgumentsInfo& messageArguments);

      public:
         bool writeObjectInfo(ObjectInfo info, bool allowMeta = false);
         void writeObjectInfo(ObjectInfo info, SyntaxNode node)
         {
            if (!writeObjectInfo(info))
               scope.raiseError(errInvalidOperation, node);
         }

         void compileAssigning(SyntaxNode node, ObjectInfo target, ObjectInfo source, bool noConversion = false);
         void compileConverting(SyntaxNode node, ObjectInfo source, ref_t targetRef, bool stackSafe);

         ObjectInfo compileSymbolRoot(SyntaxNode bodyNode, ExpressionAttribute mode, ref_t targetRef);
         ObjectInfo compileRoot(SyntaxNode node, ExpressionAttribute mode);
         ObjectInfo compileReturning(SyntaxNode node, ExpressionAttribute mode, TypeInfo outputInfo);

         ObjectInfo compile(SyntaxNode node, ref_t targetRef, ExpressionAttribute mode,
            ArgumentsInfo* updatedOuterArgs);
         ObjectInfo compileObject(SyntaxNode node, ExpressionAttribute mode, ArgumentsInfo* updatedOuterArgs);
         ObjectInfo compileCollection(SyntaxNode node, ExpressionAttribute mode);
         ObjectInfo compileTupleCollection(SyntaxNode node, ref_t targetRef);
         ObjectInfo compileKeyValue(SyntaxNode node, ExpressionAttribute mode);
         ObjectInfo compileClosureOperation(SyntaxNode node, ref_t targetRef);
         ObjectInfo compileInterpolation(SyntaxNode node);

         ObjectInfo compileSubCode(SyntaxNode node, ExpressionAttribute mode, bool withoutNewScope = false);

         Expression(Symbol& symbol, BuildTreeWriter& writer);
         Expression(Code& code, BuildTreeWriter& writer);
         Expression(Compiler* compiler, CodeScope& codeScope, BuildTreeWriter& writer);
         Expression(Compiler* compiler, SourceScope& symbolScope, BuildTreeWriter& writer);
      };

      class NestedClass : public CommonHelper
      {
         friend class Compiler;

      protected:
         BuildTreeWriter* writer;

      public:
         InlineClassScope scope;

         NestedClass(Compiler* compiler, Expression& code, ref_t nestedRef, BuildTreeWriter& writer);
      };

      class LambdaClosure : public NestedClass
      {
         friend class Compiler;

         ref_t resolveClosure(mssg_t closureMessage, ref_t outputRef);
         ref_t declareClosureParameters(MethodScope& methodScope, SyntaxNode argNode, bool& weakMessage);

      public:
         void declareClosureMessage(MethodScope& methodScope, SyntaxNode node);
         void compileExpressionMethod(MethodScope& scope, SyntaxNode node);
         void compileClosureMethod(MethodScope& scope, SyntaxNode node);

         void compile(SyntaxNode node);

         LambdaClosure(Compiler* compiler, Expression& code, ref_t nestedRef, BuildTreeWriter& writer, ref_t parentRef);
      };

      friend class CommonHelper;
      friend class Namespace;
      friend class Class;
      friend class Method;
      friend class Symbol;
      friend class Expression;

   private:
      CompilerLogic*         _logic;
      TemplateProssesorBase* _templateProcessor;
      ErrorProcessor*        _errorProcessor;
      PresenterBase*         _presenter;

      bool                   _optMode;
      bool                   _lookaheadOptMode;
      bool                   _tapeOptMode;
      bool                   _withMethodParamInfo;
      bool                   _trackingUnassigned;
      bool                   _withConditionalBoxing;
      bool                   _evaluateOp;
      bool                   _verbose;
      bool                   _noValidation;
      bool                   _withDebugInfo;
      bool                   _strictTypeEnforcing;

      void addTypeInfo(Scope& scope, SyntaxNode node, SyntaxKey key, TypeInfo typeInfo);

      void loadMetaData(ModuleScopeBase* moduleScope, ForwardResolverBase* forwardResolver, ustr_t name);

      void importExtensions(NamespaceScope& ns, ustr_t importedNs);
      void loadExtensions(NamespaceScope& ns, bool internalOne);

      void saveFrameAttributes(BuildTreeWriter& writer, Scope& scope, pos_t reserved, pos_t reservedN);

      ref_t resolveYieldType(Scope& scope, SyntaxNode node);
      ref_t declareAsyncStatemachine(StatemachineClassScope& scope, SyntaxNode node);

      pos_t saveMetaInfo(ModuleBase* module, ustr_t value, ustr_t postfix);

      ref_t mapNewTerminal(Scope& scope, ustr_t prefix, SyntaxNode nameNode, ustr_t postfix, Visibility visibility, bool ignoreDuplicates = false);
      mssg_t mapMethodName(MethodScope& scope, pos_t paramCount, ustr_t actionName, ref_t actionRef,
         ref_t flags, ref_t* signature, size_t signatureLen, bool withoutWeakMessages, bool noSignature);
      mssg_t mapMessage(Scope& scope, SyntaxNode node, bool propertyMode, bool extensionMode, bool probeMode);

      ExternalInfo mapExternal(Scope& scope, SyntaxNode node);
      static ObjectInfo mapClassSymbol(Scope& scope, ref_t classRef);
      ObjectInfo mapConstructorTarget(MethodScope& scope);

      ref_t mapConstantReference(Scope& scope);

      ref_t mapTemplateType(Scope& scope, SyntaxNode terminal, pos_t parameterCount);

      ref_t mapExtension(BuildTreeWriter& writer, Scope& scope, MessageCallContext& context,
         ObjectInfo object, mssg_t& resolvedMessage, int& stackSafeAttr);

      mssg_t defineMultimethod(Scope& scope, mssg_t messageRef, bool extensionMode);

      void declareTemplateAttributes(Scope& scope, SyntaxNode node, TemplateTypeList& parameters,
         TypeAttributes& attributes, bool declarationMode, bool objectMode);
      void declareIncludeAttributes(Scope& scope, SyntaxNode node, bool& textBlock);

      bool checkifSingleObject(Scope& scope, SyntaxNode node);

      static int defineFieldSize(Scope& scope, ObjectInfo info);

      ObjectInfo defineArrayType(Scope& scope, ObjectInfo info, bool declarationMode);
      ref_t defineArrayType(Scope& scope, ref_t elementRef, bool declarationMode);

      ref_t resolveStrongType(Scope& scope, TypeInfo typeInfo, bool declarationMode = false);
      TypeInfo resolveStrongTypeInfo(Scope& scope, TypeInfo typeInfo, bool declarationMode = false);

      ref_t retrieveType(Scope& scope, ObjectInfo info);
      ref_t resolveTypeIdentifier(Scope& scope, ustr_t identifier, SyntaxKey type,
         bool declarationMode, bool allowRole);
      ref_t resolveTypeTemplate(Scope& scope, SyntaxNode node,
         TypeAttributes& attributes, bool declarationMode, bool objectMode = false);

      ref_t resolveTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, ref_t elementRef, bool declarationMode);
      ref_t resolveClosure(Scope& scope, mssg_t closureMessage, ref_t outputRef);
      ref_t resolveStateMachine(Scope& scope, ref_t templateRef, ref_t stateRef);
      ref_t resolveWrapperTemplate(ModuleScopeBase& moduleScope, ref_t elementRef, bool declarationMode);
      ref_t resolveArrayTemplate(ModuleScopeBase& moduleScope, ref_t elementRef, bool declarationMode);
      //ref_t resolveNullableTemplate(ModuleScopeBase& moduleScope, ustr_t ns, ref_t elementRef, bool declarationMode);
      ref_t resolveArgArrayTemplate(ModuleScopeBase& moduleScope, ref_t elementRef, bool declarationMode);
      ref_t resolveTupleClass(Scope& scope, SyntaxNode node, ArgumentsInfo& items);

      int resolveSize(Scope& scope, SyntaxNode node);
      TypeInfo resolveTypeAttribute(Scope& scope, SyntaxNode node, TypeAttributes& attributes,
         bool declarationMode, bool allowRole);
      TypeInfo resolveTypeScope(Scope& scope, SyntaxNode node, TypeAttributes& attributes,
         bool declarationMode, bool allowRole);
      TypeInfo resolveStrongTypeAttribute(Scope& scope, SyntaxNode node, bool declarationMode, bool allowRole);

      ref_t retrieveTemplate(NamespaceScope& scope, SyntaxNode node, List<SyntaxNode>& parameters,
         ustr_t prefix, SyntaxKey argKey, ustr_t postFix);
      ref_t retrieveBlock(NamespaceScope& scope, SyntaxNode node);

      static mssg_t resolveOperatorMessage(ModuleScopeBase* scope, int operatorId);
      static mssg_t resolveVariadicMessage(Scope& scope, mssg_t message);

      bool isCompatible(Scope& scope, ObjectInfo source, ObjectInfo target, bool resolvePrimitives = true);

      bool isDefaultOrConversionConstructor(Scope& scope, mssg_t message, bool internalOne, bool& isProtectedDefConst);

      bool importEnumTemplate(Scope& scope, SyntaxNode node, SyntaxNode target);
      bool importTemplate(Scope& scope, SyntaxNode node, SyntaxNode target, bool weakOne);
      bool includeBlock(Scope& scope, SyntaxNode node, SyntaxNode target);
      bool importInlineTemplate(Scope& scope, SyntaxNode node, ustr_t postfix, SyntaxNode target);
      bool importPropertyTemplate(Scope& scope, SyntaxNode node, ustr_t postfix, SyntaxNode target);
      void importCode(Scope& scope, SyntaxNode node, SyntaxNode& importNode);

      void readFieldAttributes(ClassScope& scope, SyntaxNode node, FieldAttributes& attrs, bool declarationMode);

      static int allocateLocalAddress(Scope& scope, int size, bool binaryArray);

      static bool isClassClassOperation(Scope& scope, ObjectInfo target);

      static bool isProxy(Scope& scope, SyntaxNode node);

      ref_t declareMultiType(Scope& scope, SyntaxNode& node, ref_t elementRef);

      void declareClassAttributes(ClassScope& scope, SyntaxNode node, ref_t& fldeclaredFlagsags);

      void declareTemplateAttributes(TemplateScope& scope, SyntaxNode node, IdentifierString& postfix);
      void declareSymbolAttributes(SymbolScope& scope, SyntaxNode node, bool identifierDeclarationMode);
      void declareFieldAttributes(ClassScope& scope, SyntaxNode node, FieldAttributes& mode);
      void declareMethodAttributes(MethodScope& scope, SyntaxNode node, bool exensionMode);
      void declareArgumentAttributes(MethodScope& scope, SyntaxNode node, TypeInfo& typeInfo, bool declarationMode);
      void declareDictionaryAttributes(Scope& scope, SyntaxNode node, TypeInfo& typeInfo, bool& superMode);
      void declareExpressionAttributes(Scope& scope, SyntaxNode node, TypeInfo& typeInfo, ExpressionAttributes& mode);

      static ustr_t retrieveDictionaryOwner(Scope& scope, ustr_t properName, ustr_t defaultPrefix, ExpressionAttribute mode);

      void declareDictionary(Scope& scope, SyntaxNode node, Visibility visibility,
         Scope::ScopeLevel level, bool shareMode);

      void declareVMT(ClassScope& scope, SyntaxNode node, bool& withConstructors, bool& withDefaultConstructor,
         bool yieldMethodNotAllowed, bool staticNotAllowed, bool templateBased);

      void registerTemplateSignature(TemplateScope& scope, SyntaxNode node, IdentifierString& signature);
      void registerExtensionTemplateMethod(TemplateScope& scope, SyntaxNode& node);
      void registerExtensionTemplate(TemplateScope& scope, SyntaxNode& node);

      void saveTemplate(TemplateScope& scope, SyntaxNode& node);
      void saveNamespaceInfo(SyntaxNode node, NamespaceScope* nsScope, bool outerMost);

      void declareTemplate(TemplateScope& scope, SyntaxNode& node);

      void declareTemplateClass(TemplateScope& scope, SyntaxNode& node);
      void declareTemplateCode(TemplateScope& scope, SyntaxNode& node);

      InheritResult inheritClass(ClassScope& scope, ref_t parentRef/*, bool ignoreFields*/, bool ignoreSealed);

      void checkMethodDuplicates(ClassScope& scope, SyntaxNode node, mssg_t message,
         mssg_t publicMessage, bool protectedOne, bool internalOne);

      void checkUnassignedVariables(MethodScope& scope, SyntaxNode node);

      ref_t generateConstant(Scope& scope, ObjectInfo& info, ref_t reference, bool saveScope = true);

      mssg_t defineOutRefMethod(ClassScope& scope, SyntaxNode node, bool isExtension);

      void verifyMultimethods(Scope& scope, SyntaxNode node, SyntaxKey methodKey, ClassInfo& info, VirtualMethodList& implicitMultimethods);

      bool generateClassField(ClassScope& scope, FieldAttributes& attrs, ustr_t name, int sizeHint,
         TypeInfo typeInfo, bool singleField);

      void declareFieldMetaInfo(FieldScope& scope, SyntaxNode node);
      void declareFieldMetaInfos(ClassScope& scope, SyntaxNode node);

      void generateClassFlags(ClassScope& scope, ref_t declaredFlags);
      void generateParamNameInfo(ClassScope& scope, SyntaxNode node, mssg_t message);
      void generateMethodAttributes(ClassScope& scope, SyntaxNode node,
         MethodInfo& methodInfo, bool abstractBased);
      void generateMethodDeclaration(ClassScope& scope, SyntaxNode node, bool closed, bool hideDuplicate);
      void generateMethodDeclarations(ClassScope& scope, SyntaxNode node, SyntaxKey methodKey, bool closed);
      DeclResult checkAndGenerateClassField(ClassScope& scope, SyntaxNode node, ustr_t name, FieldAttributes& attrs, bool singleField);
      void generateClassStaticField(ClassScope& scope, SyntaxNode node, FieldAttributes& attrs);
      void generateClassFields(ClassScope& scope, SyntaxNode node, bool singleField);
      void generateClassDeclaration(ClassScope& scope, SyntaxNode node, ref_t declaredFlags);

      bool declareVariable(Scope& scope, SyntaxNode terminal, TypeInfo typeInfo, bool ignoreDuplicate);
      bool declareYieldVariable(Scope& scope, ustr_t name, TypeInfo typeInfo);

      void declareClassParent(ref_t parentRef, ClassScope& scope, SyntaxNode node);

      int resolveArraySize(Scope& scope, SyntaxNode node);

      void declareParameter(MethodScope& scope, SyntaxNode node, bool withoutWeakMessages,
         bool declarationMode, bool& variadicMode, bool& weakSignature, bool& noSignature,
         pos_t& paramCount, ref_t* signature, size_t& signatureLen, bool& nillable);

      ref_t declareClosureParameters(MethodScope& methodScope, SyntaxNode argNode);

      void declareVMTMessage(MethodScope& scope, SyntaxNode node, bool withoutWeakMessages, bool declarationMode, bool templateBasedMode);
      void declareClosureMessage(MethodScope& scope, SyntaxNode node);
      void declareIteratorMessage(MethodScope& scope, SyntaxNode node);

      void initializeMethod(ClassScope& scope, MethodScope& methodScope, SyntaxNode current);

      void declareSymbolMetaInfo(SymbolScope& scope, SyntaxNode node);

      void declareByRefHandler(SyntaxNode classNode, SyntaxKey methodType,
         ref_t targetRef, ClassInfo& info, mssg_t message, bool abstractOne);

      void declareMetaInfo(Scope& scope, SyntaxNode node);
      void declareMethodMetaInfo(MethodScope& scope, SyntaxNode node);
      void declareMethod(MethodScope& scope, SyntaxNode node, bool abstractMode,
         bool staticNotAllowed, bool yieldMethodNotAllowed);

      void declareSymbol(SymbolScope& scope, SyntaxNode node);

      void copyParentNamespaceExtensions(NamespaceScope& source, NamespaceScope& target);

      void declareModuleIdentifiers(ModuleScopeBase* moduleScope, SyntaxNode node, ExtensionMap* outerExtensionList);
      bool declareModule(ModuleScopeBase* moduleScope, SyntaxNode node, ExtensionMap* outerExtensionList, bool& repeatMode, bool forced);

      void inheritStaticMethods(ClassScope& scope, SyntaxNode classNode);

      void addExtensionMessage(Scope& scope, mssg_t message, ref_t extRef, mssg_t strongMessage,
         bool internalOne);
      void addExtensionTemplateMessage(Scope& scope, mssg_t message, ustr_t pattern, bool internalOne);

      void declareExtension(ClassScope& scope, mssg_t message, bool internalOne);

      void declareModuleExtensionDispatcher(NamespaceScope& scope, SyntaxNode node);

      void warnOnUnassignedLocal(SyntaxNode node, CodeScope& scope, int level);
      void warnOnUnassignedParameter(SyntaxNode node, Scope& scope, ustr_t paramName);

      ObjectInfo evalOperation(Interpreter& interpreter, Scope& scope, SyntaxNode node, ref_t operator_id, bool ignoreErrors = false);
      ObjectInfo evalExpression(Interpreter& interpreter, Scope& scope, SyntaxNode node, bool ignoreErrors = false, bool resolveMode = true);
      ObjectInfo evalObject(Interpreter& interpreter, Scope& scope, SyntaxNode node);
      ObjectInfo evalCollection(Interpreter& interpreter, Scope& scope, SyntaxNode node, bool anonymousOne, bool ignoreErrors);
      ObjectInfo evalPropertyOperation(Interpreter& interpreter, Scope& scope, SyntaxNode node, bool ignoreErrors);
      ObjectInfo evalExprValueOperation(Interpreter& interpreter, Scope& scope, SyntaxNode node, bool ignoreErrors);
      ObjectInfo evalSizeOperation(Interpreter& interpreter, Scope& scope, SyntaxNode node, bool ignoreErrors);
      ObjectInfo evalGetter(Interpreter& interpreter, Scope& scope, SyntaxNode node, bool ignoreErrors);

      void evalStatement(MetaScope& scope, SyntaxNode node);

      static void addBreakpoint(BuildTreeWriter& writer, SyntaxNode node, BuildKey bpKey);

      bool evalInitializers(ClassScope& scope, SyntaxNode node);
      bool evalClassConstant(ustr_t constName, ClassScope& scope, SyntaxNode node, ObjectInfo& constInfo);
      bool evalAccumClassConstant(ustr_t constName, ClassScope& scope, SyntaxNode node, ObjectInfo& constInfo);

      ref_t compileExtensionDispatcher(BuildTreeWriter& writer, NamespaceScope& scope, mssg_t genericMessage,
         ref_t outputRef);

      void writeParameterDebugInfo(BuildTreeWriter& writer, Scope& scope, int size, TypeInfo typeInfo,
         ustr_t name, int index);
      void writeMethodDebugInfo(BuildTreeWriter& writer, MethodScope& scope);
      void writeMessageInfo(BuildTreeWriter& writer, MethodScope& scope);

      void compileInlineInitializing(BuildTreeWriter& writer, ClassScope& classScope, SyntaxNode node);

      static ObjectInfo convertIntLiteral(ExprScope& scope, SyntaxNode node, ObjectInfo source, ref_t targetRef, bool ignoreError = false);

      bool compileSymbolConstant(SymbolScope& scope, ObjectInfo retVal);

      ObjectInfo defineTerminalInfo(Scope& scope, SyntaxNode node, TypeInfo declaredTypeInfo,
         TerminalAttributes& terminalAttrs, bool& invalid, ExpressionAttribute attrs);

      ObjectInfo mapStringConstant(Scope& scope, SyntaxNode node);
      ObjectInfo mapWideStringConstant(Scope& scope, SyntaxNode node);
      ObjectInfo mapCharacterConstant(Scope& scope, SyntaxNode node);
      ObjectInfo mapConstant(Scope& scope, SyntaxNode node);
      ObjectInfo mapIntConstant(Scope& scope, SyntaxNode node, int radix);
      ObjectInfo mapUIntConstant(Scope& scope, SyntaxNode node, int radix);
      ObjectInfo mapLongConstant(Scope& scope, SyntaxNode node, int radix);
      ObjectInfo mapFloat64Constant(Scope& scope, SyntaxNode node);
      ObjectInfo mapTerminal(Scope& scope, SyntaxNode node, TypeInfo typeInfo, ExpressionAttribute attrs);
      ObjectInfo mapMessageConstant(Scope& scope, SyntaxNode node, ref_t actionRef);
      ObjectInfo mapExtMessageConstant(Scope& scope, SyntaxNode node, ref_t actionRef, ref_t extension);

      ObjectInfo mapObject(Scope& scope, SyntaxNode node, ExpressionAttributes mode);

      ObjectInfo compileRootExpression(BuildTreeWriter& writer, CodeScope& scope, SyntaxNode node,
         ExpressionAttribute mode);
      ObjectInfo compileRetExpression(BuildTreeWriter& writer, CodeScope& scope, SyntaxNode node,
         ExpressionAttribute mode);
      ObjectInfo compileNestedExpression(BuildTreeWriter& writer, InlineClassScope& scope, ExprScope& ownerScope,
         ExpressionAttribute mode, ArgumentsInfo* updatedOuterArgs);

      void compileMultidispatch(BuildTreeWriter& writer, CodeScope& codeScope, ClassScope& classcope,
         SyntaxNode node, bool implicitMode);
      void compileDirectResendCode(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node);
      void compileDispatchCode(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node);
      void compileDispatchProberCode(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node);
      void compileConstructorDispatchCode(BuildTreeWriter& writer, CodeScope& codeScope, ClassScope& classClassScope, SyntaxNode node);
      void compileByRefHandlerInvoker(BuildTreeWriter& writer, MethodScope& scope, CodeScope& codeScope,
         mssg_t handler, ref_t targetRef);

      void compileRedirectDispatcher(BuildTreeWriter& writer, MethodScope& scope, CodeScope& codeScope, SyntaxNode node,
         bool withGenerics);
      void compileProxyDispatcher(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node);

      ObjectInfo compileResendCode(BuildTreeWriter& writer, CodeScope& codeScope, ObjectInfo source, SyntaxNode node);
      ObjectInfo compileRedirect(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node, ref_t outputRef);
      ObjectInfo compileCode(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node, bool closureMode, bool noDebugInfoMode = false);

      void beginMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node, BuildKey scopeKey,
         bool withDebugInfo);
      void endMethod(BuildTreeWriter& writer, MethodScope& scope);

      void compileMethodCode(BuildTreeWriter& writer, ClassScope* classScope, MethodScope& scope, CodeScope& codeScope,
         SyntaxNode node, bool newFrame);
      void compileConstructorCode(BuildTreeWriter& writer, SyntaxNode node, SyntaxNode current, MethodScope& scope,
         CodeScope& codeScope, ClassScope& classClassScope, bool isDefConvConstructor, ref_t classFlags, bool newFrame);
      void compileInplaceDefConstructorCode(BuildTreeWriter& writer, SyntaxNode current, SyntaxNode methodNode, MethodScope& scope,
         CodeScope& codeScope, ClassScope& classClassScope, ref_t classFlags, bool newFrame);
      void compileDefConvConstructorCode(BuildTreeWriter& writer, MethodScope& scope,
         SyntaxNode node, bool& newFrame);

      mssg_t declareInplaceConstructorHandler(MethodScope& invokerScope, ClassScope& classClassScope);
      mssg_t compileInplaceConstructorHandler(BuildTreeWriter& writer, MethodScope& invokerScope,
         ClassScope& classClassScope, SyntaxNode current, SyntaxNode methodNode, mssg_t handler);
      void compileByRefHandler(BuildTreeWriter& writer, MethodScope& invokerScope, SyntaxNode node,
         mssg_t byRefHandler);
      void compileByRefRedirectHandler(BuildTreeWriter& writer, MethodScope& invokerScope, SyntaxNode node,
         mssg_t byRefHandler);

      void compileDispatcherMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node,
         bool withGenerics, bool withOpenArgGenerics);
      void compileInitializerMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode classNode);
      void compileStaticInitializerMethod(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode classNode);
      void compileClosureMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node);
      void compileIteratorMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node);
      void compileExpressionMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node);
      void compileAbstractMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node, bool abstractMode);
      void compileMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node);
      void compileYieldMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node);
      void compileAsyncMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node);
      void compileConstructor(BuildTreeWriter& writer, MethodScope& scope, ClassScope& classClassScope,
         SyntaxNode node, bool abstractMode);
      void compileCustomDispatcher(BuildTreeWriter& writer, ClassScope& scope);
      void compileNestedClass(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node, ref_t parentRef);
      void compileStatemachineClass(BuildTreeWriter& writer, StatemachineClassScope& scope, SyntaxNode node, ref_t parentRef);

      void compileVMT(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node,
         bool exclusiveMode = false, bool ignoreAutoMultimethod = false);
      void compileClassVMT(BuildTreeWriter& writer, ClassScope& classClassScope, ClassScope& scope, SyntaxNode node);

      void compileSymbol(BuildTreeWriter& writer, SymbolScope& scope, SyntaxNode node);
      void compileClassSymbol(BuildTreeWriter& writer, ClassScope& scope);
      void compileClass(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node);
      void compileClassClass(BuildTreeWriter& writer, ClassScope& classClassScope, ClassScope& scope, SyntaxNode node);

      void compileNamespace(BuildTreeWriter& writer, NamespaceScope& ns, SyntaxNode node);

      static SyntaxNode addStaticInitializerMethod(ClassScope& scope, SyntaxNode node);
      ref_t compileStaticAssigning(ClassScope& scope, SyntaxNode node);

      void recreateFieldType(ClassScope& scope, SyntaxNode node, ustr_t fieldName);
      void validateClassFields(ClassScope& scope, SyntaxNode node);

      void validateScope(ModuleScopeBase* moduleScope);
      void validateSuperClass(ClassScope& scope, SyntaxNode node);
      void validateType(Scope& scope, ref_t typeRef, SyntaxNode node, bool ignoreUndeclared);

      void injectVirtualCode(SyntaxNode classNode, ClassScope& scope, bool interfaceBased);
      void injectVirtualMultimethod(SyntaxNode classNode, SyntaxKey methodType, Scope& scope,
         ref_t targetRef, ClassInfo& info, mssg_t multiMethod, int nillableArgs);

      void injectVirtualMethods(SyntaxNode classNode, SyntaxKey methodType, Scope& scope,
         ref_t targetRef, ClassInfo& info, VirtualMethodList& implicitMultimethods);
      void declareVirtualMethods(SyntaxNode classNode, SyntaxKey methodType, Scope& scope,
         ref_t targetRef, ClassInfo& info, VirtualMethodList& implicitMultimethods);

      void injectInitializer(SyntaxNode classNode, SyntaxKey methodType, mssg_t message);

      bool injectVirtualStrongTypedMultimethod(SyntaxNode classNode, SyntaxKey methodType, Scope& scope,
         mssg_t message, mssg_t resendMessage, TypeInfo outputInfo, Visibility visibility, bool isExtension, int nillableArgs, bool isSealed);
      bool injectVirtualStrongTypedVariadicMultimethod(SyntaxNode classNode, SyntaxKey methodType, ModuleScopeBase& scope,
         mssg_t message, mssg_t resendMessage, ref_t outputRef, Visibility visibility, bool isExtension);

      void injectVirtualMultimethod(SyntaxNode classNode, SyntaxKey methodType, Scope& scope,
         ref_t targetRef, ClassInfo& classInfo, mssg_t message, bool inherited, TypeInfo outputInfo, Visibility visibility, int nillableArgs);
      void injectVirtualMultimethod(SyntaxNode classNode, SyntaxKey methodType, Scope& scope, mssg_t message,
         mssg_t resendMessage, ref_t resendTarget, TypeInfo outputInfo, Visibility visibility, bool isExtension, bool isSealed);

      void injectVirtualTryDispatch(SyntaxNode classNode, SyntaxKey methodType, ClassInfo& info,
         mssg_t message, mssg_t dispatchMessage, bool inherited);
      void injectVirtualTryDispatch(SyntaxNode classNode, SyntaxKey methodType, mssg_t message, mssg_t dispatchMessage, ref_t resendTarget);

      void injectDefaultConstructor(ClassScope& scope, SyntaxNode node,
         bool protectedOne, bool withClearOption);

      void addVariableInfo(BuildNode node, Scope& codeScope, ustr_t name, Parameter& parameter);
      void injectVariablesInfo(BuildNode node, CodeScope& codeScope);

      void injectInterfaceDispatch(Scope& scope, SyntaxNode node, ref_t parentRef);

      void injectVirtualDispatchMethod(Scope& scope, SyntaxNode classNode, mssg_t message, ref_t outputRef, SyntaxKey key, ustr_t arg);
      void injectMethodInvoker(Scope& scope, SyntaxNode classNode, mssg_t message, SyntaxKey targetKey, ustr_t targetArg);

      void injectStrongRedirectMethod(Scope& scope, SyntaxNode node, SyntaxKey methodType, ref_t reference, mssg_t message,
         mssg_t redirectMessage, TypeInfo outputInfo);

      void callInitMethod(Expression& expression, SyntaxNode node, ClassInfo& info, ref_t reference);

      void generateOverloadListMember(ModuleScopeBase& scope, ref_t listRef, ref_t classRef,
         mssg_t messageRef, MethodHint type) override;

      void createPackageInfo(ModuleScopeBase* moduleScope, ManifestInfo& manifestInfo);

   public:
      void setOptimizationMode(int optMode)
      {
         _optMode = optMode != 0;
         _tapeOptMode = optMode > optLow;
      }

      void setMethodParamInfo(bool flag)
      {
         _withMethodParamInfo = flag;
      }

      void setConditionalBoxing(bool flag)
      {
         _withConditionalBoxing = flag;
      }

      void setEvaluateOp(bool flag)
      {
         _evaluateOp = flag;
      }

      bool checkStrictTypeFlag()
      {
         return _strictTypeEnforcing;
      }
      void setStrictTypeFlag(bool flag)
      {
         _strictTypeEnforcing = flag;
      }

      void setVerboseOn()
      {
         _verbose = true;
      }

      void setNoValidation()
      {
         _noValidation = true;
      }

      void setDebugMode(bool debugMode)
      {
         _withDebugInfo = debugMode;
      }

      void prepare(ModuleScopeBase* moduleScope, ForwardResolverBase* forwardResolver,
         ManifestInfo& manifestInfo);
      bool declare(ModuleScopeBase* moduleScope, SyntaxTree& input, ExtensionMap* outerExtensionList);
      void compile(ModuleScopeBase* moduleScope, SyntaxTree& input, BuildTree& output, ExtensionMap* outerExtensionList);

      void injectVirtualReturningMethod(Scope& scope, SyntaxNode classNode,
         mssg_t message, ustr_t retVar, TypeInfo outputTypeInfo);

      ref_t resolvePrimitiveType(ModuleScopeBase& moduleScope, TypeInfo typeInfo,
         bool declarationMode = false) override;

      ref_t generateExtensionTemplate(ModuleScopeBase& scope, ref_t templateRef, size_t argumentLen, ref_t* arguments,
         ustr_t ns, ExtensionMap* outerExtensionList) override;

      Compiler(
         PresenterBase* presenter,
         ErrorProcessor* errorProcessor,
         TemplateProssesorBase* templateProcessor,
         CompilerLogic* compilerLogic);
   };
}

#endif
