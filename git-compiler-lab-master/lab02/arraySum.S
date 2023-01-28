    .arch armv5t
    .comm arr,4
    .comm sum,4  
    .text
    .align 2
    .section .rodata
    .align 2
_str0:
    .ascii  "%d\0"
    .text
    .align  2

    .global main
main:
    push {fp,lr}
    add fp,sp,#4
    @ 变量初始化   
    ldr r0,_bridge @r0 -> &arr
    mov r1,#0 @r1 -> i
.loop1:
    @ 判断循环条件
    cmp r1,#10
    blt .content1 
    b .exit1
.content1:
    @ 数组初始化
    str r1,[r0] @arr[i] = i
    add r0,r0,#4  @arr -> arr+4 
    add r1,r1,#1  @i -> i+1
    b .loop1
.exit1:

    @ 变量初始化
    ldr r0,_bridge @r0 -> &arr
    mov r1,#0 @r1 -> i
    ldr r4,_bridge+4
    ldr r2,[r4] 
    mov r2,#0 @r2 -> sum   
.loop2:
    @ 判断循环条件
    cmp r1,#10
    blt .content2 
    b .exit2
.content2:
    @ 逐个遍历数组累加
    ldr r3,[r0] 
    add r2,r2,r3  @sum -> sum+arr[i]
    add r0,r0,#4  @arr -> arr+4 
    add r1,r1,#1  @i -> i+1
    b .loop2
.exit2:
    str r2,[r4]

    @ 打印数组结果
    mov r1,r2
    ldr r0,_bridge+8
    bl printf
    mov r0,#0
    pop {fp,pc}

_bridge:
    @ int*
    .word arr
    @ int
    .word sum
    @ string
    .word _str0

.section .note.GNU-stack,"",%progbits
