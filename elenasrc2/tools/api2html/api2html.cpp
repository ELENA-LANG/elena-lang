//---------------------------------------------------------------------------
//              E L E N A   p r o j e c t
//                Command line syntax generator main file
//                                              (C)2005-2010, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef WINVER
#define WINVER 0x0500
#endif

#include <windows.h>
#include "common.h"
#include "config.h"

#define TITLE "ELENA Standard Library 1.5.4: Package "
#define TITLE2 "ELENA&nbsp;Standard&nbsp;Library<br>1.5.4"

#define OPERATORS "+-*/=<>?!"

using namespace _ELENA_;

void writeHeader(TextFileWriter& writer, const char* package, const char* packageLink)
{
   writer.writeLine("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Frameset//EN\"\"http://www.w3.org/TR/REC-html40/frameset.dtd\">");
   writer.writeLine("<HTML>");
   writer.writeLine("<HEAD>");
   writer.writeLine("<TITLE>");
   writer.writeLiteral(TITLE);
   writer.writeLine(package);
   writer.writeLine("</TITLE>");
   writer.writeLine("<meta name=\"collection\" content=\"api\">");
   writer.writeLine("</HEAD>");
   writer.writeLine("<BODY BGCOLOR=\"white\">");

   writer.writeLine("<A NAME=\"navbar_top\"><!-- --></A>");
   writer.writeLine("<TABLE BORDER=\"0\" WIDTH=\"100%\" CELLPADDING=\"1\" CELLSPACING=\"0\">");
   writer.writeLine("<TR>");
   writer.writeLine("<TD COLSPAN=2 BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\">");
   writer.writeLine("<A NAME=\"navbar_top_firstrow\"><!-- --></A>");
   writer.writeLine("<TABLE BORDER=\"0\" CELLPADDING=\"0\" CELLSPACING=\"3\">");
   writer.writeLine("  <TR ALIGN=\"center\" VALIGN=\"top\">");
   writer.writeLine("  <TD BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\">    <A HREF=\"index.html\"><FONT CLASS=\"NavBarFont1\"><B>Overview</B></FONT></A>&nbsp;</TD>");
   if (!emptystr(packageLink)) {
      writer.writeLiteral("  <TD BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\"><A HREF=\"");
      writer.writeLiteral(packageLink);
      writer.writeLine("\"><FONT CLASS=\"NavBarFont1\"><B>Package</B></FONT></A>&nbsp;</TD>");
   }
   else writer.writeLine("  <TD BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\">    <FONT CLASS=\"NavBarFont1\">Package</FONT>&nbsp;</TD>");
   writer.writeLine("  </TR>");
   writer.writeLine("</TABLE>");
   writer.writeLine("</TD>");

   writer.writeLine("<TD ALIGN=\"right\" VALIGN=\"top\" ROWSPAN=3><EM><b>");
   writer.writeLiteral(TITLE2);
   writer.writeLine("</b></EM>");
   writer.writeLine("</TD>");
   writer.writeLine("</TR>");
   writer.writeLine("</TABLE>");

   writer.writeLine("<DL>");
   writer.writeLine("<HR>");
}

void writeSummaryHeader(TextFileWriter& writer, const char* name, const char* shortDescr)
{
   writer.writeLine("<H2>");
   writer.writeLiteral("Package ");
   writer.writeLine(name);
   writer.writeLine("</H2>");
   writer.writeLine(shortDescr);
   writer.writeLine("<P>");

   writer.writeLine("<TABLE BORDER=\"1\" CELLPADDING=\"3\" CELLSPACING=\"0\" WIDTH=\"100%\">");
   writer.writeLine("<TR BGCOLOR=\"#CCCCFF\" CLASS=\"TableHeadingColor\">");
   writer.writeLine("<TD COLSPAN=2><FONT SIZE=\"+2\">");
   writer.writeLine("<B>Class Summary</B></FONT></TD>");
   writer.writeLine("</TR>");
}

void writeSummaryTable(TextFileWriter& writer, IniConfigFile& config, const char* name, const char* bodyFileName)
{
   writer.writeLine("<TR BGCOLOR=\"white\" CLASS=\"TableRowColor\">");
   writer.writeLiteral("<TD WIDTH=\"15%\"><B><A HREF=\"");
   writer.writeLiteral(bodyFileName);
   writer.writeLiteral("#");
   writer.writeLiteral(name);
   writer.writeLiteral("\">");
   writer.writeLiteral(name);
   writer.writeLine("</A></B></TD>");
   writer.writeLiteral("<TD>");
   const char* descr = config.getSetting(name, "#shortdescr", NULL);
   if (!emptystr(descr)) {
      writer.writeLiteral(descr);
   }
   else writer.writeLiteral(name);

   writer.writeLiteral("</TD>");
   writer.writeLine("</TR>");
}

