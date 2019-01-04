.data

.text
.globl 	__main
.ent	__main

print:
move $fp, $sp
printi:
move $fp, $sp
main:
move $fp, $sp
subu $sp, $sp, 4
sw $0, ($sp)
subu $sp, $sp, 4
sw $0, ($sp)
lw $t0, 0($fp)
lw $t1, -4($fp)
addu $t0, $t0, $t1
subu $sp, $sp, 4
sw $t0, ($sp)


li $v0, 10
syscall
.end main