# Dwm-Overlay 支持全系统
## hook方式
- VEHhook & MiniHook & CALLJMP
## 渲染库
- ImGui
## 简介
- 这只是一个模板 没有读写 没有作弊功能 只有一个主菜单 可以自已魔改 代码写的很乱 但是该有的注释是有的
- 而且挂钩的点大概也是很常见的几个 PresentDWM | PresentMPO 等等 唯独不同的也就是挂钩的DLL了
- 我在研究call point 替换的时候 我发现在 `Microsoft Win11 Pro 24h2` 版本的dxgi.dll里面有个不同于其他系统的挂钩点 所以在使用这种挂钩方式的时候无法做到跟之前系统的挂钩方式一样 我只能自已构造了shell去截取寄存器的值之后jmp到我的渲染函数 这个方法可以自行查看下
- 我在研究VEHhook的时候 发现在该进程(dwm.exe)里面无法出发DR寄存器的断点异常 后来我使用了 `PAGE_GUARD` 的方式 如果有能力的可以自已去研究下 VEHHOOK.cpp 自行解决 DR寄存器的断点无法断下的情况 
- 本项目的代码臃肿 有能力的可以拉去分支自行修改一下 但是关于挂钩点的处理 注释我也都写好了 看自已的理解能力了
## 使用文档
- 你可以在 `includes.hpp` 里面找到以下宏
- 定义 `VEHHOOK` 则使用VEHHOOK 不定义则使用MinHook
- #define VEHHOOK
- 定义 `MINHOOK` 宏使用MiniHook
- #define MINHOOK
- 定义 `CALLJMP` 使用 call point替换
- #define CALLJMP
- 这个CALLJMP 两种模式 `MODEL_ONE`是虚拟机模式 `MODEL_TWO`是主机模式 切记选择CALLJMP模式一定要定义下面的宏
- #define MODEL_ONE
- #define MODEL_TWO
- 定义 `OUTADR` 宏则输出地址
- #define OUTADR
- 可以下载下来二进制文件使用Cheat Engine注入一下试试有无效果
- 如果有问题可以在我主页找到我的联系方式