inline void repeatStr(TextFileWriter& writer, const char* s, int count)
{
   for(int i = 0 ; i < count ; i++) writer.writeLiteral(s);
}

inline const char* find(const char* s, char ch)
{
   if (emptystr(s)) {
      return NULL;
   }
   else {
      int index = String::find(s, ch);
      if (index==-1)
         index = strlen(s) - 1;

      return s + index + 1;
   }
}

inline void writeLeft(TextFileWriter& writer, const char* s, const char* right)
{
   if (emptystr(right)) {
      writer.writeLiteral(s);
   }
   else writer.write(s, right - s - 1);
}

void writeLink(TextFileWriter& writer, const char* link, const char* file = NULL)
{
   const char* body = find(link, ':');

   writer.writeLiteral("<A HREF=\"");

   if (String::find(link, '#') == -1) {
      if (!emptystr(file)) {
         writer.writeLiteral(file);
      }
      writer.writeChar('#');
      writeLeft(writer, link, body);
   }
   else writeLeft(writer, link, body);

   writer.writeLiteral("\">");
   if (emptystr(body)) {
      writer.writeLiteral(link);
   }
   else writeLeft(writer, body, find(body, ';'));

   writer.writeLine("</A>");
}

void writeParamLink(TextFileWriter& writer, const char* link, const char* right, const char* file)
{
   writer.writeLiteral("<A HREF=\"");

   int pos = String::find(link, '#');

   if (pos == -1 || pos > (right - link)) {
      writer.writeLiteral(file);
      writer.writeChar('#');
	   writeLeft(writer, link, right);
   }
   else {
      writeLeft(writer, link, right);
	   link += String::find(link, '#') + 1;
   }
   writer.writeLiteral("\">");
   writeLeft(writer, link, right);
   writer.writeLine("</A>");
}

void writeMessage(TextFileWriter& writer, const char* message)
{
   const char* parameter = find(message, ',');
   const char* result = find(parameter, ',');
   const char* descr = emptystr(result) ? find(parameter, ';') : find(result, ';');

   if (emptystr(result)) {
      result = descr;
   }

   bool oper = (String::find(OPERATORS, message[0])!=-1);

   if (message[0]=='<' && message[1] == 'a') {
      oper = false;
   }
   else if (message[0]=='<') {
      writer.writeLiteral("&lt;");
      message++;
   }
   else if (message[0]=='>') {
      writer.writeLiteral("&gt;");
      message++;
   }
   writeLeft(writer, message, parameter);
   if (!emptystr(parameter) && result != parameter && parameter[0] != ',') {
      if (!oper) {
         writer.writeLiteral(" : ");
      }
      else writer.writeLiteral(" ");
      writeParamLink(writer, parameter, result, "stdprotocol.html");
   }
   if (!emptystr(result) && result != descr && result[0] != ';') {
      writer.writeLiteral(" = ");
      writeParamLink(writer, result, descr, "stdprotocol.html");
   }
}

void writeParents(TextFileWriter& writer, IniConfigFile& config, const char* name,  const char* moduleName)
{
   writer.writeLine("<PRE>");
   int indent = 0;
   ConfigCategoryIterator it = config.getCategoryIt(name);
   while (!it.Eof()) {
      if (String::compare(it.key(), "#parent")) {
         repeatStr(writer, "  ", indent - 1);
         if (indent > 0) writer.writeLine(" |");
         repeatStr(writer, "  ", indent - 1);
         if (indent > 0) writer.writeLiteral(" +--");

         writeLink(writer, *it);

         indent++;
      }
      it++;
   }
   repeatStr(writer, "  ", indent - 1);
   writer.writeLine(" |");
   repeatStr(writer, "  ", indent - 1);
   writer.writeLiteral(" +--<B>");
   writer.writeLiteral(moduleName);
   writer.writeLiteral("'");
   writer.writeLiteral(name);
   writer.writeLine("</B>");

   writer.writeLine("</PRE>");
}

void writeProtocols(TextFileWriter& writer, const char* name, IniConfigFile& config)
{
   if (!emptystr(config.getSetting(name, "#protocol", NULL))) {
      ConfigCategoryIterator it = config.getCategoryIt(name);
      writer.writeLiteral("<B>All Implemented Protocols:</B>");
      writer.writeLine("<DL>");
      while (!it.Eof()) {
         if (String::compare(it.key(), "#protocol")) {
            writer.writeLine("<DT>");
            writeLink(writer, *it, "stdprotocol.html");
            writer.writeLine("</DT>");
         }
         it++;
      }
      writer.writeLine("</DL>");
   }
}

