//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class.
//
//                                              (C)2005-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef compilerH
#define compilerH

#include "elena.h"
#include "compilercommon.h"
#include "bcwriter.h"

namespace _ELENA_
{

//struct Unresolved
//{
//   ident_t    fileName;
//   ref_t      reference;
//   _Module*   module;
//   size_t     row;
//   size_t     col;           // virtual column
//
//   Unresolved()
//   {
//      reference = 0;
//   }
//   Unresolved(ident_t fileName, ref_t reference, _Module* module, size_t row, size_t col)
//   {
//      this->fileName = fileName;
//      this->reference = reference;
//      this->module = module;
//      this->row = row;
//      this->col = col;
//   }
//};

//typedef List<Unresolved> Unresolveds;

// --- Compiler class ---
class Compiler : public _Compiler
{
public:
   struct Parameter
   {
      int    offset;
      ref_t  class_ref;
      ref_t  element_ref;
      int    size;

      Parameter()
      {
         offset = -1;
         class_ref = 0;
         element_ref = 0;
         size = 0;
      }
      Parameter(int offset)
      {
         this->offset = offset;
         this->class_ref = 0;
         this->element_ref = 0;
         this->size = 0;
      }
      Parameter(int offset, ref_t class_ref)
      {
         this->offset = offset;
         this->class_ref = class_ref;
         this->element_ref = 0;
         this->size = 0;
      }
      Parameter(int offset, ref_t class_ref, int size)
      {
         this->offset = offset;
         this->class_ref = class_ref;
         this->element_ref = 0;
         this->size = size;
      }
      Parameter(int offset, ref_t class_ref, ref_t element_ref, int size)
      {
         this->offset = offset;
         this->class_ref = class_ref;
         this->element_ref = element_ref;
         this->size = size;
      }
   };

   // InheritResult
   enum InheritResult
   {
      irNone = 0,
      irSuccessfull,
      irUnsuccessfull,
      irSealed,
      irInvalid,
      irObsolete
   };

   enum ObjectKind
   {
      okUnknown = 0,

      okObject,                       // param - class reference
      okSymbol,                       // param - reference
      okConstantSymbol,               // param - reference, extraparam - class reference
      okConstantClass,                // param - reference, extraparam - class reference
      okLiteralConstant,              // param - reference
      okWideLiteralConstant,          // param - reference
      okCharConstant,                 // param - reference
      okIntConstant,                  // param - reference, extraparam - imm argument
      okUIntConstant,                 // param - reference, extraparam - imm argument
      okLongConstant,                 // param - reference
      okRealConstant,                 // param - reference
      okMessageConstant,              // param - reference
      okExtMessageConstant,           // param - reference
      okSignatureConstant,            // param - reference
      okArrayConst,
      okField,                        // param - reference, param - field offset, extraparam - class reference
      okStaticField,                  // param - reference
      okStaticConstantField,          // param - reference
      okClassStaticConstantField,     // param - class reference / 0 (for static methods), extraparam - field offset
      okFieldAddress,                 // param - field offset, extraparam - class reference
      okOuter,                        // param - field offset, extraparam - class reference
      okOuterField,                   // param - field offset, extraparam - outer field offset
      okOuterStaticField,             // param - field offset, extraparam - outer field offset
      okClassStaticField,             // param - class reference / 0 (for static methods), extraparam - field offset
////      okCurrent,                      // param - stack offset
      okLocal,                        // param - local / out parameter offset, extraparam : class reference
      okParam,                        // param - parameter offset, extraparam = class reference
////      okParamField,
      okSubject,                      // param - parameter offset
      okSelfParam,                    // param - parameter offset, extraparam = -1 (stack allocated) / -2 (primitive array)
      okNil,
      okSuper,
      okLocalAddress,                 // param - local offset, extraparam - class reference
      okParams,                       // param - local offset
//      okBlockLocal,                   // param - local offset
      okConstantRole,                 // param - role reference
//      okExplicitConstant,             // param - reference, extraparam - subject

      okExternal,
      okInternal,
   };

   struct ObjectInfo
   {
      ObjectKind kind;
      ref_t      param;
      ref_t      extraparam;
      ref_t      element;

      ObjectInfo()
      {
         this->kind = okUnknown;
         this->param = 0;
         this->extraparam = 0;
         this->element = 0;
      }
      ObjectInfo(ObjectKind kind)
      {
         this->kind = kind;
         this->param = 0;
         this->extraparam = 0;
         this->element = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = 0;
         this->element = 0;
      }
//      ObjectInfo(ObjectKind kind, int param)
//      {
//         this->kind = kind;
//         this->param = (ref_t)param;
//         this->extraparam = 0;
//         this->element = 0;
//      }
      ObjectInfo(ObjectKind kind, ref_t param, ref_t extraparam)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = extraparam;
         this->element = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param, int extraparam)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = (ref_t)extraparam;
         this->element = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param, ref_t extraparam, ref_t element)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = extraparam;
         this->element = element;
      }
      ObjectInfo(ObjectKind kind, ref_t param, int extraparam, ref_t element)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = (ref_t)extraparam;
         this->element = element;
      }
   };

   typedef MemoryMap<ident_t, Parameter>  LocalMap;
   typedef Map<ref_t, Pair<ref_t, ref_t>> ExtensionMap;
