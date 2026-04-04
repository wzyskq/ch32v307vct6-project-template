[English](README.en.md) | 简体中文

# CH32V307VCT6 工程模板

## 概述

本仓库基于 [MRS2 (MounRiver Studio Ⅱ)](https://www.mounriver.com) **V220** 生成，经魔改后兼容 VSCode [Embedded IDE](https://em-ide.com) 插件

## 使用方法

### 前置条件
1. 配置 VSCode EIDE 的工具链和对应 OpenOCD 路径。参考教程：[VSCODE-EIDE编译调试CH32V307 - 知乎](https://zhuanlan.zhihu.com/p/1922436930869626702)
2. 下载**仓库压缩包**或者通过**Git**获取本项目
```git
git clone https://github.com/wzyskq/ch32v307vct6-project-template.git
```

### MRS2
用 MRS2 打开 `.mrs/CH32V307VCT6.wvproj` 文件，正常编译烧录即可

### Embedded IDE
有两种配置方案，默认为**方案一**，若出现烧录后需等待20s验证，可尝试方案二。

**方案一**

VSCode 打开 `.vscode/CH32V307VCT6.code-workspace` 工作区文件，编译烧录即可

**方案二**

VSCode 打开 `.vscode/CH32V307VCT6.code-workspace` 工作区文件，修改**同目录**下的烧录文件 `download.cmd`（Mac/Linux 需要你手动转换）中 OpenOCD 的路径为自己电脑上的路径。打开 `.vscode/.eide/eide.yml` 文件，修改最后一行的 `uploader: OpenOCD` 为 `uploader: Custom`，按照 VSCode 提示重新更新 EIDE，然后编译烧录即可（请先配置 EIDE 的工具链和 OpenOCD 路径）

**方案解释**

我有一段时间在用方案一烧录时出现了能烧录，但是最后校验失败的问题，要多等待 20s（虽然等待前已经烧录好程序并开始运行），所以自己改写了`download.cmd` 烧录脚本，去除了校验过程。如果你用方案一没有问题还是建议用方案一。

## 项目结构

**主项目树**
```cmd
.
├─.mrs
│  ├─.mrs
│  └─obj      （MRS2 编译程序文件夹）
├─.vscode
│  ├─.eide
│  ├─.vscode
│  ├─obj      （EIDE 编译程序文件夹）
│  └─tools    （OpenOCD 烧录接口文件）
├─app
│  ├─core
│  └─utils
├─drivers
│  └─gpio
├─sdk
└─vendor
```

**补充说明**
- 根目录下的 `.mrs` 和 `.vscode` 是由笔者创建的
- 其下的子目录 `.mrs` 和 `.vscode` 是由 MRS2 和 VSCode 生成的配置文件

## 问题反馈
如有任何问题或建议，欢迎通过 GitHub 提交 Issue 或 Pull Request。

## 版本历史
本项目版本请参阅 [VERSION](VERSION.md) 文件。

## 许可证
本项目基于 MIT 许可证进行分发。有关详细信息，请参阅 [LICENSE](LICENSE) 文件。
