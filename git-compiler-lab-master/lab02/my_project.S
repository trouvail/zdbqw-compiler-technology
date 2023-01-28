  .arch armv7-a @处理器架构
  .arm
@r0是格式化字符串，r1是对应的printf对应的第二个参数
  .text @代码段
  .global main
  .type main, %function
@主函数
main: 
  push {fp, lr} @将fp的当前值保存在堆栈上，然后将sp寄存器的值保存在fp中，lr中存储的是pc的保存在lr中
  @此处是赋值操作
  sub sp, sp, #4 @在栈中开辟一块大小为4的内存地址，用于存储即将输入的数据
  ldr r0, =_cin
  mov r1, sp @将sp的值传输给r1寄存器，使scanf传入的值存储在栈上，即栈顶的值是a
  bl scanf
  ldr r6, [sp, #0] @取出sp指针指向的地址中的内容，即栈顶中的内容（输入的a的值）
  add sp, sp, #4 @恢复栈顶，释放内存空间

  @r6中存a的值
  @接下来是判断操作
  cmp r6, #0x64
  ble COMPARE @比较0x64和r6（即100和a）的大小用于跳转
  @输出"your number is greater than 100!\n"
  ldr r0, =_bridge3
  bl printf
  b RETURN @不论怎么样都跳转

COMPARE:
  @输出"your number is below 100!\n"
  ldr r0, =_bridge2
  bl printf

RETURN:
  pop {fp, lr} @上下文切换
  bx lr @return 0
.data @数据段
_cin:
  .asciz "%d"

_bridge1:
  .asciz "%d\n"

_bridge2:
  .asciz "your number is below 100!\n"

_bridge3:
  .asciz "your number is greater than 100!\n"

.section .note.GNU-stack,"",%progbits @ do you know what's the use of this :-)
