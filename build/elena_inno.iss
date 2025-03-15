; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{3CAA69D3-0F98-44B1-A73E-E864BA51D5BD}
AppName=ELENA Programming Language
AppVersion=6.6.2
;AppVerName=ELENA Programming Language 6.6.0
AppPublisher=Alexey Rakov
AppPublisherURL=http://github.com/ELENA-LANG/elena-lang
AppSupportURL=http://github.com/ELENA-LANG/elena-lang
AppUpdatesURL=http://github.com/ELENA-LANG/elena-lang
DefaultDirName={pf}\ELENA
DefaultGroupName=ELENA Programming Language
AllowNoIcons=yes
LicenseFile=..\doc\license
InfoAfterFile=..\CHANGELOG.md
OutputBaseFilename=elena-lang-6.6.2.x86-win-setup
Compression=lzma
SolidCompression=yes
ChangesEnvironment=yes
AlwaysRestart=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "x86\bin\*"; DestDir: "{app}\bin"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "x86\doc\*"; DestDir: "{app}\doc"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "x86\examples60\*"; DestDir: "{app}\examples60"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "x86\src60\*"; DestDir: "{app}\src60"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "x86\lib60\*"; DestDir: "{app}\lib60"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\doc\license"; DestDir: "{app}";
Source: "..\readme.md"; DestDir: "{app}"; Flags: isreadme
Source: "..\CHANGELOG.md"; DestDir: "{app}";
Source: "redist\VC_redist.x86.exe"; DestDir: "{app}\redist"; Flags: deleteafterinstall
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\ELENA Programming Language"; Filename: "{app}\bin\elena-ide.exe "
Name: "{commondesktop}\ELENA Programming Language"; Filename: "{app}\bin\elena-ide.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\ELENA Programming Language"; Filename: "{app}\bin\elena-ide.exe"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\redist\VC_redist.x86.exe"; Parameters: "/install /passive /norestart"; StatusMsg: Installing VC++ 2017 Redistributables...; Check: not VCinstalled
Filename: "{app}\bin\elena-ide.exe"; Description: "{cm:LaunchProgram,ELENA Programming Language}"; Flags: nowait postinstall skipifsilent

[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}\bin"; Check: NeedsAddPath('{app}\bin')

[Code]
function NeedsAddPath(Param: string): boolean;
var
  OrigPath: string;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE,
    'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
    'Path', OrigPath)
  then begin
    Result := True;
    exit;
  end;
  // look for the path with leading and trailing semicolon
  // Pos() returns 0 if not found
  Result := Pos(';' + Param + ';', ';' + OrigPath + ';') = 0;
end;

function VCinstalled: Boolean;
 // By Michael Weiner <mailto:spam@cogit.net>
 // Function for Inno Setup Compiler
 // 13 November 2015
 // Returns True if Microsoft Visual C++ Redistributable is installed, otherwise False.
 // The programmer may set the year of redistributable to find; see below.
 var
  names: TArrayOfString;
  i: Integer;
  dName, key, year: String;
 begin
  // Year of redistributable to find; leave null to find installation for any year.
  year := '2017';
  Result := False;
  key := 'Software\Microsoft\Windows\CurrentVersion\Uninstall';
  // Get an array of all of the uninstall subkey names.
  if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, key, names) then
   // Uninstall subkey names were found.
   begin
    i := 0
    while ((i < GetArrayLength(names)) and (Result = False)) do
     // The loop will end as soon as one instance of a Visual C++ redistributable is found.
     begin
      // For each uninstall subkey, look for a DisplayName value.
      // If not found, then the subkey name will be used instead.
      if not RegQueryStringValue(HKEY_LOCAL_MACHINE, key + '\' + names[i], 'DisplayName', dName) then
       dName := names[i];
      // See if the value contains both of the strings below.
      Result := (Pos(Trim('Visual C++ ' + year),dName) * Pos('Redistributable',dName) <> 0)
      i := i + 1;
     end;
   end;
 end;