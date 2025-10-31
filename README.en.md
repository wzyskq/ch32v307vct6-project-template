English | [简体中文](README.md)

# CH32V307VCT6 Project Template

## Overview

This repository is originally generated using [MRS2 (MounRiver Studio Ⅱ)](https://www.mounriver.com). After significant modifications, it is now compatible with the VSCode [Embedded IDE](https://em-ide.com) plugin.

The author has not found a way to place MRS2 project files within a subdirectory, hence the root directory of this project appears cluttered.

## Usage

### Prerequisites
1. Configure the toolchain and OpenOCD paths in EIDE.
2. Download the repository archive or clone the project via Git:

```git
git clone https://github.com/wzyskq/ch32v307vct6-project-template.git
```

### Using MRS2
Open the `CH32V307VCT6.wvproj` file with MRS2, then compile and flash as usual.

### Using Embedded IDE
Two configuration options are available; **Option 2** is set as default.

**Option 1**

Open `CH32V307VCT6.code-workspace` in VSCode. After EIDE recognizes the project, open the `.eide\eide.yml` file, and change the last line from `uploader: Custom` to `uploader: OpenOCD`. Then, follow the VSCode prompts to reload and update EIDE before compiling and flashing. (Ensure EIDE toolchain and OpenOCD paths are already configured.)

**Option 2**

Open `CH32V307VCT6.code-workspace` in VSCode. Modify the OpenOCD path in the `download.cmd` (For Mac/Linux, you need to manually convert it) script located in the root directory to match your local path. Then compile and flash. (Ensure EIDE toolchain and OpenOCD paths are already configured.)

**Explanation:**

The author encountered an issue with Option 1 where flashing succeeds but verification fails, requiring a 20-second wait (even though the program is already flashed and running before the wait). Therefore, a custom flashing script `download.cmd` was written to skip the verification process. If Option 1 works fine for you, it is still recommended to use that option.

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

**VSCode Files:**
- ch32v307vct6.code-workspace (Workspace configuration file)
- download.cmd (Custom flashing script)
- .clang-format (Code formatting file)
- .gitignore

## Version History
### t0.2
- `Added` Onboard LED example files
- `Added` LICENSE

### t0.1
`Fixed` Custom flashing script without verification to avoid waiting 20 seconds after CRC verification errors

### t0.0
- `Added` Template files compatible with both MRS2 and VSCode EIDE
