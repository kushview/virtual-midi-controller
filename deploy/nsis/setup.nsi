; Virtual MIDI Controller NSIS Installer Script
; Generated for Kushview Virtual MIDI Controller

!define PRODUCT_NAME "Virtual MIDI Controller"
!define PRODUCT_VERSION "1.0.0"
!define PRODUCT_PUBLISHER "Kushview"
!define PRODUCT_WEB_SITE "https://kushview.net"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\Virtual MIDI Controller.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI 2.0 compatible ------
!include "MUI2.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "..\..\LICENSE.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\Virtual MIDI Controller.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "VMC-Setup-${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES\Kushview\VMC"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  
  ; Install main executable
  File "..\..\build\virtual-midi-controller_artefacts\Debug\Virtual MIDI Controller.exe"
  
  ; Create shortcuts
  CreateDirectory "$SMPROGRAMS\Kushview"
  CreateShortCut "$SMPROGRAMS\Kushview\Virtual MIDI Controller.lnk" "$INSTDIR\Virtual MIDI Controller.exe"
  CreateShortCut "$DESKTOP\Virtual MIDI Controller.lnk" "$INSTDIR\Virtual MIDI Controller.exe"
  
  ; Install additional files
  SetOutPath "$INSTDIR\devices"
  File "..\..\devices\device.lua"
  
  ; Install documentation
  SetOutPath "$INSTDIR"
  File "..\..\README.md"
  File "..\..\LICENSE.txt"
  
  ; Install data files
  SetOutPath "$INSTDIR\data"
  File "..\..\data\*.png"
SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\Kushview\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\Kushview\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Virtual MIDI Controller.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\Virtual MIDI Controller.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\Virtual MIDI Controller.exe"
  Delete "$INSTDIR\README.md"
  Delete "$INSTDIR\LICENSE.txt"
  
  Delete "$INSTDIR\data\*.png"
  RMDir "$INSTDIR\data"
  
  Delete "$INSTDIR\devices\device.lua"
  RMDir "$INSTDIR\devices"

  Delete "$SMPROGRAMS\Kushview\Uninstall.lnk"
  Delete "$SMPROGRAMS\Kushview\Website.lnk"
  Delete "$SMPROGRAMS\Kushview\Virtual MIDI Controller.lnk"
  Delete "$DESKTOP\Virtual MIDI Controller.lnk"

  RMDir "$SMPROGRAMS\Kushview"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd