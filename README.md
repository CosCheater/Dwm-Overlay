# Dwm-Overlay 支持全系统
## hook方式
- VEH & MiniHook
## 渲染库
- ImGui
## 其他简介
- 这只是一个模板 没有读写 没有作弊功能 只有一个主菜单 可以自已魔改 代码写的很乱 但是该有的注释是有的
- 而且挂钩的点大概也是很常见的几个 PresentDWM | PresentMPO 等等 唯独不同的也就是挂钩的DLL了
## 使用文档
- 定义 `VEHHOOK` 则使用VEHHOOK 不定义则使用MinHook
- #define VEHHOOK
- 定义 `OUTADR` 宏则输出地址
- #define OUTADR
- 可以下载下来二进制文件使用Cheat Engine注入一下试试有无效果
- 如果有问题可以在我主页找到我的联系方式
