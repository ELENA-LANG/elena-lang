//---------------------------------------------------------------------------
//              E L E N A   p r o j e c t
//                Command line syntax generator main file
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef WINVER
#define WINVER 0x0500
#endif

#include <windows.h>
#include "elena.h"

#define TITLE "ELENA Standard Library 5.8: Module "
#define TITLE2 "ELENA&nbsp;Standard&nbsp;Library<br>5.8"

constexpr auto ByRefPrefix = "'$auto'system@ref#1&";
constexpr auto ArrayPrefix = "'$auto'system@Array#1&";

#define REVISION_VERSION   6

//#define OPERATORS "+-*/=<>?!"

using namespace _ELENA_;

char buffer[BLOCK_SIZE];

struct ApiFieldInfo
{
   IdentifierString name;
   IdentifierString type;
};

struct ApiSymbolInfo
{
   IdentifierString fullName;
   IdentifierString name;
   IdentifierString type;
};

struct ApiMethodInfo
{
   bool prop, special, convertor, isAbstract, isInternal;
   bool isMultidispatcher, withVargs;
   bool protectedOne;
   bool privateOne;

   IdentifierString       prefix;
   IdentifierString       name;
   IdentifierString       retType;
   IdentifierString       shortdescr;

   List<IdentifierString> params;
   List<IdentifierString> paramNames;

   ApiMethodInfo()
   {
      special = prop = convertor = false;
      isAbstract = isMultidispatcher = false;
      withVargs = isInternal = false;
      privateOne = protectedOne = false;
   }
};

struct ApiClassInfo
{
   bool withStaticProps;
   bool withProps;
   bool withConstructors;
   bool withConversions;
   bool templateBased;
   bool isVirtual;

   IdentifierString prefix;
   IdentifierString fullName;
   IdentifierString name;
   IdentifierString shortdesc;

   List<IdentifierString> parents;
   List<ApiFieldInfo*> fields;
   List<ApiMethodInfo*> methods;
   List<ApiMethodInfo*> constructors;
   List<ApiMethodInfo*> extensions;

   ApiClassInfo()
      : fields(nullptr, freeobj), methods(nullptr, freeobj), constructors(nullptr, freeobj)
   {
      withStaticProps = withProps = false;
      withConstructors = false;
      withConversions = false;
      isVirtual = templateBased = false;
   }
   ~ApiClassInfo()
   {

   }
};

struct ApiModuleInfo
{
   bool withVirtuals, withClasses;

   IdentifierString name;

   List<ApiClassInfo*> classes;
   List<ApiSymbolInfo*> symbols;
   ApiModuleInfo()
      : classes(nullptr, freeobj), symbols(nullptr, freeobj)
   {
      withVirtuals = withClasses = false;
   }
   ~ApiModuleInfo()
   {

   }
};

//typedef String<char, 255> ParamString;

inline bool isTemplateBased(ident_t reference)
{
   for (int i = 0; i < getlength(reference); i++) {
      if (reference[i] == '#' && reference[i + 1] >= '0' && reference[i + 1] <= '9')
         return true;
   }

   return false;
}

void writeNs(IdentifierString& name, ApiModuleInfo* info)
{
   for (int i = 0; i < info->name.Length(); i++)
   {
      if (info->name[i] == '\'') {
         name.append('-');
      }
      else name.append(info->name[i]);
   }
}

void writeNs(TextFileWriter& writer, ident_t ns)
{
   for (int i = 0; i < getlength(ns); i++)
   {
      if (ns[i] == '\'') {
         writer.writeChar('-');
      }
      else writer.writeChar(ns[i]);
   }
}

void writeHeader(TextFileWriter& writer, const char* package, const char* packageLink)
{
   writer.writeLiteralNewLine("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Frameset//EN\"\"http://www.w3.org/TR/REC-html40/frameset.dtd\">");
   writer.writeLiteralNewLine("<HTML>");
   writer.writeLiteralNewLine("<HEAD>");
   writer.writeLiteralNewLine("<TITLE>");
   writer.writeLiteral(TITLE);
   writer.writeLiteralNewLine(package);
   writer.writeLiteralNewLine("</TITLE>");
   writer.writeLiteralNewLine("<meta name=\"collection\" content=\"api\">");
   writer.writeLiteralNewLine("<LINK REL =\"stylesheet\" TYPE=\"text/css\" HREF=\"stylesheet.css\" TITLE=\"Style\">");
   writer.writeLiteralNewLine("</HEAD>");
   writer.writeLiteralNewLine("<BODY BGCOLOR=\"white\">");

   writer.writeLiteralNewLine("<DIV CLASS=\"topNav\">");

   writer.writeLiteralNewLine("<UL CLASS=\"navList\">");

   writer.writeLiteralNewLine("<LI>");
   writer.writeLiteralNewLine("<A HREF=\"index.html\">Overview</A>");
   writer.writeLiteralNewLine("</LI>");

   writer.writeLiteralNewLine("<LI>");
   if (!emptystr(packageLink)) {
      writer.writeLiteral("<A HREF=\"");
      writer.writeLiteral(packageLink);
      writer.writeLiteralNewLine("\">Module</A>");
   }
   else writer.writeLiteralNewLine("Module");
   writer.writeLiteralNewLine("</LI>");

   writer.writeLiteralNewLine("</UL>");

   writer.writeLiteralNewLine("<DIV CLASS=\"aboutLanguage\">");
   writer.writeLiteralNewLine("<STRONG>");
   writer.writeLiteralNewLine(TITLE2);
   writer.writeLiteralNewLine("</STRONG>");
   writer.writeLiteralNewLine("</DIV>");

   writer.writeLiteralNewLine("</DIV>");
}

void writeSummaryHeader(TextFileWriter& writer, const char* name, const char* shortDescr)
{
   writer.writeLiteralNewLine("<DIV CLASS=\"header\">");
   writer.writeLiteralNewLine("<H1>");
   writer.writeLiteral("Module ");
   writer.writeLiteralNewLine(name);
   writer.writeLiteralNewLine("</H1>");
   writer.writeLiteralNewLine("</DIV>");

   writer.writeLiteralNewLine("<DIV CLASS=\"docSummary\">");
   writer.writeLiteral("<DIV CLASS=\"block\">");
   writer.writeLiteral(shortDescr);
   writer.writeLiteralNewLine("</DIV>");
   writer.writeLiteralNewLine("</DIV>");

   writer.writeLiteralNewLine("<DIV CLASS=\"contentContainer\">");
   writer.writeLiteralNewLine("<UL CLASS=\"blockList\">");
}

void writeClassSummaryHeader(TextFileWriter& writer)
{
   writer.writeLiteralNewLine("<LI CLASS=\"blockList\">");
   writer.writeLiteralNewLine("<TABLE CLASS=\"typeSummary\" BORDER=\"0\" CELLPADDING=\"3\" CELLSPACING=\"0\">");
   writer.writeLiteralNewLine("<HEADER>");
   writer.writeLiteralNewLine("Class Summary");
   writer.writeLiteralNewLine("</HEADER>");
   writer.writeLiteralNewLine("<TR>"); 
   writer.writeLiteralNewLine("<TH CLASS=\"colFirst\" scope=\"col\">Class name</TH>"); 
   writer.writeLiteralNewLine("<TH CLASS=\"colLast\" scope=\"col\">Description</TH>");
   writer.writeLiteralNewLine("</TR>");
}

void writeSymbolSummaryHeader(TextFileWriter& writer)
{
   writer.writeLiteralNewLine("<LI CLASS=\"blockList\">");
   writer.writeLiteralNewLine("<TABLE CLASS=\"typeSummary\" BORDER=\"0\" CELLPADDING=\"3\" CELLSPACING=\"0\">");
   writer.writeLiteralNewLine("<HEADER>");
   writer.writeLiteralNewLine("Symbol Summary");
   writer.writeLiteralNewLine("</HEADER>");
   writer.writeLiteralNewLine("<TR>");
   writer.writeLiteralNewLine("<TH CLASS=\"colFirst\" scope=\"col\">Symbol name</TH>");
   writer.writeLiteralNewLine("<TH CLASS=\"colLast\" scope=\"col\">Description</TH>");
   writer.writeLiteralNewLine("</TR>");

}

