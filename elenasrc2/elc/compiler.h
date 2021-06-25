//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class.
//
//                                              (C)2005-2021, by Alexei Rakov
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
      bool   unassigned;

      Parameter()
      {
         offset = -1;
         class_ref = 0;
         element_ref = 0;
         size = 0;
         unassigned = false;
      }
      Parameter(int offset)
      {
         this->offset = offset;
         this->class_ref = 0;
         this->element_ref = 0;
         this->size = 0;
         this->unassigned = false;
      }
      Parameter(int offset, ref_t class_ref)
      {
         this->offset = offset;
         this->class_ref = class_ref;
         this->element_ref = 0;
         this->size = 0;
         this->unassigned = false;
      }
      Parameter(int offset, ref_t class_ref, ref_t element_ref, int size)
      {
         this->offset = offset;
         this->class_ref = class_ref;
         this->element_ref = element_ref;
         this->size = size;
         this->unassigned = false;
      }
      Parameter(int offset, ref_t class_ref, ref_t element_ref, int size, bool unassigned)
      {
         this->offset = offset;
         this->class_ref = class_ref;
         this->element_ref = element_ref;
         this->size = size;
         this->unassigned = unassigned;
      }
   };

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

   enum ObjectKind
   {
      okUnknown = 0,

      okObject,                       // param - class reference
      okSymbol,                       // param - reference
      okConstantSymbol,               // param - reference
      okClass,                        // param - reference
      okSingleton,                    // param - reference
      okLiteralConstant,              // param - reference
      okWideLiteralConstant,          // param - reference
      okCharConstant,                 // param - reference
      okIntConstant,                  // param - reference, extraparam - imm argument
      okUIntConstant,                 // param - reference, extraparam - imm argument
      okLongConstant,                 // param - reference
      okRealConstant,                 // param - reference
      okMessageConstant,              // param - reference
      okExtMessageConstant,           // param - reference
      okMessageNameConstant,          // param - reference
      okArrayConst,
      okField,                        // param - reference, param - field offset
      okReadOnlyField,                // param - reference, param - field offset
      okStaticField,                  // param - reference
      okStaticConstantField,          // param - reference
      okClassStaticConstantField,     // param - class reference / 0 (for static methods), extraparam - field offset
      okFieldAddress,                 // param - field offset
      okReadOnlyFieldAddress,         // param - field offset, extraparam - class reference
      okOuter,                        // param - field offset
      okOuterField,                   // param - field offset, extraparam - outer field offset
      okOuterReadOnlyField,           // param - field offset, extraparam - outer field offset
      okOuterSelf,                    // param - field offset, extraparam - outer field offset
//      okOuterStaticField,             // param - field offset, extraparam - outer field offset
//      okClassStaticField,             // param - class reference / 0 (for static methods), extraparam - field offset
      okLocal,                        // param - local / out parameter offset, extraparam : class reference
      okTempLocal,
      okTempBoxableLocal,
      okBoxableLocal,
      okOutputBoxableLocal,
      okParam,                        // param - parameter offset, extraparam = class reference
      okParamField,
      okMessage,                      // param - parameter offset
      okSelfParam,                    // param - parameter offset, extraparam = -1 (stack allocated) / -2 (primitive array)
      okNil,
      okSuper,
      okTempLocalAddress,             // param - local offset
      okLocalAddress,                 // param - local offset
      okParams,                       // param - local offset
      okConstantRole,                 // param - overridden message, reference - role reference
      okExplicitConstant,             // param - reference, extraparam - subject
      okExtension,
      okClassSelf,                    // param - class reference; used in class resending expression
      okMetaField,                    // param - meta attribute id
      okInternalSelf,
      okNewOp,
      okCastOp,
      okMessageRef,

      okExternal,
      okInternal,
      okClosureInfo,
      okMemberInfo,
      //okPrimitive,                    // param * 4 = size
      //okPrimCollection                // param - length
   };

   struct ObjectInfo
   {
      ObjectKind kind;
      ref_t      param;
      // target class reference
      ref_t      reference;
      ref_t      element;
      ref_t      extraparam;

      ObjectInfo()
      {
         this->kind = okUnknown;
         this->param = 0;
         this->reference = 0;
         this->element = 0;
         this->extraparam = 0;
      }
      ObjectInfo(ObjectKind kind)
      {
         this->kind = kind;
         this->param = 0;
         this->reference = 0;
         this->extraparam = 0;
         this->element = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param)
      {
         this->kind = kind;
         this->param = param;
         this->reference = 0;
         this->element = 0;
         this->extraparam = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param, ref_t reference)
      {
         this->kind = kind;
         this->param = param;
         this->reference = reference;
         this->element = 0;
         this->extraparam = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param, ref_t reference, ref_t element, ref_t extraparam)
      {
         this->kind = kind;
         this->param = param;
         this->reference = reference;
         this->element = element;
         this->extraparam = extraparam;
      }
   };

   typedef MemoryMap<ident_t, Parameter>  LocalMap;
   typedef CachedList<ObjectInfo, 5> ArgumentsInfo;

