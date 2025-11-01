English | [简体中文](README.md)

# CH32V307VCT6 Project Template

## Overview

- This repo is originally generated using [MRS2 (MounRiver Studio Ⅱ)](https://www.mounriver.com). After simple modifications, it is now compatible with the VSCode [Embedded IDE](https://em-ide.com) plugin.

- I have not found a way to place MRS2 project files in subdirectories, so the root directory of this project may be somewhat cluttered.

## Usage

### Prerequisites
1. Configure the toolchain and OpenOCD paths in EIDE.
2. Download the repo zip or clone the project via Git:

```git
git clone https://github.com/wzyskq/ch32v307vct6-project-template.git
```

### Using MRS2
Open the `CH32V307VCT6.wvproj` file with MRS2, then compile and flash as usual.

### Using Embedded IDE
Two configuration options are available; **Option 2** is set as default.

**Option 1**

Open VSCode with the `CH32V307VCT6.code-workspace` workspace file. After EIDE recognizes the project, open the `.eide\eide.yml` file, and change the last line from `uploader: Custom` to `uploader: OpenOCD`. Then, follow the VSCode prompts to reload and update EIDE before compiling and flashing. (Ensure EIDE toolchain and OpenOCD paths are already configured.)

**Option 2**

Open VSCode with the `CH32V307VCT6.code-workspace` workspace file. Modify the OpenOCD path in the `download.cmd` (For Mac/Linux, you need to manually convert it) script located in the root directory to match your local path. Then compile and flash. (Ensure EIDE toolchain and OpenOCD paths are already configured.)

**Explanation:**

I encountered an issue with Option 1 where flashing succeeds but verification fails, requiring a 20-second wait (even though the program is already flashed and running before the wait). Therefore, a custom flashing script `download.cmd` was written to skip the verification process. If Option 1 works fine for you, it is still recommended to use that option.

## Project Structure

**Main Project Tree:**
```cmd
.
├─.eide
├─.mrs
├─.vscode
├─device (Custom code directory; name reflects personal preference)
├─obj (Compilation files for MRS2)
├─obj.eide (Compilation files for EIDE)
├─sdk (Software Development Kit)
│ ├─Core
│ ├─Debug
│ ├─Ld
│ ├─Peripheral
│ │ ├─inc
│ │ └─src
│ └─Startup
├─tools (OpenOCD flashing interface files)
└─user (Main functions, interrupt handlers, etc.)
```

**MRS2 Project Files:**
- CH32V307VCT6.wvproj
- CH32V307VCT6.launch
- .cproject
- .project
- .template
- LICENSE

**VSCode Project Files:**
- ch32v307vct6.code-workspace (Workspace configuration file)
- download.cmd (Custom flashing script)
- .clang-format (Code formatting file)
- .gitignore

## Feedback
If you have any questions or suggestions, feel free to submit an Issue or Pull Request via GitHub.

## Version
This project version can be found in the [VERSION](VERSION.md) file.

## License
This project is distributed under the MIT license. For detailed information, please refer to the [LICENSE](LICENSE) file.