void writeExtendedSummaryHeader(TextFileWriter& writer)
{
   writer.writeLiteralNewLine("<LI CLASS=\"blockList\">");
   writer.writeLiteralNewLine("<TABLE CLASS=\"typeSummary\" BORDER=\"0\" CELLPADDING=\"3\" CELLSPACING=\"0\">");
   writer.writeLiteralNewLine("<HEADER>");
   writer.writeLiteralNewLine("Extended Class Summary");
   writer.writeLiteralNewLine("</HEADER>");
   writer.writeLiteralNewLine("<TR>");
   writer.writeLiteralNewLine("<TH CLASS=\"colFirst\" scope=\"col\">Extended class name</TH>");
   writer.writeLiteralNewLine("<TH CLASS=\"colLast\" scope=\"col\">Description</TH>");
   writer.writeLiteralNewLine("</TR>");

}

void writeRefName(TextFileWriter& writer, ident_t name, bool allowResolvedTemplates)
{
   int paramIndex = 1;
   bool paramMode = false;
   for (int i = 0; i < getlength(name); i++)
   {
      if (!paramMode && name[i] == '\'') {
         writer.writeChar('-');
      }
      else if (name.compare("&lt;", i, 4)) {
         paramMode = true;
         writer.writeLiteral("&lt;");

         i += 3;
      }
      else if (name.compare("&gt;", i, 4)) {
         if (!allowResolvedTemplates) {
            String<char, 5> tmp;
            tmp.copy("T");
            tmp.appendInt(paramIndex);
            writer.writeLiteral(tmp.c_str());
         }
         writer.writeLiteral("&gt;");
         paramMode = false;

         i += 3;
      }
      else if (name[i] == ',') {
         if (!allowResolvedTemplates) {
            String<char, 5> tmp;
            tmp.copy("T");
            tmp.appendInt(paramIndex);
            writer.writeLiteral(tmp.c_str());

            paramIndex++;
         }
         writer.writeLiteral(",");
      }
      else if (name[i] == '[' && name[i + 1] == ']') {
         // skip array brackets
         i++;
      }
      else if (!paramMode || allowResolvedTemplates)
         writer.writeChar(name[i]);
   }
}

void writeClassName(TextFileWriter& writer, ApiClassInfo* info)
{
   const char* descr = info->shortdesc;
   if (!emptystr(descr)) {
      writer.writeLiteral(descr);
   }
   else {
      if (info->prefix.Length() > 0) {
         writer.writeLiteral(info->prefix.c_str());
      }
      
      writer.writeLiteral("<SPAN CLASS=\"typeNameLabel\">");
      writer.writeLiteral(info->name.c_str());
      writer.writeLiteral("</SPAN>");
   }
}

void writeSummaryTable(TextFileWriter& writer, ApiClassInfo* info, const char* bodyFileName)
{
   writer.writeLiteralNewLine("<TD CLASS=\"colFirst\">");

   writer.writeLiteral("<A HREF=\"");
   writer.writeLiteral(bodyFileName);
   writer.writeLiteral("#");
   writeRefName(writer, info->name.c_str(), false);
   writer.writeLiteral("\">");
   writer.writeLiteral(info->name.c_str());
   writer.writeLiteralNewLine("</A>");
   writer.writeLiteralNewLine("</TD>");


   writer.writeLiteralNewLine("<TD CLASS=\"colLast\">");
   writer.writeLiteralNewLine("<DIV CLASS=\"block\">");
   writeClassName(writer, info);
   writer.writeLiteralNewLine("</DIV>");
   writer.writeLiteralNewLine("</TD>");
}

void writeSymbolSummaryTable(TextFileWriter& writer, ApiSymbolInfo* info, const char* bodyFileName)
{
   writer.writeLiteralNewLine("<TD CLASS=\"colFirst\">");

   writer.writeLiteral("<A HREF=\"");
   writer.writeLiteral(bodyFileName);
   writer.writeLiteral("#");
   writeRefName(writer, info->name.c_str(), false);
   writer.writeLiteral("\">");
   writer.writeLiteral(info->name.c_str());
   writer.writeLiteralNewLine("</A>");
   writer.writeLiteralNewLine("</TD>");

   writer.writeLiteralNewLine("<TD CLASS=\"colLast\">");
   writer.writeLiteralNewLine("<DIV CLASS=\"block\">");
   writer.writeLiteral(info->name.c_str());
   writer.writeLiteralNewLine("</DIV>");
   writer.writeLiteralNewLine("</TD>");
}

void writeExtendedSummaryTable(TextFileWriter& writer, ApiClassInfo* info, const char* bodyFileName)
{
   writer.writeLiteralNewLine("<TD CLASS=\"colFirst\">");

   writer.writeLiteral("<A HREF=\"");
   writer.writeLiteral(bodyFileName);
   writer.writeLiteral("#ext-");
   writeRefName(writer, info->name.c_str(), true);
   writer.writeLiteral("\">");
   writer.writeLiteral(info->name.c_str());
   writer.writeLiteralNewLine("</A>");
   writer.writeLiteralNewLine("</TD>");

   writer.writeLiteralNewLine("<TD CLASS=\"colLast\">");
   writer.writeLiteralNewLine("<DIV CLASS=\"block\">");
   writer.writeLiteral(info->name.c_str());
   writer.writeLiteralNewLine("</DIV>");
   writer.writeLiteralNewLine("</TD>");
}

inline void repeatStr(TextFileWriter& writer, const char* s, int count)
{
   for(int i = 0 ; i < count ; i++) writer.writeLiteral(s);
}

void writeType(TextFileWriter& writer, ident_t type, bool fullReference = false)
{
   bool arrayMode = false;
   if (type.startsWith("params ")) {
      writer.writeLiteral("<I>params</I>&nbsp;");
      type = type.c_str() + 7;
   }
   else if(type.startsWith("ref ")) {
      writer.writeLiteral("<I>ref</I>&nbsp;");
      type = type.c_str() + 4;
   }
   else if (type.startsWith("arrayof ")) {
      type = type.c_str() + 8;
      arrayMode = true;
   }

   writer.writeLiteral("<SPAN CLASS=\"memberNameLink\">");
   if (type.find('\'') != NOTFOUND_POS) {
      writer.writeLiteral("<A HREF=\"");

      pos_t index = type.find("&lt;");
      NamespaceName ns(type);
      if (index != NOTFOUND_POS) {
         for (pos_t i = index; i >= 0; i--) {
            if (type[i] == '\'') {
               ns.copy(type, i);
               break;
            }
         }
      }

      if (emptystr(ns.c_str())) {
         writer.writeLiteral("#");
      }
      else {
         writeNs(writer, ns.ident());
         writer.writeLiteral(".html#");
      }
      writeRefName(writer, type.c_str() + ns.Length() + 1, false);

      writer.writeLiteral("\">");
      if (fullReference) {
         writer.writeLiteral(type.c_str());
      }
      else writer.writeLiteral(type.c_str() + ns.Length() + 1);

      writer.writeLiteral("</A>");
   }
   else writer.writeLiteral(type.c_str());

   if (arrayMode)
      writer.writeLiteral("<I>[]</I>");

   writer.writeLiteral("</SPAN>");
}

void writeParent(TextFileWriter& writer, List<IdentifierString>::Iterator& it, ApiClassInfo* info)
{
   writer.writeLiteralNewLine("<UL CLASS=\"inheritance\">");
   writer.writeLiteralNewLine("<LI>");

   if (!it.Eof()) {
      writeType(writer, (*it).ident(), true);
      writer.writeLiteralNewLine("</LI>");

      it++;
      writer.writeLiteralNewLine("<LI>");
      writeParent(writer, it, info);
   }
   else {
      writer.writeLiteral(info->fullName.c_str());
   }

   writer.writeLiteralNewLine("</LI>");

   writer.writeLiteralNewLine("</UL>");
}

void writeParents(TextFileWriter& writer, ApiClassInfo* info, ident_t moduleName)
{
   auto it = info->parents.start();
   writeParent(writer, it, info);
}

