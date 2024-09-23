# M and S Mode

> 之前提到的指令都能在**用户模式(User Mode)**下执行，除此之外，还有**机器模式(Machine Mode)**和**监管模式(Supervisor Mode)**
>
> 高特权模式不仅能访问低特权模式的功能，而且具备低特权模式不可用的**额外功能**
>
> 额外功能具象化为**特权指令**和**CSR寄存器**

## 0. 特权指令与CSR寄存器

![1727073509626](image/10_M&SMode/1727073509626.png)

指令布局可见[图](image/10_M&SMode/privileged-all.png)

CSR寄存器：[m/s]status、[m/s]scratch、[m/s]ip、[m/s]ie、[m/s]tval、[m/s]tvec、[m/s]cause、[m/s]epc

## 1. 机器模式

机器模式的最重要特性就是**拦截和处理异常**

* 异常的分类
  * 同步异常——指令执行的一种结果
    * 访问故障异常：物理内存地址不支持访问类型，如尝试写入ROM
    * 断点异常：执行ebreak指令，地址或数据与调试触发器匹配
    * 环境调用异常：执行ecall指令
    * 非法指令异常：对无效操作码进行译码
    * 不对齐地址异常：有效地址不能被访问位宽整除
  * 中断——与指令流异步的外部事件
    * 软件：通过写入一个内存映射寄存器触发，用于一个硬件线程通知另一个
    * 时钟：实时计数器(mtime)大于等于硬件线程的时间比较器(mtimecmp)时触发
    * 外部：由连接了外部设备的中断信号的平台级中断控制器触发
* 异常的处理
  * 需要用到8个CSR
    * [mstatus](image/10_M&SMode/mstatus.png)：维护各种状态，如全局中断使能MIE、MPIE
    * [mscratch](image/10_M&SMode/mtval&mepc&mscratch.png)：向异常处理程序提供一个空闲可用的寄存器
    * [mtval](image/10_M&SMode/mtval&mepc&mscratch.png)：存放与当前自陷相关的辅助信息
    * [mtvec](image/10_M&SMode/mtvec.png)：存放发生异常时处理器跳转的地址(Base or Base+(4*cause))
    * [mcause](image/10_M&SMode/mcause.png)：指示发生了何种异常(是不是中断 触发自陷的[事件指示码](image/10_M&SMode/mcause-Interrupt_ExceptionCode.png))
    * [mepc](image/10_M&SMode/mtval&mepc&mscratch.png)：指向发生异常的指令
    * [mip](image/10_M&SMode/mip&mie.png)：记录当前的中断请求
    * [mie](image/10_M&SMode/mip&mie.png)：维护处理器的中断使能状态
  * 处理异常(前4步为原子性)
    * 异常指令PC保存到mepc，PC设为mtvec
    * 异常原因写入mcause，辅助信息写入mtval
    * mstatus.MIE清零以屏蔽中断，MIE旧值保存在MPIE
    * 异常发生前的模式保存在mstatus.MPP，模式更改为M
    * mscratch用来避免覆盖整数寄存器内容
    * 异常处理程序的执行
    * mscratch的恢复
    * mret(前4步的逆过程)

> M-mode的指令：mret&wfi(告知处理器目前无实质性工作，故它进入低功耗模式，直到任意使能的中断到来，即mie&mip!=0)
>
> wfi与mstatus.MIE无关
