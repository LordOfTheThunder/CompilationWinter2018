# Generate tests from inputs... Hopefully the code is ok

exec make
set input_files [glob test*.in]
foreach file $input_files {
	set out [split [exec ./hw3 < $file] \n]
	set out_file [lindex [split $file .] 0].out
	file delete $out_file
	set fh [open $out_file "w+"]
	foreach line $out {
		puts $fh $line
	}
	close $fh
}