void writeFields(TextFileWriter& writer, ApiClassInfo* info)
{
   writer.writeLiteralNewLine("<UL CLASS=\"blockList\">");
   writer.writeLiteralNewLine("<LI CLASS=\"blockList\">");

   writer.writeLiteralNewLine("<H3>Field Summary</H3>");

   writer.writeLiteralNewLine("<TABLE CLASS=\"memberSummary\" BORDER=\"0\" CELLPADDING=\"3\" CELLSPACING=\"0\">");
   writer.writeLiteralNewLine("<TR>");
   writer.writeLiteralNewLine("<TH CLASS=\"colFirst\" scope=\"col\">Modifier and Type</TH>");
   writer.writeLiteralNewLine("<TH CLASS=\"colLast\" scope=\"col\">Field</TH>");
   writer.writeLiteralNewLine("</TR>");


   auto it = info->fields.start();
   bool alt = true;
   while (!it.Eof()) {
      if (alt) {
         writer.writeLiteralNewLine("<TR CLASS=\"altColor\">");
      }
      else {
         writer.writeLiteralNewLine("<TR CLASS=\"rowColor\">");
      }
      alt = !alt;

      writer.writeLiteralNewLine("<TD CLASS=\"colFirst\">");
      writer.writeLiteral("<CODE>");
      writeType(writer, (*it)->type);
      writer.writeLiteralNewLine("</CODE>");
      writer.writeLiteralNewLine("</TD>");
      writer.writeLiteralNewLine("<TD CLASS=\"colLast\">");
      writer.writeLiteral("<CODE>");
      writer.writeLiteral((*it)->name);
      writer.writeLiteralNewLine("</CODE>");
      writer.writeLiteralNewLine("</TD>");
      writer.writeLiteralNewLine("</TR>");

      it++;
   }

   writer.writeLiteralNewLine("</TABLE>");

   writer.writeLiteralNewLine("</LI>");
   writer.writeLiteralNewLine("</UL>");
}

bool isMethod(ApiMethodInfo* info)
{
   return !info->special && !info->prop;
}

bool isProp(ApiMethodInfo* info)
{
   return !info->special && info->prop;
}

void writeFirstColumn(TextFileWriter& writer, ApiMethodInfo* info)
{
   writer.writeLiteralNewLine("<TD CLASS=\"colFirst\">");
   writer.writeLiteralNewLine("<CODE>");
   if (info->prefix.Length() != 0) {
      writer.writeLiteral("<i>");
      writer.writeLiteral(info->prefix.ident());
      writer.writeLiteral("</i>");
      writer.writeLiteral("&nbsp;");
   }
   if (info->retType.Length() != 0) {
      writeType(writer, info->retType);
   }
   
   writer.writeLiteralNewLine("</CODE></TD>");
}

void writeSecondColumn(TextFileWriter& writer, ApiMethodInfo* info)
{
   writer.writeLiteralNewLine("<TD CLASS=\"colLast\">");
   writer.writeLiteral("<CODE>");

   writer.writeLiteral(info->name);
   writer.writeLiteral("(");

   bool first = true;
   auto it = info->params.start();
   auto name_it = info->paramNames.start();
   while (!it.Eof()) {
      if (!first) {
         writer.writeLiteral(", ");
      }
      else first = false;

      writeType(writer, (*it).c_str());
      if (!name_it.Eof()) {
         writer.writeLiteral(" ");
         writer.writeLiteral(*name_it);
         name_it++;
      }

      it++;
   }

   writer.writeLiteralNewLine(")");

   writer.writeLiteralNewLine("</CODE>");
   if (info->shortdescr.Length() > 0) {
      writer.writeLiteralNewLine("<div class=\"block\">");
      writer.writeLiteral(info->shortdescr);
      writer.writeLiteralNewLine("</div>");
   }
   writer.writeLiteralNewLine("</TD>");
}

void writeSecondPropColumn(TextFileWriter& writer, ApiMethodInfo* info)
{
   writer.writeLiteralNewLine("<TD CLASS=\"colLast\">");
   writer.writeLiteral("<CODE>");
   writer.writeLiteral(info->name);

   if (info->params.Count() > 0) {
      writer.writeLiteral("(");

      bool first = true;
      auto it = info->params.start();
      auto name_it = info->paramNames.start();
      while (!it.Eof()) {
         if (!first) {
            writer.writeLiteral(", ");
         }
         else first = false;

         writeType(writer, (*it).c_str());
         if (!name_it.Eof()) {
            writer.writeLiteral(" ");
            writer.writeLiteral(*name_it);
            name_it++;
         }

         it++;
      }

      writer.writeLiteralNewLine(")");
   }

   writer.writeLiteralNewLine("</CODE>");
   if (info->shortdescr.Length() > 0) {
      writer.writeLiteralNewLine("<div class=\"block\">");
      writer.writeLiteral(info->shortdescr);
      writer.writeLiteralNewLine("</div>");
   }
   writer.writeLiteralNewLine("</TD>");
}

void writeMethods(TextFileWriter& writer, ApiClassInfo* info)
{
   writer.writeLiteralNewLine("<UL CLASS=\"blockList\">");
   writer.writeLiteralNewLine("<LI CLASS=\"blockList\">");

   writer.writeLiteralNewLine("<H3>Method Summary</H3>");

   writer.writeLiteralNewLine("<TABLE CLASS=\"memberSummary\" BORDER=\"0\" CELLPADDING=\"3\" CELLSPACING=\"0\">");
   writer.writeLiteralNewLine("<TR>");
   writer.writeLiteralNewLine("<TH CLASS=\"colFirst\" scope=\"col\">Modifier and Type</TH>");
   writer.writeLiteralNewLine("<TH CLASS=\"colLast\" scope=\"col\">Method</TH>");
   writer.writeLiteralNewLine("</TR>");

   bool alt = true;
   auto it = info->methods.start();
   while (!it.Eof()) {
      if (isMethod(*it)) {
         if (alt) {
            writer.writeLiteralNewLine("<TR CLASS=\"altColor\">");
         }
         else {
            writer.writeLiteralNewLine("<TR CLASS=\"rowColor\">");
         }
         alt = !alt;

         writeFirstColumn(writer, *it);
         writeSecondColumn(writer, *it);

         writer.writeLiteralNewLine("</TR>");
      }
      it++;
   }

   writer.writeLiteralNewLine("</TABLE>");

   writer.writeLiteralNewLine("</LI>");
   writer.writeLiteralNewLine("</UL>");
}

void writeProperties(TextFileWriter& writer, ApiClassInfo* info)
{
   writer.writeLiteralNewLine("<UL CLASS=\"blockList\">");
   writer.writeLiteralNewLine("<LI CLASS=\"blockList\">");

   writer.writeLiteralNewLine("<H3>Property Summary</H3>");

   writer.writeLiteralNewLine("<TABLE CLASS=\"memberSummary\" BORDER=\"0\" CELLPADDING=\"3\" CELLSPACING=\"0\">");
   writer.writeLiteralNewLine("<TR>");
   writer.writeLiteralNewLine("<TH CLASS=\"colFirst\" scope=\"col\">Modifier and Type</TH>");
   writer.writeLiteralNewLine("<TH CLASS=\"colLast\" scope=\"col\">Property accessor</TH>");
   writer.writeLiteralNewLine("</TR>");

   bool alt = true;
   auto it = info->methods.start();
   while (!it.Eof()) {
      if (isProp(*it)) {
         if (alt) {
            writer.writeLiteralNewLine("<TR CLASS=\"altColor\">");
         }
         else {
            writer.writeLiteralNewLine("<TR CLASS=\"rowColor\">");
         }
         alt = !alt;

         writeFirstColumn(writer, *it);
         writeSecondPropColumn(writer, *it);

         writer.writeLiteralNewLine("</TR>");
      }
      it++;
   }

   writer.writeLiteralNewLine("</TABLE>");

   writer.writeLiteralNewLine("</LI>");
   writer.writeLiteralNewLine("</UL>");
}

