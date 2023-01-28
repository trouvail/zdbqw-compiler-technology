# 一些开始的记录

对中间代码进行自顶向下的遍历，从而生成目标代码


只需要完成一些基础表达式的翻译，再将汇编代码打印出来

## Instruction

- LoadMInstruction     从内存地址中加载值到寄存器中
- StoreMInstruction    将值存储到内存地址中
- BinaryMInstruction   二元运算指令, 包含一个目的操作数和两个源操作数
- CmpMInstruction      关系运算指令
- MovMInstruction      将源操作数的值赋值给目的操作数
- BranchMInstruction   跳转指令
- StackMInstruction    寄存器压栈、弹栈指令`(不是很熟悉)`

## Operand

- IMM                  立即数
- VREG                 虚拟寄存器。在进行目标代码转换时，我们首先假设有无穷多个寄存器，每个临时变量都会得到一个虚拟寄存器
- REG                  物理寄存器。在进行了寄存器分配之后，每个虚拟寄存器都会分配得到一个物理寄存器
- LABEL                地址标签，主要为 BranchMInstruction 及 LoadMInstruction 的操作数

# 汇编代码生成

指令说的都是中间代码的，中间有M的才是汇编代码

## 访存指令

> 重点：获得其地址

### 加载全局变量或常量

1. 从标签（Lable）加载其地址到寄存器
2. 访问刚加载到寄存器的地址获取值

### 加载栈中临时变量

以FP为基址寄存器，根据其栈内偏移生成一条加载指令

### 加载数组元素

其地址存放在一个临时变量中，只需生成一条加载指令


**模仿LoadInstruction完成StoreInstruction的翻译即可**

## 内存分配指令（不用做）

已经完成了对于 AllocaInstruction 的翻译，思路：


**将其相对于FP寄存器的偏移存在符号表中**

## 二元指令

仿照加法运算

> 中间代码中二元运算指令的两个操作数都可以是立即数，但在汇编指令中不被允许

解决：提前插入LOAD汇编指令，将其存到寄存器


注意：第二个源操作数是立即数时，数值范围有一定限制（大小限制）

## 控制流指令

### UncondBrInstruction

生成一条无条件跳转指令：


跳转目的操作数只需要调用 `genMachineLabel()` 函数即可，参数为目的基本块号

### CondBrInstruction

前提：在中间代码中该指令一定位于 CmpInstruction 之后


做法：

1. 在AsmBuilder（帮助知道现在在哪的辅助类）中添加成员记录前一条CmpInstruction的条件码，在CondBrInstruction处生成对应的条件跳转指令跳转到`True Branch`
2. 生成无条件跳转指令跳转到`False Branch`

### RetInstruction

1. 若有返回值，需要生成MOV指令，将返回值保存在R0寄存器
2. 生成ADD指令恢复栈帧，(如果该函数有`Callee saved`寄存器，还需要生成POP指令恢复这些寄存器)，生成POP指令恢复FP寄存器
3. 生成跳转指令返回Caller

## 函数定义

1. 生成PUSH指令保存FP寄存器及一些`Callee Saved`寄存器
2. 生成MOV指令使得FP寄存器指向新栈底
3. 生成SUB指令为局部变量分配栈内空间

注意：完成寄存器分配后再确定实际的函数栈空间


解决：

- 一种思路：不在目标代码生成时插入SUB指令，在后续调用output()函数打印目标代码时直接将该条指令打印出来，因为在打印时可以获取到实际栈内空间大小
- 另一种思路：先记录操作数还没有确定的指令，在指令的操作数确定后进行设置

**翻译RetInstruction时可采取相同思路**

## 函数调用指令

1. 含参函数需用 `R0 - R3` 寄存器传递参数(参数个数大于四个需生成PUSH指令传递参数)
2. 生成跳转指令进入Callee函数
3. 若之前通过压栈传递参数（即参数个数大于4），需要恢复SP寄存器
4. 若结果被用，需保存R0寄存器中返回值

# 实现寄存器分配(线性扫描寄存器分配算法)

## 活跃区间分析(不用做)

为每个VREG（虚拟寄存器）计算活跃区间


参考：龙书第二版P391

## 寄存器分配


### 涉及两个集合：

一个活跃区间对应一个虚拟寄存器，集合元素为活跃区间（interval）


intervals:还未分配寄存器的活跃区间集合


按照开始位置递增排序


active:当前正在占用物理寄存器的活跃区间集合


按照结束位置进行递增排序

### 完善`linearScanRegisterAllocation()`

对任意`unhandled interval`进行如下处理:

1. 遍历active，看是否存在结束时间早于unhandled interval的（即与当前unhandled interval的活跃区间不冲突），若有，将其在 active 列表删除
2. 判断active列表中数目和可用的物理寄存器数目是否相等
    1. 若相等：unhandled interval 的结束时间更晚，置位其 spill 标志位，若 active 列表中的 interval 结束时间更晚，需置位 spill 标志位，并将其占用的寄存器分配给 unhandled interval，再按照 unhandled interval活跃区间结束位置，将其插入到 active 列表中
    2. 若不相等：按照活跃区间结束位置，将其插入到 active 列表中


## 生成溢出代码（没溢出就不用做什么）

### 有溢出

要在操作该临时变量时插入对应的LoadMInstruction和StoreMInstruction，就是切分该临时变量的活跃区间，以便重新进行寄存器分配

### 完善`LinearScan::spillAtInterval(Interval *interval)`

1. 分配栈空间，获取当前相对FP偏移
2. 遍历USE指令列表，在USE指令前插入LoadMInstruction，将其从栈内加载到目前的虚拟寄存器
3. 遍历DEF指令列表，在DEF指令后插入StoreMInstruction，将其从目前的虚拟寄存器中存到栈内

***插入结束后，会迭代进行以上过程，重新活跃变量分析，进行寄存器分配，直至没有溢出情况出现***

# 评分标准

## 基本要求

> 实现基本的IR指令到汇编指令的翻译，完善`genMachineCode()`函数

1. 数据访存指令的翻译，主要只需要完成StoreInstruction
2. 二元运算指令的翻译:BinaryInstrction
3. 比较指令的翻译
4. 控制流指令的翻译，UncondBrInstr 语句、CondBrInstr 语句、RetInstru 语句等
5. 函数定义及函数调用的翻译

> 实现汇编指令的打印

1. 实现变量及常量数据的声明函数（即需要在目标代码开头打印出全局变量及常量的声明伪指令，在lab2中对该部分已经有所讲解）
2. 完善基础汇编指令的`Output()`函数
3. 完善寄存器分配算法中`linearScanRegisterAllocation()`函数
4. 完善寄存器分配算法中`spillAtInterval()`函数

## 进阶要求

1. 实现数组的翻译
2. 实现浮点类型的翻译
3. 实现 break、continue 语句的翻译
4. 实现非叶函数的翻译