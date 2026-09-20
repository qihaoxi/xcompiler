# xcompiler 总体架构：多语言框架与语言 Profile

> 定位说明：xcompiler 不是针对某个具体语言设计的编译器，而是一个支持多语言的编译框架。
> 语言相关的 lexer / parser / AST / 语义规则作为相互关联的整体（语言 Profile）接入框架。
> 本文档记录这一架构模型，作为后续演进的基准；Phoenix 相关文档只视为第一个 Profile 的规格参考。

## 1. 总体定位

框架核心本身**不认识任何语言**。它的交付物是：

- 稳定的阶段间契约（stage boundary）
- 流水线装配与编排（driver）
- 基础设施：诊断、日志、arena、状态码、平台/后端选择
- IR 之后的共享路径（优化、LLVM 工具链对接）

这与经典多语言编译器结构同构：

| 参照系 | 语言相关部分 | 语言无关部分 |
|---|---|---|
| GCC | 各语言前端 | GIMPLE/RTL + 共享后端 |
| LLVM | 各语言前端（Clang 等） | LLVM IR + 优化 + 后端 |
| xcompiler | 语言 Profile | 框架核心（契约 + 编排 + IR 后路径） |

## 2. 两类组成

### 2.1 框架核心（语言无关）

- 阶段边界契约：每个环节的输入输出边界，不关心具体语言
- driver：按装配信息编排流水线
- 基础设施：诊断收集、日志、内存 arena、状态码、平台识别与后端偏好
- IR 之后的共享路径：优化、目标代码生成、LLVM 工具链对接

### 2.2 语言 Profile（语言相关，整体存在）

一个 Profile = **一组相互关联的实现束**：

- token 集（token kind 空间）
- 文法（parser 规则）
- AST 节点族（AST kind 空间）
- 语义规则（符号表、类型检查、DSL 约束）
- DSL 扩展点（`prefer`、`microblock`、`syntactic class`、pattern matching 等）

这些部分是**同一份语言定义在不同阶段上的投影**，彼此强耦合：token kind 空间、AST kind
空间都是该语言私有的，不同 Profile 的 lexer 与 parser 不可混搭。因此 Profile 是框架的一种
**工件（artifact）**：可注册、可描述、可版本化，以整体为单位插拔。

## 3. 双轴替换

“每个环节可替换”发生在两个正交的轴上：

| 轴 | 替换对象 | 粒度 |
|---|---|---|
| 轴一：换 Profile | 整组语言前端（token + 文法 + AST + 语义） | 整体替换，覆盖前端到 IR 边界 |
| 轴二：换框架级环节 | 优化策略、后端目标、平台选择 | 单环节替换，与语言无关 |

替换**不**发生在 Profile 内部的单个 lexer/parser 之间——它们属于 Profile 内部结构，
不构成独立替换单元。

## 4. 关键边界：IR

IR 是**语言特异性消失的地方**：

- Profile 的最终交付物是一份 IR artifact
- 语言相关性终止于 IR 边界，之后全部走框架共享路径
- IR 之前各阶段可以每语言一套，IR 之后必须全语言共享

## 5. Kind 空间的所有权

- token kind 空间、AST kind 空间归 **Profile 所有**
- 框架层只约定能力契约：token 必须携带 range / lexeme，AST 节点必须携带位置、可遍历
- 当前 `include/xcompiler/frontend/frontend.h` 中的 `xc_token_kind_t` / `xc_ast_node_kind_t`
  实际属于**内置默认 Profile**，后续应迁入 Profile 侧，公共头只保留能力契约

## 6. Profile 内部分层

Profile 内部沿用前端分层设计（见 `xcompiler-frontend-dsl-layering.md`）：

- 基础词法层 / 语法骨架层 / DSL 扩展层 / 语义层

这些层是 Profile **内部**的组织方式；对外仍以一个 Profile 为单位暴露。
分层原则不变：lexer 只识别边界，DSL 自己处理语义。

## 7. 与当前代码的对照

### 7.1 已体现的部分

- `xc_stage_descriptor_t` / `xc_module_descriptor_t` 元数据体系：阶段与模块可描述
- 目录按环节职责切分（lexer / parser / semantic / ir / driver），而非按语言切分
- 对外只导出 4 个模块级公共头，内部头不出 `src/`
- `xc_compile` 中 IR 阶段 `NOT_READY` 降级跳过：阶段渐进接入已被考虑

### 7.2 待演进的部分

- `src/api/compiler.c` 中 `xc_compile` 是硬编码串联链 → 应改为阶段接口 + 注册/装配
- descriptor 目前只是元数据 → 应升级为装配信息的来源，驱动流水线组装
- kind 枚举位于公共头 → 应迁入 Profile 侧

## 8. 演进路径

1. 抽出统一的阶段接口（`init` / `run` / `finalize` + 输入输出边界）
2. 将默认前端打包为第一个 Profile（含 kind 空间迁移）
3. descriptor 升级为装配信息，driver 按其组装流水线
4. 定型 IR 边界 artifact，接入 LLVM 后端

## 9. 一句话总结

> **框架管契约与编排，语言以 Profile 整体接入，语言特异性终止于 IR 边界。**
