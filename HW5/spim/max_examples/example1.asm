.data

.text
.globl 	main
.ent	main

print:
move $fp, $sp
printi:
move $fp, $sp
main:
move $fp, $sp
li $t0, 3
subu $sp, $sp, 4
sw $t0, ($sp)
li $t1, 5
subu $sp, $sp, 4
sw $t1, ($sp)
lw $t2, -4($fp)
lw $t3, -8($fp)
addu $t2, $t2, $t3
subu $sp, $sp, 4
sw $t2, ($sp)


li $v0, 10
syscall
.end main