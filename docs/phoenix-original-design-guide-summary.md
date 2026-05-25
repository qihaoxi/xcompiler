# Phoenix 原始设计向导总结

> 约束：不修改原始文档；这里只做当前阅读后的摘要。

## 1. 总体结论

Phoenix 的设计不是“一个大而全的单层语言”，而是：

- 语言本体 + 多个独立 DSL
- 通过 `quasi lexeme`、`syntactic class`、`prefer`、`neta`、`annotation` 等机制挂接 DSL
- 每个 DSL 有自己的入口、语义和扩展方式
- lexer 只负责识别边界和基础 token
- 真正复杂的结构由语法层 / DSL 层处理

## 2. 四类关键文档的共同主题

### 2.1 `functional-programming.txt`
Phoenix 同时支持：
- OOP
- FP
- ADT / sum type / product type
- pattern matching
- pipeline programming
- monad / monoid / effect

它强调：
- `g:` / `t:` 是命名与访问体系的一部分
- `::`、`|>`、`<|`、`>>` 等符号是 FP 结构的核心入口
- ADT 和类、函数、模式匹配可以组合使用

### 2.2 `pattern-matching.txt`
Phoenix 的 pattern matching 不是普通 `switch` 替代，而是：
- predicate action
- case closure
- case assignment
- function totality
- pipe branch

并且它强调：
- exhaustive analysis
- usefulness analysis
- fragility analysis

这说明 pattern matching 是静态语义核心，而不是单纯语法糖。

### 2.3 `phoenix-extra.txt`
这里最重要的思想是：
- `g:` / `t:` / `fqn` 是命名与访问体系
- `prefer` 是重要的 DSL 容器
- `syntactic class` 像“独立语法插件”
- `micro` / `prefer` / `neta` 是承载子 DSL 的结构
- 有些结构（如 `if`、`defer`）是被弱化或替换掉的

这说明 Phoenix 不是把复杂度堆在 lexer，而是**分散到多个语义域**。

### 2.4 `quasi-lexeme.txt`
Phoenix 把这些看成 quasi lexeme：
- syntactic class
- lexical annotation
- import item
- language variable

它们看起来像普通代码，但实际上是：
- 编译期控制结构
- DSL 入口
- 代码生成 / 语义扩展钩子

也就是说，它们是**可编程语言的扩展点**，不是普通 token。

## 3. 你提醒我修正的理解

现在我把 `microblock`、`syntatic class`、`prefer` 的定位修正为：

- 它们**不是 lexer 层的复杂规则集合**
- 它们更像独立 DSL / 子 DSL 的承载点
- 由各自语法层或扩展处理器解析和完成工作
- lexer 只需要识别入口和边界符号

## 4. 性能文档给出的教训

### 4.1 `rose-compile-performance-analysis.txt`
主要结论：
- 编译慢不是 I/O 问题
- 第一瓶颈是 parser 的回退式判别
- lexer 的正则匹配成本也高
- 但更大的放大器是：
  - 多路分派
  - 回退
  - 解释器里的 hash/dict/list/string 操作

### 4.2 `rose-compile-optimization-plan.txt`
优化方向非常清楚：
- 正则缓存
- lexer 快速路径
- 绕开正则引擎
- 把简单字符类改成直接判断
- 减少 dict / typval / 字符串拷贝

这对 `xcompiler` 的启发是：
- lexer 要轻
- parser 要少回退
- token 分类要尽量前置明确

## 5. 对 xcompiler 的直接启发

### 保留的部分
- 丰富 token 分类
- 状态机式扫描
- 精确位置与范围
- DSL 入口识别

### 不要照搬的部分
- 大量回退式判别
- 把 lexer 做成半个 parser
- 把 DSL 语义塞进词法层
- 让 token 扫描依赖太多正则回放

## 6. 一句话总结

Phoenix 的真正设计哲学可以概括为：

> **语言本体 + 多个独立 DSL，lexer 只管边界，DSL 自己管语义。**

