Set oShell = CreateObject ("Wscript.Shell") 
Dim strArgs
scriptdir = CreateObject("Scripting.FileSystemObject").GetParentFolderName(WScript.ScriptFullName)
batfile = scriptdir + "\\buildContent.bat"
strArgs = "cmd /c "
strArgs = strArgs & batfile
oShell.Run strArgs, 0, false