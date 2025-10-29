# 概述

- 本仓库基于 MS2 (MounRiver Studio Ⅱ) 生成，经魔改后兼容 VSCode EIDE 插件

- 笔者暂未发现 MS2 项目文件能放置在子目录中，故本项目根目录较乱

# 目录

**主项目结构**：
```cmd
.
├─.eide
├─.mrs
├─.vscode
├─device    （自定义存放代码的地方，文件夹名为笔者个人习惯）
├─obj       （MS2 的编译文件）
├─obj.eide  （EIDE 的编译文件）
├─sdk       （软件开发工具包）
│  ├─Core
│  ├─Debug
│  ├─Ld
│  ├─Peripheral
│  │  ├─inc
│  │  └─src
│  └─Startup
├─tools     （EIDE 配置）
└─user      （主函数/中断函数等文件）
```

**MS2项目文件**：
- CH32V307VCT6.wvproj
- CH32V307VCT6.launch
- .cproject
- .project
- .template

**VSCode文件**：
- ch32v307vct6.code-workspace（工作区配置文件）
- .clang-format              （代码风格文件）
- .gitignore
