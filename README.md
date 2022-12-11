
# 概述
**FUnLua**是适用于UE的一个高度优化的**Lua脚本解决方案**。它解决Unlua的一些安全性的问题，相对于UnLua, 它更安全，更高效。

# 在UE中使用Lua
* 直接访问所有的UCLASS, UPROPERTY, UFUNCTION, USTRUCT, UENUM，无须胶水代码。
* 替换蓝图中定义的实现 ( Event / Function )。
* 处理各类事件通知 ( Replication / Animation / Input )。

# 优化特性
* UFUNCTION调用，包括持久化参数缓存、优化的参数传递、优化的非常量引用和返回值处理。
* 访问容器类（TArray, TSet, TMap），内存布局与引擎一致，Lua Table和容器之间不需要转换。
* 高效的结构体创建、访问、GC。
* 支持自定义静态导出类、成员变量、成员函数、全局函数、枚举。

# 快速开始
## 安装
  1. 复制 `Plugins` 目录到你的UE工程根目录。
  2. 重新启动你的UE工程

# 更多示例
  * [01_HelloWorld](Content/Script/Tutorials/FCUIEntrypoint.lua) 快速开始的例子

# 模块说明
* 暂无

# 文档
* 暂无

# 许可证
* FUnLua根据[MIT](LICENSE.TXT)分发。