//   typedef Map<ref_t, SubjectList*>       AutoExtensionMap;

private:
   // - Scope -
   struct Scope
   {
         enum ScopeLevel
         {
            slNamespace,
            slClass,
            slSymbol,
            slMethod,
            slCode,
            slOwnerClass,
            //slTemplate,
         };
   
         _CompilerScope* moduleScope;
         _Module*        module;
         Scope*          parent;
   
         virtual void raiseError(const char* message)
         {
            moduleScope->project->raiseError(message);
         }
         virtual void raiseError(const char* message, SNode terminal)
         {
            parent->raiseError(message, terminal);
         }
         virtual void raiseWarning(int level, const char* message, SNode terminal)
         {
            parent->raiseWarning(level, message, terminal);
         }
         virtual void raiseWarning(int level, const char* message, ident_t identifier)
         {
            parent->raiseWarning(level, message, identifier);
         }
         virtual void raiseWarning(int level, const char* message)
         {
            parent->raiseWarning(level, message);
         }

         virtual pos_t saveSourcePath(ByteCodeWriter& writer)
         {
            return parent->saveSourcePath(writer);
         }
         virtual pos_t saveSourcePath(ByteCodeWriter& writer, ident_t path)
         {
            return parent->saveSourcePath(writer, path);
         }

         virtual bool resolveAutoType(ObjectInfo& info, ref_t reference, ref_t element)
         {
            if (parent) {
               return parent->resolveAutoType(info, reference, element);
            }
            else return false;
         }
   
         virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne)
         {
            if (parent) {
               return parent->mapTerminal(identifier, referenceOne);
            }
            else return ObjectInfo();
         }
   
         virtual Scope* getScope(ScopeLevel level)
         {
            if (parent) {
               return parent->getScope(level);
            }
            else return NULL;
         }
   
//   //      virtual ref_t mapSubject(SNode terminal, IdentifierString& output)
//   //      {
//   //         return moduleScope->mapSubject(terminal, output);
//   //      }
//   //
//   //      virtual ref_t mapSubject(SNode terminal)
//   //      {
//   //         return moduleScope->mapSubject(terminal);
//   //      }
   
         Scope(_CompilerScope* moduleScope)
         {
            this->parent = NULL;
            this->moduleScope = moduleScope;
            this->module = moduleScope->module;
         }
         Scope(Scope* parent)
         {
            this->parent = parent;
            this->moduleScope = parent->moduleScope;
            this->module = parent->module;
         }
   };

   // - NamespaceScope -
   struct NamespaceScope : Scope
   {
      // imported namespaces
      IdentifierList importedNs;
      ForwardMap     forwards;       // forward declarations

      // symbol hints
      Map<ref_t, ref_t> constantHints;

      // extensions
      ExtensionMap      extensions;
//      AutoExtensionMap  autoExtensions;
//
//      // action hints
//      SubjectMap        actionHints;
//
//      ref_t packageReference;

//      // list of references to the current module which should be checked after the project is compiled
//      Unresolveds* forwardsUnresolved;

      ident_t        ns;
      ident_t        sourcePath;

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slNamespace) {
            return this;
         }
         else return Scope::getScope(level);
      }

//      ObjectInfo mapObject(SNode identifier);
//
//      virtual ref_t mapReference(ident_t reference, bool existing = false);
//      virtual ref_t mapAttribute(SNode terminal);

//      ObjectInfo mapReferenceInfo(ident_t reference, bool existing = false);

      void defineConstantSymbol(ref_t reference, ref_t classReference)
      {
         constantHints.add(reference, classReference);
      }

      virtual void raiseError(const char* message)
      {
         Scope::raiseError(message);
      }
      virtual void raiseError(const char* message, SNode terminal)
      {
         moduleScope->raiseError(message, sourcePath, terminal);
      }
      virtual void raiseWarning(int level, const char* message, SNode terminal)
      {
         moduleScope->raiseWarning(level, message, sourcePath, terminal);
      }
      virtual void raiseWarning(int level, const char* message)
      {
         moduleScope->raiseWarning(level, message, sourcePath);
      }
      virtual void raiseWarning(int level, const char* message, ident_t identifier)
      {
         moduleScope->raiseWarning(level, message, sourcePath, identifier);
      }
      //virtual void raiseWarning(int level, const char* message, int row, int col, ident_t sourcePath, ident_t terminal)
      //{
      //   moduleScope->raiseWarning(level, message, sourcePath, );
      //}

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne);

      ObjectInfo mapGlobal(ident_t identifier);

      virtual pos_t saveSourcePath(ByteCodeWriter& writer);
      virtual pos_t saveSourcePath(ByteCodeWriter& writer, ident_t path);


