property bochs_path : "bochs.app/Contents/MacOS/bochs"

set script_name to name of (info for (path to me))
set script_path to POSIX path of (path to me)

-- replace target string using delimiters
set AppleScript's text item delimiters to the script_name
set the text_item_list to every text item of the script_path
set AppleScript's text item delimiters to ""
set the script_path to the text_item_list as string

tell application "Terminal"
	activate
	do script "cd \"" & script_path & "\";" & bochs_path
	-- Wait for Terminal to change the name first, then change it to ours
	delay 1
	set AppleScript's text item delimiters to "/"
	set the text_item_list to every text item of the script_path
	set AppleScript's text item delimiters to ""
	set next_to_last to ((count of text_item_list) - 1)
	set the folder_name to item next_to_last of text_item_list
	set name of front window to "Running bochs in ../" & folder_name & "/"
end tell
