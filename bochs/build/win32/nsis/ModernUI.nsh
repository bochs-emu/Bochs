;Modern UI Header File version 1.01
;Written by Joost Verburg

;See Example.nsi & Multilanguage.nsi for an example of usage

!ifndef MUI_MACROS_USED

!define MUI_MACROS_USED

!macro MUI_INTERFACE UI ICON UNICON CHECKS PROGRESSBAR

  ;User interface
  
  Icon "${ICON}"
  UninstallIcon "${UNICON}"
  XPStyle On
  ChangeUI all "${UI}"
  SetFont Tahoma 8
  CheckBitmap "${CHECKS}"
  InstallColors /windows
  InstProgressFlags "${PROGRESSBAR}"
  BrandingText /TRIMRIGHT

!macroend

!macro MUI_FINISHHEADER CALL

  ;Finish text on the header (white rectangle)
  
  IntOp ${CURRENTPAGE} ${CURRENTPAGE} + 1
  Call ${CALL}

!macroend

!macro MUI_INNERDIALOG_INIT

  ;Extra text elements on the inner dialogs

  Push ${TEMP1}
  Push ${TEMP2}
  
  FindWindow ${TEMP1} "#32770" "" $HWNDPARENT

!macroend

!macro MUI_INNERDIALOG_START PAGE

  StrCmp ${CURRENTPAGE} ${PAGE} "" done_${PAGE}
  
!macroend

!macro MUI_INNERDIALOG_TEXT LANGID ELEMENT TEXT

  StrCmp $LANGUAGE ${LANGID} "" +3
    GetDlgItem ${TEMP2} ${TEMP1} ${ELEMENT}
    SendMessage ${TEMP2} ${WM_SETTEXT} 0 "${TEXT}"
	
!macroend

!macro MUI_INNERDIALOG_STOP PAGE

  done_${PAGE}:
  
!macroend

!macro MUI_INNERDIALOG_END

  Pop ${TEMP2}
  Pop ${TEMP1}

!macroend

!macro MUI_NEXTPAGE_OUTER

  Push ${TEMP1}
  Push ${TEMP2}  

  ;Set backgrounds & fonts for the outer dialog (only once)
  StrCmp ${CURRENTPAGE} "" "" no_first_run
   
    GetDlgItem ${TEMP1} $HWNDPARENT 1037
    CreateFont ${TEMP2} "Tahoma" 10 1000
    SendMessage ${TEMP1} ${WM_SETFONT} ${TEMP2} 0
    SetStaticBkColor ${TEMP1} 0x00FFFFFF
 
    GetDlgItem ${TEMP1} $HWNDPARENT 1038
    SetStaticBkColor ${TEMP1} 0x00FFFFFF

    GetDlgItem ${TEMP1} $HWNDPARENT 1034
    SetStaticBkColor ${TEMP1} 0x00FFFFFF

    GetDlgItem ${TEMP1} $HWNDPARENT 1039
    SetStaticBkColor ${TEMP1} 0x00FFFFFF

    no_first_run:
    
  Pop ${TEMP2}  
  Pop ${TEMP1}
  
!macroend

!macro MUI_NEXTPAGE CALL

  IntOp ${CURRENTPAGE} ${CURRENTPAGE} + 1

  Call "${CALL}"

!macroend

!macro MUI_PREVPAGE

  IntOp ${CURRENTPAGE} ${CURRENTPAGE} - 1

  Call SetHeader
  
!macroend

!macro MUI_HEADER_INIT

  Push ${TEMP1}
  Push ${TEMP2}

    GetDlgItem ${TEMP1} $HWNDPARENT 1037
    GetDlgItem ${TEMP2} $HWNDPARENT 1038
  
!macroend

!macro MUI_HEADER_START PAGE

   StrCmp ${CURRENTPAGE} ${PAGE} "" done_${PAGE}
  
!macroend

!macro MUI_HEADER_TEXT LANGID TEXT SUBTEXT

  ;Text on the white rectangle

  StrCmp $LANGUAGE ${LANGID} "" +4
    SendMessage ${TEMP1} ${WM_SETTEXT} 0 "${TEXT}"
    SendMessage ${TEMP2} ${WM_SETTEXT} 0 "${SUBTEXT}"
    Goto done

!macroend

!macro MUI_HEADER_STOP PAGE

  done_${PAGE}:
  
!macroend

!macro MUI_HEADER_END
  
  done:
  
  Pop ${TEMP2}
  Pop ${TEMP1}
  
!macroend

!macro MUI_DESCRIPTION_INIT

  Push ${TEMP1}

  FindWindow ${TEMP1} "#32770" "" $HWNDPARENT
  GetDlgItem ${TEMP1} ${TEMP1} 1043

!macroend

!macro MUI_DESCRIPTION_TEXT LANGID VAR TEXT

  ;Set text on the Description frame

  StrCmp $LANGUAGE ${LANGID} "" +4
    StrCmp $0 ${VAR} "" +3
      SendMessage ${TEMP1} ${WM_SETTEXT} 0 "${TEXT}"
      Goto done
    
!macroend

!macro MUI_DESCRIPTION_END

  done:
  Pop ${TEMP1}
  
!macroend

!macro MUI_ABORTWARNING LANGID TEXT

  ;Warning when Cancel button is pressed

  StrCmp $LANGUAGE ${LANGID} "" +3
    MessageBox MB_YESNO|MB_ICONEXCLAMATION "${TEXT}" IDYES quit
    Abort

!macroend

!macro MUI_ABORTWARNING_END

  quit:
  
!macroend

!endif