////      void raiseError(const char* message);
////      void raiseError(const char* message, int row, int col, ident_t sourcePath, ident_t terminal);
//////      void raiseWarning(int level, const char* message);
////
////      virtual void raiseError(const char* message, ident_t sourcePath, SNode terminal);
////      virtual void raiseWarning(int level, const char* message, ident_t sourcePath, SNode terminal);
//
////      bool doesReferenceExist(ident_t referenceName);

      ref_t resolveFullReference(ident_t name);
      ref_t resolveImplicitIdentifier(ident_t name, bool referenceOne);

////      ref_t mapNewSubject(ident_t terminal);
//
//      // NOTE : the function returns 0 for implicit subjects
//      // in any case output is set (for explicit one - the namespace is copied as well)
//      ref_t mapSubject(SNode terminal, IdentifierString& output);
//      ref_t mapSubject(SNode terminal);

      ref_t mapNewTerminal(SNode terminal);

//      ///*virtual */ref_t mapTerminal(SNode terminal, bool existing = false);

      ObjectInfo defineObjectInfo(ref_t reference, bool checkState = false);

//      virtual _Module* loadReferenceModule(ref_t& reference);

      ref_t loadSymbolExpressionInfo(SymbolExpressionInfo& info, ident_t symbol);

//      bool loadAttributes(_Module* module);
      void loadExtensions(ident_t ns);
      void loadExtensions(ident_t ns, ident_t subns)
      {
         IdentifierString fullName(ns);
         if (!emptystr(subns)) {
            fullName.append("'");
            fullName.append(subns);
         }
         loadExtensions(fullName.c_str());
      }
//      void loadActions(_Module* module);
//      void loadAutogeneratedExtension(_Module* module);

//      virtual void saveAutogerenatedExtension(ref_t attr, ref_t extension);
      void saveExtension(ref_t message, ref_t type, ref_t role);
//      void saveAction(ref_t message, ref_t reference);      
      void saveIncludedModule(_Module* extModule);

//      virtual void validateReference(SNode terminal, ref_t reference);
//
//      ref_t getBaseLazyExpressionClass();

//      void loadModuleInfo(_Module* extModule)
//      {
//         bool dummy1, dummy2;
//         loadModuleInfo(extModule, dummy1, dummy2);
//      }
      void loadModuleInfo(ident_t name)
      {
         loadExtensions(name);
//         loadActions(extModule);
//         loadAutogeneratedExtension(extModule);         
      }

      /*virtual */ref_t mapAnonymous();

//      bool defineForward(ident_t forward, ident_t referenceName)
//      {
//         ObjectInfo info = mapReferenceInfo(referenceName, false);
//      
//         return forwards.add(forward, info.param, true);
//      }

//      virtual SubjectList* getAutogerenatedExtensions(ref_t attr)
//      {
//         return autoExtensions.get(attr);
//      }

