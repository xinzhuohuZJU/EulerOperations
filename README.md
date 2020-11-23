# Euler Operation

**ZJU 三维CAD建模 2020-2021 Fall**



## 项目说明

- 定义半边数据结构
- 基于半边数据结构，实现5个欧拉操作(Euler Operations)
- 基于欧拉操作，实现扫掠(Sweep)操作，并将基于扫掠操作构建地实体模型进行图形显示



## 实验环境和库

- **操作系统:** Windows
- **实验平台:** Microsoft VS 2015 pro
- **实验环境:** Debug, x86
- **第三方库:** 
  - freeglut 3.2.1: http://freeglut.sourceforge.net/
  - jsoncpp: https://github.com/open-source-parsers/jsoncpp



## 项目结构

```
.
├── Brep.cpp               // 半边数据结构实现
├── Brep.h                 // 半边数据结构定义
├── EulerOperations.cpp    // 欧拉操作和扫掠操作实现
├── EulerOperations.h      // 欧拉操作和扫掠操作定义
├── SolidProcess.cpp       // 具体建模实现
├── SolidProcess.h         // 具体建模定义
└── main.cpp               // 主函数s,包含opengl绘制框架 

```



## 使用方法

本程序使用`json`格式的文件作为模型文件输入程序。具体格式可参考代码目录下的txt文件。

使用VS生成`EulerOperations.exe`程序后，运行该程序，输入

```
0
```

或者

```
1
```

输出示例模型。输入

```
xxx.txt/json
```

输出对应的模型。



同时本程序还实现了一些交互操作

- 用鼠标左键拖动物体旋转

- 用上下左右键移动物体

- 使用"+","-"键来对物体进行缩放



## 运行效果
- 实体带有一个通孔
![Solid1](https://raw.githubusercontent.com/xinzhuohuZJU/EulerOperations/master/Fig/Solid1.png)
- 实体带有多个通孔
![Solid2](https://raw.githubusercontent.com/xinzhuohuZJU/EulerOperations/master/Fig/Solid2.png)
![Solid3](https://raw.githubusercontent.com/xinzhuohuZJU/EulerOperations/master/Fig/Solid3.png)



## 参考资料

- opengl下实现绘制凹多边形，以及带有凹面的体的功能：
  - **OpenGL Tessellation**: http://www.songho.ca/opengl/gl_tessellation.html