private:
   // - Scope -
   struct Scope : _CompileScope
   {
      enum class ScopeLevel
      {
         slNamespace,
         slClass,
         slSymbol,
         slMethod,
         slCode,
         slYieldScope,
         slExpression,
         slOwnerClass,
      };

      _Module*      module;
      Scope*        parent;

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
      virtual bool resolveAutoOutput(ref_t reference)
      {
         if (parent) {
            return parent->resolveAutoOutput(reference);
         }
         else return false;
      }

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, EAttr mode)
      {
         if (parent) {
            return parent->mapTerminal(identifier, referenceOne, mode);
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

      virtual void markAsAssigned(ObjectInfo)
      {
         // by default is not implemented
      }

      Scope(_ModuleScope* moduleScope)
      {
         this->moduleScope = moduleScope;
         this->parent = NULL;
         this->module = moduleScope->module;
      }
      Scope(Scope* parent)
      {
         this->moduleScope = parent->moduleScope;
         this->ns = parent->ns;
         this->parent = parent;
         this->module = parent->module;
      }
   };

   // - NamespaceScope -
   struct NamespaceScope : Scope
   {
      // imported namespaces
      IdentifierList    importedNs;
      ForwardMap        forwards;       // forward declarations

      // symbol hints
      Map<ref_t, ref_t> constantHints;
      Map<ref_t, int>   intConstants;

      // extensions
      Map<ref_t, ref_t> extensionDispatchers;
      Map<ref_t, ref_t> extensionTargets;
      ExtensionMap      extensions;
      ExtensionTmplMap  extensionTemplates;

      ExtensionMap      declaredExtensions;

      // COMPILER MAGIC : used for extension template compilation
      ExtensionMap*     outerExtensionList;

//      // list of references to the current module which should be checked after the project is compiled
//      Unresolveds* forwardsUnresolved;

      Visibility        defaultVisibility;

      IdentifierString  nsName;
      IdentifierString  name;
      IdentifierString  sourcePath;

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == ScopeLevel::slNamespace) {
            return this;
         }
         else return Scope::getScope(level);
      }

      void defineConstantSymbol(ref_t reference, ref_t classReference)
      {
         constantHints.add(reference, classReference);
      }

      void defineIntConstant(ref_t reference, int value)
      {
         intConstants.add(reference, value);
      }

      virtual void raiseError(const char* message)
      {
         Scope::raiseError(message);
      }
      virtual void raiseError(const char* message, SNode terminal)
      {
         ident_t path = sourcePath;
         if (terminal.existChild(lxSourcePath)) {
            path = terminal.findChild(lxSourcePath).identifier();
         }

         moduleScope->raiseError(message, path, terminal);
      }
      virtual void raiseWarning(int level, const char* message, SNode terminal)
      {
         ident_t path = sourcePath;
         if (terminal.existChild(lxSourcePath)) {
            path = terminal.findChild(lxSourcePath).identifier();
         }

         moduleScope->raiseWarning(level, message, path, terminal);
      }

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, EAttr mode);

      ObjectInfo mapGlobal(ident_t identifier);
      ObjectInfo mapWeakReference(ident_t identifier, bool directResolved);

      virtual pos_t saveSourcePath(ByteCodeWriter& writer);
      virtual pos_t saveSourcePath(ByteCodeWriter& writer, ident_t path);

      ref_t resolveImplicitIdentifier(ident_t name, bool referenceOne, bool innermost);

      ref_t mapNewTerminal(SNode terminal, Visibility visibility);

      ObjectInfo __FASTCALL defineObjectInfo(ref_t reference, bool checkState = false);

      void loadExtensions(ident_t ns);
      void loadExtensions(ident_t ns, ident_t subns, bool internalOne)
      {
         IdentifierString fullName(ns);
         if (internalOne)
            fullName.append(PRIVATE_PREFIX_NS, getlength(PRIVATE_PREFIX_NS) - 1); // HOTFIX : to exclude the tailing quote symbol

         if (!emptystr(subns)) {
            fullName.append("'");
            fullName.append(subns);
         }
         loadExtensions(fullName.c_str());
      }

      ref_t resolveExtensionTarget(ref_t reference);

      void saveExtension(mssg_t message, ref_t extRef, mssg_t strongMessage, bool internalOne);
      void saveExtensionTemplate(mssg_t message, ident_t pattern);

      void loadModuleInfo(ident_t name)
      {
         loadExtensions(name);
      }

//      bool defineForward(ident_t forward, ident_t referenceName)
//      {
//         ObjectInfo info = mapTerminal(referenceName, true, EAttr::eaNone);
//
//         return forwards.add(forward, info.param, true);
//      }

      NamespaceScope(_ModuleScope* moduleScope, ExtensionMap* outerExtensionList);
      NamespaceScope(NamespaceScope* parent);
   };

   // - SourceScope -
   struct SourceScope : public Scope
   {
      ref_t      reference;
      Visibility visibility;

      SourceScope(Scope* parent, ref_t reference, Visibility visibility);
   };

   // - ClassScope -
   struct ClassScope : public SourceScope
   {
      ClassInfo   info;
      ref_t       extensionClassRef;
      bool        stackSafe;
      bool        classClassMode;
      bool        abstractMode;
      bool        abstractBaseMode;
      bool        withInitializers;
      bool        extensionDispatcher;

      ObjectInfo mapField(ident_t identifier, EAttr scopeMode);

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, EAttr mode);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == ScopeLevel::slClass || level == ScopeLevel::slOwnerClass) {
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

      bool checkAttribute(mssg_t message, int attribute)
      {
         ClassInfo::Attribute attr(message, attribute);

         return info.methodHints.exist(attr);
      }

      ref_t getAttribute(mssg_t message, int attribute)
      {
         ClassInfo::Attribute attr(message, attribute);

         return info.methodHints.get(attr);
      }

      void addAttribute(mssg_t message, int attribute, ref_t value)
      {
         ClassInfo::Attribute attr(message, attribute);

         info.methodHints.exclude(attr);
         info.methodHints.add(attr, value);
      }
      int getHint(mssg_t message)
      {
         ClassInfo::Attribute attr(message, maHint);

         return info.methodHints.get(attr);
      }
      void addHint(mssg_t message, int hint)
      {
         ClassInfo::Attribute attr(message, maHint);

         hint |= info.methodHints.get(attr);
         info.methodHints.exclude(attr);
         info.methodHints.add(attr, hint);
      }
      void removeHint(mssg_t message, int hintToRemove)
      {
         ClassInfo::Attribute attr(message, maHint);

         int hints = info.methodHints.get(attr);
         hints &= ~hintToRemove;
         info.methodHints.exclude(attr);
         if (hints != 0)
            info.methodHints.add(attr, hints);
      }

      bool include(mssg_t message)
      {
         // check if the method is inhreited and update vmt size accordingly
         auto it = info.methods.getIt(message);
         if (it.Eof()) {
            info.methods.add(message, true);

            return true;
         }
         else {
            (*it) = true;

            return false;
         }
      }

      ClassScope(Scope* parent, ref_t reference, Visibility visibility);
      ClassScope(NamespaceScope* parent, Visibility visibility)
         : ClassScope(parent, 0, visibility)
      {
      }
   };

   // - SymbolScope -
   struct SymbolScope : public SourceScope
   {
      SymbolExpressionInfo info;
      bool                 staticOne;
      bool                 preloaded;

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == ScopeLevel::slSymbol) {
            return this;
         }
         else return Scope::getScope(level);
      }

      void save();

      SymbolScope(NamespaceScope* parent, ref_t reference, Visibility visibility);
      SymbolScope(NamespaceScope* parent, Visibility visibility)
         : SymbolScope(parent, 0, visibility)
      {
      }
   };

   // - MethodScope -
   struct MethodScope : public Scope
   {
      mssg_t       message;
      LocalMap     parameters;
      EAttr        scopeMode;
      int          reserved1;             // defines managed frame size
      int          reserved2;             // defines unmanaged frame size (excluded from GC frame chain)
      int          hints;
      ref_t        outputRef;
      bool         withOpenArg;
      bool         classStacksafe;
      bool         generic;
      bool         mixinFunction;
      bool         extensionMode;
      bool         multiMethod;
      bool         functionMode;
      bool         nestedMode;
      bool         abstractMethod;
//      bool         yieldMethod;
      bool         embeddableRetMode;
      bool         targetSelfMode;        // used for script generated methods - self refers to __target
      bool         constMode;

      ref_t __FASTCALL getAttribute(MethodAttribute attr, bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? ScopeLevel::slOwnerClass : ScopeLevel::slClass);

         return scope->getAttribute(message, attr);
      }

      ref_t getAttribute(mssg_t attrMessage, MethodAttribute attr, bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? ScopeLevel::slOwnerClass : ScopeLevel::slClass);

         return scope->getAttribute(attrMessage, attr);
      }

      void addAttribute(MethodAttribute attr, ref_t argument, bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? ScopeLevel::slOwnerClass : ScopeLevel::slClass);

         scope->addAttribute(message, attr, argument);
      }

      bool __FASTCALL isPrivate()
      {
         return (hints & tpMask) == tpPrivate;
      }

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == ScopeLevel::slMethod) {
            return this;
         }
         else return parent->getScope(level);
      }

      ref_t __FASTCALL getReturningRef(bool ownerClass = true)
      {
         if (outputRef == INVALID_REF) {
            ClassScope* scope = (ClassScope*)getScope(ownerClass ? ScopeLevel::slOwnerClass : ScopeLevel::slClass);

            outputRef = scope ? scope->info.methodHints.get(ClassInfo::Attribute(message, maReference)) : 0;
         }
         return outputRef;
      }

      Visibility __FASTCALL getClassVisibility(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? ScopeLevel::slOwnerClass : ScopeLevel::slClass);

         return scope->visibility;
      }

      ref_t __FASTCALL getClassFlags(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? ScopeLevel::slOwnerClass : ScopeLevel::slClass);

         return scope ? scope->info.header.flags : 0;
      }
      ref_t __FASTCALL getClassRef(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? ScopeLevel::slOwnerClass : ScopeLevel::slClass);

         return scope ? scope->reference : 0;
      }

      virtual bool resolveAutoOutput(ref_t reference)
      {
         if (outputRef == V_AUTO) {
            outputRef = reference;

            return true;
         }
         else return Scope::resolveAutoOutput(reference);
      }

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, EAttr mode);

      ObjectInfo __FASTCALL mapSelf();
      ObjectInfo __FASTCALL mapGroup();
      ObjectInfo mapParameter(Parameter param, EAttr mode);

      MethodScope(ClassScope* parent);
   };

