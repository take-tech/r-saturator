#define MyAppName "R-Saturator"
#define MyAppVersion GetEnv("RSATURATOR_VERSION")
#if MyAppVersion == ""
#define MyAppVersion "0.2.0"
#endif
#define MyAppPublisher "Ranze"
#define MyAppURL "https://github.com/take-tech/r-saturator"
#define SourceVst3 GetEnv("RSATURATOR_VST3_DIR")
#if SourceVst3 == ""
#define SourceVst3 "..\\..\\build\\RSaturator_artefacts\\Release\\VST3\\R-Saturator.vst3"
#endif

[Setup]
AppId={{2E8D4C1F-1B79-4D8B-99E7-936C516B36BA}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={commoncf}\VST3
DisableDirPage=yes
DisableProgramGroupPage=yes
OutputBaseFilename=R-Saturator-{#MyAppVersion}-Windows-x64-Setup
OutputDir=..\..\build\packages
Compression=lzma2
SolidCompression=yes
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=admin
UninstallDisplayName={#MyAppName}

[Files]
Source: "{#SourceVst3}\*"; DestDir: "{commoncf}\VST3\R-Saturator.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs

[InstallDelete]
Type: filesandordirs; Name: "{commoncf}\VST3\R-Saturator.vst3"