void writeStaticProperties(TextFileWriter& writer, ApiClassInfo* info)
{
   writer.writeLiteralNewLine("<UL CLASS=\"blockList\">");
   writer.writeLiteralNewLine("<LI CLASS=\"blockList\">");

   writer.writeLiteralNewLine("<H3>Static Property Summary</H3>");

   writer.writeLiteralNewLine("<TABLE CLASS=\"memberSummary\" BORDER=\"0\" CELLPADDING=\"3\" CELLSPACING=\"0\">");
   writer.writeLiteralNewLine("<TR>");
   writer.writeLiteralNewLine("<TH CLASS=\"colFirst\" scope=\"col\">Modifier and Type</TH>");
   writer.writeLiteralNewLine("<TH CLASS=\"colLast\" scope=\"col\">Property accessor</TH>");
   writer.writeLiteralNewLine("</TR>");

   auto it = info->constructors.start();
   bool alt = true;
   while (!it.Eof()) {
      if ((*it)->prop) {
         if (alt) {
            writer.writeLiteralNewLine("<TR CLASS=\"altColor\">");
         }
         else {
            writer.writeLiteralNewLine("<TR CLASS=\"rowColor\">");
         }
         alt = !alt;

         writeFirstColumn(writer, *it);
         writeSecondPropColumn(writer, *it);

         writer.writeLiteralNewLine("</TR>");
      }
      it++;
   }

   writer.writeLiteralNewLine("</TABLE>");

   writer.writeLiteralNewLine("</LI>");
   writer.writeLiteralNewLine("</UL>");
}

void writeExtensions(TextFileWriter& writer, ApiClassInfo* info)
{
   writer.writeLiteralNewLine("<UL CLASS=\"blockList\">");
   writer.writeLiteralNewLine("<LI CLASS=\"blockList\">");

   writer.writeLiteralNewLine("<H3>Extension Summary</H3>");

   writer.writeLiteralNewLine("<TABLE CLASS=\"memberSummary\" BORDER=\"0\" CELLPADDING=\"3\" CELLSPACING=\"0\">");
   writer.writeLiteralNewLine("<TR>");
   writer.writeLiteralNewLine("<TH CLASS=\"colFirst\" scope=\"col\">Modifier and Type</TH>");
   writer.writeLiteralNewLine("<TH CLASS=\"colLast\" scope=\"col\">Extension Method</TH>");
   writer.writeLiteralNewLine("</TR>");

   bool alt = true;
   auto it = info->extensions.start();
   while (!it.Eof()) {
      if (!(*it)->special) {
         if (alt) {
            writer.writeLiteralNewLine("<TR CLASS=\"altColor\">");
         }
         else {
            writer.writeLiteralNewLine("<TR CLASS=\"rowColor\">");
         }
         alt = !alt;         

         writeFirstColumn(writer, *it);
         writeSecondColumn(writer, *it);

         writer.writeLiteralNewLine("</TR>");
      }

      it++;
   }

   writer.writeLiteralNewLine("</TABLE>");

   writer.writeLiteralNewLine("</LI>");
   writer.writeLiteralNewLine("</UL>");
}

void writeConstructors(TextFileWriter& writer, ApiClassInfo* info)
{
   writer.writeLiteralNewLine("<UL CLASS=\"blockList\">");
   writer.writeLiteralNewLine("<LI CLASS=\"blockList\">");

   writer.writeLiteralNewLine("<H3>Constructor Summary</H3>");

   writer.writeLiteralNewLine("<TABLE CLASS=\"memberSummary\" BORDER=\"0\" CELLPADDING=\"3\" CELLSPACING=\"0\">");
   writer.writeLiteralNewLine("<TR>");
   writer.writeLiteralNewLine("<TH CLASS=\"colFirst\" scope=\"col\">Modifier and Type</TH>");
   writer.writeLiteralNewLine("<TH CLASS=\"colLast\" scope=\"col\">Constructor</TH>");
   writer.writeLiteralNewLine("</TR>");

   auto it = info->constructors.start();
   bool alt = true;
   while (!it.Eof()) {
      if (isMethod(*it)) {
         if (alt) {
            writer.writeLiteralNewLine("<TR CLASS=\"altColor\">");
         }
         else {
            writer.writeLiteralNewLine("<TR CLASS=\"rowColor\">");
         }
         alt = !alt;

         writeFirstColumn(writer, *it);
         writeSecondColumn(writer, *it);

         writer.writeLiteralNewLine("</TR>");
      }

      it++;
   }

   writer.writeLiteralNewLine("</TABLE>");

   writer.writeLiteralNewLine("</LI>");
   writer.writeLiteralNewLine("</UL>");
}

void writeConversions(TextFileWriter& writer, ApiClassInfo* info)
{
   writer.writeLiteralNewLine("<UL CLASS=\"blockList\">");
   writer.writeLiteralNewLine("<LI CLASS=\"blockList\">");

   writer.writeLiteralNewLine("<H3>Conversion Summary</H3>");

   writer.writeLiteralNewLine("<TABLE CLASS=\"memberSummary\" BORDER=\"0\" CELLPADDING=\"3\" CELLSPACING=\"0\">");
   writer.writeLiteralNewLine("<TR>");
   writer.writeLiteralNewLine("<TH CLASS=\"colFirst\" scope=\"col\">Type</TH>");
   writer.writeLiteralNewLine("<TH CLASS=\"colLast\" scope=\"col\">Conversion Method</TH>");
   writer.writeLiteralNewLine("</TR>");

   bool alt = true;
   auto it = info->methods.start();
   while (!it.Eof()) {
      if ((*it)->convertor) {
         if (alt) {
            writer.writeLiteralNewLine("<TR CLASS=\"altColor\">");
         }
         else {
            writer.writeLiteralNewLine("<TR CLASS=\"rowColor\">");
         }
         alt = !alt;

         writeFirstColumn(writer, *it);
         writeSecondColumn(writer, *it);

         writer.writeLiteralNewLine("</TR>");
      }

      it++;
   }

   writer.writeLiteralNewLine("</TABLE>");

   writer.writeLiteralNewLine("</LI>");
   writer.writeLiteralNewLine("</UL>");
}

void copyName(IdentifierString& name, ident_t fullName)
{
   pos_t index = fullName.find("&lt;");
   if (index == NOTFOUND_POS) {
      name.copy(fullName.c_str() + fullName.findLast('\'') + 1);
   }
   else {
      if (index != NOTFOUND_POS) {
         for (size_t i = index; i >= 0; i--) {
            if (fullName[i] == '\'') {
               name.copy(fullName.c_str() + i + 1);
               break;
            }
         }
      }
   }
}

void parseNs(IdentifierString& ns, ident_t root, ident_t fullName)
{
   if (isWeakReference(fullName)) {
      ns.copy(root);
   }

   size_t ltIndex = fullName.find("&lt;");
   size_t last = 0;
   for (size_t i = 0; i < getlength(fullName); i++) {
      if (fullName[i] == '\'') {
         last = i;
      }
      else if (fullName[i] == '#' || (fullName[i] == '&' && i == ltIndex))
         break;
   }

   ns.append(fullName, last);
}

