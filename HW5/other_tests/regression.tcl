proc set_variables_in_runner_file {file_descriptor file s_file res_file} {
	puts $file_descriptor "set m_file $file"
	puts $file_descriptor "set res_file $res_file"
	puts $file_descriptor {catch {[exec ./hw5 < $m_file > $s_file]} }
	puts $file_descriptor {catch {[exec ./spim -file $s_file > $res_file]} }
}

set test_files [glob regression/OurTestsCompi5/t*.in]
set num_tests [llength $test_files]
exec make
# source add_main_to_tests.tcl
foreach file $test_files {
	puts "Running $file"
	set runner_file_descriptor [open runner.tcl w+]
	set res_file [lindex [split $file .] 0].res
	set s_file [lindex [split $file .] 0].s
	set out_file [lindex [split $file .] 0].out
	set_variables_in_runner_file $runner_file_descriptor $file $s_file $res_file
	close $runner_file_descriptor
	source runner.tcl
	set differences [catch {exec diff $res_file $out_file} output]
	if {$differences == 0} {
	    puts "TEST PASSED"
	} else {
	    if {[lindex $::errorCode 0] eq "CHILDSTATUS"} {
		if {[lindex $::errorCode 2] == 1} {
		    puts "TEST FAILED"
		    # show output without "child process exited abnormally" message
		    puts [string replace $output end-31 end ""]
		 } else {
		 puts "diff error: $output"
		}
	    } else {
		puts "error calling diff: $output"
		}
	}
}
