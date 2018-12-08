proc comp_file {file1 file2} {
    # optimization: check file size first
    set equal 0
    if {[file size $file1] == [file size $file2]} {
        set fh1 [open $file1 r]
        set fh2 [open $file2 r]
        set equal [string equal [read $fh1] [read $fh2]]
        close $fh1
        close $fh2
    }
    return $equal
}
set test_files [glob test*.in]
set num_tests [llength $test_files]
exec make
foreach file $test_files {
	puts "-I- sourcing $file"
	set res_file [lindex [split $file .] 0].res
	set out_file [lindex [split $file .] 0].out
	set res [split [exec ./hw3 < $file] \n]
	file delete $res_file
	set fh [open $res_file "w+"]
	foreach line $res {
		puts $fh $line
	}
	close $fh
	if {[comp_file $out_file $res_file]} {
		incr num_tests -1
		puts "Test $file clean"
		file delete $res_file
	}
}
if {$num_tests == 0} {
	puts "############################################################"
	puts "################### ALL CLEAN ##############################"
	puts "############################################################"
} else {
	
	puts "############################################################"
	puts "################### Failed #################################"
	puts "############################################################"
	set res_files [glob test*.res]
	puts "To check results run the following commands"
	foreach file $res_files {
		puts "diff [lindex [split $file .] 0].out $file"
	}
}