void writeBody(TextFileWriter& writer, ApiClassInfo* info, const char* rootNs)
{
   const char* title = /*config.getSetting(name, "#title", NULL)*/nullptr;
   if (title==NULL)
      title = info->name.c_str();

   IdentifierString moduleName;
   parseNs(moduleName, rootNs, info->fullName.c_str());

   writer.writeLiteral("<A NAME=\"");
   writer.writeLiteral(info->name.c_str());
   writer.writeLiteralNewLine("\">");
   writer.writeLiteralNewLine("</A>");

   writer.writeLiteralNewLine("<!-- ======== START OF CLASS DATA ======== -->");

   writer.writeLiteralNewLine("<DIV CLASS=\"header\">");

   writer.writeLiteralNewLine("<DIV CLASS=\"subTitle\">");
   writer.writeLiteral(moduleName);
   writer.writeLiteral("'");
   writer.writeLiteralNewLine("</DIV>");

   writer.writeLiteral("<H2 title=\"");
   writer.writeLiteral(title);
   writer.writeLiteral("\" class=\"title\">");
   writer.writeLiteral(title);
   writer.writeLiteralNewLine("</H2>");

   writer.writeLiteralNewLine("</DIV>");
   
   writer.writeLiteralNewLine("<DIV CLASS=\"contentContainer\">");

   if (info->parents.Count() > 0) {
      writeParents(writer, info, rootNs);
   }

   writer.writeLiteralNewLine("<DIV CLASS=\"description\">");
      writer.writeLiteralNewLine("<BR>");
      writer.writeLiteralNewLine("<HR>");
      //const char* descr = nullptr;
      writer.writeLiteralNewLine("<PRE STYLE=\"padding-top: 15px;\">");
      writeClassName(writer, info);
      writer.writeLiteralNewLine("</PRE>");
      writer.writeLiteralNewLine("<BR>");
      writer.writeLiteralNewLine("</DIV>");

   writer.writeLiteralNewLine("<DIV CLASS=\"summary\">");
   writer.writeLiteralNewLine("<UL CLASS=\"blockList\">");
   writer.writeLiteralNewLine("<LI CLASS=\"blockList\">");

   if (info->fields.Count() > 0) {
      writer.writeLiteralNewLine("<!-- =========== FIELD SUMMARY =========== -->");
      writeFields(writer, info);
   }

   if (info->withStaticProps) {
      writer.writeLiteralNewLine("<!-- ========== CONSTRUCTOR SUMMARY =========== -->");
      writeStaticProperties(writer, info);
   }

   if (info->withConstructors) {
      writer.writeLiteralNewLine("<!-- ========== CONSTRUCTOR SUMMARY =========== -->");
      writeConstructors(writer, info);
   }

   if (info->withConversions) {
      writer.writeLiteralNewLine("<!-- ========== COVERSION SUMMARY =========== -->");
      writeConversions(writer, info);
   }

   if (info->withProps) {
      writer.writeLiteralNewLine("<!-- =========== PROPERTY SUMMARY =========== -->");
      writeProperties(writer, info);
   }

   writer.writeLiteralNewLine("<!-- ========== METHOD SUMMARY =========== -->");
   writeMethods(writer, info);

   if (info->extensions.Count() > 0) {
      writer.writeLiteralNewLine("<!-- ========== EXTNSION SUMMARY =========== -->");
      writeExtensions(writer, info);
   }

   writer.writeLiteralNewLine("</LI>");
   writer.writeLiteralNewLine("</UL>");
   writer.writeLiteralNewLine("</DIV>");

   writer.writeLiteralNewLine("<HR>");

   writer.writeLiteralNewLine("</DIV>");
}

void writeExtendedBody(TextFileWriter& writer, ApiClassInfo* info, const char* rootNs)
{
   const char* title = /*config.getSetting(name, "#title", NULL)*/nullptr;
   if (title == NULL)
      title = info->name.c_str();

   IdentifierString moduleName;
   parseNs(moduleName, rootNs, info->fullName.c_str());

   writer.writeLiteral("<A NAME=\"ext-");
   writer.writeLiteral(info->name.c_str());
   writer.writeLiteralNewLine("\">");
   writer.writeLiteralNewLine("</A>");

   writer.writeLiteralNewLine("<!-- ======== START OF EXTENSION DATA ======== -->");

   writer.writeLiteralNewLine("<DIV CLASS=\"header\">");

   writer.writeLiteralNewLine("<DIV CLASS=\"subTitle\">");
   writer.writeLiteral(moduleName);
   writer.writeLiteral("'");
   writer.writeLiteralNewLine("</DIV>");

   writer.writeLiteral("<H2 title=\"");
   writer.writeLiteral(title);
   writer.writeLiteral("\" class=\"title\">");
   writer.writeLiteral(title);
   writer.writeLiteralNewLine("</H2>");

   writer.writeLiteralNewLine("</DIV>");

   writer.writeLiteralNewLine("<DIV CLASS=\"contentContainer\">");

   writer.writeLiteralNewLine("<DIV CLASS=\"description\">");
   writer.writeLiteralNewLine("<BR>");
   writer.writeLiteralNewLine("<HR>");
   //const char* descr = nullptr;
   writer.writeLiteralNewLine("<PRE STYLE=\"padding-top: 15px;\">");
   writeClassName(writer, info);
   writer.writeLiteralNewLine("</PRE>");
   writer.writeLiteralNewLine("<BR>");
   writer.writeLiteralNewLine("</DIV>");

   writer.writeLiteralNewLine("<DIV CLASS=\"summary\">");
   writer.writeLiteralNewLine("<UL CLASS=\"blockList\">");
   writer.writeLiteralNewLine("<LI CLASS=\"blockList\">");

   if (info->extensions.Count() > 0) {
      writer.writeLiteralNewLine("<!-- ========== EXTNSION SUMMARY =========== -->");
      writeExtensions(writer, info);
   }

   writer.writeLiteralNewLine("</LI>");
   writer.writeLiteralNewLine("</UL>");
   writer.writeLiteralNewLine("</DIV>");

   writer.writeLiteralNewLine("<HR>");

   writer.writeLiteralNewLine("</DIV>");
}

void writeSymbolBody(TextFileWriter& writer, ApiSymbolInfo* info, const char* rootNs)
{
   const char* title = /*config.getSetting(name, "#title", NULL)*/nullptr;
   if (title == NULL)
      title = info->name.c_str();

   IdentifierString moduleName;
   parseNs(moduleName, rootNs, info->fullName.c_str());

   writer.writeLiteral("<A NAME=\"");
   writer.writeLiteral(info->name.c_str());
   writer.writeLiteralNewLine("\">");
   writer.writeLiteralNewLine("</A>");


   writer.writeLiteralNewLine("<!-- ======== START OF SYMBOL DATA ======== -->");

   writer.writeLiteralNewLine("<DIV CLASS=\"header\">");

   writer.writeLiteralNewLine("<DIV CLASS=\"subTitle\">");
   writer.writeLiteral(moduleName);
   writer.writeLiteral("'");
   writer.writeLiteralNewLine("</DIV>");

   writer.writeLiteral("<H2 title=\"");
   writer.writeLiteral(title);
   writer.writeLiteral("\" class=\"title\">");
   writer.writeLiteral(title);
   writer.writeLiteralNewLine("</H2>");

   writer.writeLiteralNewLine("</DIV>");

   writer.writeLiteralNewLine("<DIV CLASS=\"contentContainer\">");

   writer.writeLiteralNewLine("<DIV CLASS=\"summary\">");
   writer.writeLiteralNewLine("<UL CLASS=\"blockList\">");
   writer.writeLiteralNewLine("<LI CLASS=\"blockList\">");

   writer.writeLiteralNewLine("<H3>Symbol Summary</H3>");

   writer.writeLiteralNewLine("<TABLE CLASS=\"memberSummary\" BORDER=\"0\" CELLPADDING=\"3\" CELLSPACING=\"0\">");
   writer.writeLiteralNewLine("<TR>");
   writer.writeLiteralNewLine("<TH CLASS=\"colFirst\" scope=\"col\">Modifier and Type</TH>");
   writer.writeLiteralNewLine("<TH CLASS=\"colLast\" scope=\"col\">Name</TH>");
   writer.writeLiteralNewLine("</TR>");

   writer.writeLiteralNewLine("<TR CLASS=\"rowColor\">");
   writer.writeLiteralNewLine("<TD CLASS=\"colFirst\">");
   writer.writeLiteral("<CODE>");
   writeType(writer, info->type);
   writer.writeLiteralNewLine("</CODE>");
   writer.writeLiteralNewLine("</TD>");
   writer.writeLiteralNewLine("<TD CLASS=\"colLast\">");
   writer.writeLiteral("<CODE>");
   writer.writeLiteral("symbol ");
   writer.writeLiteral(info->name.c_str());
   writer.writeLiteralNewLine("</CODE>");
   writer.writeLiteralNewLine("</TD>");
   writer.writeLiteralNewLine("</TR>");
   writer.writeLiteralNewLine("</TABLE>");

   writer.writeLiteralNewLine("</LI>");
   writer.writeLiteralNewLine("</UL>");
   writer.writeLiteralNewLine("</DIV>");

   writer.writeLiteralNewLine("<HR>");

   writer.writeLiteralNewLine("</DIV>");

}