void writeFields(TextFileWriter& writer, IniConfigFile& config, const char* name)
{
   // field section
   writer.writeLine("<TR BGCOLOR=\"#CCCCFF\" CLASS=\"TableHeadingColor\">");
   writer.writeLine("<TD COLSPAN=2><FONT SIZE=\"+2\">");
   writer.writeLine("<B>Field Summary</B></FONT></TD>");

   ConfigCategoryIterator it = config.getCategoryIt(name);
   while (!it.Eof()) {
      if (String::compare(it.key(), "#field")) {
         const char* descr = find(*it, ';');
         writer.writeLine("<TR BGCOLOR=\"white\" CLASS=\"TableRowColor\">");
         writer.writeLine("<TD ALIGN=\"right\" VALIGN=\"top\" WIDTH=\"30%\">");
         writer.writeLine("<CODE>");
         writeLeft(writer, *it, descr);
         writer.writeLine("&nbsp;</CODE>");
         writer.writeLine("</TD>");
         writer.writeLiteral("<TD><CODE>");
         writer.writeLiteral(descr);
         writer.writeLine("</CODE>");
         writer.writeLine("</TD>");
         writer.writeLine("</TR>");
      }
      it++;
   }
}

void writeProperties(TextFileWriter& writer, IniConfigFile& config, const char* name)
{
   // property section
   writer.writeLine("<TR BGCOLOR=\"#CCCCFF\" CLASS=\"TableHeadingColor\">");
   writer.writeLine("<TD COLSPAN=2><FONT SIZE=\"+2\">");
   writer.writeLine("<B>Property Summary</B></FONT></TD>");

   ConfigCategoryIterator it = config.getCategoryIt(name);
   while (!it.Eof()) {
      if (String::compare(it.key(), "#property")) {
         const char* descr = find(*it, ';');
         writer.writeLine("<TR BGCOLOR=\"white\" CLASS=\"TableRowColor\">");
         writer.writeLine("<TD ALIGN=\"right\" VALIGN=\"top\" WIDTH=\"30%\">");

         writeLink(writer, *it, "stdproperties.html");
         writer.writeLine("</DT>");

         writer.writeLiteral("<TD><CODE>");
         writer.writeLiteral(descr);
         writer.writeLine("</CODE>");
         writer.writeLine("</TD>");
         writer.writeLine("</TR>");

      }
      it++;
   }
}

void writeMethods(TextFileWriter& writer, IniConfigFile& config, const char* name)
{
   if (emptystr(config.getSetting(name, "#method", NULL)))
      return;

   // method section
   writer.writeLine("<A NAME=\"method_summary\"><!-- --></A>");

   writer.writeLine("<TR BGCOLOR=\"#CCCCFF\" CLASS=\"TableHeadingColor\">");
   writer.writeLine("<TD COLSPAN=2><FONT SIZE=\"+2\">");
   writer.writeLine("<B>Method Summary</B></FONT></TD>");
   writer.writeLine("</TR>");

   ConfigCategoryIterator it = config.getCategoryIt(name);
   while (!it.Eof()) {
      if (String::compare(it.key(), "#method")) {
         writer.writeLine("<TR BGCOLOR=\"white\" CLASS=\"TableRowColor\">");
         writer.writeLine("<TD ALIGN=\"right\" VALIGN=\"top\" WIDTH=\"30%\">");
         writer.writeLine("<CODE>&nbsp;");

         const char* message = *it;
         const char* descr = find(message, ';');

         writeMessage(writer, message);

         writer.writeLine("</TD>");
         writer.writeLiteral("<TD><CODE>");

         if (!emptystr(descr)) {
            writer.writeLiteral(descr);
         }
         else writer.writeLiteral("&nbsp;");
         writer.writeLine("</CODE>");
         writer.writeLine("</TD>");
         writer.writeLine("</TR>");
      }
      it++;
   }
}

