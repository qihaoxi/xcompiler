# xcompiler 总体架构：多语言框架、语言 Profile 与可替换后端

> 定位说明：xcompiler 不是针对某个具体语言设计的编译器，而是一个支持多语言的编译框架。
> 语言相关的 lexer / parser / AST / 语义规则作为相互关联的整体（语言 Profile）接入框架；
> 后端同样是可替换的框架级环节，LLVM 只是其中的首选后端，不是 IR 契约本身。
> 本文档记录这一架构模型，作为后续演进的基准；Phoenix 相关文档只视为第一个 Profile 的规格参考。

## 1. 总体定位

框架核心本身**不认识任何语言，也不绑定任何执行目标**。它的交付物是：

- 稳定的阶段间契约（stage boundary）
- 流水线装配与编排（driver）
- 基础设施：诊断、日志、arena、状态码、平台/后端选择
- 框架 IR 与 IR 之后的共享路径（优化、可插拔的后端）

这与经典多语言编译器结构同构：

| 参照系 | 语言相关部分 | 语言无关部分 |
|---|---|---|
| GCC | 各语言前端 | GIMPLE/RTL + 共享后端 |
| LLVM | 各语言前端（Clang 等） | LLVM IR + 优化 + 后端 |
| xcompiler | 语言 Profile | 框架核心（契约 + 编排 + 框架 IR + 可替换后端） |

## 2. 两类组成

### 2.1 框架核心（语言无关）

- 阶段边界契约：每个环节的输入输出边界，不关心具体语言
- driver：按装配信息编排流水线
- 基础设施：诊断收集、日志、内存 arena、状态码、平台识别与后端偏好
- 框架 IR 与后端层：IR artifact 的定义权 + 一组可插拔的后端（见第 4、5 节）

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
| 轴二：换框架级环节 | 优化策略、后端（执行目标）、平台选择 | 单环节替换，与语言无关 |

替换**不**发生在 Profile 内部的单个 lexer/parser 之间——它们属于 Profile 内部结构，
不构成独立替换单元。

## 4. 关键边界：框架 IR

IR 是**语言特异性消失的地方**：

- Profile 的最终交付物是一份框架 IR artifact
- 语言相关性终止于 IR 边界，之后全部走框架共享路径

**IR 归框架所有，不归 LLVM 所有**：

- IR artifact 的契约由框架定义；LLVM IR 只是“LLVM 后端”的目标格式，不是边界本身
- 这保证更换后端（解释器换 LLVM、LLVM 换 C 转译……）不需要动任何 Profile
- README 阶段 3 遗留的“设计项目内部 IR，或直接生成 LLVM IR”二选一在此定案：**设计框架内部 IR**

### 4.1 IR 形状决策：SSA 虚拟寄存器

框架 IR 采用寄存器式（无限虚拟寄存器 + 显式基本块与控制流），并逐步 SSA 化。决策理由：

- **边界稳定性**：目标生态（LLVM 工具链）是寄存器/SSA 形状的；栈式 artifact 接 LLVM
  时需要整体等价变换，等于推翻边界契约重写，违反框架最核心的稳定性承诺
- **解释执行不需要寄存器分配**：分配只对物理寄存器机器是硬需求；解释无限虚拟寄存器
  只需一张 `value_id → 值` 的表，比维护操作数栈更直白
- **与首选后端同构**：向 LLVM IR 的发射退化为近乎同构的翻译/打印
- **栈式的真实优势（编码紧凑）在此无意义**：框架 IR 是内存结构 + 可读文本，
  不需要二进制序列化的紧凑性

## 5. 后端层：可替换的执行目标

后端是轴二替换的典型实例：**消费框架 IR，产出执行结果或目标产物**。

```text
Profile 前端（整体可替换）
        │ 交付
        ▼
框架 IR artifact（SSA 虚拟寄存器，框架所有）
        │ 消费
        ▼
后端层（可替换，由配置选择）
  ├── interpreter：直译执行，零外部依赖，日常快速验证
  ├── llvm：发射 .ll，接 lli / llc / clang，集成验证与真目标
  └── 未来：C 转译、WASM、原生代码生成、栈式字节码序列化……
```

