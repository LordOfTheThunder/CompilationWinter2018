.data

.text
.globl     main
.ent     main
main:
subu $sp, $sp, 4
sw $ra, ($sp)
jal __main
ExitCode: li $v0, 10
syscall
__print:
lw $a0, 0($sp)
li $v0, 4
syscall
jr $ra
__printi:
lw $a0, 0($sp)
li $v0, 1
syscall
jr $ra
DivisionByZero: .asciiz "Error division by zero\n"
j ExitCode
__main:
move $fp, $sp
li $t0, 3
subu $sp, $sp, 4
sw $t0, ($sp)
li $t1, 0
subu $sp, $sp, 4
sw $t1, ($sp)
lw $t2, -4($fp)
lw $t3, -8($fp)
beq $t3, 0, DivisionByZero
div $t2, $t2, $t3
subu $sp, $sp, 4
sw $t2, ($sp)


li $v0, 10
syscall
.end main