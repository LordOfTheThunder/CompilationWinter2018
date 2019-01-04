.data

.text
.globl 	main
.ent	main

main:
li $t0, 0
addu $t0, $t0, 4

li $v0, 10
syscall
.end main