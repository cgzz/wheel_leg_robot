# 前端代码重构说明 (V4)

本次重构旨在优化代码结构、提高可读性和可维护性，同时保持原有功能和外观完全不变。

## 文件结构

- **`index.html`**: 页面主结构。
- **`/assets/css/main.css`**: 统一的样式文件，已重新组织并添加注释。
- **`/assets/js/lib/`**: 第三方库（Chart.js, Three.js）。
- **`/assets/js/config.js`**: **核心配置文件**。统一管理应用的状态（State）、所有 DOM 元素的选择器和引用（DOMs）、以及全局常量。
- **`/assets/js/main.js`**: **主程序入口**。负责初始化所有模块和 WebSocket 连接。
- **`/assets/js/ui.js`**: **UI 交互模块**。负责处理所有用户界面的事件绑定和简单的 DOM 更新（如指示灯、日志等）。
- **`/assets/js/services/websocket.js`**: **通信服务**。封装了所有 WebSocket 相关的逻辑。
- **`/assets/js/modules/`**: **功能模块文件夹**。
  - `charts.js`: 图表模块，负责 Chart.js 的初始化和数据更新。
  - `joystick.js`: 遥感模块，负责虚拟摇杆的逻辑。
  - `pid.js`: PID 控制模块，负责 PID 滑块的交互。
  - `robot3D.js`: 3D 姿态模块，负责 Three.js 模型的渲染和更新。
  - `testMode.js`: 测试模式模块，负责测试模式下的 UI 和逻辑。

## 重构核心思想

1. **关注点分离 (SoC)**:
    - **配置分离**: 将状态、DOM 引用、常量统一到 `config.js`。
    - **逻辑分离**: 将不同功能（UI、WebSocket、3D、图表等）拆分到独立的模块文件中。
    - **样式分离**: 所有样式集中在 `main.css`。

2. **模块化**: 每个 JS 文件都作为一个 ES6 模块，通过 `import` 和 `export` 管理依赖关系，避免全局变量污染。

3. **代码清晰化**:
    - 对 CSS 样式按功能区域（布局、卡片、摇杆等）进行分组和注释。
    - 对 JS 函数和变量进行了重新命名，使其更具语义化。
    - 关键逻辑部分增加了必要的注释。
