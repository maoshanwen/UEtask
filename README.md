# project1（UE 自定义 Shader 项目）

这个项目不是“做一个完整游戏”，而是一个偏底层的 Unreal Engine 图形实验场：

- 我在 UE 里自己挂载 `.usf`，走 `GlobalShader` 路径，不只依赖材质编辑器。
- 用 **Compute Shader** 做一条最小但完整的 GPU 计算链路：输入数组 -> GPU 运算 -> Readback 回 CPU。
- 用 **Graphics Shader** 跑一条最小渲染链路：自定义顶点结构 -> VS/PS -> 输出到 `RenderTarget`。
- 再把这些能力通过 `UBlueprintFunctionLibrary` 暴露给蓝图，保证“能被美术/关卡侧调用”，而不只是 C++ demo。

---

## 1. 项目在做什么

一句话：**在 UE 里把 Shader 当成“可编程模块”来做，而不是只当材质节点来拼。**

我想验证三件事：

1. UE 的渲染线程模型下，Compute 和 Graphics 两类 Shader 能否都被我稳定调起来。
2. GPU 资源（Buffer / VertexDeclaration / RenderTarget）在生命周期和线程切换下是否可控。
3. 从工程实践角度，能不能把这些能力封装成蓝图函数，形成可复用入口。

对应代码结构：

- `Source/ExampleComputeShader/`：Compute Shader 模块，负责 dispatch 和 readback。
- `Source/ExampleGraphicsShader/`：Graphics Shader 模块，负责 VS/PS 渲染流程。
- `Source/ShaderFunctionLibrary/`：蓝图函数库，给蓝图层提供调用入口。
- `Shaders/`：`.usf` 文件（Compute 与 Graphics）。

---

## 2. 已实现能力

### Compute Shader（`ExampleComputeShader`）

- 挂载 Shader 目录并注册 `FunctionMultiply` 入口。
- 使用 `RWStructuredBuffer<float>` 做输入/输出。
- 通过参数 `Scale`、`Translate` 执行：
  - `Output[i] = Input[i] * Scale + Translate`
- 在游戏线程发起，在渲染线程执行，最后读回 CPU。

### Graphics Shader（`ExampleGraphicsShader`）

- 自定义 `VertexAttributes`（位置/颜色/法线/时间/变换矩阵）。
- 自己创建 VertexBuffer + VertexDeclaration。
- 定义全局 VS/PS，使用 RenderPass + PipelineState 绘制到 RenderTarget。
- 在 VS 里尝试了时间驱动的顶点动画逻辑，在 PS 里做了基础光照强度混合。

### Blueprint 入口（`ShaderFunctionLibrary`）

- `Execute ExampleComputeShader`：蓝图可直接调用 GPU 计算。
- `Render ExampleGraphicsShader with Color`：蓝图可触发渲染到目标纹理。

---

## 3. 过程中遇到的典型问题（真实踩坑记录）

这个项目最难的不是写 HLSL，而是 **UE 渲染流程中的“时机”和“边界”**。

### 坑 1：Shader 路径映射和入口函数名不一致

在 UE 里，`IMPLEMENT_SHADER_TYPE` 的路径和入口函数名只要有一点点不匹配，就会直接编译/运行失败。

我的经验：

- 先确保 `AddShaderSourceDirectoryMapping` 的虚拟路径和 `.usf` 实际路径一一对应。
- 再核对 `IMPLEMENT_SHADER_TYPE` 的入口函数名（区分大小写）。
- 出问题先看最小链路，不要一上来怀疑渲染状态机。

### 坑 2：GameThread 和 RenderThread 的职责混淆

刚开始很容易在错误线程里碰 GPU 资源，结果就是“不崩但不对”或者“偶发崩溃”。

最后稳定下来的原则：

- 游戏线程只做参数组织和任务投递。
- 真正的资源更新、dispatch、draw 都放进 `ENQUEUE_RENDER_COMMAND`。

### 坑 3：Readback 时机不对，拿到旧数据或空数据

Compute 跑完以后如果没有同步点，CPU 很可能读到未完成数据。

这里我用了 `FlushRenderingCommands()` 去保证顺序，虽然不是最高性能方案，但作为学习项目它非常直观，能先把“正确性”建立起来。

### 坑 4：顶点语义与 CPU 结构体布局对不上

`FVertexElement` 的 offset / stride 和 HLSL 的 `ATTRIBUTE` 语义必须严格对齐。

这个问题最容易让画面“有东西但不对劲”，我当时就是靠逐项对照结构体字段+日志去定位。

### 坑 5：矩阵与坐标空间处理容易自相矛盾

我在 VS 里做过两套位置计算（一套带视图投影，一套直接覆盖），这其实暴露了当时对“最终输出位置来自哪一套空间”的思路还在迭代。

这不是坏事，反而说明这个项目是“边做边验证”的实验场，而不是只追求一次写对。

---

## 4. 这个项目背后的思考

我做这个项目时有一个很明确的想法：

**先把图形系统拆到足够小，再逐步把控制权拿回来。**

在 UE 里，很多功能可以被高层系统“自动做好”，但我更想知道：

- 数据到底什么时候进 GPU？
- Shader 参数在哪一层绑定最稳？
- 渲染通路里哪些是必须项，哪些是可替换项？

所以这个仓库看起来会有一些“实验痕迹”（比如多个 map、一些命名不统一、不同阶段的尝试代码），这是刻意留下的学习轨迹。对我来说，它不只是结果，更是过程记录。

---

## 5. 当前状态与下一步

当前状态：

- Compute 链路已可用，支持输入数组运算和读回。
- Graphics 链路已跑通，能向 RenderTarget 输出并进行基础动态效果尝试。
- 蓝图层已经可以调用两类 Shader 功能。

下一步计划：

1. 清理 Graphics Shader 中重复/冲突的位置计算逻辑，统一空间变换约定。
2. 把 Compute Readback 从“强同步”逐步改成更工程化的异步读回方案。
3. 补充一个最小 UI 或 Actor 示例，让项目开箱就能看到调用路径。
4. 统一命名和模块注释，降低后续维护门槛。

---

## 6. 如何打开项目

- 使用 Unreal Engine 打开 `project1.uproject`。
- 主要实验资源在 `Content/` 里的多个测试关卡（`Example`、`work*`、`homework2` 等）。
- 若修改了 `Shaders/*.usf`，建议重新编译并观察模块启动日志确认映射生效。

---

## 7. 说明

这是一个学习导向、实验导向的项目。它的价值不是“功能多完整”，而是“把 UE 的 Shader 工作流从黑盒变成可理解、可控制、可复用的工程能力”。
