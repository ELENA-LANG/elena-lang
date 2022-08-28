//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class.
//
//                                             (C)2021-2022, by Aleksey Rakov
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
      TypeDictionary,
      TypeList,

      MetaConstant,
      StringLiteral,
      CharacterLiteral,
      IntLiteral,
      MssgNameLiteral,
      MssgLiteral,
      Template,
      Nil,
      Symbol,
      Class,
      Object,
      Singleton,
      InternalProcedure,
      Param,
      ParamField,
      Local,
      TempLocal,
      SelfLocal,
      ReadOnlySelfLocal,
      LocalAddress,
      TempLocalAddress,
      ParamBoxable,
      ParamFieldBoxable,
      SelfLocalBoxable,
      Extern,
      NewVariable,
      ReadOnlyFieldAddress,
      FieldAddress,
      ReadOnlyField,
      Field,
      Closure,
      Extension,
      ConstantRole,
      ClassConstant,
      SelfName,
      StaticField,
      StaticConstField,
      Wrapper,
   };

   enum TargetMode
   {
      None,
      Probe,
      External,
      WinApi,
      CreatingArray,
      Creating,
      Casting
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
      ObjectInfo(ObjectKind kind, TypeInfo typeInfo, ref_t reference, int extra)
      {
         this->kind = kind;
         this->typeInfo = typeInfo;
         this->reference = reference;
         this->extra = extra;
         mode = TargetMode::None;
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

   typedef Pair<ObjectKind, ref_t, ObjectKind::Unknown, 0>                                  ObjectKey;
   typedef MemoryMap<ObjectKey, ObjectInfo, Map_StoreKey<ObjectKey>, Map_GetKey<ObjectKey>> ObjectKeys;

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

   // --- Interpreter ---
   class Interpreter
   {
      ModuleScopeBase* _scope;
      CompilerLogic*   _logic;

      ObjectInfo mapStringConstant(ustr_t s);

      void setAttributeMapValue(ref_t dictionaryRef, ustr_t key, int value);
      void setTypeMapValue(ref_t dictionaryRef, ustr_t key, ref_t reference);

      //void setDeclDictionaryValue(ref_t dictionaryRef, ustr_t key, ref_t reference);
      void addTypeListItem(ref_t dictionaryRef, ref_t symbolRef);

      bool evalDictionaryOp(ref_t operator_id, ArgumentsInfo& args);

      //bool evalDeclDictionaryOp(ref_t operator_id, ArgumentsInfo& args);
      bool evalObjArrayOp(ref_t operator_id, ArgumentsInfo& args);
      bool evalDeclOp(ref_t operator_id, ArgumentsInfo& args, ObjectInfo& retVal);

   public:
      bool eval(BuildKey key, ref_t operator_id, ArgumentsInfo& args, ObjectInfo& retVal);

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
         ReferenceName    nsName;
         IdentifierString sourcePath;

         // forward declarations
         ForwardMap       forwards;
         // imported namespaces
         IdentifierList   importedNs;
         // extensions
         ExtensionMap     extensions;
         ResolvedMap      extensionTargets;
         ResolvedMap      extensionDispatchers;
//         ExtensionMap     declaredExtensions;

         Visibility       defaultVisibility;

         ErrorProcessor*  errorProcessor;

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

         ref_t resolveImplicitIdentifier(ustr_t name, bool referenceOne, bool innnerMost);

         ref_t mapNewIdentifier(ustr_t identifier, Visibility visibility) override;

         ObjectInfo mapIdentifier(ustr_t identifier, bool referenceOne, ExpressionAttribute mode) override;
         ObjectInfo mapGlobal(ustr_t identifier, ExpressionAttribute mode);
         ObjectInfo mapWeakReference(ustr_t identifier, bool directResolved);

         NamespaceScope(ModuleScopeBase* moduleScope, ErrorProcessor* errorProcessor, CompilerLogic* compilerLogic) :
            Scope(nullptr),
            forwards(0),
            importedNs(nullptr),
            extensions({}),
            extensionTargets(INVALID_REF),
            extensionDispatchers(INVALID_REF)
            //declaredExtensions({})
         {
            this->moduleScope = moduleScope;
            this->module = moduleScope->module;
            // by default - private visibility
            this->defaultVisibility = Visibility::Private;
            this->errorProcessor = errorProcessor;
            this->compilerLogic = compilerLogic;
         }
         NamespaceScope(NamespaceScope* parent);
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

         ObjectInfo mapField(ustr_t identifier, ExpressionAttribute attr);

         ObjectInfo mapIdentifier(ustr_t identifier, bool referenceOne, ExpressionAttribute attr) override;

         void save();

         ClassScope(Scope* parent, ref_t reference, Visibility visibility);
      };

      struct MethodScope : Scope
      {
         mssg_t       message;
         LocalMap     parameters;

         int          selfLocal;

         MethodInfo   info;
         pos_t        reserved1;             // defines managed frame size
         pos_t        reserved2;             // defines unmanaged frame size (excluded from GC frame chain)
         pos_t        reservedArgs;          // contains the maximal argument list

         bool         functionMode;
         bool         closureMode;
         bool         constructorMode;
         bool         isEmbeddable;

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
         ObjectInfo mapSelf();

         bool isPrivate() const
         {
            return test(message, STATIC_MESSAGE);
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

         ref_t getClassRef(bool ownerClass = true)
         {
            ClassScope* scope = Scope::getScope<ClassScope>(*this, ownerClass ? ScopeLevel::OwnerClass : ScopeLevel::Class);

            return scope ? scope->reference : 0;
         }

         bool resolveAutoOutput(ref_t reference) override
         {
            if (info.outputRef == V_AUTO) {
               info.outputRef = reference;

               return true;
            }
            else return Scope::resolveAutoOutput(reference);
         }

         MethodScope(ClassScope* classScope);
      };

      struct CodeScope : Scope
      {
         // scope local variables
         LocalMap locals;

         pos_t    allocated1, reserved1;       // defines managed frame size
         pos_t    allocated2, reserved2;       // defines unmanaged frame size

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

         bool resolveAutoType(ObjectInfo& info, TypeInfo typeInfo) override;

         void markAsAssigned(ObjectInfo object) override;

         ObjectInfo mapIdentifier(ustr_t identifier, bool referenceOne, ExpressionAttribute attr) override;

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
         ObjectInfo mapDecl();

         ObjectInfo mapIdentifier(ustr_t identifier, bool referenceOne, ExpressionAttribute attr) override;

         MetaScope(Scope* parent);
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
               return scope->mapSelf();
            }
            else return {};
         }

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
         Scope* getScope(ScopeLevel level) override
         {
            if (level == ScopeLevel::Class) {
               return this;
            }
            else return Scope::getScope(level);
         }

         InlineClassScope(ExprScope* owner, ref_t reference);
      };

   private:
      CompilerLogic*         _logic;
      TemplateProssesorBase* _templateProcessor;
      ErrorProcessor*        _errorProcessor;

      bool                   _optMode;

      bool reloadMetaData(ModuleScopeBase* moduleScope, ustr_t name);

      void importExtensions(NamespaceScope& ns, ustr_t importedNs);
      void loadExtensions(NamespaceScope& ns, bool internalOne);

      void saveFrameAttributes(BuildTreeWriter& writer, Scope& scope, pos_t reserved, pos_t reservedN);

      ref_t mapNewTerminal(Scope& scope, ustr_t prefix, SyntaxNode nameNode, ustr_t postfix, Visibility visibility);
      mssg_t mapMethodName(Scope& scope, pos_t paramCount, ustr_t actionName, ref_t actionRef, 
         ref_t flags, ref_t* signature, size_t signatureLen);
      mssg_t mapMessage(ExprScope& scope, SyntaxNode node, bool propertyMode, bool extensionMode, bool probeMode);

      ExternalInfo mapExternal(Scope& scope, SyntaxNode node);
      ObjectInfo mapClassSymbol(Scope& scope, ref_t classRef);

      ref_t mapNested(ExprScope& ownerScope, ExpressionAttribute mode);

      ref_t mapTemplateType(Scope& scope, SyntaxNode node);

      ref_t mapExtension(BuildTreeWriter& writer, Scope& scope, mssg_t& resolvedMessage, ref_t implicitSignatureRef,
         ObjectInfo object);

      mssg_t defineMultimethod(ClassScope& scope, mssg_t messageRef);

      void declareTemplateAttributes(Scope& scope, SyntaxNode node, List<SyntaxNode>& parameters, 
         bool declarationMode);

      ObjectInfo defineArrayType(Scope& scope, ObjectInfo info);
      ref_t defineArrayType(Scope& scope, ref_t elementRef);

      ref_t retrieveStrongType(Scope& scope, ObjectInfo info);
      ref_t retrieveType(Scope& scope, ObjectInfo info);
      ref_t resolvePrimitiveType(Scope& scope, TypeInfo typeInfo, bool declarationMode);
      ref_t resolveTypeIdentifier(Scope& scope, ustr_t identifier, SyntaxKey type, 
         bool declarationMode);
      ref_t resolveTypeTemplate(Scope& scope, SyntaxNode node, bool declarationMode);

      ref_t resolveTemplate(Scope& scope, ref_t templateRef, ref_t elementRef, bool declarationMode);
      ref_t resolveClosure(Scope& scope, mssg_t closureMessage, ref_t outputRef);
      ref_t resolveWrapperTemplate(Scope& scope, ref_t elementRef, bool declarationMode);
      ref_t resolveArrayTemplate(Scope& scope, ref_t elementRef, bool declarationMode);

      int resolveSize(Scope& scope, SyntaxNode node);
      TypeInfo resolveTypeAttribute(Scope& scope, SyntaxNode node, bool declarationMode);
      ref_t resolveStrongTypeAttribute(Scope& scope, SyntaxNode node, bool declarationMode);

      bool resolveAutoType(ExprScope& scope, ObjectInfo source, ObjectInfo& target);

      ref_t retrieveTemplate(NamespaceScope& scope, SyntaxNode node, List<SyntaxNode>& parameters, ustr_t prefix); 

      mssg_t resolveMessageAtCompileTime(BuildTreeWriter& writer, ObjectInfo target, ExprScope& scope, mssg_t weakMessage,
         ref_t implicitSignatureRef, bool ignoreExtensions, ref_t& resolvedExtensionRef, int& stackSafeAttr);
      mssg_t resolveOperatorMessage(ModuleScopeBase* scope, int operatorId);

      bool isDefaultOrConversionConstructor(Scope& scope, mssg_t message/*, bool& isProtectedDefConst*/);

      void importTemplate(Scope& scope, SyntaxNode node, ustr_t prefix, SyntaxNode target);
      void importCode(Scope& scope, SyntaxNode node, SyntaxNode& importNode);

      void readFieldAttributes(ClassScope& scope, SyntaxNode node, FieldAttributes& attrs);

      int allocateLocalAddress(CodeScope* codeScope, int size, bool binaryArray);

      ObjectInfo allocateResult(ExprScope& scope, ref_t resultRef);

      void declareTemplateAttributes(TemplateScope& scope, SyntaxNode node, IdentifierString& postfix);
      void declareSymbolAttributes(SymbolScope& scope, SyntaxNode node);
      void declareClassAttributes(ClassScope& scope, SyntaxNode node, ref_t& fldeclaredFlagsags);
      void declareFieldAttributes(ClassScope& scope, SyntaxNode node, FieldAttributes& mode);
      void declareMethodAttributes(MethodScope& scope, SyntaxNode node, bool exensionMode);
      void declareArgumentAttributes(MethodScope& scope, SyntaxNode node, TypeInfo& typeInfo, bool declarationMode);
      void declareDictionaryAttributes(Scope& scope, SyntaxNode node, TypeInfo& typeInfo);
      void declareExpressionAttributes(Scope& scope, SyntaxNode node, TypeInfo& typeInfo, ExpressionAttributes& mode);

      void declareDictionary(Scope& scope, SyntaxNode node, Visibility visibility);

      void saveTemplate(TemplateScope& scope, SyntaxNode& node);
      void saveNamespaceInfo(SyntaxNode node, NamespaceScope* nsScope, bool outerMost);

      void declareTemplate(TemplateScope& scope, SyntaxNode& node);

      void declareTemplateClass(TemplateScope& scope, SyntaxNode& node);
      void declareTemplateCode(TemplateScope& scope, SyntaxNode& node);

      InheritResult inheritClass(ClassScope& scope, ref_t parentRef/*, bool ignoreFields, bool ignoreSealed*/);

      void checkMethodDuplicates(ClassScope& scope, SyntaxNode node, mssg_t message, 
         mssg_t publicMessage, bool protectedOne, bool internalOne);

      ref_t generateConstant(Scope& scope, ObjectInfo& info, ref_t reference);

      void generateClassFlags(ClassScope& scope, ref_t declaredFlags);
      void generateMethodAttributes(ClassScope& scope, SyntaxNode node, 
         MethodInfo& methodInfo, bool abstractBased);
      void generateMethodDeclaration(ClassScope& scope, SyntaxNode node, bool closed);
      void generateMethodDeclarations(ClassScope& scope, SyntaxNode node, SyntaxKey methodKey, bool closed);
      void generateClassField(ClassScope& scope, SyntaxNode node, FieldAttributes& attrs, bool singleField);
      void generateClassStaticField(ClassScope& scope, SyntaxNode node, bool isConst, TypeInfo typeInfo);
      void generateClassFields(ClassScope& scope, SyntaxNode node, bool singleField);
      void generateClassDeclaration(ClassScope& scope, SyntaxNode node, ref_t declaredFlags);

      void declareVariable(Scope& scope, SyntaxNode terminal, TypeInfo typeInfo);

      ObjectInfo declareTempStructure(ExprScope& scope, int size);

      void declareClassParent(ref_t parentRef, ClassScope& scope, SyntaxNode node);
      void resolveClassParent(ClassScope& scope, SyntaxNode node, bool extensionMode);

      int resolveArraySize(Scope& scope, SyntaxNode node);

      void declareParameter(MethodScope& scope, SyntaxNode node, bool withoutWeakMessages, bool declarationMode, 
         bool& weakSignature, pos_t& paramCount, ref_t* signature, size_t& signatureLen);

      ref_t declareClosureParameters(MethodScope& methodScope, SyntaxNode argNode);

      void declareVMTMessage(MethodScope& scope, SyntaxNode node, bool withoutWeakMessages, bool declarationMode);
      void declareClosureMessage(MethodScope& scope, SyntaxNode node);

      void initializeMethod(ClassScope& scope, MethodScope& methodScope, SyntaxNode current);

      void declareMetaInfo(Scope& scope, SyntaxNode node);
      void declareMethodMetaInfo(MethodScope& scope, SyntaxNode node);
      void declareMethod(MethodScope& scope, SyntaxNode node, bool abstractMode);
      void declareVMT(ClassScope& scope, SyntaxNode node, bool& withConstructors, bool& withDefaultConstructor);

      void declareSymbol(SymbolScope& scope, SyntaxNode node);
      void declareClassClass(ClassScope& classClassScope, SyntaxNode node);
      void declareClass(ClassScope& scope, SyntaxNode node);

      void declareNamespace(NamespaceScope& ns, SyntaxNode node, bool ignoreImport = false, 
         bool ignoreExtensions = false);
      void declareMembers(NamespaceScope& ns, SyntaxNode node);
      void declareMemberIdentifiers(NamespaceScope& ns, SyntaxNode node);

      void declareModuleIdentifiers(ModuleScopeBase* moduleScope, SyntaxNode node);
      void declareModule(ModuleScopeBase* moduleScope, SyntaxNode node);

      void addExtensionMessage(Scope& scope, mssg_t message, ref_t extRef, mssg_t strongMessage, 
         bool internalOne);

      void declareExtension(ClassScope& scope, mssg_t message, bool internalOne);

      ObjectInfo evalOperation(Interpreter& interpreter, Scope& scope, SyntaxNode node, ref_t operator_id);
      ObjectInfo evalExpression(Interpreter& interpreter, Scope& scope, SyntaxNode node);
      ObjectInfo evalObject(Interpreter& interpreter, Scope& scope, SyntaxNode node);

      void evalStatement(MetaScope& scope, SyntaxNode node);

      void writeObjectInfo(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo info);

      void addBreakpoint(BuildTreeWriter& writer, SyntaxNode node, BuildKey bpKey);

      bool evalInitializers(ClassScope& scope, SyntaxNode node);
      bool evalClassConstant(ustr_t constName, ClassScope& scope, SyntaxNode node, ObjectInfo& constInfo);

      ref_t compileExtensionDispatcher(BuildTreeWriter& writer, NamespaceScope& scope, mssg_t genericMessage, 
         ref_t outputRef);

      ref_t compileMessageArguments(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode current, 
         ArgumentsInfo& arguments, ExpressionAttribute mode);

      ObjectInfo boxArgumentInPlace(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo info, ref_t targetRef = 0);
      ObjectInfo boxArgument(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo info, 
         bool stackSafe, bool boxInPlace, ref_t targetRef = 0);
      ObjectInfo boxArgumentLocally(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo info, bool boxInPlace);

      ObjectInfo saveToTempLocal(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo object);
      ObjectInfo declareTempLocal(ExprScope& scope, ref_t typeRef);

      ObjectInfo typecastObject(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo source, ref_t targetRef);
      ObjectInfo convertObject(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo source, ref_t targetRef);

      bool compileSymbolConstant(SymbolScope& scope, ObjectInfo retVal);

      ObjectInfo compileExternalOp(BuildTreeWriter& writer, ExprScope& scope, ref_t externalRef, bool stdCall, 
         ArgumentsInfo& arguments);

      ObjectInfo compileNewArrayOp(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo source, ref_t targetRef, ArgumentsInfo& arguments);
      ObjectInfo compileNewOp(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
         ObjectInfo source, ref_t signRef, ArgumentsInfo& arguments);

      ObjectInfo compileMessageOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo target, 
         mssg_t message, ref_t implicitSignatureRef, ArgumentsInfo& arguments, ExpressionAttributes mode);
      ObjectInfo compileMessageOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
         ref_t targetRef, ExpressionAttribute attrs);
      ObjectInfo compilePropertyOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
         ref_t targetRef, ExpressionAttribute attrs);

      ObjectInfo compileAssigning(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode loperand, SyntaxNode roperand);

      ObjectInfo compileOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode loperand, SyntaxNode roperand, int operatorId);
      ObjectInfo compileIndexerOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, int operatorId);
      ObjectInfo compileOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, int operatorId);
      ObjectInfo compileBranchingOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, int operatorId);
      ObjectInfo compileCatchOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node);

      ObjectInfo mapStringConstant(Scope& scope, SyntaxNode node);
      ObjectInfo mapCharacterConstant(Scope& scope, SyntaxNode node);
      ObjectInfo mapIntConstant(Scope& scope, SyntaxNode node, int radix);
      ObjectInfo mapUIntConstant(Scope& scope, SyntaxNode node, int radix);
      ObjectInfo mapTerminal(Scope& scope, SyntaxNode node, TypeInfo typeInfo, ExpressionAttribute attrs);
      ObjectInfo mapMessageConstant(Scope& scope, SyntaxNode node, ref_t actionRef);

      ObjectInfo mapObject(Scope& scope, SyntaxNode node, ExpressionAttributes mode);

      ObjectInfo validateObject(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo retVal,
         ref_t targetRef, bool noPrimitives, bool paramMode);

      ObjectInfo compileNested(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ExpressionAttribute mode);
      ObjectInfo compileClosure(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ExpressionAttribute mode);
      ObjectInfo compileObject(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
         ExpressionAttribute mode);
      ObjectInfo compileExpression(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
         ref_t targetRef, ExpressionAttribute mode);
      ObjectInfo compileLoopExpression(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
         ExpressionAttribute mode);
      ObjectInfo compileSubCode(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
         ExpressionAttribute mode);
      ObjectInfo compileRootExpression(BuildTreeWriter& writer, CodeScope& scope, SyntaxNode node);
      ObjectInfo compileRetExpression(BuildTreeWriter& writer, CodeScope& scope, SyntaxNode node);
      ObjectInfo compileNestedExpression(InlineClassScope& scope, ExpressionAttribute mode);

      void compileMultidispatch(BuildTreeWriter& writer, CodeScope& codeScope, ClassScope& classcope, SyntaxNode node);
      void compileDispatchCode(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node);
      void compileDispatchProberCode(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node);
      void compileConstructorDispatchCode(BuildTreeWriter& writer, CodeScope& codeScope, ClassScope& classClassScope, SyntaxNode node);

      ObjectInfo compileResendCode(BuildTreeWriter& writer, CodeScope& codeScope, ObjectInfo source, SyntaxNode node);
      ObjectInfo compileCode(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node, bool closureMode);

      void beginMethod(BuildTreeWriter& writer, MethodScope& scope, BuildKey scopeKey);
      void endMethod(BuildTreeWriter& writer, MethodScope& scope);

      void compileMethodCode(BuildTreeWriter& writer, MethodScope& scope, CodeScope& codeScope, 
         SyntaxNode node, bool newFrame);
      void compileDefConvConstructorCode(BuildTreeWriter& writer, MethodScope& scope, 
         SyntaxNode node, bool newFrame);

      void compileInitializerMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode classNode);
      void compileClosureMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node);
      void compileAbstractMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node, bool abstractMode);
      void compileMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node);
      void compileConstructor(BuildTreeWriter& writer, MethodScope& scope, ClassScope& classClassScope, SyntaxNode node);
      void compileNestedClass(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node, ref_t parentRef);
      void compileClosureClass(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node);
      void compileVMT(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node,
         bool exclusiveMode = false, bool ignoreAutoMultimethod = false);
      void compileClassVMT(BuildTreeWriter& writer, ClassScope& classClassScope, ClassScope& scope, SyntaxNode node);

      void compileSymbol(BuildTreeWriter& writer, SymbolScope& scope, SyntaxNode node);
      void compileClassSymbol(BuildTreeWriter& writer, ClassScope& scope);
      void compileClass(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node);
      void compileClassClass(BuildTreeWriter& writer, ClassScope& classClassScope, ClassScope& scope, SyntaxNode node);

      void compileNamespace(BuildTreeWriter& writer, NamespaceScope& ns, SyntaxNode node);

      void validateScope(ModuleScopeBase* moduleScope);
      void validateSuperClass(ClassScope& scope, SyntaxNode node);
      void validateType(Scope& scope, ref_t typeRef, SyntaxNode node);

      void injectVirtualCode(SyntaxNode classNode, ClassScope& scope, bool interfaceBased);
      void injectVirtualMultimethods(SyntaxNode classNode, SyntaxKey methodType, ModuleScopeBase& scope, 
         ClassInfo& info, List<mssg_t>& implicitMultimethods);

      void injectInitializer(SyntaxNode classNode, SyntaxKey methodType, mssg_t message);

      void injectVirtualMultimethod(SyntaxNode classNode, SyntaxKey methodType, ModuleScopeBase& scope, 
         ClassInfo& classInfo, mssg_t message, bool inherited, ref_t outputRef);
      void injectVirtualMultimethod(SyntaxNode classNode, SyntaxKey methodType, mssg_t message, 
         mssg_t resendMessage, ref_t resendTarget, ref_t outputRef);

      void injectVirtualTryDispatch(SyntaxNode classNode, SyntaxKey methodType, ClassInfo& info, 
         mssg_t message, mssg_t dispatchMessage, bool inherited);
      void injectVirtualTryDispatch(SyntaxNode classNode, SyntaxKey methodType, mssg_t message, mssg_t dispatchMessage, ref_t resendTarget);

      void injectDefaultConstructor(ClassScope& scope, SyntaxNode node);

      void injectVariableInfo(BuildNode node, CodeScope& codeScope);

      void generateOverloadListMember(ModuleScopeBase& scope, ref_t listRef, ref_t classRef, 
         mssg_t messageRef, MethodHint type) override;

   public:
      void prepare(ModuleScopeBase* moduleScope, ForwardResolverBase* forwardResolver);
      void declare(ModuleScopeBase* moduleScope, SyntaxTree& input);
      void compile(ModuleScopeBase* moduleScope, SyntaxTree& input, BuildTree& output);

      void injectVirtualReturningMethod(ModuleScopeBase* scope, SyntaxNode classNode,
         mssg_t message, ustr_t retVar, ref_t classRef) override;

      Compiler(
         ErrorProcessor* errorProcessor, 
         TemplateProssesorBase* templateProcessor,
         CompilerLogic* compilerLogic);
   };
}

#endif