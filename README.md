[English](README.en.md) | 简体中文

# CH32V307VCT6 工程模板

## 概述

- 本仓库基于 [MRS2 (MounRiver Studio Ⅱ)](https://www.mounriver.com) 生成，经魔改后兼容 VSCode [Embedded IDE](https://em-ide.com) 插件

- 笔者暂未发现 MRS2 项目文件能放置在子目录中，故本项目根目录较乱

## 用法

### 前置准备
1. 配置 EIDE 的工具链和 OpenOCD 路径
2. 下载**仓库压缩包**或者通过**Git**获取本项目
```git
git clone https://github.com/wzyskq/ch32v307vct6-project-template.git
```

### MRS2
用 MRS2 打开 `CH32V307VCT6.wvproj` 文件，正常编译烧录即可

### Embedded IDE
有两种配置方案，默认为**方案二**

**方案一**

VSCode 打开 `CH32V307VCT6.code-workspace` 文件，被 EIDE 识别后打开 `.eide\eide.yml` 文件，修改最后一行的 `uploader: Custom` 为 `uploader: OpenOCD`，按照 VSCode 提示重新更新 EIDE 后编译烧录即可（请先配置 EIDE 的工具链和 OpenOCD 路径）

**方案二**

VSCode 打开 `CH32V307VCT6.code-workspace` 文件，修改根目录下 `download.cmd`（Mac/Linux 需要你手动转换）中 OpenOCD 的路径为自己电脑上的路径，编译烧录即可（请先配置 EIDE 的工具链和 OpenOCD 路径）


**解释：**

笔者在用方案一烧录时出现了能烧录，但是最后校验失败的问题，要多等待 20s（虽然等待前已经烧录好程序并运行了），所以自己改写了`download.cmd` 烧录脚本，去除了校验过程。如果你用方案一没有问题还是建议用方案一。

## 项目树

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
├─tools     （OpenOCD 烧录接口文件）
└─user      （主函数/中断函数等文件）
```

**MS2项目文件**：
- CH32V307VCT6.wvproj
- CH32V307VCT6.launch
- .cproject
- .project
- .template
- LICENSE

**VSCode文件**：
- ch32v307vct6.code-workspace（工作区配置文件）
- download.cmd               （自定义烧录脚本）
- .clang-format              （代码风格文件）
- .gitignore

## 版本

### t0.2
- `新增` 板载 LED 示例文件
- `新增` LICENSE

### t0.1
- `修复` 自定义无校验烧录脚本，避免 CRC 校验报错后等待 20s

### t0.0
- `新增` 模板文件，兼容 MS2 和 VSCode EIDE