//   struct YieldScope : public Scope
//   {
//      List<SNode> yieldLocals;
//      List<SNode> yieldContext;
//
//      virtual Scope* getScope(ScopeLevel level)
//      {
//         if (level == ScopeLevel::slYieldScope) {
//            return this;
//         }
//         else return parent->getScope(level);
//      }
//
//      YieldScope(MethodScope* parent);
//   };

   // - CodeScope -
   struct CodeScope : public Scope
   {
      // scope local variables
      LocalMap     locals;
      bool         genericMethod;
      bool         withRetStatement;

      int reserved1, allocated1; // managed scope stack allocation
      int reserved2, allocated2; // unmanaged scope stack allocation

      int newLocal()
      {
         allocated1++;
         if (allocated1 > reserved1)
            reserved1 = allocated1;

         return allocated1;
      }

      void mapLocal(ident_t local, int level)
      {
         locals.add(local, Parameter(level));
      }
      void mapLocal(ident_t local, int level, ref_t class_ref/*, int size*/)
      {
         locals.add(local, Parameter(level, class_ref/*, size*/));
      }
      void mapLocal(ident_t local, int level, ref_t class_ref, ref_t element_ref, int size)
      {
         locals.add(local, Parameter(level, class_ref, element_ref, size));
      }
      void mapLocal(ident_t local, int level, ref_t class_ref, ref_t element_ref, int size, bool unassigned)
      {
         locals.add(local, Parameter(level, class_ref, element_ref, size, unassigned));
      }

      virtual void markAsAssigned(ObjectInfo object);

      ObjectInfo mapLocal(ident_t identifier);

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, EAttr mode);
      virtual bool resolveAutoType(ObjectInfo& info, ref_t reference, ref_t element);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == ScopeLevel::slCode) {
            return this;
         }
         else return parent->getScope(level);
      }

      mssg_t getMessageID()
      {
         MethodScope* scope = (MethodScope*)getScope(ScopeLevel::slMethod);

         return scope ? scope->message : 0;
      }

      ref_t __FASTCALL getReturningRef()
      {
         MethodScope* scope = (MethodScope*)getScope(ScopeLevel::slMethod);

         return scope ? scope->getReturningRef() : 0;
      }

      ref_t getClassRefId(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? ScopeLevel::slOwnerClass : ScopeLevel::slClass);

         return scope ? scope->reference : 0;
      }

      ref_t __FASTCALL getClassFlags(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? ScopeLevel::slOwnerClass : ScopeLevel::slClass);

         return scope ? scope->info.header.flags : 0;
      }

      bool withEmbeddableRet()
      {
         MethodScope* scope = (MethodScope*)getScope(ScopeLevel::slMethod);

         return scope ? scope->embeddableRetMode : false;
      }

      void syncStack(MethodScope* methodScope)
      {
         if (reserved1 < allocated1) {
            reserved1 = allocated1;
         }
         if (reserved2 < allocated2) {
            reserved2 = allocated2;
         }

         if (reserved1 > methodScope->reserved1)
            methodScope->reserved1 = reserved1;

         if (reserved2 > methodScope->reserved2)
            methodScope->reserved2 = reserved2;
      }

      void syncStack(CodeScope* codeScope)
      {
         if (reserved1 > codeScope->reserved1)
            codeScope->reserved1 = reserved1;

         if (reserved2 > codeScope->reserved2)
            codeScope->reserved2 = reserved2;
      }

      CodeScope(SourceScope* parent);
      CodeScope(MethodScope* parent);
      CodeScope(CodeScope* parent);
//      CodeScope(YieldScope* parent);
   };

   struct ExprScope : public Scope
   {
//      SNode exprNode;

      bool ignoreDuplicates; // used for code templates, should be applied only to the statement

      int  tempAllocated1;
      int  tempAllocated2;

      Map<ClassInfo::Attribute, int> tempLocals;
//      Map<int, int> originals;

      virtual void markAsAssigned(ObjectInfo object)
      {
         parent->markAsAssigned(object);
      }

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == ScopeLevel::slExpression) {
            return this;
         }
         else return parent->getScope(level);
      }

      int newTempLocal();
      int newTempLocalAddress();

      mssg_t getMessageID()
      {
         MethodScope* scope = (MethodScope*)getScope(ScopeLevel::slMethod);

         return scope ? scope->message : 0;
      }

      ref_t getClassRefId(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? ScopeLevel::slOwnerClass : ScopeLevel::slClass);

         return scope ? scope->reference : 0;
      }

