系统架构设计：独立使用 C 语言从零开发了一个参数化的硬件 Cache 模拟器，支持动态配置组索引（Set Index）、相联度（Associativity）及块大小（Block Size），精确模拟 CPU 的访存命中、不命中及驱逐行为。极客级内存管理：摒弃低效的静态数组，利用多级指针与 malloc/free 实现 Cache-Set-Line 层次化结构的三维动态内存分配，并通过严谨的生命周期管理做到零内存泄漏（经 Valgrind 验证）。
底层数据解构：熟练运用 C 语言位运算（Bitwise Operations），从 64 位指令流中精准切片提取 Tag 与 Set Index，并实现基于全局时间戳的 LRU（最近最少使用）页面替换算法。
硬件友好型算法优化：针对直接映射缓存（Direct-Mapped Cache）中高发的“冲突不命中（Conflict Misses）”问题，在受限的寄存器与局部变量规则下，引入**分块技术（Blocking / Tiling）**重构 $32\times32$ 矩阵转置算法，极致压榨空间局部性，将 Cache Miss 降低至极致水平。
