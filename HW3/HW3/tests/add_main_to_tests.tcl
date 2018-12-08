proc addMainForProc {file} {
	# File to list
	set fh [open $file "r"]
	set text [read $fh]
	close $fh
	regexp {void[\ \r\n\t]+main} $text res
	if {![info exist res]} {
		set fh [open $file "w+"]
		puts $fh $text
		puts $fh "void main() { print(\"Max is cool\"); }"
		close $fh
		return
	}
	unset res
}
set input_files [glob test*.in]
foreach file $input_files {
	addMainForProc $file
}