//      ref_t getAttribute(mssg_t message, MethodAttribute attr, bool ownerClass = true)
//      {
//         ClassScope* scope = (ClassScope*)getScope(ownerClass ? ScopeLevel::slOwnerClass : ScopeLevel::slClass);
//
//         return scope->getAttribute(message, attr);
//      }
//
//      ref_t getAttribute(MethodAttribute attr)
//      {
//         MethodScope* methodScope = (MethodScope*)getScope(Scope::ScopeLevel::slMethod);
//         return methodScope ? methodScope->getAttribute(attr) : 0;
//      }
//
//      bool isInitializer()
//      {
//         return getMessageID() == moduleScope->init_message;
//      }

      // check if a local was declared in one of nested code scopes
      bool checkLocal(ident_t local)
      {
         ObjectInfo info = mapTerminal(local, false, EAttr::eaNone);
         return info.kind == okLocal || info.kind == okLocalAddress;
      }

//      void setCodeRetStatementFlag(bool retStatement)
//      {
//         if (retStatement) {
//            CodeScope* codeScope = (CodeScope*)getScope(Scope::ScopeLevel::slCode);
//            codeScope->withRetStatement = true;
//         }
//      }

      ObjectInfo mapGlobal(ident_t identifier);
      ObjectInfo mapMember(ident_t identifier);

      ExprScope(CodeScope* parent);
      ExprScope(SourceScope* parent);
   };

   // --- ResendScope ---
   struct ResendScope : public ExprScope
   {
      bool withFrame;
      bool constructionMode;

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, EAttr mode);

      ResendScope(CodeScope* parent)
         : ExprScope(parent)
      {
         constructionMode = withFrame = false;
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

//      bool                    returningMode;
      Map<ident_t, Outer>     outers;
//      ClassInfo::FieldTypeMap outerFieldTypes;

      Outer mapSelf();
      Outer mapOwner();
      Outer mapParent();

//      ObjectInfo allocateRetVar();

      bool markAsPresaved(ObjectInfo object);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == ScopeLevel::slClass) {
            return this;
         }
         else return Scope::getScope(level);
      }

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, EAttr mode);

      InlineClassScope(ExprScope* owner, ref_t reference);
   };

   _CompilerLogic*   _logic;
   ByteCodeWriter    _writer;
   MessageMap        _operators;                        // list of operators
   MessageMap        _unaryOperators;

   // optimization rules
   int               _optFlag;
   bool              _autoSystemImport;
//   bool              _dynamicDispatching;
//   bool              _stackEvenMode;
   bool              _trackingUnassigned;
   TransformTape     _rules;
   SyntaxTrie        _sourceRules;
//   int               _reservedAling;

   // optmimization routines
   bool applyRules(CommandTape& tape);
   bool optimizeIdleBreakpoints(CommandTape& tape);
   bool optimizeJumps(CommandTape& tape);
   void optimizeTape(CommandTape& tape);

   void validateType(Scope& scope, SNode current, ref_t typeRef, bool ignoreUndeclared, bool allowType);

   bool calculateIntOp(int operation_id, int arg1, int arg2, int& retVal);
   bool calculateRealOp(int operation_id, double arg1, double arg2, double& retVal);

   bool isDefaultOrConversionConstructor(Scope& scope, mssg_t message, bool& isProtectedDefConst);
   bool __FASTCALL isSelfCall(ObjectInfo info);

   bool __FASTCALL isMethodEmbeddable(MethodScope& scope);

   bool __FASTCALL isConstantList(ArgumentsInfo& members);

   ref_t retrieveImplicitIdentifier(NamespaceScope& scope, ident_t identifier, bool referenceOne, bool innermost);

   void writeMessageInfo(SyntaxWriter& writer, _ModuleScope& scope, mssg_t messageRef);
   void initialize(ClassScope& scope, MethodScope& methodScope);

   ref_t resolveMessageOwnerReference(_ModuleScope& scope, ClassInfo& classInfo, ref_t reference, mssg_t message,
      bool ignoreSelf = false);

   int checkMethod(_ModuleScope& scope, ref_t reference, mssg_t message)
   {
      _CompilerLogic::ChechMethodInfo dummy;

      return _logic->checkMethod(scope, reference, message, dummy, false);
   }

   int checkMethod(_ModuleScope& scope, ref_t reference, mssg_t message, ref_t& protectedRef)
   {
      _CompilerLogic::ChechMethodInfo dummy;

      int retVal = _logic->checkMethod(scope, reference, message, dummy, true);

      protectedRef = dummy.protectedRef;

      return retVal;
   }

   bool loadAttributes(_ModuleScope& scope, ident_t name, MessageMap* attributes, bool silentMode);

   static ObjectInfo mapClassSymbol(Scope& scope, int classRef);

   ref_t __FASTCALL resolveMultimethod(ClassScope& scope, mssg_t messageRef);

   virtual ref_t resolvePrimitiveReference(_CompileScope& scope, ref_t argRef, ref_t elementRef, bool declarationMode);

   ref_t resolvePrimitiveArray(_CompileScope& scope, ref_t templateRef, ref_t elementRef, bool declarationMode);

   ref_t resolveReferenceTemplate(_CompileScope& scope, ref_t operandRef, bool declarationMode);

   ref_t resolveConstantObjectReference(_CompileScope& scope, ObjectInfo object);
   ref_t __FASTCALL resolveObjectReference(_CompileScope& scope, ObjectInfo object, bool noPrimitivesMode,
      bool unboxWrapper = true);

   ref_t resolveTypeIdentifier(Scope& scope, ident_t terminal, LexicalType terminalType,
      bool declarationMode, bool extensionAllowed);
   ref_t resolveTypeIdentifier(Scope& scope, SNode terminal, bool declarationMode, bool extensionAllowed);

   ref_t resolveConstant(ObjectInfo retVal, ref_t& parentRef);
   ref_t generateConstant(_CompileScope& scope, ObjectInfo retVal);

   void saveExtension(ClassScope& scope, mssg_t message, bool internalOne);
   ref_t mapExtension(Scope& scope, mssg_t& messageRef, ref_t implicitSignatureRef, ObjectInfo target, int& stackSafeAttr);

   void importCode(SyntaxWriter& writer, SNode node, Scope& scope, ref_t reference, mssg_t message);

   int defineFieldSize(Scope& scope, int offset);

   InheritResult inheritClass(ClassScope& scope, ref_t parentRef, bool ignoreFields, bool ignoreSealed);
//   void inheritClassConstantList(_ModuleScope& scope, ref_t sourceRef, ref_t targetRef);

   // NOTE : the method is used to set template pseudo variable
   void declareProcedureDebugInfo(SyntaxWriter& writer, SNode node, MethodScope& scope, bool withSelf, bool withTargetSelf);
   void declareCodeDebugInfo(SyntaxWriter& writer, SNode node, MethodScope& scope);

   int resolveSize(SNode node, Scope& scope);
   ref_t resolveParentRef(SNode node, Scope& moduleScope, bool silentMode);
