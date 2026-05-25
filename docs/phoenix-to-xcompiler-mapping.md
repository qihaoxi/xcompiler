# Phoenix → xcompiler 映射表

> 说明：这里参考的是 `~/candybox/sense/raw-spofer-object-sense/phoenix/` 下的 phoenix 代码。
> 旧的 rainbow 代码不纳入参考。
>
> 目标不是直接搬运实现，而是把 phoenix 的词法/语法设计，映射成当前 `xcompiler` 的 C 版前端骨架。

## 1. 总体策略

### 1.1 设计原则

- **保留 phoenix 的表达能力，但先做最小可运行子集。**
- **不复制 phoenix 实现代码，只参考 token/grammar/AST 结构。**
- **优先把语言前端切成稳定层次：lexer → parser → ast → semantic。**
- **先支持最常见的表达式和语句，再逐步补齐 phoenix 特有能力。**

### 1.2 xcompiler 中的对应模块

| phoenix 概念 | xcompiler 目标模块 | 说明 |
|---|---|---|
| `Lexer.ose` | `src/frontend/lexer/lexer.c` + `include/xcompiler/frontend/lexer.h` | 词法分析器 |
| `Grammar.ose` | `include/xcompiler/frontend/ast.h` + `src/frontend/ast/ast.c` | AST 节点与语法结构 |
| `Parser.ose` | `src/frontend/parser/parser.c` + `include/xcompiler/frontend/parser.h` | 递归下降解析器 |
| `Expression.ose` | `src/frontend/parser/parser.c` 内的表达式子解析函数 | 按优先级拆分 |
| `Statement.ose` | `src/frontend/parser/parser.c` 内的语句子解析函数 | 按语句类型拆分 |
| `Syntactic.ose` / `Functional.ose` | 语法辅助与扩展分支 | 先作为后续扩展 |

---

## 2. 词法层映射

phoenix 的 lexer 里 token 种类很多，适合分成两层：

1. **基础 token**：标识符、字面量、符号、关键字、注释、换行。
2. **phoenix 扩展 token**：pipe、deep copy、member check、dual check、variadic、micro block 等。

### 2.1 基础 token 对照

| phoenix token / 规则 | 语义 | xcompiler 首版建议 |
|---|---|---|
| `ID` | 标识符 | `XC_TOKEN_IDENTIFIER` |
| `DECIMAL` | 十进制整数 | `XC_TOKEN_NUMBER` |
| `HEX` | 十六进制整数 | `XC_TOKEN_NUMBER` + 进制信息 |
| `BINARY` | 二进制整数 | `XC_TOKEN_NUMBER` + 进制信息 |
| `OCTAL` | 八进制整数 | `XC_TOKEN_NUMBER` + 进制信息 |
| `FLOAT` | 浮点数 | `XC_TOKEN_NUMBER` + 浮点标志 |
| `STRING` | 字符串 | `XC_TOKEN_STRING` |
| `REGEX` | 正则字面量 | 后续可做 `XC_TOKEN_REGEX` |
| `NL` | 换行 | 可选：作为语句分隔 token |
| `LINE_COMMENT` | 行注释 | 词法阶段跳过 |
| `BLOCK_COMMENT` | 块注释 | 词法阶段跳过 |
| `SPECIAL_CHAR` | 单字符符号 | 拆成 punctuation token |

### 2.2 phoenix 的扩展 token 对照

| phoenix token | 语义 | xcompiler 处理建议 |
|---|---|---|
| `THIN_ARROW` (`->`) | 箭头/参数到表达式 | 首版可用于函数类型或 lambda |
| `FAT_ARROW` (`=>`) | fat arrow | 可留给后续 |
| `DO_OPERATOR` (`.,`) | 组合式操作 | 后续扩展 |
| `DEEPCOPY` (`..`) | 深拷贝前缀 | 可映射为 unary/postfix 扩展 |
| `MEMBERCHECK` (`.?`) | 成员检查 | 后续扩展 |
| `NOT_MEMBERCHECK` (`!.?`) | 否定成员检查 | 后续扩展 |
| `DUALCHECK` (`??`) | 双向检查 | 后续扩展 |
| `NOT_DUALCHECK` (`!??`) | 否定双向检查 | 后续扩展 |
| `VARIADIC` (`...`) | 可变参数/剩余项 | 首版可以用于参数列表 |
| `COMPARISON` | 比较运算符族 | `== != >= <= > < %= !% =~ !~` 等 |
| `AUG_ASSIGN` | 复合赋值 | `+= -= *= /= %= .= ..=` |
| `BLOCK_ASSIGN` (`__`) | block 赋值 | 后续扩展 |
| `DOUBLE_ASSIGN` (`_=`) | 双重赋值 | 后续扩展 |
| `IMPLICIT_ASSIGN` (`:=`) | 隐式赋值 | 后续扩展 |
| `KINDDEF` (`::`) | kind/type 定义 | 后续扩展 |
| `PIPE` (`|>`) | 管道操作 | 后续扩展 |
| `LABEL` | 数字/标签型语法 | 首版可忽略 |
| `QUOTES` | 引号边界 | 字符串辅助 |

