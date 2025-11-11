; -- rcalc.iss --

[Setup]
AppName=RegExpCalc
AppVerName=RegExpCalc (regular expression evaluator) 1.0 files
VersionInfoVersion=1.0.0.0
AppContact=http://dorlov.no-ip.com
DefaultDirName={autopf}\FC
DefaultGroupName=fcalc
UninstallDisplayIcon={app}\RegExpCalc.exe
LicenseFile=fclic.txt
Compression=lzma/ultra
SolidCompression=yes
OutputDir=Setup
OutputBaseFilename=RXcalc_setup
CreateUninstallRegKey=yes
PrivilegesRequired=admin
UsedUserAreasWarning=no
ArchitecturesAllowed=x86 x64
ArchitecturesInstallIn64BitMode=x64

[Types]
Name: "custom"; Description: "custom"; Flags: iscustom

[Components]
Name: "program"; Description: "RegExpCalc (regular expression evaluator)"; Types: custom; Flags: fixed

[Files]
; 64-bit version for x64 systems
Source: ".\x64\Release\RegExpCalc.exe"; DestDir: "{app}"; Components: program; Flags: ignoreversion; Check: Is64BitInstallMode
; 32-bit version for x86 systems  
Source: ".\Release\RegExpCalc.exe"; DestDir: "{app}"; Components: program; Flags: ignoreversion; Check: not Is64BitInstallMode
; Help file for both architectures
Source: "RegExpCalc.chm"; DestDir: "{app}"; Components: program; Flags: ignoreversion

; Visual C++ Redistributables
Source: "..\fcalc\APICalc\Redistr\VC_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall; Check: Is64BitInstallMode
Source: "..\fcalc\APICalc\Redistr\VC_redist.x86.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall; Check: not Is64BitInstallMode

[Registry]
; CurrentValues - initial values
Root: HKCU; Subkey: "Software\RegExpCalc\CurrentValues"; ValueType: string; ValueName: "Source"; ValueData: "<tag>content</tag><tag>more</tag>"; Components: program; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\RegExpCalc\CurrentValues"; ValueType: string; ValueName: "RegExp"; ValueData: "<tag>((.+?))</tag>"; Components: program; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\RegExpCalc\CurrentValues"; ValueType: string; ValueName: "Parameters"; ValueData: "10, 25.54"; Components: program; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\RegExpCalc\CurrentValues"; ValueType: string; ValueName: "Data"; ValueData: "10 25.4"; Components: program; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\RegExpCalc\CurrentValues"; ValueType: string; ValueName: "PrintfFormat"; ValueData: "%d \n %f"; Components: program; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\RegExpCalc\CurrentValues"; ValueType: string; ValueName: "ScanfFormat"; ValueData: "%d %f"; Components: program; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\RegExpCalc\CurrentValues"; ValueType: string; ValueName: "Replace"; ValueData: "--\0--"; Components: program; Flags: createvalueifdoesntexist

; Settings - application settings
Root: HKCU; Subkey: "Software\RegExpCalc\Settings"; ValueType: dword; ValueName: "CurrentMode"; ValueData: "0"; Components: program; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\RegExpCalc\Settings"; ValueType: dword; ValueName: "CurrentEngine"; ValueData: "2"; Components: program; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\RegExpCalc\Settings"; ValueType: dword; ValueName: "FontSize"; ValueData: "14"; Components: program; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\RegExpCalc\Settings"; ValueType: dword; ValueName: "Opacity"; ValueData: "100"; Components: program; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\RegExpCalc\Settings"; ValueType: dword; ValueName: "AlwaysOnTop"; ValueData: "0"; Components: program; Flags: createvalueifdoesntexist

Root: HKCU; Subkey: "Software\RegExpCalc\Settings"; ValueType: dword; ValueName: "WindowX"; ValueData: "100"; Components: program; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\RegExpCalc\Settings"; ValueType: dword; ValueName: "WindowY"; ValueData: "100"; Components: program; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\RegExpCalc\Settings"; ValueType: dword; ValueName: "RegExpWindowX"; ValueData: "100"; Components: program; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\RegExpCalc\Settings"; ValueType: dword; ValueName: "RegExpWindowY"; ValueData: "100"; Components: program; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\RegExpCalc\Settings"; ValueType: dword; ValueName: "RegExpWindowHeight"; ValueData: "400"; Components: program; Flags: createvalueifdoesntexist

Root: HKCU; Subkey: "Software\RegExpCalc\Settings"; ValueType: dword; ValueName: "PrintfWindowX"; ValueData: "100"; Components: program; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\RegExpCalc\Settings"; ValueType: dword; ValueName: "PrintfWindowY"; ValueData: "100"; Components: program; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\RegExpCalc\Settings"; ValueType: dword; ValueName: "PrintfWindowHeight"; ValueData: "200"; Components: program; Flags: createvalueifdoesntexist

Root: HKCU; Subkey: "Software\RegExpCalc\Settings"; ValueType: dword; ValueName: "ScanfWindowX"; ValueData: "100"; Components: program; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\RegExpCalc\Settings"; ValueType: dword; ValueName: "ScanfWindowY"; ValueData: "100"; Components: program; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\RegExpCalc\Settings"; ValueType: dword; ValueName: "ScanfWindowHeight"; ValueData: "200"; Components: program; Flags: createvalueifdoesntexist

[UninstallDelete]
Type: dirifempty; Name: "{app}"

[Icons]
Name: "{group}\RegExpCalc"; Filename: "{app}\RegExpCalc.exe"; WorkingDir: "{app}"; Components: program
Name: "{group}\Uninstall"; Filename: "{app}\unins000.exe"
Name: "{userdesktop}\RegExpCalc"; Filename: "{app}\RegExpCalc.exe"; WorkingDir: "{app}"; Components: program

[Run]
; Install VC++ Redistributable silently before main installation
Filename: "{tmp}\VC_redist.x64.exe"; Parameters: "/quiet /norestart"; StatusMsg: "Installing Visual C++ Redistributable (x64)..."; Check: Is64BitInstallMode and VCRedistNeedsInstall; Flags: waituntilterminated
Filename: "{tmp}\VC_redist.x86.exe"; Parameters: "/quiet /norestart"; StatusMsg: "Installing Visual C++ Redistributable (x86)..."; Check: not Is64BitInstallMode and VCRedistNeedsInstall; Flags: waituntilterminated

; Launch application after installation
Filename: "{app}\RegExpCalc.exe"; Description: "Launch application"; Flags: postinstall nowait skipifsilent unchecked

[Code]
function VCRedistNeedsInstall: Boolean;
var
  Version: String;
begin
  // Check VC++ 2015-2022 Redistributable
  if Is64BitInstallMode then
  begin
    Result := not RegQueryStringValue(HKEY_LOCAL_MACHINE, 
      'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\X64', 'Version', Version);
    if Result then
      Result := not RegQueryStringValue(HKEY_LOCAL_MACHINE,
        'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 'Version', Version);
  end
  else
  begin
    Result := not RegQueryStringValue(HKEY_LOCAL_MACHINE,
      'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\X86', 'Version', Version);
    if Result then
      Result := not RegQueryStringValue(HKEY_LOCAL_MACHINE,
        'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x86', 'Version', Version);
  end;
end;