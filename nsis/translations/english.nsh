!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_ENGLISH}

!insertmacro CHEWING_LANG_BEGIN

LicenseLangString LICENSE_FILE ${CURLANG} "License.txt"

LangString NAME				${CURLANG} "New Chewing ${PRODUCT_VERSION}"

LangString DETECTED_OLD_VERSION		${CURLANG} "Old version already installed"
LangString REINSTALL_CONFIRM		${CURLANG} ", do you want to uninstall old version?"

LangString VISIT_WEBSITE		${CURLANG} "Visit New Chewing web site: "

LangString INSTALL_FAIL			${CURLANG} "Install failed. Please make sure you have administrator privilege and old version is not in use.$\n$\nYou can uninstall old version from Control Panel and try to install again."

LangString UNINSTALL_SUCCESS		${CURLANG} "Uninstall $(^Name) successful"
LangString UNINSTALL_CONFIRM		${CURLANG} "Are you sure to uninstall $(^Name) and all its components?"

LangString STARTMENU_FOLDER_NAME	${CURLANG} "New Chewing"
LangString LNK_USER_MANUAL		${CURLANG} "User Manual"
LangString LNK_DB_EDITOR		${CURLANG} "User word database edit tool"
LangString LNK_UPDATE_CHECKER		${CURLANG} "Online Update"
LangString LNK_UNINSTALL		${CURLANG} "Uninstall"

!insertmacro CHEWING_LANG_END
