# RVA23

> RISC-V相较于其他ISA的特点是可以用户自定义扩展指令集。有的组织会在内部针对特定的硬件做软件适配和开发，这没问题；但也有的组织希望以发放二进制的方式让用户能够普遍使用，这就要求其适配各种硬件。
>
> RVA Profiles的目标就是为了软件兼容性而制定一套配置，即告诉软件开发者针对某一代RISC-V处理器其肯定支持哪些指令集。
>
> RVA Profiles里面除了mandatory extensions之外，还有四种可选扩展类型：
>
> * Localized Options:永远是可选的；
> * Development Options:在后续的RVA Profile中可能称为强制性，应优先开发这些扩展；
> * Expansion Options:实现成本高，并非所有平台都需要；
> * Transitory Options:未来走向不明确，可能被弃用；
>
> RVA Profile通过强制指令集，来保证所有符合该Profile的RISC-V处理器都支持相同的基础功能，从而实现二进制软件在不同厂商处理器之间的兼容性；RVA Profile通过提供可选指令集来向社区和厂商发出明确的信号，指明哪些扩展是有发展潜力的、哪些可能成为未来的强制要求。
>
> 该Profile包含RVA23U64和RVA23S64两部分，详情见[RVA23 Profile](https://github.com/riscv/riscv-profiles/blob/main/src/rva23-profile.adoc)。
>
> 除了RVA之外，还有RVB和RVM，它们聚焦于定制64位应用处理器和微控制器。

[00 &#34;B&#34; Extension for Bit Manipulation](B_Extension/readme.md)
