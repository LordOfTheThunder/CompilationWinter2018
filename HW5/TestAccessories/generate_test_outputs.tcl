# Generate tests from inputs... Hopefully the code is ok

source add_main_to_tests.tcl
set input_files [glob test*.in]
foreach file $input_files {
	set out [exec ./hw5 < $file]
	set out_file [lindex [split $file .] 0].out
	set asm_file [lindex [split $file .] 0].asm
	exec ./hw5 < $file > $asm_file
	exec ./spim -file $asm_file > $out_file
	file delete $asm_file
}
