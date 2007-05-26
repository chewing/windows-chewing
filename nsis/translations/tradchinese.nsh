!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_TRADCHINESE}

!insertmacro CHEWING_LANG_BEGIN

LicenseLangString LICENSE_FILE ${CURLANG} "License.txt"

LangString NAME				${CURLANG} "新酷音輸入法 ${PRODUCT_VERSION}"

LangString DETECTED_OLD_VERSION		${CURLANG} "偵測到已安裝舊版"
LangString REINSTALL_CONFIRM		${CURLANG} "，是否要移除舊版後繼續安裝新版？"

LangString VISIT_WEBSITE		${CURLANG} "參觀新酷音輸入法網站： "

LangString INSTALL_FAIL			${CURLANG} "安裝發生錯誤，請確定你有系統管理員權限，以及舊版不在執行中$\n$\n建議到控制台輸入法設定當中，移除舊版並重開機後再安裝一次。"

LangString UNINSTALL_SUCCESS		${CURLANG} "已成功地從你的電腦移除 $(^Name) 。"
LangString UNINSTALL_CONFIRM		${CURLANG} "你確定要完全移除 $(^Name) ，其及所有的元件？"

LangString STARTMENU_FOLDER_NAME	${CURLANG} "新酷音輸入法"
LangString LNK_USER_Manual		${CURLANG} "新酷音輸入法使用說明"
LangString LNK_DB_EDITOR		${CURLANG} "使用者詞庫編輯工具"
LangString LNK_UPDATE_CHECKER		${CURLANG} "線上檢查是否有新版本"
LangString LNK_UNINSTALL		${CURLANG} "解除安裝"

!insertmacro CHEWING_LANG_END