////   bool isDependentOnNotDeclaredClass(SNode baseNode, Scope& scope);

////   bool isValidAttributeType(Scope& scope, _CompilerLogic::FieldAttributes& attrs);

   void resolveMetaConstant(SNode node);

   void compileParentDeclaration(SNode baseNode, ClassScope& scope, ref_t parentRef, bool ignoreFields = false);
   void compileParentDeclaration(SNode node, ClassScope& scope, bool extensionMode, LexicalType parentType);
   void generateClassFields(SNode member, ClassScope& scope, bool singleField);
   void validateClassFields(SNode node, ClassScope& scope);

   void declareSymbolAttributes(SNode node, SymbolScope& scope, bool declarationMode, bool ignoreType);
   void declareClassAttributes(SNode node, ClassScope& scope, bool visibilityOnly);
   void declareFieldAttributes(SNode member, ClassScope& scope, _CompilerLogic::FieldAttributes& attrs);
   void declareVMT(SNode member, ClassScope& scope, bool& withConstructors, bool& withDefaultConstructor);

   ref_t mapTemplateAttribute(SNode node, Scope& scope);
   void declareMethodAttributes(SNode member, MethodScope& scope);

   bool resolveAutoType(ObjectInfo source, ObjectInfo& target, ExprScope& scope);

   mssg_t resolveVariadicMessage(Scope& scope, mssg_t message);
   ref_t resolveOperatorMessage(Scope& scope, ref_t operator_id, pos_t paramCount);
   ref_t resolveMessageAtCompileTime(ObjectInfo target, ExprScope& scope, mssg_t generalMessageRef, ref_t implicitSignatureRef,
                                     bool withExtension, int& stackSafeAttr, ref_t& resolvedExtensionRef);
   mssg_t mapMessage(SNode node, ExprScope& scope, bool extensionCall, bool newOpCall, bool propMode);
   mssg_t mapMethodName(MethodScope& scope, int paramCount, ref_t actionRef, int flags,
      IdentifierString& actionStr, ref_t* signature, size_t signatureLen,
      bool withoutWeakMessages, bool noSignature);

   size_t resolveArraySize(SNode node, Scope& scope);

   ref_t resolveTypeAttribute(SNode node, Scope& scope, bool declarationMode, bool allowRole);
   ref_t resolveTemplateDeclaration(SNode node, Scope& scope, bool declarationMode);

   ObjectInfo compileSwitchExpression(SyntaxWriter& writer, SNode node, ExprScope& scope);

   bool isVariableDeclaration(SNode node, ExprScope& scope, ref_t& typeReg);
   static void __FASTCALL recognizeExprAttrs(SNode node, EAttr& mode);

   LexicalType declareVariableType(CodeScope& scope, ObjectInfo& variable, ClassInfo& localInfo, int size, bool binaryArray,
                                    int& variableArg, ident_t& className);
   void declareVariable(SyntaxWriter& writer, SNode node, ExprScope& scope, ref_t typeRef, bool canBeIdle);

   ObjectInfo injectImplicitConversion(SyntaxWriter& writer, SNode node, ExprScope& scope,
      ObjectInfo source, ref_t targetRef);

   ObjectInfo compileClosure(SyntaxWriter& writer, SNode node, ExprScope& ownerScope, EAttr mode,
      ArgumentsInfo* preservedArgs);
   ObjectInfo compileClosure(SyntaxWriter& writer, SNode node, ExprScope& ownerScope, InlineClassScope& scope,
      EAttr mode, ArgumentsInfo* preservedArgs);
   ObjectInfo compileCollection(SyntaxWriter& writer, SNode objectNode, ExprScope& scope/*, ObjectInfo target*/, EAttr mode);

   ObjectInfo compileMessageReference(SyntaxWriter& writer, SNode objectNode, SNode argNode, ExprScope& scope);
   ObjectInfo compileSubjectReference(SNode objectNode, ExprScope& scope, EAttr mode);
//   ObjectInfo compileYieldExpression(SNode objectNode, ExprScope& scope, EAttr mode);
//
//   void setSuperTerminal(SNode& node, ExprScope& scope, ObjectInfo object, EAttr mode, LexicalType type);
//   void setParamTerminal(SNode& node, ExprScope& scope, ObjectInfo object, EAttr mode, LexicalType type);
//   void setVariableTerminal(SNode& node, _CompileScope& scope, ObjectInfo object, EAttr mode,
//      LexicalType type, int fixedSize = 0);
//   void setParamFieldTerminal(SNode& node, ExprScope& scope, ObjectInfo object, EAttr mode, LexicalType type);
//   void setParamsTerminal(SNode& node, _CompileScope& scope, ObjectInfo object, EAttr mode, ref_t wrapRef);
//   void appendBoxingInfo(SNode node, _CompileScope& scope, ObjectInfo object, bool noUnboxing,
//      int fixedSize, ref_t targetRef);
//
//   ObjectInfo compileTypeSymbol(SNode node, ExprScope& scope, EAttr mode);

   ObjectInfo saveToTempLocal(SyntaxWriter& writer, ExprScope& scope, ObjectInfo retVal);

   ObjectInfo compileOperation(SyntaxWriter& writer, SNode node, ExprScope& scope, int operator_id, ObjectInfo loperand,
      ArgumentsInfo* arguments, bool assignMode, bool shortCircuitMode, ref_t expectedRef);
   ObjectInfo compileOperation(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode, int operator_id,
      bool assingMode, bool shortCircuitMode, ref_t expectedRef);
   ObjectInfo compileUnaryOperation(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode, int operator_id);
   ObjectInfo compileOperationExpression(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode, ref_t expectedRef);
   ObjectInfo compileUnaryExpression(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode);
   ObjectInfo compileIsNilOperator(SyntaxWriter& writer, SNode node, ExprScope& scope);
   void compileBranchingNodes(SyntaxWriter& writer, SNode loperandNode, ExprScope& scope, ref_t ifReference,
      bool loopMode, bool switchMode);
   void compileBranchingOp(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode, int operator_id,
      ObjectInfo loperand, ObjectInfo& retVal, EAttr operationMode, ArgumentsInfo* preservedArgs);
   ObjectInfo compileBranchingOperation(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode,
      int operator_id, ArgumentsInfo* preservedArgs);

   ref_t resolveStrongArgument(ExprScope& scope, ArgumentsInfo* arguments);

   ObjectInfo compileAssigning(SyntaxWriter& writer, SNode node, ExprScope& scope, ObjectInfo target, EAttr mode);
   ObjectInfo compileAssigningExpression(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode);

   ref_t compileMessageParameters(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode, ref_t expectedSignRef,
      bool& variadicOne, /*bool& inlineArg, */ArgumentsInfo& arguments, ArgumentsInfo* presavedArgs);

   ObjectInfo compileResendMessageOperation(SyntaxWriter& writer, SNode node, ExprScope& scope,
      ObjectInfo target, ref_t exptectedRef, EAttr mode);
   ObjectInfo compileMessageExpression(SyntaxWriter& writer, SNode node, ExprScope& scope, ref_t expectedRef, EAttr mode);
   ObjectInfo compileMessageOperation(SyntaxWriter& writer, SNode current, SNode opNode, ExprScope& scope, ObjectInfo target,
      mssg_t messageRef, ref_t expectedSignRef, EAttr mode, ArgumentsInfo* presavedArgs, ref_t expectedRef);
   ObjectInfo compileMessage(SyntaxWriter& writer, SNode node, ExprScope& scope, ObjectInfo target, mssg_t messageRef,
      ArgumentsInfo* arguments, EAttr mode, int stackSafeAttr, ref_t expectedRef, ArgumentsInfo* presavedArgs);

   SNode injectAttributeIdentidier(SNode current, Scope& scope);
   void compileTemplateAttributes(SNode current, List<SNode>& parameters, Scope& scope, bool declarationMode);
   EAttr recognizeExpressionAttributes(SNode& current, Scope& scope, ref_t& typeRef, bool& newVariable);
   EAttr declareExpressionAttributes(SyntaxWriter& writer, SNode& node, ExprScope& scope, EAttr mode);
