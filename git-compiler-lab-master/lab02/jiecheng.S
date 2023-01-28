    .arch armv5t
    .comm i,4
    .comm n,4  
    .comm f,4
    .text
    .align 2
    .section .rodata
    .align 2
_str0:
    .ascii  "%d\0"
    .text
    .align  2

    .global loop
loop:
    @ 保存栈帧
    str fp,[sp,#-4]!
    mov fp,sp
    @ 开辟栈帧，保存变量
    sub sp,sp,#16
    str r0,[fp,#-8]  @i
    str r1,[fp,#-12] @n
    str r2,[fp,#-16] @f
.L3:
    cmp r1,r0
    blt .L2 @branch if exit
    @ 复合语句
    mul r4,r0,r2 @f = f * i
    mov r2,r4
    add r0,r0,#1 @i = i + 1
    b .L3
.L2:
    @ 恢复栈帧
    add sp,fp,#0
    ldr fp,[sp],#4
    @ 函数返回
    bx lr
    .global main
main:
    push {fp,lr}
    add fp,sp,#4
    ldr r2,_bridge @r2 = &f
    ldr r1,_bridge+4 @r1 = &n
    ldr r0,_bridge+8 @r0 = &i
    @ 赋值语句
    mov r5,#1
    mov r4,#4
    mov r3,#2
    str r5,[r2] @f = 1
    str r4,[r1] @n = 4
    str r3,[r0] @i = 2
    ldr r5,_bridge
    ldr r4,_bridge+4
    ldr r3,_bridge+8
    @ 传递参数
    ldr r2,[r5]
    ldr r1,[r4]
    ldr r0,[r3]
    @ 函数跳转
    bl loop
    mov r1,r2
    ldr r0,_bridge+12
    bl printf
    mov r0,#0
    pop {fp,pc}
_bridge:
    @ int
    .word f @r2 and r5
    .word n @r1 and r4
    .word i @r0 and r3
    @ string
    .word _str0

.section .note.GNU-stack,"",%progbits