//      pos_t saveSourcePath(ByteCodeWriter& writer, ident_t sourcePath);

      NamespaceScope(_CompilerScope* moduleScope, ident_t path, ident_t ns, IdentifierList* imported, bool withFullInfo);
   };

   // - SourceScope -
   struct SourceScope : public Scope
   {
      ref_t          reference;

      SourceScope(Scope* parent, ref_t reference/*, ident_t sourcePath*/);
   };

   // - ClassScope -
   struct ClassScope : public SourceScope
   {
      ClassInfo   info;
      ref_t       extensionClassRef;
      bool        embeddable;
      bool        classClassMode;
      bool        abstractMode;
      bool        abstractBaseMode;

      void copyStaticFields(ClassInfo::StaticFieldMap& statics, ClassInfo::StaticInfoMap& staticValues);

      ObjectInfo mapField(ident_t identifier);

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slClass || level == slOwnerClass) {
            return this;
         }
         else return Scope::getScope(level);
      }

      void save()
      {
         // save class meta data
         MemoryWriter metaWriter(moduleScope->mapSection(reference | mskMetaRDataRef, false), 0);
         metaWriter.Memory()->trim(0);
         info.save(&metaWriter);
      }

      void addHint(ref_t message, int hint)
      {
         ClassInfo::Attribute attr(message, maHint);

         hint |= info.methodHints.get(attr);
         info.methodHints.exclude(attr);
         info.methodHints.add(attr, hint);
      }

      bool include(ref_t message)
      {
         // check if the method is inhreited and update vmt size accordingly
         ClassInfo::MethodMap::Iterator it = info.methods.getIt(message);
         if (it.Eof()) {
            info.methods.add(message, true);

            return true;
         }
         else {
            (*it) = true;

            return false;
         }
      }

      ClassScope(Scope* parent, ref_t reference);
   };

   // - SymbolScope -
   struct SymbolScope : public SourceScope
   {
      bool  constant;
      bool  staticOne;
      bool  preloaded;
      ref_t outputRef;

//      virtual ObjectInfo mapTerminal(ident_t identifier);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slSymbol) {
            return this;
         }
         else return Scope::getScope(level);
      }

      void save();

      SymbolScope(NamespaceScope* parent, ref_t reference);
   };

   // - MethodScope -
   struct MethodScope : public Scope
   {
      ref_t        message;
      LocalMap     parameters;
      int          reserved;           // defines inter-frame stack buffer (excluded from GC frame chain)
      int          rootToFree;         // by default is 1, for open argument - contains the list of normal arguments as well
      int          hints;
      bool         withOpenArg;
      bool         stackSafe;
      bool         classEmbeddable;
      bool         generic;
      bool         genericClosure;
      bool         extensionMode;
      bool         multiMethod;
      bool         closureMode;
      bool         nestedMode;
      bool         subCodeMode;
      bool         abstractMethod;
      
      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slMethod) {
            return this;
         }
         else return parent->getScope(level);
      }

      ref_t getReturningRef(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

         return scope->info.methodHints.get(ClassInfo::Attribute(message, maReference));
      }

      //ref_t getClassFlags(bool ownerClass = true)
      //{
      //   ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

      //   return scope ? scope->info.header.flags : 0;
      //}
      ref_t getClassRef(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

         return scope ? scope->reference : 0;
      }

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne);

      ObjectInfo mapSelf(bool forced = false);
      ObjectInfo mapParameter(Parameter param);

      MethodScope(ClassScope* parent);
   };

   // - CodeScope -
   struct CodeScope : public Scope
   {
      // scope local variables
      LocalMap     locals;
      int          level;

      // scope stack allocation
      int          reserved;  // allocated for the current statement
      int          saved;     // permanently allocated

      int newLocal()
      {
         level++;

         return level;
      }

      void mapLocal(ident_t local, int level)
      {
         locals.add(local, Parameter(level));
      }
      void mapLocal(ident_t local, int level, ref_t class_ref, int size)
      {
         locals.add(local, Parameter(level, class_ref, size));
      }
      void mapLocal(ident_t local, int level, ref_t class_ref, ref_t element_ref, int size)
      {
         locals.add(local, Parameter(level, class_ref, element_ref, size));
      }

//      void freeSpace()
//      {
//         reserved = saved;
//      }

      ObjectInfo mapMember(ident_t identifier);

      ObjectInfo mapGlobal(ident_t identifier);

      ObjectInfo mapLocal(ident_t identifier);

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne);
      virtual bool resolveAutoType(ObjectInfo& info, ref_t reference, ref_t element);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slCode) {
            return this;
         }
         else return parent->getScope(level);
      }

      bool isInitializer()
      {
         return getMessageID() == (encodeAction(INIT_MESSAGE_ID) | SPECIAL_MESSAGE);
      }

      ref_t getMessageID()
      {
         MethodScope* scope = (MethodScope*)getScope(slMethod);

         return scope ? scope->message : 0;
      }

      ref_t getReturningRef()
      {
         MethodScope* scope = (MethodScope*)getScope(slMethod);

         return scope ? scope->getReturningRef() : 0;
      }

      ref_t getClassRefId(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

         return scope ? scope->reference : 0;
      }

      ref_t getClassFlags(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

         return scope ? scope->info.header.flags : 0;
      }

      CodeScope(SourceScope* parent);
      CodeScope(MethodScope* parent);
      CodeScope(CodeScope* parent);
   };

   // --- ResendScope ---
   struct ResendScope : public CodeScope
   {
      bool withFrame;
      bool consructionMode;

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne);

      ResendScope(CodeScope* parent)
         : CodeScope(parent)
      {
         consructionMode = withFrame = false;
      }
   };

   // - InlineClassScope -
   struct InlineClassScope : public ClassScope
   {
      struct Outer
      {
         ref_t      reference;
         bool       preserved;
         ObjectInfo outerObject;

         Outer()
         {
            reference = INVALID_REF;
            preserved = false;
         }
         Outer(int reference, ObjectInfo outerObject)
         {
            this->reference = reference;
            this->outerObject = outerObject;
            this->preserved = false;
         }
      };

      bool                    returningMode;
      Map<ident_t, Outer>     outers;
      ClassInfo::FieldTypeMap outerFieldTypes;

      Outer mapSelf();
      Outer mapOwner();
      Outer mapParent();

      ObjectInfo allocateRetVar();

      bool markAsPresaved(ObjectInfo object);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slClass) {
            return this;
         }
         else return Scope::getScope(level);
      }

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne);

      InlineClassScope(CodeScope* owner, ref_t reference);
   };

   //struct WarningScope
   //{
   //   ident_t terminal;

   //   int warningMask;
   //   int col;
   //   int row;

   //   //void raise(Scope& scope, int level, ident_t message, SNode node)
   //   //{
   //   //   if (test(warningMask, level)) {
   //   //      if (col != 0) {
   //   //         scope.raiseWarning(level, message, row, col, terminal);
   //   //      }
   //   //      else if(node != lxNone)
   //   //         scope.raiseWarning(level, message, node);
   //   //   }
   //   //}

   //   WarningScope(int mask)
   //   {
   //      warningMask = mask;
   //      col = row = 0;
   //      terminal = NULL;
   //   }
   //   WarningScope()
   //   {
   //      warningMask = 0;
   //      col = row = 0;
   //      terminal = NULL;
   //   }
   //};

   _CompilerLogic*  _logic;

   ByteCodeWriter _writer;

   MessageMap     _verbs;                            // list of verbs
   MessageMap     _operators;                        // list of operators

   int            _optFlag;

   // optimization rules
   TransformTape _rules;

   // optmimization routines
   bool applyRules(CommandTape& tape);
   bool optimizeIdleBreakpoints(CommandTape& tape);
   bool optimizeJumps(CommandTape& tape);
   void optimizeTape(CommandTape& tape);

   bool calculateIntOp(int operation_id, int arg1, int arg2, int& retVal);
   bool calculateRealOp(int operation_id, double arg1, double arg2, double& retVal);

   void writeMessageInfo(SyntaxWriter& writer, _CompilerScope& scope, ref_t messageRef);
   void initialize(ClassScope& scope, MethodScope& methodScope);

   int checkMethod(_CompilerScope& scope, ref_t reference, ref_t message)
   {
      _CompilerLogic::ChechMethodInfo dummy;

      return _logic->checkMethod(scope, reference, message, dummy);
   }

   int retrieveGenericArgParamCount(ClassScope& scope);

   void loadAttributes(_CompilerScope& scope, ident_t name, MessageMap* attributes);

   ref_t resolveConstantObjectReference(CodeScope& scope, ObjectInfo object);
   ref_t resolveObjectReference(_CompilerScope& scope, ObjectInfo object);
   ref_t resolveObjectReference(CodeScope& scope, ObjectInfo object);
   ref_t resolveObjectReference(CodeScope& scope, ObjectInfo object, ref_t targetRef);
   ref_t resolveImplicitIdentifier(Scope& scope, SNode terminal);
   ref_t resolveImplicitIdentifier(Scope& scope, ident_t identifier, bool referenceOne, bool gloabalOne = false);

   void saveExtension(ClassScope& scope, ref_t message);
   //ref_t mapExtension(_CompilerScope& scope, SubjectMap* typeExtensions, ref_t& messageRef, ref_t implicitSignatureRef);
   ref_t mapExtension(CodeScope& scope, ref_t& messageRef, ref_t implicitSignatureRef, ObjectInfo target, bool& dynamicReqiered);

   void importCode(SyntaxWriter& writer, SNode node, Scope& scope, ident_t reference, ref_t message);

   int defineFieldSize(CodeScope& scope, int offset);

   InheritResult inheritClass(ClassScope& scope, ref_t parentRef, bool ignoreSealed);
   void inheritClassConstantList(_CompilerScope& scope, ref_t sourceRef, ref_t targetRef);

   // NOTE : the method is used to set template pseudo variable
   void declareParameterDebugInfo(SyntaxWriter& writer, SNode node, MethodScope& scope, bool withSelf/*, bool withTargetSelf*/);

   ref_t resolveParentRef(SNode node, Scope& moduleScope, bool silentMode);
   bool isDependentOnNotDeclaredClass(SNode baseNode, Scope& scope);

   void compileParentDeclaration(SNode baseNode, ClassScope& scope, ref_t parentRef, bool ignoreSealed = false);
   void compileParentDeclaration(SNode node, ClassScope& scope);
   void generateClassFields(SNode member, ClassScope& scope, bool singleField);

   void declareSymbolAttributes(SNode node, SymbolScope& scope);
   void declareClassAttributes(SNode node, ClassScope& scope);
   void declareLocalAttributes(SNode hints, CodeScope& scope, ObjectInfo& variable, int& size);
   void declareFieldAttributes(SNode member, ClassScope& scope, ref_t& fieldRef, ref_t& elementRef, int& size, bool& isStaticField, bool& isSealed, bool& isConstant);
   void declareVMT(SNode member, ClassScope& scope);
