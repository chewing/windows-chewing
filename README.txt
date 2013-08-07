Build step:
1. Checkout windows-chewing in the same libchewing folder.
   It should be follow list.
x:\libchewing
x:\windows-chewing

2. Make "libchewing_win32" folder ,and generators libchewing's makefile by cmake

3. build "chewing.vcxproj" debug mode in "libchewing_win32" folder to create chewing.lib in debug folder.
   you can find the chewing.lib in your "x:\libchewing\debug\" folder.
   
4. Now you can build the windows-chewing.

Know Issue:
1. HashEd_UTF8 project will build error
2. ChewingIME and ChewingServer can build success, but still cannot use.