# Decoder

## 1 思路

前面搭建好了框架，下面就开始实现第一个组件，即指令解码器，输入为32bit的01数值，输出为inst_t结构体的指令。

```c
uint32_t raw_inst

->

typedef struct
{
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    int32_t imm;
    bool rvc;
    enum inst_type_t type;
    bool goon;
    uint16_t csr;
    uint8_t rs3;
} inst_t;
```

## 2 实现

目前的decoder实现了RV64IMFDC，实现过程很枯燥、很头疼，就是翻着手册去写代码，值得说的有几个点：

* RVC是硬件辅助的伪指令，在机器执行之前，会翻译成32位的标准指令；
* 指令中的立即数是符号扩展还是零扩展需要格外注意；
* 位操作的优先级低于加减乘除；
* RVC的低两位是00，01，10；RV的低两位是11，靠此特征来区分指令是否被压缩；
* 指令的解码就是靠指令中的funct字段来分类；