////   void declareClassVMT(SNode member, ClassScope& classClassScope, ClassScope& classScope);

   void declareMethodAttributes(SNode member, MethodScope& scope);

   bool resolveAutoType(ObjectInfo source, ObjectInfo& target, CodeScope& scope);

   ref_t resolveMessageAtCompileTime(ObjectInfo& target, CodeScope& scope, ref_t generalMessageRef, ref_t implicitSignatureRef,
                                     bool withExtension, bool& dynamicReqiered);
   ref_t resolveMessageAtCompileTime(ObjectInfo& target, CodeScope& scope, ref_t generalMessageRef, ref_t implicitSignatureRef)
   {
      bool dummy;
      return resolveMessageAtCompileTime(target, scope, generalMessageRef, implicitSignatureRef, false, dummy);
   }
   ref_t mapMessage(SNode node, CodeScope& scope);

//   void compileSwitch(SyntaxWriter& writer, SNode node, CodeScope& scope);
   void compileVariable(SyntaxWriter& writer, SNode node, CodeScope& scope);

   ObjectInfo compileClosure(SyntaxWriter& writer, SNode node, CodeScope& ownerScope, int mode);
   ObjectInfo compileClosure(SyntaxWriter& writer, SNode node, CodeScope& ownerScope, InlineClassScope& scope);
