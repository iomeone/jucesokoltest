# jucesokoltest

This project demonstrates a test setup using the JUCE framework with Sokol integration. To compile and run the project, please follow the instructions below.

## Prerequisites

To build this project, download the appropriate JUCE library package for your platform from the [JUCE releases page](https://github.com/juce-framework/JUCE/releases).

### Required Files
Download the following version of JUCE:

- **Linux**: [juce-8.0.3-linux.zip](https://github.com/juce-framework/JUCE/releases/download/8.0.3/juce-8.0.3-linux.zip)
- **macOS**: [juce-8.0.3-osx.zip](https://github.com/juce-framework/JUCE/releases/download/8.0.3/juce-8.0.3-osx.zip)
- **Windows**: [juce-8.0.3-windows.zip](https://github.com/juce-framework/JUCE/releases/download/8.0.3/juce-8.0.3-windows.zip)

## Setup Instructions

1. **Extract the JUCE package**: After downloading, extract the package for your operating system.
  
2. **Rename JUCE Directory**: Rename the extracted directory to `JUCE`.

3. **Project Structure**: Place the `JUCE` directory in the same parent directory as `jucesokoltest`. Your folder structure should look like this:

    ```
    ├── JUCE
    └── jucesokoltest
    ```

4. **Open Project Configuration**:
   - The `Projucer` executable is located inside the `JUCE` directory.
   - In the `jucesokoltest` folder, open `test.jucer` with JUCE's Projucer.
   - Navigate to `exporters` in the lower-left corner of the Projucer window.
   - Select your platform’s exporter (e.g., Xcode for macOS/iOS, VS2022/VS2019 for Windows, Makefile for Linux, or Android).

5. **Configure JUCE Path**:
   - Set the JUCE library path in Projucer to the `JUCE` directory you downloaded and renamed.
  
6. **Open in IDE**:
   - In Projucer, click the IDE icon in the upper right corner to open the project in your selected IDE.
   
7. **Build**:
   - Compile the project within your IDE.

---

# jucesokoltest (中文)

该项目演示了使用 JUCE 框架和 Sokol 的集成测试设置。要编译和运行项目，请按照以下步骤操作。

## 前置条件

要构建此项目，请从 [JUCE 发布页面](https://github.com/juce-framework/JUCE/releases) 下载适合您平台的 JUCE 库包。

### 所需文件
下载以下版本的 JUCE：

- **Linux**: [juce-8.0.3-linux.zip](https://github.com/juce-framework/JUCE/releases/download/8.0.3/juce-8.0.3-linux.zip)
- **macOS**: [juce-8.0.3-osx.zip](https://github.com/juce-framework/JUCE/releases/download/8.0.3/juce-8.0.3-osx.zip)
- **Windows**: [juce-8.0.3-windows.zip](https://github.com/juce-framework/JUCE/releases/download/8.0.3/juce-8.0.3-windows.zip)

## 设置说明

1. **解压 JUCE 包**: 下载完成后，解压适合您操作系统的 JUCE 包。

2. **重命名 JUCE 目录**: 将解压后的目录重命名为 `JUCE`。

3. **项目结构**: 将 `JUCE` 目录放在 `jucesokoltest` 同级目录下。文件夹结构应如下所示：

    ```
    ├── JUCE
    └── jucesokoltest
    ```

4. **打开项目配置**:
   - `Projucer` 执行文件位于 `JUCE` 目录内。
   - 在 `jucesokoltest` 文件夹中，用 Projucer 打开 `test.jucer` 文件。
   - 在 Projucer 窗口左下角导航到 `exporters`。
   - 选择您平台的导出选项（例如，macOS/iOS 的 Xcode，Windows 的 VS2022/VS2019，Linux 的 Makefile，或 Android）。

5. **配置 JUCE 路径**:
   - 在 Projucer 中将 JUCE 库路径设置为下载并重命名的 `JUCE` 目录。

6. **在 IDE 中打开**:
   - 在 Projucer 中点击右上角的 IDE 图标，以在选择的 IDE 中打开项目。

7. **构建**:
   - 在您的 IDE 中编译项目。