### 2.3 xcompiler 首版 lexer 建议

建议先把 phoenix 的复杂 token 收缩为下面几类：

- `IDENTIFIER`
- `NUMBER`
- `STRING`
- `PUNCTUATION`
- `OPERATOR`
- `KEYWORD`
- `NEWLINE`（可选）
- `EOF`

并增加必要的元信息：

- 数字进制：decimal / hex / binary / octal / float
- 字符串类型：普通字符串 / 插值字符串 / 正则（如果后续支持）
- 位置信息：`line / column / offset`

---

## 3. 语法层映射

phoenix 的语法层拆得非常细，适合直接映射成递归下降的函数组。

### 3.1 表达式层级对照

| phoenix 语法 | 作用 | xcompiler 对应实现 |
|---|---|---|
| `Expression` | 顶层表达式入口 | `parse_expression()` |
| `ArrowExpr` | 箭头表达式 | 首版可延后 |
| `ConditionalExpr` | 三目条件 | `parse_conditional_expr()` |
| `LogicOrExpr` | 逻辑或 | `parse_logical_or_expr()` |
| `LogicAndExpr` | 逻辑与 | `parse_logical_and_expr()` |
| `BitwiseOrExpr` | 位或 | `parse_bitwise_or_expr()` |
| `BitwiseXorExpr` | 位异或 | `parse_bitwise_xor_expr()` |
| `BitwiseAndExpr` | 位与 | `parse_bitwise_and_expr()` |
| `ComparisonExpr` | 比较 | `parse_comparison_expr()` |
| `ShiftExpr` | 位移 | `parse_shift_expr()` |
| `AdditiveExpr` | 加减 | `parse_additive_expr()` |
| `MultiplicativeExpr` | 乘除模 | `parse_multiplicative_expr()` |
| `TypeCastExpr` | 类型转换 | `parse_cast_expr()` |
| `UnaryExpr` | 一元运算 | `parse_unary_expr()` |
| `PostfixExpr` | 后缀运算 | `parse_postfix_expr()` |
| `PrimaryExpr` | 基础表达式 | `parse_primary_expr()` |
| `ListExpr` | 列表字面量 | `parse_list_expr()` |
| `DictExpr` | 字典字面量 | `parse_dict_expr()` |
| `FunctionCallExpr` | 函数调用 | `parse_call_expr()` |
| `MemberExpr` | 成员访问 | `parse_member_expr()` |
| `SubscriptExpr` | 下标 / 切片 | `parse_subscript_expr()` |
| `LambdaExpr` | lambda | 后续支持 |
| `Interpolation` | 插值 | 后续支持 |
| `RegexLiteral` | 正则字面量 | 后续支持 |

### 3.2 语句层级对照

| phoenix 语法 | 作用 | xcompiler 对应实现 |
|---|---|---|
| `Statement` | 语句总入口 | `parse_statement()` |
| `OnelinerStmt` | 单行语句 | 后续可支持 |
| `PipeStmt` | 管道链语句 | 后续可支持 |
| `SimpleStmt` | 简单语句集合 | `parse_simple_stmt()` |
| `BlockStmt` | 语句块 | `parse_block_stmt()` |
| `AssignmentStmt` | 赋值语句 | `parse_assignment_stmt()` |
| `IfStmt` | if/else | `parse_if_stmt()` |
| `WhileStmt` | while | `parse_while_stmt()` |
| `ForStmt` | for | `parse_for_stmt()` |
| `CaseStmt` | case | `parse_case_stmt()` |
| `ReturnStmt` | return | `parse_return_stmt()` |
| `BreakStmt` | break | `parse_break_stmt()` |
| `ContinueStmt` | continue | `parse_continue_stmt()` |
| `ThrowStmt` | throw | 后续支持 |
| `ImportStmt` | import | 后续支持 |
| `CommandStmt` | shell/command 形式 | 首版可忽略 |
| `ReceiveStmt` | `<-` 接收 | 后续扩展 |
| `ExpressionStmt` | 表达式作为语句 | `parse_expression_stmt()` |
| `PredicateAction` | 条件动作链 | 后续扩展 |

### 3.3 phoenix 特有结构，xcompiler 首版建议