//
//   void recognizeTerminal(SNode& node, ObjectInfo info, ExprScope& scope, EAttr mode);

   ObjectInfo mapMetaField(ident_t token);

   ObjectInfo mapIntConstant(ExprScope& scope, int value);
   ObjectInfo mapRealConstant(ExprScope& scope, double val);

   ObjectInfo mapTarget(SNode node, ExprScope& scope);
   ObjectInfo mapTerminal(SNode node, ExprScope& scope, EAttr mode);

   void writeTerminal(SyntaxWriter& writer, ObjectInfo objectInfo, ExprScope& scope);

   ObjectInfo compileNewArrOperation(SyntaxWriter& writer, SNode node, ExprScope& scope, ObjectInfo object,
      ref_t expecteRef, EAttr mode);

   ObjectInfo compileObject(SyntaxWriter& writer, SNode& node, ExprScope& scope,
      EAttr mode, ArgumentsInfo* preservedArgs);
//   ObjectInfo compileExpression(SNode node, ExprScope& scope, ObjectInfo objectInfo, ref_t targetRef, EAttr mode);
   ObjectInfo compileExpression(SyntaxWriter& writer, SNode node, ExprScope& scope, ref_t targetRef,
      EAttr mode, ArgumentsInfo* preservedArgs);

   ObjectInfo compileCastingExpression(SyntaxWriter& writer, SNode node, ExprScope& scope, ObjectInfo target, EAttr mode);
   ObjectInfo compileReferenceExpression(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode);
   ObjectInfo compileVariadicUnboxing(SNode node, ExprScope& scope, EAttr mode);
//   ObjectInfo compileAssigning(SNode node, ExprScope& scope, ObjectInfo target, bool accumulateMode);
   ObjectInfo compileArrAssigning(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode);
   ObjectInfo compilePropAssigning(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode);
   ObjectInfo compileRootExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, ref_t targetRef, EAttr mode);
   ObjectInfo compileRetExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, EAttr mode);
   void compileEmbeddableRetExpression(SyntaxWriter& writer, SNode node, ExprScope& scope);

   ObjectInfo compileSubCode(SyntaxWriter& writer, SNode thenNode, ExprScope& scope, bool branchingMode, bool& withRetStatement);

   bool recognizeCompileTimeAssigning(SNode node, ClassScope& scope);
   void compileCompileTimeAssigning(SNode node, ClassScope& scope);
   void compileStaticAssigning(SNode node, ClassScope& scope/*, int mode*/);
   void compileClassConstantAssigning(ObjectInfo target, SNode node, ClassScope& scope/*, bool accumulatorMode*/);
   void compileMetaConstantAssigning(ObjectInfo target, SNode node, int bookmarkReference, ClassScope& scope);

//   ObjectInfo compileOperation(SNode& node, ExprScope& scope, ObjectInfo objectInfo, ref_t expectedRef,
//      EAttr mode, bool propMode);

   ObjectInfo compileCatchOperator(SyntaxWriter& writer, SNode node, ExprScope& scope, ref_t operator_id);
   ObjectInfo compileAltOperator(SyntaxWriter& writer, SNode node, ExprScope& scope);

   void importClassMembers(SNode classNode, SNode importNode, NamespaceScope& scope);

   int allocateStructure(bool bytearray, int alignment, int& allocatedSize, int& reserved);
   int allocateStructure(SNode node, int alignment, int& size);
   bool allocateStructure(CodeScope& scope, int size, bool binaryArray, ObjectInfo& exprOperand);
   bool allocateTempStructure(ExprScope& scope, int size, bool binaryArray, ObjectInfo& exprOperand);

   ObjectInfo compileExternalCall(SyntaxWriter& writer, SNode node, ExprScope& scope, ref_t expectedRef,
      ArgumentsInfo* arguments);
   ObjectInfo compileInternalCall(SyntaxWriter& writer, SNode node, ExprScope& scope, mssg_t message,
      ref_t signature, ObjectInfo info, ArgumentsInfo* arguments);

//   void warnOnUnresolvedDispatch(SNode node, Scope& scope, mssg_t message, bool errorMode);
   void warnOnUnassignedLocal(SNode node, Scope& scope, int offset);

   void compileConstructorResendExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, ClassScope& classClassScope,
      bool& withFrame);
   void compileConstructorDispatchExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, bool isDefault);
   void compileResendExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, bool multiMethod/*, bool extensionMode*/);
   void compileDispatchExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, bool withGenericMethods);
   void compileMultidispatch(SyntaxWriter& writer, SNode node, CodeScope& scope, ClassScope& classScope);

   ObjectInfo compileCode(SyntaxWriter& writer, SNode node, CodeScope& scope);

   void declareArgumentAttributes(SNode node, Scope& scope, ref_t& classRef, ref_t& elementRef, bool declarationMode);
   void declareArgumentList(SNode node, MethodScope& scope, bool withoutWeakMessages, bool declarationMode);
   ref_t declareInlineArgumentList(SNode node, MethodScope& scope, bool declarationMode);
   bool declareActionScope(ClassScope& scope, SNode argNode, MethodScope& methodScope, EAttr mode);

   void compileActionMethod(SyntaxWriter& writer, SNode member, MethodScope& scope);
   void compileExpressionMethod(SyntaxWriter& writer, SNode member, MethodScope& scope, bool lazyMode);
   void compileDispatcher(SyntaxWriter& writer, SNode node, MethodScope& scope, LexicalType methodType,
      bool withGenericMethods = false, bool withOpenArgGenerics = false);

   void beginMethod(SyntaxWriter& writer, SNode node, MethodScope& scope, LexicalType type);
   void endMethod(SyntaxWriter& writer, MethodScope& scope);
   void compileMethodCode(SyntaxWriter& writer, SNode body, MethodScope& scope, CodeScope& codeScope);

   void predefineMethod(SNode node, ClassScope& classScope, MethodScope& scope);
   void compileEmbeddableMethod(SyntaxWriter& writer, SNode node, MethodScope& scope);
   void compileMethod(SyntaxWriter& writer, SNode node, MethodScope& scope);
   void compileAbstractMethod(SyntaxWriter& writer, SNode node, MethodScope& scope);
   void compileConstructor(SyntaxWriter& writer, SNode node, MethodScope& scope, ClassScope& classClassScope);
   void compileInitializer(SyntaxWriter& writer, SNode node, MethodScope& scope);

