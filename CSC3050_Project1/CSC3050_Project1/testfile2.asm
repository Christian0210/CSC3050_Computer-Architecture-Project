P1: add $t0, $t1, $t2
P2: addu $sp, $t1, $v0  # comment #
    addi $sp, $sp, -8
    addiu $a0, $v0, 1 # $ra, comment, PPP:
    and $v0, $v0, $t6
    andi $v0, $v0, -1
    
    j P1
    j P2
    jal P1
    # comment line PP:
    jal P2
    div $t2, $t1

    # also a comment
P3: divu $t2, $t3 # comment
    mult $a0, $a1
    multu $a1, $a2
    nor $v0, $fp,                       $ra
    jr $ra
    jr $t0
    or $t3, $t4, $t5
    ori $t1, $ra, 0
    sll $t0, $t1, 31
    sllv $t1, $t2, $t3
    sra $ra, $ra, 1
    srav $ra, $sp, $v1
    srl  $t0, $t1, 12
    srlv $t0, $t1, $t3
    
            
    sub $s0, $s7, $zero
    subu $k0, $k1, $gp
    xor $v0, $v1, $k0
    xori $v1, $zero, 123
    lui $v1, 65535
    slt 	$ra,                 $gp, $sp
    sltu $t0, $t1, $t2
    slti $v0, $v1, 5261
    sltiu $v0, $v1, 5524
L1: beq  $t0, $t1, L1
L2: beq  $t0, $t2, L2
__special1: bgez $v0, P1
    bgezal $v1, __special1
    bgtz $k0, __special2 
__special2: blez $k1,       __special1 # comment 
    bltzal $s4, P3
    bltz $ra, __special2
    bne $v0, $v1, L2
    jalr $v0, $k1
    teq $v0, $v1
    teqi $ra, 123
    tne $k0, $k1
    tnei $gp, 0
    tge $v0, $v1
    tgei $fp, 0
    tgeu $fp, $gp
    tgeiu $v0, 999
    
    tlt $v0, $v1
    tlti $fp, 0
    tltu $fp, $gp
    tltiu $v0, 999
    lb $ra, -12($sp)
    lb $ra, 0($sp)
    lb $v0, 12($v1)
    lbu $ra, -12($sp)
    lbu $ra, 0($sp)
    lbu $v0, 12($v1)
    lh  $t0, 5000($sp)
    lh $ra, 0($sp)
    lh $v0, 12($v1)
    lhu  $t0, 5000($gp)
    lhu $ra, 0($fp)
    lhu $v0, 12($v0)
    lhu  $t0, -5000($gp)
    lw $ra, -1($fp)
    lw $v0, 24($v0)
    lwl $ra, -1($fp)
    lwl $v0, 24($v0)
    lwr $ra, -1($fp)
    lwr $v0, 24($v0)
    ll  $ra, -1($fp)
    ll  $v0, 24($v0)
    
    
    
    
    
    
    
    
    sb $ra, -12($sp)
    sb $ra, 0($sp)
    sb $v0, 12($v1)
    sh $ra, -12($sp)
    sh $ra, 0($sp)
    sh $v0, 12($v1)
    sw  $t0, 5000($sp)
    sw $ra, 0($sp)
    sw $v0, 12($v1)
    swl  $t0, 5000($gp)
    swl $ra, 0($fp)
    swl $v0, 12($v0)
    swr  $t0, -5000($gp)
    swr $ra, -1($fp)
    swr $v0, 24($v0)
    sc $ra, -1($fp)
    sc $v0, 24($v0)
    sc $ra, -1($fp)
    
    mfhi $ra
    mflo $sp
    mthi $t0
    mtlo $v0
    
    