| phoenix 结构 | 建议 |
|---|---|
| `Micro*` 系列 | 暂不实现，先预留 AST 扩展点 |
| `PipeStmt` / `|>` | 可作为后续语法扩展 |
| `VertiCallExpr` | 暂不实现 |
| `MemberCheck` / `DualCheck` | 暂不实现 |
| `SetStmt` / `CommandStmt` | 如果目标语言不是 shell 风格，可先去掉 |
| `NetaStmt` / `FlawDef` / `PumpFunction` 等 | 作为 phoenix 特有语法保留，不纳入首版 |

---

## 4. AST 映射建议

当前 `xcompiler` 已有的 AST 层还是很骨架化的，建议把 phoenix 的节点族映射成以下几类：

### 4.1 顶层节点

| phoenix 节点族 | xcompiler 节点建议 |
|---|---|
| `Program` / `Module` | `XC_AST_PROGRAM` |
| `FunctionDef` | `XC_AST_DECLARATION` |
| `ImportStmt` | `XC_AST_DECLARATION` 或单独 `IMPORT` 节点 |
| `BlockStmt` | `XC_AST_STATEMENT` |

### 4.2 表达式节点

| phoenix 节点族 | xcompiler 节点建议 |
|---|---|
| `BinaryExpr` | `XC_AST_EXPRESSION` |
| `UnaryExpr` | `XC_AST_EXPRESSION` |
| `CallExpr` | `XC_AST_EXPRESSION` |
| `MemberExpr` | `XC_AST_EXPRESSION` |
| `IndexExpr` / `SliceExpr` | `XC_AST_EXPRESSION` |
| `ListExpr` / `DictExpr` | `XC_AST_EXPRESSION` |
| `StringLiteral` / `NumberLiteral` | `XC_AST_EXPRESSION` |
| `Identifier` | `XC_AST_EXPRESSION` |

### 4.3 首版 AST 设计建议

建议后续把 AST 扩成这几种常见节点：

- `PROGRAM`
- `FUNCTION_DECL`
- `PARAMETER`
- `BLOCK`
- `RETURN_STMT`
- `IF_STMT`
- `WHILE_STMT`
- `FOR_STMT`
- `ASSIGNMENT_STMT`
- `EXPRESSION_STMT`
- `BINARY_EXPR`
- `UNARY_EXPR`
- `CALL_EXPR`
- `IDENTIFIER`
- `NUMBER_LITERAL`
- `STRING_LITERAL`
- `LIST_LITERAL`
- `DICT_LITERAL`
- `MEMBER_EXPR`
- `INDEX_EXPR`

---

## 5. xcompiler 首版实现优先级

建议按下面顺序做：

### 阶段 A：最小可用词法

- 标识符
- 关键字
- 数字字面量
- 字符串字面量
- 运算符与分隔符
- 注释与空白跳过

### 阶段 B：表达式 parser

- primary
- postfix
- unary
- multiplicative
- additive
- shift
- comparison
- logical and/or
- conditional

### 阶段 C：语句 parser

- block
- expression stmt
- assignment
- return
- if / while / for
- function declaration

### 阶段 D：语义和 IR

- 符号表
- 作用域
- 类型检查
- IR lowering 到 LLVM 之前的中间表示

---

## 6. 和当前 xcompiler 代码的接口对接

现有 `xcompiler` 里已经有这些头文件：

- `include/xcompiler/frontend/lexer.h`
- `include/xcompiler/frontend/parser.h`
- `include/xcompiler/frontend/ast.h`
- `include/xcompiler/frontend/semantic.h`
- `include/xcompiler/frontend/frontend.h`

建议后续把 phoenix 对应的内容逐步填进去：

| 当前接口 | 下一步填充内容 |
|---|---|
| `xc_lexer_*` | 参考 phoenix 的 token 规则实现扫描 |
| `xc_parser_*` | 参考 phoenix 的表达式/语句层级实现递归下降 |
| `xc_ast_*` | 逐步扩充节点类型和子节点数组 |
| `xc_semantic_*` | 语义检查与 symbol table |
| `xc_ir_*` | 生成更接近 LLVM IR 的中间层 |

---

## 7. 建议的落地路径

### 7.1 先做一个 phoenix 子集

建议首版只支持：

- 变量声明 / 赋值
- 数字和字符串字面量
- 二元运算
- 函数调用
- block
- return / if / while

### 7.2 暂缓的 phoenix 特性

- `micro` 系列
- pipe / verticall
- command stmt
- regex / interpolation
- member check / dual check
- 特殊 destructure 语法

---

## 8. 下一步建议

如果你同意这个方向，下一步我建议直接做这两个文件：

1. `docs/phoenix-to-xcompiler-token-map.md`
2. `docs/phoenix-to-xcompiler-ast-map.md`

然后我再开始把 `lexer/parser` 的 C 实现拆进去。

