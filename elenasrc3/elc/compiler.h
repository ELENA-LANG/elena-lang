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
      StaticConstField,
      ClassStaticConstField,
      Wrapper,
      ClosureInfo,
      MemberInfo,
      LocalField,
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
      BoxingPtr,
      Conditional,
      ConditionalUnboxingRequired,
      Weak,
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

         virtual bool resolveAutoType(ObjectInfo& info, TypeInfo typeInfo)
         {
            if (parent) {
               return parent->resolveAutoType(info, typeInfo);
            }
            else return false;
         }

         virtual bool resolveAutoOutput(ref_t reference)
         {
            if (parent) {
               return parent->resolveAutoOutput(reference);
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
               this->compilerLogic = compilerLogic;
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
         bool         isStatic;

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
         
         ObjectInfo mapIdentifier(ustr_t identifier, bool referenceOne, ExpressionAttribute attr) override;

         ObjectInfo mapDictionary(ustr_t identifier, bool referenceOne, ExpressionAttribute mode) override;

         void save();

         ClassScope(Scope* parent, ref_t reference, Visibility visibility);
      };

      class ClassClassScope : public ClassScope
      {
         // the reference to the proper class info (used for resolving class constants)
         ClassInfo* classInfo;

      public:
         ObjectInfo mapField(ustr_t identifier, ExpressionAttribute attr) override;

         ClassClassScope(Scope* parent, ref_t reference, Visibility visibility, ClassInfo* classInfo);
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
         ObjectInfo mapSelf(bool memberMode = true);
         ObjectInfo mapSuper();

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
         static bool checkType(MethodInfo& methodInfo, MethodHint type);

         bool isGeneric()
         {
            return checkHint(MethodHint::Generic);
         }
         bool isYieldable()
         {
            return checkHint(MethodHint::Yieldable);
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

         bool resolveAutoOutput(ref_t reference) override
         {
            if (info.outputRef == V_AUTO) {
               if (!reference)
                  reference = moduleScope->buildins.superReference;

               info.outputRef = reference;

               return true;
            }
            else return Scope::resolveAutoOutput(reference);
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

         ref_t getOutputRef()
         {
            MethodScope* scope = Scope::getScope<MethodScope>(*this, ScopeLevel::Method);

            return scope ? scope->info.outputRef : 0;
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

         bool resolveAutoType(ObjectInfo& info, TypeInfo typeInfo) override;

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

         ObjectInfo mapSelf()
         {
            MethodScope* scope = Scope::getScope<MethodScope>(*this, ScopeLevel::Method);
            if (scope) {
               return scope->mapSelf(!scope->isExtension);
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

      struct MessageResolution
      {
         bool   resolved;
         mssg_t message;
         ref_t  extensionRef;
         int    stackSafeAttr;

         MessageResolution()
         {
            this->resolved = false;
            this->message = 0;
            this->extensionRef = 0;
            this->stackSafeAttr = 0;
         }
         MessageResolution(bool resolved, mssg_t message)
         {
            this->resolved = resolved;
            this->message = message;
            this->extensionRef = 0;
            this->stackSafeAttr = 0;
         }
         MessageResolution(mssg_t message)
         {
            this->resolved = false;
            this->message = message;
            this->extensionRef = 0;
            this->stackSafeAttr = 0;
         }
      };

      struct WriterContext
      {
         BuildTreeWriter* writer;
         ExprScope*       scope;
         SyntaxNode       node;
      };

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

      void showContextInfo(ExprScope& scope, mssg_t message, ref_t targetRef);

      void loadMetaData(ModuleScopeBase* moduleScope, ForwardResolverBase* forwardResolver, ustr_t name);

      void importExtensions(NamespaceScope& ns, ustr_t importedNs);
      void loadExtensions(NamespaceScope& ns, bool internalOne);

      void saveFrameAttributes(BuildTreeWriter& writer, Scope& scope, pos_t reserved, pos_t reservedN);

      pos_t saveMetaInfo(ModuleBase* module, ustr_t value, ustr_t postfix);

      ref_t mapNewTerminal(Scope& scope, ustr_t prefix, SyntaxNode nameNode, ustr_t postfix, Visibility visibility);
      mssg_t mapMethodName(MethodScope& scope, pos_t paramCount, ustr_t actionName, ref_t actionRef,
         ref_t flags, ref_t* signature, size_t signatureLen, bool withoutWeakMessages, bool noSignature);
      mssg_t mapMessage(Scope& scope, SyntaxNode node, bool propertyMode, bool extensionMode, bool probeMode);

      ExternalInfo mapExternal(Scope& scope, SyntaxNode node);
      ObjectInfo mapClassSymbol(Scope& scope, ref_t classRef);
      ObjectInfo mapConstructorTarget(MethodScope& scope);

      ref_t mapNested(ExprScope& ownerScope, ExpressionAttribute mode);
      ref_t mapConstantReference(Scope& scope);

      ref_t mapTemplateType(Scope& scope, SyntaxNode terminal, pos_t parameterCount);

      ref_t mapExtension(BuildTreeWriter& writer, Scope& scope, mssg_t& resolvedMessage, ref_t& implicitSignatureRef,
         ObjectInfo object, int& stackSafeAttr);
         
      mssg_t defineMultimethod(Scope& scope, mssg_t messageRef, bool extensionMode);

      void declareTemplateAttributes(Scope& scope, SyntaxNode node, TemplateTypeList& parameters, 
         TypeAttributes& attributes, bool declarationMode, bool objectMode);

      int defineFieldSize(Scope& scope, ObjectInfo info);

      ObjectInfo defineArrayType(Scope& scope, ObjectInfo info, bool declarationMode);
      ref_t defineArrayType(Scope& scope, ref_t elementRef, bool declarationMode);

      ref_t retrieveStrongType(Scope& scope, ObjectInfo info);
      ref_t retrieveType(Scope& scope, ObjectInfo info);
      ref_t resolvePrimitiveType(Scope& scope, TypeInfo typeInfo, bool declarationMode);
      ref_t resolveTypeIdentifier(Scope& scope, ustr_t identifier, SyntaxKey type, 
         bool declarationMode, bool allowRole);
      ref_t resolveTypeTemplate(Scope& scope, SyntaxNode node,
         TypeAttributes& attributes, bool declarationMode, bool objectMode = false);

      ref_t resolveTemplate(ModuleScopeBase& moduleScope, ustr_t ns, ref_t templateRef, ref_t elementRef, bool declarationMode);
      ref_t resolveClosure(Scope& scope, mssg_t closureMessage, ref_t outputRef);
      ref_t resolveWrapperTemplate(ModuleScopeBase& moduleScope, ustr_t ns, ref_t elementRef, bool declarationMode);
      ref_t resolveArrayTemplate(ModuleScopeBase& moduleScope, ustr_t ns, ref_t elementRef, bool declarationMode);
      ref_t resolveNullableTemplate(ModuleScopeBase& moduleScope, ustr_t ns, ref_t elementRef, bool declarationMode);
      ref_t resolveArgArrayTemplate(ModuleScopeBase& moduleScope, ustr_t ns, ref_t elementRef, bool declarationMode);
      ref_t resolveTupleClass(Scope& scope, SyntaxNode node, ArgumentsInfo& items);

      int resolveSize(Scope& scope, SyntaxNode node);
      TypeInfo resolveTypeAttribute(Scope& scope, SyntaxNode node, TypeAttributes& attributes,
         bool declarationMode, bool allowRole);
      TypeInfo resolveTypeScope(Scope& scope, SyntaxNode node, TypeAttributes& attributes,
         bool declarationMode, bool allowRole);
      ref_t resolveStrongTypeAttribute(Scope& scope, SyntaxNode node, bool declarationMode, bool allowRole);

      bool resolveAutoType(ExprScope& scope, ObjectInfo source, ObjectInfo& target);

      ref_t retrieveTemplate(NamespaceScope& scope, SyntaxNode node, List<SyntaxNode>& parameters, 
         ustr_t prefix, SyntaxKey argKey, ustr_t postFix);

      MessageResolution resolveByRefHandler(BuildTreeWriter& writer, ObjectInfo source, ExprScope& scope, ref_t expectedRef,
         mssg_t weakMessage, ref_t& signatureRef, bool noExtensions);
      MessageResolution resolveMessageAtCompileTime(BuildTreeWriter& writer, ObjectInfo target, ExprScope& scope, 
         mssg_t weakMessage, ref_t implicitSignatureRef, bool ignoreExtensions, bool ignoreVariadics);
      mssg_t resolveOperatorMessage(ModuleScopeBase* scope, int operatorId);
      mssg_t resolveVariadicMessage(Scope& scope, mssg_t message);

      bool isDefaultOrConversionConstructor(Scope& scope, mssg_t message, bool internalOne, bool& isProtectedDefConst);

      bool importTemplate(Scope& scope, SyntaxNode node, SyntaxNode target, bool weakOne);
      bool importInlineTemplate(Scope& scope, SyntaxNode node, ustr_t postfix, SyntaxNode target);
      bool importPropertyTemplate(Scope& scope, SyntaxNode node, ustr_t postfix, SyntaxNode target);
      void importCode(Scope& scope, SyntaxNode node, SyntaxNode& importNode);

      void readFieldAttributes(ClassScope& scope, SyntaxNode node, FieldAttributes& attrs, bool declarationMode);

      int allocateLocalAddress(Scope& scope, int size, bool binaryArray);

      ObjectInfo allocateResult(ExprScope& scope, ref_t resultRef);

      ref_t declareMultiType(Scope& scope, SyntaxNode& node, ref_t elementRef);

      void declareTemplateAttributes(TemplateScope& scope, SyntaxNode node, IdentifierString& postfix);
      void declareSymbolAttributes(SymbolScope& scope, SyntaxNode node, bool identifierDeclarationMode);
      void declareClassAttributes(ClassScope& scope, SyntaxNode node, ref_t& fldeclaredFlagsags);
      void declareFieldAttributes(ClassScope& scope, SyntaxNode node, FieldAttributes& mode);
      void declareMethodAttributes(MethodScope& scope, SyntaxNode node, bool exensionMode, bool templateBased);
      void declareArgumentAttributes(MethodScope& scope, SyntaxNode node, TypeInfo& typeInfo, bool declarationMode);
      void declareDictionaryAttributes(Scope& scope, SyntaxNode node, TypeInfo& typeInfo, bool& superMode);
      void declareExpressionAttributes(Scope& scope, SyntaxNode node, TypeInfo& typeInfo, ExpressionAttributes& mode);

      void declareDictionary(Scope& scope, SyntaxNode node, Visibility visibility, Scope::ScopeLevel level);

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

      ref_t generateConstant(Scope& scope, ObjectInfo& info, ref_t reference, bool saveScope = true);

      mssg_t defineByRefMethod(ClassScope& scope, SyntaxNode node, bool isExtension);

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
      void generateClassField(ClassScope& scope, SyntaxNode node, FieldAttributes& attrs, bool singleField);
      void generateClassStaticField(ClassScope& scope, SyntaxNode node, FieldAttributes& attrs);
      void generateClassFields(ClassScope& scope, SyntaxNode node, bool singleField);
      void generateClassDeclaration(ClassScope& scope, SyntaxNode node, ref_t declaredFlags);

      bool declareVariable(Scope& scope, SyntaxNode terminal, TypeInfo typeInfo, bool ignoreDuplicate);
      bool declareYieldVariable(Scope& scope, ustr_t name, TypeInfo typeInfo);

      ObjectInfo declareTempStructure(ExprScope& scope, SizeInfo sizeInfo);

      void declareClassParent(ref_t parentRef, ClassScope& scope, SyntaxNode node);
      void resolveClassPostfixes(ClassScope& scope, SyntaxNode node, bool extensionMode);

      int resolveArraySize(Scope& scope, SyntaxNode node);

      void declareParameter(MethodScope& scope, SyntaxNode node, bool withoutWeakMessages, 
         bool declarationMode, bool& variadicMode, bool& weakSignature, bool& noSignature,
         pos_t& paramCount, ref_t* signature, size_t& signatureLen);

      ref_t declareClosureParameters(MethodScope& methodScope, SyntaxNode argNode);

      void declareVMTMessage(MethodScope& scope, SyntaxNode node, bool withoutWeakMessages, bool declarationMode);
      void declareClosureMessage(MethodScope& scope, SyntaxNode node);

      void initializeMethod(ClassScope& scope, MethodScope& methodScope, SyntaxNode current);

      void declareSymbolMetaInfo(SymbolScope& scope, SyntaxNode node);

      void declareMetaInfo(Scope& scope, SyntaxNode node);
      void declareMethodMetaInfo(MethodScope& scope, SyntaxNode node);
      void declareMethod(MethodScope& scope, SyntaxNode node, bool abstractMode, bool staticNotAllowed);
      void declareVMT(ClassScope& scope, SyntaxNode node, bool& withConstructors, bool& withDefaultConstructor, 
         bool& yieldMethodNotAllowed, bool staticNotAllowed, bool templateBased);

      void declareSymbol(SymbolScope& scope, SyntaxNode node);
      void declareClassClass(ClassScope& classClassScope, SyntaxNode node, ref_t parentRef);
      void declareClass(ClassScope& scope, SyntaxNode node);

      void declareNamespace(NamespaceScope& ns, SyntaxNode node, bool ignoreImport = false, 
         bool ignoreExtensions = false);
      void declareMembers(NamespaceScope& ns, SyntaxNode node);
      void declareMemberIdentifiers(NamespaceScope& ns, SyntaxNode node);

      void copyParentNamespaceExtensions(NamespaceScope& source, NamespaceScope& target);

      void declareModuleIdentifiers(ModuleScopeBase* moduleScope, SyntaxNode node, ExtensionMap* outerExtensionList);
      void declareModule(ModuleScopeBase* moduleScope, SyntaxNode node, ExtensionMap* outerExtensionList);

      void inheritStaticMethods(ClassScope& scope, SyntaxNode classNode);

      void addExtensionMessage(Scope& scope, mssg_t message, ref_t extRef, mssg_t strongMessage, 
         bool internalOne);
      void addExtensionTemplateMessage(Scope& scope, mssg_t message, ustr_t pattern, bool internalOne);

      void declareExtension(ClassScope& scope, mssg_t message, bool internalOne);

      void declareModuleExtensionDispatcher(NamespaceScope& scope, SyntaxNode node);

      void warnOnUnassignedLocal(SyntaxNode node, CodeScope& scope, int level);

      ObjectInfo evalOperation(Interpreter& interpreter, Scope& scope, SyntaxNode node, ref_t operator_id, bool ignoreErrors = false);
      ObjectInfo evalExpression(Interpreter& interpreter, Scope& scope, SyntaxNode node, bool ignoreErrors = false, bool resolveMode = true);
      ObjectInfo evalObject(Interpreter& interpreter, Scope& scope, SyntaxNode node);
      ObjectInfo evalCollection(Interpreter& interpreter, Scope& scope, SyntaxNode node, bool anonymousOne, bool ignoreErrors);
      ObjectInfo evalPropertyOperation(Interpreter& interpreter, Scope& scope, SyntaxNode node, bool ignoreErrors);

      void evalStatement(MetaScope& scope, SyntaxNode node);

      void writeObjectInfo(WriterContext& context, ObjectInfo info);

      void addBreakpoint(BuildTreeWriter& writer, SyntaxNode node, BuildKey bpKey);

      bool evalInitializers(ClassScope& scope, SyntaxNode node);
      bool evalClassConstant(ustr_t constName, ClassScope& scope, SyntaxNode node, ObjectInfo& constInfo);
      bool evalAccumClassConstant(ustr_t constName, ClassScope& scope, SyntaxNode node, ObjectInfo& constInfo);

      ref_t compileExtensionDispatcher(BuildTreeWriter& writer, NamespaceScope& scope, mssg_t genericMessage, 
         ref_t outputRef);

      ref_t compileMessageArguments(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode current, 
         ArgumentsInfo& arguments, ref_t expectedSignRef, ExpressionAttribute mode, ArgumentsInfo* updatedOuterArgs, bool& variadicArgList);

      void writeParameterDebugInfo(BuildTreeWriter& writer, Scope& scope, int size, TypeInfo typeInfo, 
         ustr_t name, int index);
      void writeMethodDebugInfo(BuildTreeWriter& writer, MethodScope& scope);
      void writeMessageInfo(BuildTreeWriter& writer, MethodScope& scope);

      void compileInlineInitializing(BuildTreeWriter& writer, ClassScope& classScope, SyntaxNode node);

      void writeMessageArguments(WriterContext& context, ObjectInfo& target, 
         mssg_t message, ArgumentsInfo& arguments, ObjectInfo& lenLocal, int& stackSafeAttr,
         bool targetOverridden, bool found, bool argUnboxingRequired, bool stackSafe);

      bool validateShortCircle(ExprScope& scope, mssg_t message, ObjectInfo target);

      ObjectInfo boxArgumentInPlace(WriterContext& context, ObjectInfo info, ref_t targetRef = 0);
      ObjectInfo boxRefArgumentInPlace(WriterContext& context, ObjectInfo info, ref_t targetRef = 0);
      ObjectInfo boxArgument(WriterContext& context, ObjectInfo info, 
         bool stackSafe, bool boxInPlace, bool allowingRefArg, ref_t targetRef = 0);
      ObjectInfo boxArgumentLocally(WriterContext& context, ObjectInfo info, bool stackSafe, bool forced);
      ObjectInfo boxLocally(WriterContext& context, ObjectInfo info, bool stackSafe);
      ObjectInfo boxPtrLocally(WriterContext& context, ObjectInfo info);
      ObjectInfo boxVariadicArgument(WriterContext& context, ObjectInfo info);

      ObjectInfo unboxArguments(WriterContext& context, ObjectInfo retVal, ArgumentsInfo* updatedOuterArgs);
      void unboxArgumentLocaly(WriterContext& context, ObjectInfo tempLocal, ObjectKey targetKey);
      void unboxOuterArgs(WriterContext& context, ArgumentsInfo* updatedOuterArgs);

      ObjectInfo saveToTempLocal(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo object);
      ObjectInfo declareTempLocal(ExprScope& scope, ref_t typeRef, bool dynamicOnly = true);

      ObjectInfo typecastObject(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo source, ref_t targetRef);
      ObjectInfo convertObject(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo source, 
         ref_t targetRef, bool dynamicRequired, bool withoutBoxing);
      ObjectInfo convertIntLiteral(ExprScope& scope, SyntaxNode node, ObjectInfo source, ref_t targetRef, bool ignoreError = false);

      void convertIntLiteralForOperation(ExprScope& scope, SyntaxNode node, int operatorId, ArgumentsInfo& messageArguments);

      bool compileSymbolConstant(SymbolScope& scope, ObjectInfo retVal);

      ObjectInfo compileExternalOp(WriterContext& context, ref_t externalRef, bool stdCall, 
         ArgumentsInfo& arguments, ref_t expectedRef);

      ObjectInfo compileNewArrayOp(WriterContext& context, ObjectInfo source,
         ref_t targetRef, ArgumentsInfo& arguments);
      ObjectInfo compileNewOp(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
         ObjectInfo source, ref_t signRef, ArgumentsInfo& arguments);
      ObjectInfo compileNativeConversion(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo source, ref_t operationKey);

      ObjectInfo compileLookAheadExpression(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
         ref_t targetRef, ExpressionAttribute attrs);

      ObjectInfo compileMessageOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo target, 
         MessageResolution resolution, ref_t implicitSignatureRef, ArgumentsInfo& arguments, ExpressionAttributes mode, ArgumentsInfo* updatedOuterArgs);
      ObjectInfo compileMessageOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
         ref_t targetRef, ExpressionAttribute attrs);
      ObjectInfo compilePropertyOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
         ref_t targetRef, ExpressionAttribute attrs);

      bool compileAssigningOp(WriterContext& context, ObjectInfo target, ObjectInfo source);
      ObjectInfo compileAssigning(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode loperand, 
         SyntaxNode roperand, ExpressionAttribute mode);

      ObjectInfo compileMessageOperationR(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo target, 
         SyntaxNode node, bool propertyMode);

      ObjectInfo compileWeakOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ref_t* arguments, pos_t argLen,
         ObjectInfo& loperand, ArgumentsInfo& messageArguments, mssg_t message, ref_t expectedRef, ArgumentsInfo* updatedOuterArgs);

      ObjectInfo compileOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ArgumentsInfo& messageArguments,
         int operatorId, ref_t expectedRef, ArgumentsInfo* updatedOuterArgs);
      ObjectInfo compileOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode loperand, SyntaxNode roperand, 
         int operatorId, ref_t expectedRef);
      ObjectInfo compileAssignOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
         int operatorId, ref_t expectedRef);
      ObjectInfo compileBoolOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, int operatorId);
      ObjectInfo compileIndexerOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, int operatorId, ref_t expectedRef);
      ObjectInfo compileOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, int operatorId,
         ref_t expectedRef, ExpressionAttribute mode);
      ObjectInfo compileSpecialOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, int operatorId, ref_t expectedRef);
      ObjectInfo compileBranchingOperands(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode rnode,
         SyntaxNode r2node, bool retValExpected, bool withoutDebugInfo);
      ObjectInfo compileBranchingOperation(WriterContext& context, ObjectInfo loperand, SyntaxNode rnode,
         SyntaxNode r2node, int operatorId, ArgumentsInfo* updatedOuterArgs, bool retValExpected, bool withoutDebugInfo);
      ObjectInfo compileBranchingOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
         int operatorId, bool retValExpected, bool withoutDebugInfo);
      ObjectInfo compileCatchOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node);
      ObjectInfo compileFinalOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node);
      ObjectInfo compileAltOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node);
      ObjectInfo compileIsNilOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node);
      ObjectInfo compileTupleAssigning(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node);

      void compileSwitchOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node);
      void compileYieldOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node);

      ObjectInfo defineTerminalInfo(Scope& scope, SyntaxNode node, TypeInfo declaredTypeInfo, bool variableMode,
         bool forwardMode, bool refOp, bool mssgOp, bool memberMode, bool& invalid, ExpressionAttribute attrs);

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

      ObjectInfo validateObject(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo retVal,
         ref_t targetRef, bool noPrimitives, bool paramMode, bool dynamicRequired);

      ObjectInfo compileNested(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ExpressionAttribute mode,
         ArgumentsInfo* updatedOuterArgs);
      ObjectInfo compileClosure(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ExpressionAttribute mode,
         ArgumentsInfo* updatedOuterArgs);
      ObjectInfo compileObject(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
         ExpressionAttribute mode, ArgumentsInfo* updatedOuterArgs);
      ObjectInfo compileExpression(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
         ref_t targetRef, ExpressionAttribute mode, ArgumentsInfo* updatedOuterArgs);
      ObjectInfo compileLoopExpression(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
         ExpressionAttribute mode);
      ObjectInfo compileExternExpression(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
         ExpressionAttribute mode);
      ObjectInfo compileSubCode(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
         ExpressionAttribute mode, bool withoutNewScope = false);
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

      ObjectInfo compileResendCode(BuildTreeWriter& writer, CodeScope& codeScope, ObjectInfo source, SyntaxNode node);
      ObjectInfo compileRedirect(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node);
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
      mssg_t compileByRefHandler(BuildTreeWriter& writer, MethodScope& invokerScope, SyntaxNode node,
         mssg_t byRefHandler);
      void compileByRefRedirectHandler(BuildTreeWriter& writer, MethodScope& invokerScope, SyntaxNode node,
         mssg_t byRefHandler);

      void compileDispatcherMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node, 
         bool withGenerics, bool withOpenArgGenerics);
      void compileYieldInitializing(BuildTreeWriter& writer, CodeScope& scope, SyntaxNode node);
      void compileInitializerMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode classNode);
      void compileStaticInitializerMethod(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode classNode);
      void compileClosureMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node);
      void compileExpressionMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node);
      void compileAbstractMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node, bool abstractMode);
      void compileMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node);
      void compileConstructor(BuildTreeWriter& writer, MethodScope& scope, ClassScope& classClassScope, 
         SyntaxNode node, bool abstractMode);
      void compileCustomDispatcher(BuildTreeWriter& writer, ClassScope& scope);
      void compileNestedClass(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node, ref_t parentRef);
      void compileClosureClass(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node);
      void compileVMT(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node,
         bool exclusiveMode = false, bool ignoreAutoMultimethod = false);
      void compileClassVMT(BuildTreeWriter& writer, ClassScope& classClassScope, ClassScope& scope, SyntaxNode node);

      ObjectInfo compileCollection(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ExpressionAttribute mode);
      ObjectInfo compileTupleCollectiom(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ref_t targetRef);

      void compileSymbol(BuildTreeWriter& writer, SymbolScope& scope, SyntaxNode node);
      void compileClassSymbol(BuildTreeWriter& writer, ClassScope& scope);
      void compileClass(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node);
      void compileClassClass(BuildTreeWriter& writer, ClassScope& classClassScope, ClassScope& scope, SyntaxNode node);

      void compileNamespace(BuildTreeWriter& writer, NamespaceScope& ns, SyntaxNode node);

      ref_t compileStaticAssigning(ClassScope& scope, SyntaxNode node);

      void recreateFieldType(ClassScope& scope, SyntaxNode node, ustr_t fieldName);
      void validateClassFields(ClassScope& scope, SyntaxNode node);

      void validateScope(ModuleScopeBase* moduleScope);
      void validateSuperClass(ClassScope& scope, SyntaxNode node);
      void validateType(Scope& scope, ref_t typeRef, SyntaxNode node, bool ignoreUndeclared, bool allowRole);

      void injectVirtualCode(SyntaxNode classNode, ClassScope& scope, bool interfaceBased);
      void injectVirtualMultimethod(SyntaxNode classNode, SyntaxKey methodType, ModuleScopeBase& scope, 
         ref_t targetRef, ClassInfo& info, mssg_t multiMethod);
      void injectVirtualEmbeddableWrapper(SyntaxNode classNode, SyntaxKey methodType, ModuleScopeBase& scope,
         ref_t targetRef, ClassInfo& info, mssg_t multiMethod, bool abstractOne);

      void injectVirtualMethods(SyntaxNode classNode, SyntaxKey methodType, ModuleScopeBase& scope,
         ref_t targetRef, ClassInfo& info, VirtualMethodList& implicitMultimethods);

      void injectInitializer(SyntaxNode classNode, SyntaxKey methodType, mssg_t message);

      bool injectVirtualStrongTypedMultimethod(SyntaxNode classNode, SyntaxKey methodType, ModuleScopeBase& scope, 
         mssg_t message, mssg_t resendMessage, ref_t outputRef, Visibility visibility, bool isExtension);

      void injectVirtualMultimethod(SyntaxNode classNode, SyntaxKey methodType, ModuleScopeBase& scope, 
         ref_t targetRef, ClassInfo& classInfo, mssg_t message, bool inherited, ref_t outputRef, Visibility visibility);
      void injectVirtualMultimethod(SyntaxNode classNode, SyntaxKey methodType, mssg_t message, 
         mssg_t resendMessage, ref_t resendTarget, ref_t outputRef, Visibility visibility, bool isExtension);

      void injectVirtualTryDispatch(SyntaxNode classNode, SyntaxKey methodType, ClassInfo& info, 
         mssg_t message, mssg_t dispatchMessage, bool inherited);
      void injectVirtualTryDispatch(SyntaxNode classNode, SyntaxKey methodType, mssg_t message, mssg_t dispatchMessage, ref_t resendTarget);

      void injectDefaultConstructor(ClassScope& scope, SyntaxNode node, 
         bool protectedOne, bool withClearOption);

      void injectVariableInfo(BuildNode node, CodeScope& codeScope);

      void injectInterfaceDispatch(Scope& scope, SyntaxNode node, ref_t parentRef);

      void injectVirtualDispatchMethod(Scope& scope, SyntaxNode classNode, mssg_t message, ref_t outputRef, SyntaxKey key, ustr_t arg);

      void injectStrongRedirectMethod(SyntaxNode node, SyntaxKey methodType, ref_t reference, mssg_t message, mssg_t redirectMessage, ref_t outputRef);

      void callInitMethod(BuildTreeWriter& writer, SyntaxNode node, ExprScope& exprScope, ClassInfo& info, ref_t reference);

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

      void setVerboseOn()
      {
         _verbose = true;
      }

      void prepare(ModuleScopeBase* moduleScope, ForwardResolverBase* forwardResolver,
         ManifestInfo& manifestInfo);
      void declare(ModuleScopeBase* moduleScope, SyntaxTree& input, ExtensionMap* outerExtensionList);
      void compile(ModuleScopeBase* moduleScope, SyntaxTree& input, BuildTree& output, ExtensionMap* outerExtensionList);

      void injectVirtualReturningMethod(ModuleScopeBase* scope, SyntaxNode classNode,
         mssg_t message, ustr_t retVar, ref_t classRef) override;

      ref_t resolvePrimitiveType(ModuleScopeBase& moduleScope, ustr_t ns, TypeInfo typeInfo, 
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