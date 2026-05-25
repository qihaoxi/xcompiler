# xcompiler 前端 DSL 分层设计

> 约束：不修改原始 Phoenix 文件；这里只记录当前 `xcompiler` 的落地设计。

## 1. 设计目标

`xcompiler` 的前端不应该把所有复杂性都压进 lexer 或 parser，而应把语言拆成几个清晰层次：

- **基础词法层**：负责 token、位置、源范围、注释、字符串、数字、运算符。
- **语法骨架层**：负责普通声明、表达式、语句、AST 组织。
- **DSL 扩展层**：负责 `microblock`、`syntactic class`、`prefer`、`neta`、pattern matching 等独立结构。
- **对外 API 层**：只暴露一个前端总入口给外界使用。

核心原则是：**lexer 只识别边界，DSL 自己处理语义**。

## 2. 分层职责

### 2.1 基础词法层
职责：
- 标识符 / 关键字
- 数字字面量
- 字符串 / 正则字面量
- 注释
- 复合运算符
- 源码位置与范围

建议模块：
- `src/frontend/lexer/lexer.h`
- `src/frontend/lexer/lexer.c`
- `src/frontend/lexer/char_table.h`
- `src/frontend/lexer/char_table.c`

### 2.2 语法骨架层
职责：
- 语句、表达式、声明的主干解析
- AST 节点创建与组织
- 少回退、少猜测
- 将 token 组织成稳定语法结构

建议模块：
- `src/frontend/ast/ast.h`
- `src/frontend/ast/ast.c`
- `src/frontend/parser/parser.h`
- `src/frontend/parser/parser.c`

### 2.3 DSL 扩展层
职责：
- `microblock`
- `syntactic class`
- `prefer`
- `neta`
- pattern matching
- quasi lexeme 类结构

这些结构应视为**独立 DSL 入口**，不要在 lexer 中展开完毕，而应交给语法层或独立 DSL 处理器。

建议未来扩展目录：

```text
src/frontend/dsl/
src/frontend/micro/
src/frontend/quasi/
```

### 2.4 语义层
职责：
- 语义检查
- 作用域 / 绑定 / 类型信息
- DSL 语义约束
- pattern matching 完整性 / 规则合法性

建议模块：
- `src/frontend/semantic/semantic.h`
- `src/frontend/semantic/semantic.c`

### 2.5 对外 API 层
对外只保留模块级入口：
- `include/xcompiler/api/compiler.h`
- `include/xcompiler/common/log.h`
- `include/xcompiler/frontend/frontend.h`
- `include/xcompiler/ir/ir.h`

## 3. Phoenix 设计对 xcompiler 的启发

Phoenix 的文档表明：

- `microblock`、`syntactic class`、`prefer`、`neta` 不是普通 token 的简单堆叠
- 它们更像**独立 DSL 的承载点**
- 这些结构的真正工作不应该在 lexer 中完成
- lexer 只应负责：
  - 识别入口符号
  - 保持正确的 token 边界
  - 提供行列与范围信息

因此，`xcompiler` 更适合采用：

- **轻 lexer**
- **结构化 parser**
- **DSL 单独扩展**

## 4. 推荐实现顺序

### Phase 1
先实现最小可用 lexer：
- identifier / keyword
- number
- string
- comment
- operator
- EOF / unknown

### Phase 2
实现基础 parser：
- 声明
- 语句
- 表达式
- AST

### Phase 3
逐步加入 DSL：
- `prefer`
- `microblock`
- `syntactic class`
- `pattern matching`
- `quasi lexeme`

### Phase 4
补语义层：
- exhaustive / usefulness / fragility 检查
- DSL 约束校验
- 更丰富的诊断

## 5. 设计底线

不要做的事：
- 不要让 lexer 变成半个 parser
- 不要在 lexer 里写大量回退式判别
- 不要把 DSL 语义硬塞进 token 化阶段
- 不要把所有复杂结构都当作普通表达式处理

## 6. 一句话总结

`xcompiler` 的前端应该是：

> **基础 token 轻量化，语法骨架稳定化，DSL 独立化。**