void writeFooter(TextFileWriter& writer, const char* packageLink)
{
   writer.writeLiteralNewLine("<DIV CLASS=\"bottomNav\">");

   writer.writeLiteralNewLine("<UL CLASS=\"navList\">");

   writer.writeLiteralNewLine("<LI>");
   writer.writeLiteralNewLine("<A HREF=\"index.html\">Overview</A>");
   writer.writeLiteralNewLine("</LI>");

   writer.writeLiteralNewLine("<LI>");
   if (!emptystr(packageLink)) {
      writer.writeLiteral("<A HREF=\"");
      writer.writeLiteral(packageLink);
      writer.writeLiteralNewLine("\">Module</A>");
   }
   else writer.writeLiteralNewLine("Module");
   writer.writeLiteralNewLine("</LI>");

   writer.writeLiteralNewLine("</UL>");

   writer.writeLiteralNewLine("<DIV CLASS=\"aboutLanguage\">");
   writer.writeLiteralNewLine("<STRONG>");
   writer.writeLiteralNewLine(TITLE2);
   writer.writeLiteralNewLine("</STRONG>");
   writer.writeLiteralNewLine("</DIV>");

   writer.writeLiteralNewLine("</DIV>");

   writer.writeLiteralNewLine("</BODY>");
   writer.writeLiteralNewLine("</HTML>");
}

void writeSummaryFooter(TextFileWriter& writer)
{
   writer.writeLiteralNewLine("</UL>");
   writer.writeLiteralNewLine("</DIV>");
}

void writeClassSummaryFooter(TextFileWriter& writer)
{
   writer.writeLiteralNewLine("</TABLE>");
   writer.writeLiteralNewLine("</LI>");
}

bool readLine(String<char, LINE_LEN>& line, TextFileReader& reader)
{
   if (reader.readLine(line, buffer)) {
      line.trim('\n');
      line.trim('\r');

      return true;
   }
   else return false;
}

ApiModuleInfo* findModule(List<ApiModuleInfo*>& modules, ident_t ns)
{
   auto it = modules.start();
   while (!it.Eof()) {
      if ((*it)->name.compare(ns))
         return *it;

      it++;
   }

   return nullptr;
}

ApiClassInfo* findClass(ApiModuleInfo* module, ident_t name)
{
   auto it = module->classes.start();
   while (!it.Eof()) {
      if ((*it)->fullName.compare(name))
         return *it;

      it++;
   }

   return nullptr;
}

void parseTemplateName(IdentifierString& line)
{
   IdentifierString temp(line);

   size_t index = line.ident().findLast('\'');
   line.clear();

   int last = index;
   bool first = true;
   for (size_t i = index; i < temp.Length(); i++) {
      if (temp[i] == '@') {
         temp[i] = '\'';
         if(!first)
            last = i;
      }
      else if (temp[i] == '#') {
         line.append(temp.c_str() + last + 1, i - last - 1);
      }
      else if (temp[i] == '&') {
         if (first) {
            line.append("&lt;");
            first = false;
         }
         else {
            line.append(temp.c_str() + last + 1, i - last - 1);
            line.append(',');
         }
      }
   }

   line.append(temp.c_str() + last + 1);
   line.append("&gt;");
}

void parseTemplateType(IdentifierString& line, int index, bool argMode)
{
   IdentifierString temp(line);

   line.truncate(0);

   int last = index;
   bool first = true;
   bool noCurlybrackets = false;
   if (argMode && temp.ident().startsWith(ByRefPrefix)) {
      // HOTFIX : recognize byref argument

      temp.cut(0, getlength(ByRefPrefix));

      line.append("ref ");
      noCurlybrackets = true;
   }
   else if (argMode && temp.ident().startsWith(ArrayPrefix)) {
      // HOTFIX : recognize byref argument

      temp.cut(0, getlength(ArrayPrefix));

      line.append("arrayof ");
      noCurlybrackets = true;
   }

   for (int i = index; i < temp.Length(); i++) {
      if (temp[i] == '@') {
         temp[i] = '\'';
      }
      else if (temp[i] == '#') {
         line.append(temp.c_str() + last + 1, i - last - 1);
      }
      else if (temp[i] == '&') {
         if (first) {
            last = i;
            line.append("&lt;");
            first = false;
         }
         else {
            line.append(temp.c_str() + last + 1, i - last - 1);
            line.append(',');
            last = i;
         }
      }
   }

   if (noCurlybrackets) {
      line.append(temp.c_str());
   }
   else {
      line.append(temp.c_str() + last + 1);
      line.append("&gt;");
   }
}

void validateTemplateType(IdentifierString& type, bool templateBased, bool argMode)
{
   if (templateBased) {
      if (isTemplateBased(type)) {
         if (isTemplateWeakReference(type.c_str()))
            type.cut(0, 6);

         parseTemplateName(type);
      }
      else if (type.ident().find("$private'T") != NOTFOUND_POS) {
         int index = type.ident().findLast('\'');
         type.cut(0, index + 1);
      }
   }
   else if (isTemplateWeakReference(type.c_str())) {
      NamespaceName ns(type);

      parseTemplateType(type, ns.Length(), argMode);
   }
}

void readType(IdentifierString& type, ident_t line, ident_t rootNs, bool templateBased, bool argMode)
{
   if (isTemplateWeakReference(line.c_str())) {
      type.copy(line);
   }
   else if (line[0] == '\'') {
      type.copy(rootNs);
      type.append(line);
   }
   else type.copy(line);

   validateTemplateType(type, templateBased, argMode);
}

void parseName(ApiMethodInfo* info, bool extensionOne, bool templateBased, ident_t rootNs)
{
   bool skipOne = extensionOne;

   int sign_index = info->name.ident().find("<");
   if (sign_index != NOTFOUND_POS) {
      IdentifierString param;
      IdentifierString type;
      for (int i = sign_index + 1; i < info->name.Length(); i++) {
         if (info->name[i] == ',' || info->name[i] == '>') {
            if (skipOne) {
               skipOne = false;
            }
            else {
               readType(type, param.c_str(), rootNs, templateBased, true);

               if (info->withVargs && info->name[i] == '>') {
                  type.append("[]");

                  type.insert("params ", 0);
               }

               info->params.add(type.c_str());
            }
            param.clear();
            type.clear();
         }
         else param.append(info->name[i]);
      }

      info->name.truncate(sign_index);
   }
}

void parseMethod(ApiMethodInfo* info, ident_t messageLine, bool staticOne, bool extensionOne, bool templateBased, ident_t rootNs)
{
   int paramCount = 0;

   info->isAbstract = messageLine.find("@abstract") != NOTFOUND_POS;
   info->isMultidispatcher = messageLine.find("@multidispatcher") != NOTFOUND_POS;
   info->isInternal = messageLine.find("@internal") != NOTFOUND_POS;
   info->protectedOne = messageLine.find("@protected") != NOTFOUND_POS;
   info->privateOne = messageLine.find("@private") != NOTFOUND_POS;

   pos_t retPos = messageLine.find(" of ");
   if (retPos != NOTFOUND_POS) {
      readType(info->retType, messageLine.c_str() + retPos + 4, rootNs, templateBased, false);
   }
   else {
      info->retType.copy("system'Object");
      retPos = getlength(messageLine);
   }

   pos_t index = messageLine.find('.') + 1;
   int propIndex = messageLine.find(".prop#");
   int vargIndex = messageLine.find(".params#");
   if (propIndex != NOTFOUND_POS) {
      info->prop = true;

      info->name.copy(messageLine.c_str() + propIndex + 6, retPos - propIndex - 6);
   }
   else if (vargIndex != NOTFOUND_POS) {
      info->withVargs = true;

      info->name.copy(messageLine.c_str() + vargIndex + 8, retPos - propIndex - 8);
   }
   else info->name.copy(messageLine.c_str() + index, retPos - index);

   pos_t end = info->name.ident().find('[') + 1;
   if (end != 0)  {
      pos_t size_end = info->name.ident().find(']') + 1;
      String<char, 4> tmp;
      tmp.copy(info->name.c_str() + end, size_end - end - 1);
      paramCount = ident_t(tmp.c_str()).toInt();
      if (!info->special)
         paramCount--;

      info->name.truncate(end - 1);
   }

   if (info->protectedOne) {
      int p_index = info->name.ident().find("$$");
      if (p_index != NOTFOUND_POS) {
         info->name.cut(0, p_index + 2);
      }
   }

   parseName(info, extensionOne, templateBased, rootNs);

   if (info->params.Count() < paramCount) {
      // if weak message
      for (int i = 0; i < paramCount; i++) {
         info->params.add("system'Object");
      }
   }

   if (info->name.compare("#dispatch"))
      info->special = true;
   else if (info->name.compare("#new"))
      info->special = true;
   else if (info->name.compare("#constructor")) {
      info->name.copy("<i>constructor</i>");
   }
   else if (info->name.compare("#constructor2")) {
      info->protectedOne = true;
      info->name.copy("<i>constructor</i>");
   }
   else if (info->name.compare("#cast")) {
      info->special = true;
      info->convertor = true;
      info->name.copy("<i>cast</i>");

      // HOTFIX : conversion should not contain arguments
      info->params.clear();
   }
   else if (info->name.compare("#generic")) {
      info->name.copy("<i>generic</i>");
   }
   else if (info->name.compare("#invoke")) {
      info->name.copy("<i>invoke</i>");
   }

   if (info->isMultidispatcher && !info->isAbstract)
      info->special = true;

   if (info->isInternal)
      info->special = true;

   if (info->privateOne)
      info->prefix.append("private ");
   else if (info->protectedOne)
      info->prefix.append("protected ");

   if (extensionOne)
      info->prefix.append("extension ");

   if (info->isAbstract)
      info->prefix.append("abstract ");

   if (staticOne && info->prop)
      info->prefix.append("static ");

   if (info->prop && paramCount == 0) {
      info->prefix.append("get");
   }
   else if (info->prop && paramCount == 1) {
      info->prefix.append("set");

      info->retType.clear();
   }
}

