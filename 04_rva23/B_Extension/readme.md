# "B" Extension for Bit Manipulation, Version 1.0.0

B标准扩展包含Zba，Zbb和Zbs三个子扩展。

## 1 Zb*

B扩展的目的是使代码大小减少、性能提升和能耗降低。每个子扩展包括一组据有相似目的的位操作指令，有些指令只在一个子扩展中可用，有些则在多个子扩展中可用。

子扩展有：

* Address generation instructions
* Basic bit-manipulation
* Carry-less multiplication
* Single-bit instructions

## 2 Zba : Address generation

Zba用于加速地址生成。

其包含的指令如下：

![1755227090997](image/readme/1755227090997.png)

## 3 Zbb : Basic bit-manipulation

### 3-1 Logical with negate

![1755227268612](image/readme/1755227268612.png)

### 3-2 Count leading/trailing zero bits

![1755227282826](image/readme/1755227282826.png)

### 3-3 Count population

这些指令统计的是为1的bit位数

![1755227297473](image/readme/1755227297473.png)

### 3-4 Integer minimum/maximum

返回两个数中较小/较大的那个数

![1755227354525](image/readme/1755227354525.png)

### 3-5 Sign extension and zero extension

实现符号扩展和零扩展，替代了：

* 对于8-bit/16-bit的符号扩展：slli rd,rs,(XLEN-`<size>`) + srai
* 对于16-bit的零扩展：slli + srli

![1755227408702](image/readme/1755227408702.png)

## 4 Bitwise rotation

一个很形象的形容：“circular shifts”

![1755227585718](image/readme/1755227585718.png)

## 5 OR Combine

可用于优化strlen和strcpy等字符串处理函数。

![1755227691598](image/readme/1755227691598.png)

## 6 Byte-reverse

![1755227735795](image/readme/1755227735795.png)

## 7 Zbc : Carry-less multiplication

![1755227878976](image/readme/1755227878976.png)

## 8 Zbs : Single-bit instructions

![1755227910951](image/readme/1755227910951.png)

## 9 Zbkb : Bit-manipulation for Cryptography

![1755227930095](image/readme/1755227930095.png)

## 10 Zbkc : Carry-less multiplication for Cryptography

![1755227996969](image/readme/1755227996969.png)

## 11 Zbkx : Crossbar permutations

![1755228025846](image/readme/1755228025846.png)
