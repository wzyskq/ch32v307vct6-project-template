[English](README.en.md) | 简体中文

# CH32V307VCT6 工程模板

## 概述

- 本仓库基于 [MRS2 (MounRiver Studio Ⅱ)](https://www.mounriver.com) 生成，经魔改后兼容 VSCode [Embedded IDE](https://em-ide.com) 插件

- 暂未发现 MRS2 项目文件能放置在子目录中，故本项目根目录也许较为混乱

## 使用方法

### 前置条件
1. 配置 VSCode EIDE 的工具链和对应 OpenOCD 路径
2. 下载**仓库压缩包**或者通过**Git**获取本项目
```git
git clone https://github.com/wzyskq/ch32v307vct6-project-template.git
```

### MRS2
用 MRS2 打开 `CH32V307VCT6.wvproj` 文件，正常编译烧录即可

### Embedded IDE
有两种配置方案，默认为**方案二**

**方案一**

VSCode 打开 `CH32V307VCT6.code-workspace` 工作区文件，被 EIDE 识别后打开 `.eide\eide.yml` 文件，修改最后一行的 `uploader: Custom` 为 `uploader: OpenOCD`，按照 VSCode 提示重新更新 EIDE 后编译烧录即可（请先配置 EIDE 的工具链和 OpenOCD 路径）

**方案二**

VSCode 打开 `CH32V307VCT6.code-workspace` 工作区文件，修改根目录下 `download.cmd`（Mac/Linux 需要你手动转换）中 OpenOCD 的路径为自己电脑上的路径，编译烧录即可（请先配置 EIDE 的工具链和 OpenOCD 路径）


**解释：**

我在用方案一烧录时出现了能烧录，但是最后校验失败的问题，要多等待 20s（虽然等待前已经烧录好程序并运行了），所以自己改写了`download.cmd` 烧录脚本，去除了校验过程。如果你用方案一没有问题还是建议用方案一。

## 项目结构

**主项目树**：
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
├─tools     （OpenOCD 烧录接口文件）
└─user      （主函数/中断函数等文件）
```

**MS2 项目文件**：
- CH32V307VCT6.wvproj
- CH32V307VCT6.launch
- .cproject
- .project
- .template

**VSCode 项目文件**：
- ch32v307vct6.code-workspace（工作区配置文件）
- download.cmd               （自定义烧录脚本）
- .clang-format              （代码风格文件）

## 问题反馈
如有任何问题或建议，欢迎通过 GitHub 提交 Issue 或 Pull Request。

## 版本历史
本项目版本请参阅 [VERSION](VERSION.md) 文件。

## 许可证
本项目基于 MIT 许可证进行分发。有关详细信息，请参阅 [LICENSE](LICENSE) 文件。