void parseField(ApiClassInfo* info, ident_t line, ident_t rootNs)
{
   auto fieldInfo = new ApiFieldInfo();

   pos_t retPos = line.find(" of ");
   if (retPos != NOTFOUND_POS) {
      fieldInfo->name.copy(line, retPos);

      readType(fieldInfo->type, line.c_str() + retPos + 4, rootNs, info->templateBased, false);
   }
   else {
      fieldInfo->type.copy("system'Object");
      fieldInfo->name.copy(line);
   }

   info->fields.add(fieldInfo);

}

void readParamNames(ApiMethodInfo* methodInfo, ident_t line, int& descr_index)
{
   IdentifierString name;

   int param_index = line.find(descr_index, '|', NOTFOUND_POS);
   while (param_index != NOTFOUND_POS) {
      name.copy(line.c_str() + descr_index, param_index - descr_index);

      methodInfo->paramNames.add(name.c_str());

      descr_index = param_index + 1;
      param_index = line.find(descr_index, '|', NOTFOUND_POS);
   }
}

void readClassMembers(String<char, LINE_LEN>& line, TextFileReader& reader, ApiClassInfo* info, ident_t rootNs,
   bool& isAbstract, bool& isClosed, bool& isSealed, bool& isStateless, bool& isRole)
{
   while (true) {
      if (!readLine(line, reader))
         return;

      if (ident_t(line).startsWith("@method ")) {
         if (ident_t(line.c_str()).find("#private&") != NOTFOUND_POS || ident_t(line.c_str()).find("auto#") != NOTFOUND_POS) {
            // ignore private methods
         }
         else {
            ApiMethodInfo* methodInfo = new ApiMethodInfo();
            int info_index = ident_t(line).find(";;");
            if (info_index != NOTFOUND_POS) {
               int descr_index = info_index + 2;

               readParamNames(methodInfo, line.c_str(), descr_index);

               methodInfo->shortdescr.copy(line.c_str() + descr_index);
               line.truncate(info_index);
            }

            parseMethod(methodInfo, line.c_str() + 8, false, false, info->templateBased, rootNs);

            if (methodInfo->prop)
               info->withProps = true;

            if (methodInfo->convertor)
               info->withConversions = true;

            info->methods.add(methodInfo);
         }
      }
      else if (ident_t(line).startsWith("@flag ")) {
         if (ident_t(line).endsWith("elAbstract"))
            isAbstract = true;
         else if (ident_t(line).endsWith("elClosed"))
            isClosed = true;
         else if (ident_t(line).endsWith("elSealed"))
            isSealed = true;
         else if (ident_t(line).endsWith("elRole"))
            isRole = true;
         else if (ident_t(line).endsWith("elStateless"))
            isStateless = true;
      }
      else if (ident_t(line).startsWith("@parent ")) {
         IdentifierString type;
         readType(type, line + 8, rootNs, info->templateBased, false);

         info->parents.add(type.c_str());
      }
      else if (ident_t(line).startsWith("@field ")) {
         parseField(info, line.c_str() + 7, rootNs);
      }
      else if (ident_t(line).startsWith("@classinfo ")) {
         info->shortdesc.copy(line.c_str() + 11);
      }
      else if (line.Length() == 0)
         return;
   }
}

void readClassClassMembers(String<char, LINE_LEN>& line, TextFileReader& reader, ApiClassInfo* info, ident_t rootNs)
{
   while (true) {
      if (!readLine(line, reader))
         return;

      if (ident_t(line).startsWith("@method ")) {
         if (ident_t(line.c_str()).find("#private&") != NOTFOUND_POS || ident_t(line.c_str()).find("auto#") != NOTFOUND_POS) {
            // ignore private methods
         }
         else {
            ApiMethodInfo* methodInfo = new ApiMethodInfo();
            int info_index = ident_t(line).find(";;");
            if (info_index != NOTFOUND_POS) {
               int descr_index = info_index + 2;

               readParamNames(methodInfo, line.c_str(), descr_index);

               methodInfo->shortdescr.copy(line.c_str() + descr_index);
               line.truncate(info_index);
            }

            parseMethod(methodInfo, line.c_str() + 8, true, false, info->templateBased, rootNs);

            if (isMethod(methodInfo))
               info->withConstructors = true;
            if (methodInfo->prop)
               info->withStaticProps = true;

            info->constructors.add(methodInfo);
         }
      }
      else if (line.Length() == 0)
         return;
   }
}

void readExtensions(String<char, LINE_LEN>& line, TextFileReader& reader, ApiClassInfo* info, ident_t rootNs)
{
   while (true) {
      if (!readLine(line, reader))
         return;

      if (ident_t(line).startsWith("@method ")) {
         if (ident_t(line.c_str()).find("#private&") != NOTFOUND_POS || ident_t(line.c_str()).find("auto#") != NOTFOUND_POS) {
            // ignore private methods
         }
         else {
            ApiMethodInfo* methodInfo = new ApiMethodInfo();
            int info_index = ident_t(line).find(";;");
            if (info_index != NOTFOUND_POS) {
               int descr_index = info_index + 2;

               readParamNames(methodInfo, line.c_str(), descr_index);

               methodInfo->shortdescr.copy(line.c_str() + descr_index);
               line.truncate(info_index);
            }

            parseMethod(methodInfo, line.c_str() + 8, true, true, info->templateBased, rootNs);

            info->extensions.add(methodInfo);
         }
      }
      else if (line.Length() == 0)
         return;
   }
}

