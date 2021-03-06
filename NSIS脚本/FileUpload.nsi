; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "FileUpload"
!define PRODUCT_VERSION "1.0"
!define PRODUCT_PUBLISHER "GeekRRK"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\Update.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define LOCAL_DIR "D:\Projects\FileUpload\NSIS脚本"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${LOCAL_DIR}\img\setup.ico"
!define MUI_UNICON "${LOCAL_DIR}\img\uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\FileUpload.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "SimpChinese"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "Setup.exe"
InstallDir "$PROGRAMFILES\FileUpload"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Function .onInit
  ;关闭进程
  Push $R0
  CheckProc:
    Push "FileUpload.exe"
    ProcessWork::existsprocess
    Pop $R0
    IntCmp $R0 0 Done
    Push "FileUpload.exe"
    Processwork::KillProcess
    Sleep 1000
    Goto CheckProc
    Done:
    Pop $R0
FunctionEnd

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite on
  File "${LOCAL_DIR}\exe\Update.exe"
  File "${LOCAL_DIR}\exe\FileUpload.exe"
  File "${LOCAL_DIR}\cfg\config.ini"
  CreateShortCut "$DESKTOP\FileUpload.lnk" "$INSTDIR\FileUpload.exe" ${LOCAL_DIR}\img\upload.ico
  CreateDirectory "$SMPROGRAMS\FileUpload"
  CreateShortCut "$SMPROGRAMS\FileUpload\FileUpload.lnk" "$INSTDIR\FileUpload.exe" ${LOCAL_DIR}\img\upload.ico
SectionEnd

Section -AdditionalIcons
  CreateShortCut "$SMPROGRAMS\FileUpload\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Update.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\Update.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) 已成功地从你的计算机移除。"
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "你确实要完全移除 $(^Name) ，其及所有的组件？" IDYES +2
  Abort
  ;关闭进程
  Push $R0
  CheckProc:
    Push "FileUpload.exe"
    ProcessWork::existsprocess
    Pop $R0
    IntCmp $R0 0 Done
    Push "FileUpload.exe"
    Processwork::KillProcess
    Sleep 1000
    Goto CheckProc
    Done:
    Pop $R0
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\config.ini"
  Delete "$INSTDIR\FileUpload.exe"
  Delete "$INSTDIR\Update.exe"

  Delete "$SMPROGRAMS\FileUpload\Uninstall.lnk"
  Delete "$DESKTOP\FileUpload.lnk"
  Delete "$SMPROGRAMS\FileUpload\FileUpload.lnk"

  RMDir "$SMPROGRAMS\FileUpload"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