//   ObjectInfo compileCollection(SyntaxWriter& writer, SNode objectNode, CodeScope& scope);
//   ObjectInfo compileCollection(SyntaxWriter& writer, SNode objectNode, CodeScope& scope, ref_t vmtReference);

   ObjectInfo compileMessageReference(SyntaxWriter& writer, SNode objectNode, CodeScope& scope, int mode);
   void writeTerminal(SyntaxWriter& writer, SNode& terminal, CodeScope& scope, ObjectInfo object, int mode);
   void writeParamTerminal(SyntaxWriter& writer, CodeScope& scope, ObjectInfo object, int mode, LexicalType type);
   void writeTerminalInfo(SyntaxWriter& writer, SNode node);

   ObjectInfo compileTerminal(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);
   ObjectInfo compileObject(SyntaxWriter& writer, SNode objectNode, CodeScope& scope, int mode);

   ObjectInfo compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, int operator_id, int paramCount, ObjectInfo loperand, ObjectInfo roperand, ObjectInfo roperand2);
   ObjectInfo compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, int mode, int operator_id);
   ObjectInfo compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, int mode);
   void compileBranchingNodes(SyntaxWriter& writer, SNode loperandNode, CodeScope& scope, ref_t ifReference, bool loopMode, bool switchMode);
   void compileBranchingOperand(SyntaxWriter& writer, SNode roperandNode, CodeScope& scope, int mode, int operator_id, ObjectInfo loperand, ObjectInfo& retVal);
   ObjectInfo compileBranchingOperator(SyntaxWriter& writer, SNode& node, CodeScope& scope, ObjectInfo target, int mode, int operator_id);

//   void resolveStrongArgument(CodeScope& scope, ObjectInfo info, bool& anonymous, IdentifierString& signature);
//   ref_t resolveStrongArgument(CodeScope& scope, ObjectInfo info);

   ref_t compileMessageParameters(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode = 0);

   ObjectInfo compileMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, int mode);
   ObjectInfo compileMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, int messageRef, int mode);
   ObjectInfo compileExtensionMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, ObjectInfo role, ref_t targetRef = 0);

   ObjectInfo compileBoxingExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);
   ObjectInfo compileReferenceExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);
   ObjectInfo compileAssigning(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, int mode);
   ObjectInfo compilePropAssigning(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, int mode);
   ObjectInfo compileExtension(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target);
   ObjectInfo compileExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, ref_t targetRef, int mode);
   ObjectInfo compileRetExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);

   ObjectInfo compileBranching(SyntaxWriter& writer, SNode thenNode, CodeScope& scope/*, ObjectInfo target, int verb, int subCodinteMode*/);

   void compileStaticAssigning(ObjectInfo target, SNode node, ClassScope& scope/*, int mode*/);
   void compileClassConstantAssigning(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo retVal);

   ObjectInfo compileOperation(SyntaxWriter& writer, SNode current, CodeScope& scope, ObjectInfo objectInfo, int mode);

   void compileTrying(SyntaxWriter& writer, SNode node, CodeScope& scope);
   void compileAltOperation(SyntaxWriter& writer, SNode node, CodeScope& scope);
   void compileLoop(SyntaxWriter& writer, SNode node, CodeScope& scope);

   int allocateStructure(bool bytearray, int& allocatedSize, int& reserved);
   int allocateStructure(SNode node, int& size);
   bool allocateStructure(CodeScope& scope, int size, bool bytearray, ObjectInfo& exprOperand);

   ObjectInfo compileExternalCall(SyntaxWriter& writer, SNode node, CodeScope& scope);
   ObjectInfo compileInternalCall(SyntaxWriter& writer, SNode node, CodeScope& scope, ref_t message, ref_t signature, ObjectInfo info);

   void compileConstructorResendExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, ClassScope& classClassScope, bool& withFrame);
   void compileConstructorDispatchExpression(SyntaxWriter& writer, SNode node, CodeScope& scope);
   void compileResendExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, bool multiMethod/*, bool extensionMode*/);
   void compileDispatchExpression(SyntaxWriter& writer, SNode node, CodeScope& scope);
   void compileMultidispatch(SyntaxWriter& writer, SNode node, CodeScope& scope, ClassScope& classScope);

   ObjectInfo compileCode(SyntaxWriter& writer, SNode node, CodeScope& scope);

   ref_t declareArgumentType(SNode node, Scope& scope/*, bool& first, IdentifierString& messageStr, IdentifierString& signature*/, ref_t& elementRef);
   void declareArgumentList(SNode node, MethodScope& scope);
//   ref_t declareInlineArgumentList(SNode node, MethodScope& scope);
   bool declareActionScope(ClassScope& scope, SNode argNode, MethodScope& methodScope, int mode/*, bool alreadyDeclared*/);

//   void declareSingletonClass(SNode node, ClassScope& scope);

   void compileActionMethod(SyntaxWriter& writer, SNode member, MethodScope& scope);
   void compileLazyExpressionMethod(SyntaxWriter& writer, SNode member, MethodScope& scope);
   void compileDispatcher(SyntaxWriter& writer, SNode node, MethodScope& scope, bool withGenericMethods = false, bool withOpenArgGenerics = false);

   void compileMethod(SyntaxWriter& writer, SNode node, MethodScope& scope);
   void compileAbstractMethod(SyntaxWriter& writer, SNode node, MethodScope& scope);
   void compileConstructor(SyntaxWriter& writer, SNode node, MethodScope& scope, ClassScope& classClassScope);
   void compileImplicitConstructor(SyntaxWriter& writer, SNode node, MethodScope& scope);

   void compileSpecialMethodCall(SyntaxWriter& writer, ClassScope& classScope, ref_t message);

   void compileDefaultConstructor(SyntaxWriter& writer, MethodScope& scope);
   void compileDynamicDefaultConstructor(SyntaxWriter& writer, MethodScope& scope);

   void compilePreloadedCode(SymbolScope& scope);
   void compilePreloadedCode(_CompilerScope& scope, SNode node);
   void compileSymbolCode(ClassScope& scope);

   void compileAction(SNode node, ClassScope& scope, SNode argNode, int mode/*, bool alreadyDeclared = false*/);
   void compileNestedVMT(SNode node, InlineClassScope& scope);

   void compileVMT(SyntaxWriter& writer, SNode node, ClassScope& scope);
   void compileClassVMT(SyntaxWriter& writer, SNode node, ClassScope& classClassScope, ClassScope& classScope);

   void generateClassField(ClassScope& scope, SNode node, ref_t fieldRef, ref_t elementRef, int sizeHint, bool singleField);
   void generateClassStaticField(ClassScope& scope, SNode current, ref_t fieldRef, ref_t elementRef, bool isSealed, bool isConst);

   void generateClassFlags(ClassScope& scope, SNode node/*, bool& closureBaseClass*/);
   void generateMethodAttributes(ClassScope& scope, SyntaxTree::Node node, ref_t message, bool allowTypeAttribute);

   void generateMethodDeclaration(SNode current, ClassScope& scope, bool hideDuplicates, bool closed, bool allowTypeAttribute/*, bool closureBaseClass*/);
   void generateMethodDeclarations(SNode node, ClassScope& scope, bool closed, LexicalType methodType/*, bool closureBaseClass*/);
   void generateClassDeclaration(SNode node, ClassScope& scope, bool classClassMode, bool nestedDeclarationMode = false);

   void generateClassImplementation(SNode node, ClassScope& scope);

   void compileClassDeclaration(SNode node, ClassScope& scope);
   void compileClassImplementation(SyntaxTree& expressionTree, SNode node, ClassScope& scope);
   void compileClassClassDeclaration(SNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileClassClassImplementation(SyntaxTree& expressionTree, SNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileSymbolDeclaration(SNode node, SymbolScope& scope);
   void compileSymbolImplementation(SyntaxTree& expressionTree, SNode node, SymbolScope& scope);
   bool compileSymbolConstant(SNode node, SymbolScope& scope, ObjectInfo retVal, bool accumulatorMode = false);
////   void compileIncludeModule(SNode node, ModuleScope& scope);
//   void compileForward(SNode node, ModuleScope& scope);
//
//   bool validate(_ProjectManager& project, _Module* module, int reference);

   ObjectInfo assignResult(SyntaxWriter& writer, CodeScope& scope, ref_t targetRef, ref_t elementRef = 0);

   bool convertObject(SyntaxWriter& writer, Scope& scope, ref_t targetRef, ref_t sourceRef, ref_t elementRef);
   bool typecastObject(SyntaxWriter& writer, Scope& scope, ref_t targetRef);

   void compileExternalArguments(SNode node, NamespaceScope& scope/*, WarningScope& warningScope*/);

   ref_t analizeOp(SNode current, NamespaceScope& scope/*, WarningScope& warningScope*/);
   ref_t analizeSymbol(SNode& node, NamespaceScope& scope/*, WarningScope& warningScope*/);
   ref_t analizeAssigning(SNode node, NamespaceScope& scope/*, WarningScope& warningScope*/);
   ref_t analizeBoxing(SNode node, NamespaceScope& scope, /*WarningScope& warningScope, */int mode);
   ref_t analizeArgBoxing(SNode node, NamespaceScope& scope, /*WarningScope& warningScope, */int mode);
   ref_t analizeArgUnboxing(SNode node, NamespaceScope& scope, /*WarningScope& warningScope, */int mode);
   ref_t analizeMessageCall(SNode node, NamespaceScope& scope, int mode);
   ref_t analizeExpression(SNode node, NamespaceScope& scope, /*WarningScope& warningScope, */int mode = 0);
   ref_t analizeInternalCall(SyntaxTree::Node node, NamespaceScope& scope/*, WarningScope& warningScope*/);
   ref_t analizeExtCall(SyntaxTree::Node node, NamespaceScope& scope/*, WarningScope& warningScope*/, int mode);
   ref_t analizeNestedExpression(SNode node, NamespaceScope& scope/*, WarningScope& warningScope*/);
   void analizeExpressionTree(SNode node, NamespaceScope& scope, /*WarningScope& warningScope, */int mode = 0);
   void analizeBranching(SNode node, NamespaceScope& scope, /*WarningScope& warningScope, */int mode = 0);
   void analizeCode(SNode node, NamespaceScope& scope/*, WarningScope& warningScope*/);
   void analizeMethod(SNode node, NamespaceScope& scope/*, WarningScope& warningScope*/);
   void analizeClassTree(SNode node, ClassScope& scope/*, WarningScope& warningScope*/);
   void analizeSymbolTree(SNode node, Scope& scope);

   void defineEmbeddableAttributes(ClassScope& scope, SyntaxTree::Node node);

//   void createPackageInfo(_Module* module, _ProjectManager& project);
//
   bool compileDeclarations(SNode node, NamespaceScope& scope, bool& repeatMode);
   void compileImplementations(SNode node, NamespaceScope& scope);

//   void generateSyntaxTree(SyntaxWriter& writer, SNode node, ModuleScope& scope, SyntaxTree& autogenerated);

   //void generateListMember(_CompilerScope& scope, ref_t listRef, LexicalType type, ref_t argument);

   void generateClassSymbol(SyntaxWriter& writer, ClassScope& scope);
//   void generateSymbolWithInitialization(SyntaxWriter& writer, ClassScope& scope, ref_t implicitConstructor);

public:
   void loadRules(StreamReader* optimization);
   void turnOnOptimiation(int level)
   {
      _optFlag |= level;
   }

   // return true if no forward class declarations are encountered
   bool declareModule(SyntaxTree& tree, _CompilerScope& scope, ident_t path, ident_t ns, IdentifierList* imported, bool& repeatMode);
   void compileModule(SyntaxTree& syntaxTree, _CompilerScope& scope, ident_t path, ident_t ns, IdentifierList* imported/*, Unresolveds& unresolveds*/);

//   void compileSyntaxTree(_ProjectManager& project, ident_t file, SyntaxTree& tree, ModuleInfo& moduleInfo, Unresolveds& unresolveds);

   void initializeScope(ident_t name, _CompilerScope& scope, bool withDebugInfo);

//   void validateUnresolved(Unresolveds& unresolveds, _ProjectManager& project);

   // _Compiler interface implementation
   //virtual void injectVirtualReturningMethod(SyntaxWriter& writer, ref_t messagRef, LexicalType type, int argument);
   virtual void injectBoxing(SyntaxWriter& writer, _CompilerScope& scope, LexicalType boxingType, int argument, ref_t targetClassRef, bool arrayMode = false);
   virtual void injectLocalBoxing(SNode node, int size);
   virtual void injectConverting(SyntaxWriter& writer, LexicalType convertOp, int convertArg, LexicalType createOp, int createArg, ref_t targetClassRef, 
      ref_t targetRef, bool stacksafe);
   virtual void injectEmbeddableGet(SNode assignNode, SNode callNode, ref_t subject);
   virtual void injectEmbeddableOp(SNode assignNode, SNode callNode, ref_t subject, int paramCount, int verb);
////   virtual void injectFieldExpression(SyntaxWriter& writer);
   virtual void injectEmbeddableConstructor(SNode classNode, ref_t message, ref_t privateRef, ref_t genericMessage);
   virtual void injectVirtualMultimethod(_CompilerScope& scope, SNode classNode, ref_t message, LexicalType methodType, ref_t parentRef = 0);
   virtual void injectVirtualArgDispatcher(_CompilerScope& scope, SNode classNode, ref_t message, LexicalType methodType);
   virtual void injectVirtualReturningMethod(_CompilerScope& scope, SNode classNode, ref_t message, ident_t variable);
   virtual void injectVirtualDispatchMethod(SNode classNode, ref_t message, LexicalType type, ident_t argument);
   virtual void injectVirtualStaticConstField(_CompilerScope& scope, SNode classNode, ident_t fieldName, ref_t fieldRef);
   virtual void generateListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef);
   virtual void generateOverloadListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef);
   virtual void generateClosedOverloadListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef, ref_t classRef);
   virtual void generateSealedOverloadListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef, ref_t classRef);

   //virtual ref_t readEnumListMember(_CompilerScope& scope, _Module* extModule, MemoryReader& reader);

   Compiler(_CompilerLogic* logic);
};

} // _ELENA_

#endif // compilerH