bool readClassInfo(String<char, LINE_LEN>& line, TextFileReader& reader, List<ApiModuleInfo*>& modules, ident_t rootNs)
{
   if (emptystr(line)) {
      if (!readLine(line, reader))
         return false;
   }
   
   if (ident_t(line).startsWith("class ")) {
      ApiModuleInfo* moduleInfo = nullptr;

      NamespaceName ns(rootNs, line + 6);

      bool templateBased = false;
      bool classClassMode = false;
      IdentifierString fullName;
      fullName.copy(ns);
      fullName.append(line + ident_t(line).findLast('\''));

      if (ident_t(line).endsWith("#class")) {
         classClassMode = true;
         fullName.truncate(fullName.Length() - 6);
      }

      if (isTemplateBased(fullName)) {
         templateBased = true;

         parseTemplateName(fullName);

         ns.copy(fullName.c_str(), fullName.ident().findLast('\''));
      }

      moduleInfo = findModule(modules, ns.c_str());
      if (!moduleInfo) {
         moduleInfo = new ApiModuleInfo();
         moduleInfo->name.copy(ns.c_str());

         modules.add(moduleInfo);
      }

      ApiClassInfo* info = findClass(moduleInfo, fullName.c_str());

      if (classClassMode) {
         readClassClassMembers(line, reader, info, rootNs);
      }
      else {
         if (!info) {
            info = new ApiClassInfo();

            moduleInfo->classes.add(info);
         }
         else info->isVirtual = false;

         moduleInfo->withClasses = true;

         info->templateBased = templateBased;
         info->fullName.copy(fullName.c_str());

         pos_t index = fullName.ident().findLast('\'');
         info->name.copy(fullName.c_str() + index + 1);

         bool isAbstract = false, isClosed = false, isSealed = false;
         bool isStateless = false, isRole = false;
         readClassMembers(line, reader, info, rootNs, isAbstract, isClosed, isSealed, isStateless, isRole);

         info->prefix.clear();
         if (isSealed) {
            info->prefix.append("sealed ");
         }

         if (info->templateBased) {
            if (isRole && isStateless) {
               info->prefix.append("Singleton ");
            }
            else if (isAbstract && isClosed) {
               info->prefix.append("Interface ");
            }
            else if (isAbstract) {
               info->prefix.append("abstract ");
            }
            info->prefix.append("Template ");
         }
         else if (isAbstract && isClosed) {
            info->prefix.append("Interface ");
         }
         else if (isAbstract) {
            info->prefix.append("abstract Class ");
         }
         else if (isRole && isStateless) {
            info->prefix.append("Singleton ");
         }
         else info->prefix.append("Class ");
      }
   }
   else if (ident_t(line).startsWith("extension ")) {
      ApiModuleInfo* moduleInfo = nullptr;

      pos_t index = ident_t(line).find(" of ");
      IdentifierString targetName(line + index + 4);
      line[index] = 0;

      NamespaceName ns(rootNs, line + 10);

      moduleInfo = findModule(modules, ns.c_str());
      if (!moduleInfo) {
         moduleInfo = new ApiModuleInfo();
         moduleInfo->name.copy(ns.c_str());

         modules.add(moduleInfo);
      }

      IdentifierString targetFullName;
      readType(targetFullName, targetName.c_str(), rootNs, false, false);

      //if (isWeakReference(targetName.c_str())) {
      //   targetFullName.copy(rootNs);
      //   targetFullName.append(targetName.c_str());
      //}
      //else targetFullName.copy(targetName.c_str());

      ApiClassInfo* info = findClass(moduleInfo, targetFullName);
      if (info == nullptr) {
         info = new ApiClassInfo();

         info->fullName.copy(targetFullName.c_str());
         copyName(info->name, info->fullName.ident());
         info->isVirtual = true;

         moduleInfo->classes.add(info);

         moduleInfo->withVirtuals = true;
      }

      readExtensions(line, reader, info, rootNs);
   }
   else if (ident_t(line).startsWith("symbol ")) {
      ApiSymbolInfo* info = new ApiSymbolInfo();
      ApiModuleInfo* moduleInfo = nullptr;

      pos_t retPos = ident_t(line).find(" of ");
      if (retPos != NOTFOUND_POS) {
         readType(info->type, line.c_str() + retPos + 4, rootNs, false, false);

         line.truncate(retPos);
      }

      NamespaceName ns(rootNs, line + 7);

      moduleInfo = findModule(modules, ns.c_str());
      if (!moduleInfo) {
         moduleInfo = new ApiModuleInfo();
         moduleInfo->name.copy(ns.c_str());

         modules.add(moduleInfo);
      }

      info->fullName.copy(ns);
      info->fullName.append(line.c_str() + 7);
      copyName(info->name, info->fullName.ident());

      moduleInfo->symbols.add(info);

      if (!readLine(line, reader))
         return false;
   }

   return true;
}

int main(int argc, char* argv[])
{
   printf("ELENA command line Html Documentation generator %d.%d.%d (C)2006-21 by Alexei Rakov\n", ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, REVISION_VERSION);

   if (argc != 2) {
      printf("api2html <file>\n");
      return 0;
   }

   Path path;
   path.copy(argv[1]);

   pos_t pos = ident_t(argv[1]).findLast('\\', -1);
   IdentifierString ns;
   ns.copy(argv[1] + pos + 1);

   TextFileReader reader(path.c_str(), _ELENA_::feUTF8, true);
   if (!reader.isOpened()) {
      printf("Cannot open the temporal file %s", path.c_str());
      return -1;
   }

   List<ApiModuleInfo*> modules;

   String<char, LINE_LEN> line;
   line[0] = 0;

   printf("Reading...");

   while (readClassInfo(line, reader, modules, ns));
   printf("\n");

   auto it = modules.start();
   while (!it.Eof()) {
      printf("generating %s\n", (*it)->name.c_str());

      IdentifierString name;
      writeNs(name, *it);

      name.append(".html");
      
      IdentifierString summaryname;
      writeNs(summaryname, *it);

      summaryname.append("-summary");
      summaryname.append(".html");

      Path outPath;
      outPath.copy(name);
      
      Path outSumPath;
      outSumPath.copy(summaryname);

      TextFileWriter bodyWriter(outPath.str(), feUTF8, false);
      TextFileWriter summaryWriter(outSumPath.str(), feUTF8, false);

      writeHeader(summaryWriter, (*it)->name.c_str(), nullptr);
      writeHeader(bodyWriter, (*it)->name.c_str(), summaryname);

      //	const char* package = config.getSetting("#general#", "#name");
      const char* shortDescr = /*config.getSetting("#general#", "#shortdescr")*/nullptr;

      writeSummaryHeader(summaryWriter, (*it)->name.c_str(), shortDescr);

      if ((*it)->withClasses) {
         // classes
         writeClassSummaryHeader(summaryWriter);

         auto class_it = (*it)->classes.start();
         bool alt = true;
         while (!class_it.Eof()) {
            if (!(*class_it)->isVirtual) {
               if (alt) {
                  summaryWriter.writeLiteralNewLine("<TR CLASS=\"altColor\">");
               }
               else {
                  summaryWriter.writeLiteralNewLine("<TR CLASS=\"rowColor\">");
               }
               alt = !alt;

               writeSummaryTable(summaryWriter, *class_it, name);
               writeBody(bodyWriter, *class_it, ns);

               summaryWriter.writeLiteralNewLine("</TR>");
            }

            class_it++;
         }

         writeClassSummaryFooter(summaryWriter);
      }

      // symbols
      if ((*it)->symbols.Count() > 0) {
         writeSymbolSummaryHeader(summaryWriter);

         auto symbol_it = (*it)->symbols.start();
         bool alt = true;
         while (!symbol_it.Eof()) {
            if (alt) {
               summaryWriter.writeLiteralNewLine("<TR CLASS=\"altColor\">");
            }
            else {
               summaryWriter.writeLiteralNewLine("<TR CLASS=\"rowColor\">");
            }
            alt = !alt;

            writeSymbolSummaryTable(summaryWriter, *symbol_it, name);
            writeSymbolBody(bodyWriter, *symbol_it, ns);

            summaryWriter.writeLiteralNewLine("</TR>");

            symbol_it++;
         }
         writeClassSummaryFooter(summaryWriter);
      }

      // extensions
      if ((*it)->withVirtuals) {
         writeExtendedSummaryHeader(summaryWriter);

         auto class_it = (*it)->classes.start();
         bool alt = true;
         while (!class_it.Eof()) {
            if ((*class_it)->isVirtual) {
               if (alt) {
                  summaryWriter.writeLiteralNewLine("<TR CLASS=\"altColor\">");
               }
               else {
                  summaryWriter.writeLiteralNewLine("<TR CLASS=\"rowColor\">");
               }
               alt = !alt;

               writeExtendedSummaryTable(summaryWriter, *class_it, name);
               writeExtendedBody(bodyWriter, *class_it, ns);

               summaryWriter.writeLiteralNewLine("</TR>");
            }

            class_it++;
         }

         writeClassSummaryFooter(summaryWriter);
      }

      writeSummaryFooter(summaryWriter);

      writeFooter(summaryWriter, NULL);
      writeFooter(bodyWriter, summaryname);

      it++;
   }

   return 0;
}

