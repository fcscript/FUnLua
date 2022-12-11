
# 概述
**FUnLua**是适用于UE的一个高度优化的**Lua脚本解决方案**。它解决Unlua的一些安全性的问题，相对于UnLua, 它更安全，更高效。
# 与UnLua的差异
* 安全，无循环引用，lua中对所有UObject的属性缓存，在UObject释放后，继续访问不会引起Crash
* 任意的UObject对象都可以绑定脚本，无需蓝图对象，同一个类型的对象，可以绑定N个不同的脚本，没有同类限制
* 委托，不管是DelegateEvent还是MulticastDelegateEvent，都可以实现多播，接口统一，自动识别，自动防止重复添加，也无需释放。
* GC问题，所有LUA中引用的UE对象，不需要手动释放，这个完全走UE的GC，lua中引用UE对象，不会自动增加GC计数
* 性能更高，反射读写性能是UnLua的5倍左右
* 类型安全，所有的引用侧的对象都带类型，C++侧能很容易检查，性能开销很小，几乎无影响

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
  * [01_HelloWorld](Content/Script) 快速开始的例子
  * 暂无

# 模块说明
* 更新中...

# 文档
* 更新中...

# 许可证
* FUnLua根据[MIT](LICENSE.TXT)分发。
