property bochs_path : "Contents/MacOS/bochs"
property bochs_app : ""

on run
	tell application "Finder" to get container of (path to me) as string
	set script_path to POSIX path of result
	
	-- Locate bochs
	set bochs_alias to findBochs()
	
	-- Tell Terminal to run bochs from the command line
	--Use the script's directory as the current directory
	tell application "Terminal"
		activate
		do script "cd '" & script_path & "';exec '" & (POSIX path of bochs_app) & bochs_path&"'"
		-- Wait for Terminal to change the name first, then change it to ours
		delay 1
		set AppleScript's text item delimiters to "/"
		set the text_item_list to every text item of the script_path
		set AppleScript's text item delimiters to ""
		
		
		set next_to_last to ((count of text_item_list) - 1)
		set the folder_name to item next_to_last of text_item_list
		set name of front window to "Running bochs in ../" & folder_name & "/"
	end tell
end run

-- Taken from examples at http://www.applescriptsourcebook.com/tips/findlibrary.html
to Hunt for itemName at folderList
	--Returns path to itemName as string, or empty string if not found
	repeat with aFolder in folderList
		try
			if class of aFolder is constant then
				return alias ((path to aFolder as string) & itemName) as string
			else if folder of (info for alias aFolder) then
				return alias (aFolder & itemName) as string
			end if
		on error number -43 --item not there, go to next folder
		end try
	end repeat
	return "" --return empty string if item not found
end Hunt

on findBochs()
	try
		if bochs_app is "" then error number -43
		return alias bochs_app
	on error number -43
		-- bochs_app no good, go hunting
		try
			tell application "Finder" to get container of (path to me) as string
			set this_dir_alias to alias result
			tell application "Finder" to get container of (this_dir_alias) as string
			set one_up_dir_alias to alias result
			set TheUsualPlaces to {this_dir_alias as string, one_up_dir_alias as string}
			Hunt for "bochs.app" at TheUsualPlaces
			set result_alias to result
			if result_alias is "" then error number -43
			set bochs_app to result_alias as string
			return result_alias
		on error number -43
			--Give up seeking, Ask the user
			choose application with prompt "Please locate Bochs:" as alias
			set result_alias to result
			set bochs_app to result_alias as string
			return result_alias
		end try
	end try
end findBochs