void writeBody(TextFileWriter& writer, IniConfigFile& config, const char* name,  const char* moduleName)
{
   const char* title = config.getSetting(name, "#title", NULL);
   if (title==NULL)
      title = name;

   writer.writeLiteral("<A NAME=\"");
   writer.writeLiteral(name);
   writer.writeLine("\">");
   writer.writeLine("<HR>");
   writer.writeLine("<!-- ======== START OF CLASS DATA ======== -->");
   writer.writeLine("<H2>");
   writer.writeLine("<FONT SIZE=\"-1\">");
   writer.writeLiteral(moduleName);
   writer.writeLine("</FONT>");
   writer.writeLine("<BR>");
   writer.writeLiteral(title);
   writer.writeLine("</H2>");

   if (!emptystr(config.getSetting(name, "#parent", NULL))) {
      writeParents(writer, config, name, moduleName);
   }
   writeProtocols(writer, name, config);
   const char* descr = config.getSetting(name, "#shortdescr", NULL);
   if (!emptystr(descr)) {
      writer.writeLine("<P>");
      writer.writeLine(descr);
      writer.writeLine("<P>");
   }
   else writer.writeLine(name);

   writer.writeLine("<P>");

   writer.writeLine("<TABLE BORDER=\"1\" CELLPADDING=\"3\" CELLSPACING=\"0\" WIDTH=\"100%\">");

   if (!emptystr(config.getSetting(name, "#field", NULL))) {
      writer.writeLine("<!-- =========== FIELD SUMMARY =========== -->");
      writeFields(writer, config, name);
   }

   if (!emptystr(config.getSetting(name, "#property", NULL))) {
      writer.writeLine("<!-- =========== PROPERTY SUMMARY =========== -->");
      writeProperties(writer, config, name);
   }

   writer.writeLine("<!-- ========== METHOD SUMMARY =========== -->");
   writeMethods(writer, config, name);

   writer.writeLine("</TABLE>");
}

void writeFooter(TextFileWriter& writer, const char* packageLink)
{
   writer.writeLine("<HR>");
   writer.writeLine("<A NAME=\"navbar_top\"><!-- --></A>");
   writer.writeLine("<TABLE BORDER=\"0\" WIDTH=\"100%\" CELLPADDING=\"1\" CELLSPACING=\"0\">");
   writer.writeLine("<TR>");
   writer.writeLine("<TD COLSPAN=2 BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\">");
   writer.writeLine("<A NAME=\"navbar_top_firstrow\"><!-- --></A>");
   writer.writeLine("<TABLE BORDER=\"0\" CELLPADDING=\"0\" CELLSPACING=\"3\">");
   writer.writeLine("  <TR ALIGN=\"center\" VALIGN=\"top\">");
   writer.writeLine("  <TD BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\">    <A HREF=\"index.html\"><FONT CLASS=\"NavBarFont1\"><B>Overview</B></FONT></A>&nbsp;</TD>");
   if (!emptystr(packageLink)) {
      writer.writeLiteral("  <TD BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\"><A HREF=\"");
      writer.writeLiteral(packageLink);
      writer.writeLine("\"><FONT CLASS=\"NavBarFont1\"><B>Package</B></FONT></A>&nbsp;</TD>");
   }
   else writer.writeLine("  <TD BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\">    <FONT CLASS=\"NavBarFont1\">Package</FONT>&nbsp;</TD>");
   writer.writeLine("  </TR>");
   writer.writeLine("</TABLE>");
   writer.writeLine("</TD>");

   writer.writeLine("<TD ALIGN=\"right\" VALIGN=\"top\" ROWSPAN=3><EM><b>");
   writer.writeLiteral(TITLE2);
   writer.writeLine("</b></EM>");
   writer.writeLine("</TD>");
   writer.writeLine("</TR>");
   writer.writeLine("</TABLE>");
}

void writeSummaryFooter(TextFileWriter& writer)
{
   writer.writeLine("</TABLE>");
}

int main(int argc, char* argv[])
{
   printf("ELENA command line Html Documentation generator (C)2006-08 by Alexei Rakov\n");

   IniConfigFile config(true);

   if (argc != 2) {
      printf("api2html <file>\n");
      return 0;
   }

   config.load(argv[1], /*CP_OEMCP*/feAnsi); // !! temporal

   FileName fileName(argv[1]);
   String name(fileName);
   name.append(".html");

   String summaryname(fileName);
   summaryname.append("-summary");
   summaryname.append(".html");

   TextFileWriter bodyWriter(name, feAnsi, false);
   TextFileWriter summaryWriter(summaryname, feAnsi, false);

	const char* package = config.getSetting("#general#", "#name");
	const char* shortDescr = config.getSetting("#general#", "#shortdescr");
	writeHeader(summaryWriter, package, NULL);
   writeHeader(bodyWriter, package, summaryname);
   writeSummaryHeader(summaryWriter, package, shortDescr);

   ConfigCategoryIterator classNode = config.getCategoryIt("#list#");
   while (!classNode.Eof()) {
      writeSummaryTable(summaryWriter, config, classNode.key(), name);
      writeBody(bodyWriter, config, classNode.key(), package);

      classNode++;
   }
   writeSummaryFooter(summaryWriter);
   writeFooter(summaryWriter, NULL);
   writeFooter(bodyWriter, summaryname);
   return 0;
}