- 后端选择是**配置**（`xc_compiler_config_t` / platform 后端偏好），不是代码改动；
  `platform` 层的 `xc_preferred_backend()` 已为这一选择预留了位置
- 首批实现两个后端：解释器 + LLVM 发射器。两者同接口并存，本身就是“框架级环节可替换”
  的最小证明——比换 Profile 便宜得多的替换演示
- 不直接以现成 VM（JVM / CIL / WASM / Lua）作为验证目标：完整 VM 规范拖着类型系统、
  对象模型、GC、调用约定等与验证框架无关的包袱；唯一值得复用的现成件是 `lli`
  （LLVM 自带的 IR 解释器），作为 LLVM 后端的集成校验点

### 5.1 栈式 vs 寄存式问题的消解

在“后端可替换”的模型下，**栈式还是寄存式不再是框架级决策，而是每个后端内部的编码决策**：

- 框架 IR 统一保持寄存器形状；任何栈式目标（WASM 栈机编码、紧凑字节码 VM 等）
  在它自己的后端内完成“寄存器 → 栈”的降级——LLVM 面向 WASM 时正是这么做的
- 因此框架层面只需要回答一个问题：IR artifact 是什么形状（答：SSA 虚拟寄存器，见 4.1）

## 6. Kind 空间的所有权

- token kind 空间、AST kind 空间归 **Profile 所有**
- 框架层只约定能力契约：token 必须携带 range / lexeme，AST 节点必须携带位置、可遍历
- 当前 `include/xcompiler/frontend/frontend.h` 中的 `xc_token_kind_t` / `xc_ast_node_kind_t`
  实际属于**内置默认 Profile**，后续应迁入 Profile 侧，公共头只保留能力契约

## 7. Profile 内部分层

Profile 内部沿用前端分层设计（见 `xcompiler-frontend-dsl-layering.md`）：

- 基础词法层 / 语法骨架层 / DSL 扩展层 / 语义层

这些层是 Profile **内部**的组织方式；对外仍以一个 Profile 为单位暴露。
分层原则不变：lexer 只识别边界，DSL 自己处理语义。

## 8. 与当前代码的对照

### 8.1 已体现的部分

- `xc_stage_descriptor_t` / `xc_module_descriptor_t` 元数据体系：阶段与模块可描述
- 目录按环节职责切分（lexer / parser / semantic / ir / driver），而非按语言切分
- 对外只导出 4 个模块级公共头，内部头不出 `src/`
- `xc_compile` 中 IR 阶段 `NOT_READY` 降级跳过：阶段渐进接入已被考虑
- `platform` 层的 `xc_preferred_backend()` 与 `xc_compiler_config_t.target_triple`：
  后端选择的配置入口已有伏笔

### 8.2 待演进的部分

- `src/api/compiler.c` 中 `xc_compile` 是硬编码串联链 → 应改为阶段接口 + 注册/装配
- descriptor 目前只是元数据 → 应升级为装配信息的来源，驱动流水线组装
- kind 枚举位于公共头 → 应迁入 Profile 侧
- 后端层尚不存在 → 应抽出后端接口，并实现 interpreter 与 llvm 两个后端

## 9. 演进路径

1. 抽出统一的阶段接口（`init` / `run` / `finalize` + 输入输出边界）
2. 将默认前端打包为第一个 Profile（含 kind 空间迁移）
3. 定型框架 IR artifact（SSA 虚拟寄存器；可先做非 SSA 的虚拟寄存器，再 SSA 化）
4. 抽出后端接口；实现两个后端：迷你解释器（零依赖快速验证）+ LLVM `.ll` 发射器（集成验证）
5. descriptor 升级为装配信息，driver 按其组装流水线

## 10. 一句话总结

> **框架管契约与编排，语言以 Profile 整体接入，后端按目标插拔；语言特异性终止于框架 IR 边界。**