//   void compileYieldDispatch(SNode node, MethodScope& scope);
////   void compileYieldEnd(SyntaxWriter& writer, int index);
//   void compileYieldableMethod(SNode node, MethodScope& scope);

   void compileSpecialMethodCall(SyntaxWriter& writer, ClassScope& classScope, mssg_t message);

   void compileDefConvConstructor(SyntaxWriter& writer, SNode node, MethodScope& scope);

   ref_t compileClassPreloadedCode(_ModuleScope& scope, ref_t classRef, SNode node);
   void compilePreloadedCode(SymbolScope& scope);
   void compilePreloadedExtensionCode(ClassScope& scope);

   void compileSymbolCode(SyntaxTree& expressionTree, ClassScope& scope);

   void compileAction(SNode node, ClassScope& scope, SNode argNode, EAttr mode);
   void compileNestedVMT(SNode& node, InlineClassScope& scope);

   void compileVMT(SyntaxWriter& writer, SNode node, ClassScope& scope, bool exclusiveMode = false,
      bool ignoreAutoMultimethods = false);
   void compileClassVMT(SyntaxWriter& writer, SNode node, ClassScope& classClassScope, ClassScope& classScope);

   void compileModuleExtensionDispatcher(NamespaceScope& scope);
   ref_t compileExtensionDispatcher(NamespaceScope& scope, mssg_t genericMessageRef);

   void generateClassField(ClassScope& scope, SNode node, _CompilerLogic::FieldAttributes& attrs, bool singleField);
   void generateClassStaticField(ClassScope& scope, SNode current, ref_t fieldRef, ref_t elementRef, bool isStatic,
      bool isConst, bool isArray);

   void generateClassFlags(ClassScope& scope, SNode node);
   void generateParamNameInfo(ClassScope& scope, SNode node, mssg_t message);
   void generateMethodAttributes(ClassScope& scope, SNode node, mssg_t message, bool allowTypeAttribute);

   void generateMethodDeclaration(SNode current, ClassScope& scope, bool hideDuplicates, bool closed,
      bool allowTypeAttribute);
   void generateMethodDeclarations(SNode node, ClassScope& scope, bool closed, LexicalType methodType,
      bool allowTypeAttribute);
   void generateClassDeclaration(SNode node, ClassScope& scope, bool nestedDeclarationMode = false);

   void generateClassImplementation(SNode node, ClassScope& scope);

   void compileClassDeclaration(SNode node, ClassScope& scope);
   void compileClassImplementation(SyntaxTree& expressionTree, SNode node, ClassScope& scope);
   void compileClassClassDeclaration(SNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileClassClassImplementation(SyntaxTree& expressionTree, SNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileSymbolDeclaration(SNode node, SymbolScope& scope);
   void compileSymbolImplementation(SyntaxTree& expressionTree, SNode node, SymbolScope& scope);
   bool compileSymbolConstant(SymbolScope& scope, ObjectInfo retVal/*, bool accumulatorMode, ref_t accumulatorRef*/);
   void compileSymbolAttribtes(_ModuleScope& scope, ref_t reference, bool publicAttr);

   ObjectInfo allocateResult(ExprScope& scope, /*bool fpuMode, */ref_t targetRef, ref_t elementRef = 0);

   // NOTE : if the conversion is not possible - the methods return unknown result
   ObjectInfo convertObject(SyntaxWriter& writer, SNode node, ExprScope& scope, ref_t targetRef, ObjectInfo source, EAttr mode);
   ObjectInfo sendTypecast(SyntaxWriter& writer, SNode node, ExprScope& scope, ref_t targetRef, ObjectInfo source, bool silentMode);

   void compileExternalArguments(SyntaxWriter& writer, SNode node, ExprScope& scope, ArgumentsInfo* arguments);

   void appendCopying(SyntaxWriter& writer, int size, bool variadic, /*bool primArray, */ObjectInfo target,
      ExprScope& scope, int tempLocal);
   void appendCreating(SyntaxWriter& writer, int size, ref_t typeRef);

//   void boxExpressionInRoot(SNode boxNode, SNode objNode, ExprScope& scope, LexicalType tempType,
//      int tempLocal, bool localBoxingMode, bool condBoxing);
//
//   void injectMemberPreserving(SNode node, ExprScope& scope, LexicalType tempType, int tempLocal,
//      ObjectInfo member, int memberIndex, int& oriTempLocal);
//   void injectIndexBoxing(SNode node, SNode objNode, ExprScope& scope);
   void analizeCodePatterns(SNode node, NamespaceScope& scope);
   void analizeMethod(SNode node, NamespaceScope& scope);
   void analizeClassTree(SNode node, ClassScope& scope, bool(*cond)(LexicalType));
   void analizeSymbolTree(SNode node, Scope& scope);

   ObjectInfo boxExternal(SyntaxWriter& writer, ObjectInfo target, ExprScope& scope);
   ObjectInfo boxArgumentInPlace(SyntaxWriter& writer, SNode boxNode, ObjectInfo arg, ExprScope& scope,
      bool localBoxingMode, bool condBoxing);
   ObjectInfo boxArgumentInPlace(SyntaxWriter& writer, SNode boxNode, ObjectInfo soource, ref_t targetRef, ExprScope& scope,
      bool localBoxingMode, bool condBoxing);
   void boxArgument(SyntaxWriter& writer, SNode boxNode, ObjectInfo& arg, ExprScope& scope,
      bool withLocalBoxing/*, bool inPlace, bool condBoxing*/);
//   void analizeOperand(SNode& node, ExprScope& scope, bool boxingMode, bool withoutLocalBoxing, bool inPlace);
   void analizeArguments(SyntaxWriter& writer, SNode node, ExprScope& scope, int stackSafeAttr,
      ObjectInfo& target, ArgumentsInfo* arguments/*, bool inPlace*/);
   void unboxArgument(SyntaxWriter& writer, ObjectInfo& target, ExprScope& scope);
   void unboxArguments(SyntaxWriter& writer, ExprScope& scope, ObjectInfo& target, ArgumentsInfo* arguments);
   void unboxPreservedArgs(SyntaxWriter& writer, ExprScope& scope, ArgumentsInfo* arguments);

   void defineEmbeddableAttributes(ClassScope& scope, SNode node);

   void createPackageInfo(_Module* module, _ProjectManager& project);

   void declareMembers(SNode node, NamespaceScope& scope);

   bool compileDeclarations(SNode node, NamespaceScope& scope, bool forced, bool& repeatMode);
   void compileImplementations(SNode node, NamespaceScope& scope);

   void generateClassSymbol(SyntaxWriter& writer, ClassScope& scope);

   void copyParentNamespaceExtensions(NamespaceScope& source, NamespaceScope& target);
   void declareNamespace(SNode& node, NamespaceScope& scope, bool withImports, bool withFullInfo);

   void registerExtensionTemplateMethod(SNode node, NamespaceScope& scope, ref_t extensionRef);
   void registerExtensionTemplate(SNode node, NamespaceScope& scope, ref_t extensionRef);
   void registerTemplateSignature(SNode node, NamespaceScope& scope, IdentifierString& signature);

   bool matchTriePatterns(_ModuleScope& scope, SNode& node, SyntaxTrie& trie, List<SyntaxTrieNode>& matchedPatterns);
   bool optimizeTriePattern(_ModuleScope& scope, SNode& node, int patternId);
   bool optimizeConstProperty(_ModuleScope& scope, SNode& node);
   bool optimizeEmbeddable(_ModuleScope& scope, SNode& node);
//   bool optimizeCallDoubleAssigning(_ModuleScope& scope, SNode& node);
   bool optimizeConstantAssigning(_ModuleScope& scope, SNode& node);
//   bool optimizeOpDoubleAssigning(_ModuleScope& scope, SNode& node);
   bool optimizeBranching(_ModuleScope& scope, SNode& node);
//   bool optimizeDispatchingExpr(_ModuleScope& scope, SNode& node);
   bool optimizeTempAllocating(_ModuleScope& scope, SNode& node);

   int saveMetaInfo(_ModuleScope& scope, ident_t info);

   void saveNamespaceInfo(SNode node, NamespaceScope& scope, bool innerMost);
   void declareTemplate(SNode node, NamespaceScope& scope);

public:
   void turnAutoImport(bool value)
   {
      _autoSystemImport = value;
   }

   void loadRules(StreamReader* optimization);
   void loadSourceRules(StreamReader* optimization);
   void turnOnOptimiation(int level)
   {
      _optFlag |= level;
   }

   //void turnOnEvenStack()
   //{
   //   _stackEvenMode = true;
   //}

   void turnOnTrackingUnassigned()
   {
      _trackingUnassigned = true;
   }

   void declareModuleIdentifiers(SyntaxTree& tree, _ModuleScope& scope);

   // return true if no forward class declarations are encountered
   bool declareModule(SyntaxTree& tree, _ModuleScope& scope, bool forced, bool& repeatMode, ExtensionMap* outerExtensionList);
   void compileModule(SyntaxTree& syntaxTree, _ModuleScope& scope, ident_t greeting, ExtensionMap* outerExtensionList);

   void initializeScope(ident_t name, _ModuleScope& scope, bool withDebugInfo, int stackAlignment);

   // _Compiler interface implementation
//   virtual SNode injectTempLocal(SNode node, int size, bool boxingMode);
   //virtual void injectConverting(SNode& node, LexicalType convertOp, int convertArg, LexicalType targetOp, int targetArg,
   //   ref_t targetClassRef, int stacksafeAttr, bool embeddableAttr);
   virtual bool injectEmbeddableOp(_ModuleScope& scope, SNode assignNode, SNode callNode, SNode copyNode, ref_t subject, int paramCount);
   virtual void injectEmbeddableConstructor(SNode classNode, mssg_t message, mssg_t privateRef);
   virtual void injectVirtualMultimethod(_ModuleScope& scope, SNode classNode, mssg_t message,
      LexicalType methodType, ClassInfo& info);
   void injectVirtualMultimethod(_ModuleScope& scope, SNode classNode, mssg_t message, LexicalType methodType,
      mssg_t resendMessage, bool privateOne, ref_t callTargetRef);
   bool injectVirtualStrongTypedMultimethod(_ModuleScope& scope, SNode classNode, mssg_t message, LexicalType methodType,
      mssg_t resendMessage, bool privateOne);
   virtual void injectVirtualReturningMethod(_ModuleScope& scope, SNode classNode, mssg_t message, ident_t variable, ref_t outputRef);
   virtual void injectVirtualDispatchMethod(_ModuleScope& scope, SNode classNode, mssg_t message, LexicalType type, ident_t argument);
//   virtual void injectVirtualField(SNode classNode, LexicalType sourceType, ref_t sourceArg, int postfixIndex);
   virtual void injectDefaultConstructor(_ModuleScope& scope, SNode classNode, ref_t classRef, bool protectedOne);
   virtual void injectExprOperation(SNode& node, int size, int tempLocal, LexicalType op, int opArg);
   virtual void generateOverloadListMember(_ModuleScope& scope, ref_t enumRef, ref_t memberRef);
   virtual void generateClosedOverloadListMember(_ModuleScope& scope, ref_t enumRef, ref_t memberRef, ref_t classRef);
   virtual void generateSealedOverloadListMember(_ModuleScope& scope, ref_t enumRef, ref_t memberRef, ref_t classRef);

   void generateResendToEmbeddable(SyntaxTree& tree, MethodScope& scope, ref_t resendMessage, LexicalType methodType);

//   //virtual void registerExtensionTemplate(SyntaxTree& tree, _ModuleScope& scope, ident_t ns, ref_t extensionRef);
   virtual ref_t generateExtensionTemplate(_ModuleScope& scope, ref_t templateRef, size_t argumentLen,
      ref_t* arguments, ident_t ns, ExtensionMap* outerExtensionList);

   bool __FASTCALL localBoxingRequired(_ModuleScope& scope, ObjectInfo& info);
   static bool __FASTCALL boxingRequired(ObjectInfo& info);
   static bool __FASTCALL condBoxingRequired(ObjectInfo& info);
   static bool __FASTCALL unboxingRequired(ObjectInfo& info);

   Compiler(_CompilerLogic* logic);
};

} // _ELENA_

#endif // compilerH
