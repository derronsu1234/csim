# 电路仿真

电路仿真通过解析网表文件构建电路，支持 **OP（直流工作点）、DC 扫描、AC 扫描、瞬态（TRAN）** 四种分析，并将仿真结果导出为 CSV 文件。

## 功能特性

- ✅ 网表解析（支持 `.subckt` 子电路展开、`.model` 二极管模型）
- ✅ 元件支持：
  - 无源：电阻、电容、电感
  - 独立源：直流电压源、直流电流源
  - 受控源：VCCS（压控电流源）、VCVS（压控电压源）、CCCS（流控电流源）、CCVS（流控电压源）
  - 非线性：二极管（`.model`）
- ✅ 分析类型：
  - `.op` — 直流工作点
  - `.dc` — 直流扫描（扫描电压源）
  - `.ac` — 交流频率扫描
  - `.tran` — 瞬态分析
- ✅ 探针：`v(节点名)` 电压探针、`i(元件名)` 电流探针
- ✅ 结果导出 CSV（AC 默认输出幅值，DC/TRAN 输出实部）

## 目录结构

```
csim/
├── CMakeLists.txt          # 顶层构建脚本
├── main.cc                 # 主程序入口
├── include/                # 公共头文件（parse.h / modelmanager.h 等）
├── core/                   # 核心库（Circuit / Dataset / Netlist / 解析器 / 求解器 / 分析器）
│   ├── analyzer/           # 分析器实现（OP / DC / AC / TRAN / Sweep）
│   ├── models/             # 元件模型（R / C / L / VDC / 二极管 / 受控源等）
│   ├── solver/             # 数值求解（线性求解、积分校正/预测等）
│   └── parser/             # Bison/Flex 语法解析
├── optimizer/              # Gmin 优化
├── testcases/              # GoogleTest 单元测试
├── test/                   # 示例网表文件（*.sp）
├── third_party/msys/       # 内置的 bison/flex/m4/sed 及运行时（自包含构建，见下）
└── bin/                    # 构建产物（circuit.exe 及动态库）
```

## 环境要求

| 依赖 | 说明 |
|------|------|
| CMake | ≥ 3.9 |
| C++ 编译器 | MinGW-w64（默认，`MinGW Makefiles`）、MSVC（VS2022，`Visual Studio 17 2022`）或 Linux g++（`Unix Makefiles`） |
| Bison / Flex | Windows 已内置（`third_party/msys`）；Linux 需系统安装（`apt install bison flex`） |

> 平台支持 Windows（`CSIM_IN_WIN32`）与 Unix（`CSIM_IN_UNIX`）。

### 内置解析工具（third_party/msys）

工程已将构建所需的外部工具打包到 `third_party/msys/usr/bin/`（bison、flex、m4、sed 及 msys 运行时 DLL），目录结构镜像 MSYS2 的 `/usr` 布局，因此**构建时无需安装 MSYS2**，也不要求 bison/flex 在 PATH 中：

```
third_party/msys/usr/bin/       bison.exe flex.exe m4.exe sed.exe msys-2.0.dll ...
third_party/msys/usr/share/bison/   bison 语法骨架文件
```

- CMake（`CMakeLists.txt` 与 `core/utils/CMakeLists.txt`）在 `find_package(BISON/FLEX)` 前自动检测并优先使用内置工具（`BISON_EXECUTABLE` / `FLEX_EXECUTABLE` / `SED_EXECUTABLE`）。
- 四个一键脚本优先把 `third_party/msys/usr/bin` 加入 PATH，再回退到 MSYS2（若存在）。
- bison 运行时会调用 `/usr/bin/m4`，因 msys 运行时按 `msys-2.0.dll` 位置解析根目录，内置目录的 `usr/` 布局保证了 m4 与骨架文件能被找到。
- 该目录内容与 MSYS2 运行时版本相关，如需升级工具，替换其中的 exe/DLL 并保持 `usr/` 布局即可。

> 编译器选择：MinGW 或 MSVC 均可完整编译并运行。MSVC 动态模式下 DLL 输出自动带 `lib` 前缀（与 `constants.h` 的加载名一致）；MSVC 编译会自动启用 `/utf-8` 与 `/std:c++17`。MSVC 下 flex 生成的词法文件依赖 `<unistd.h>`，由 `compat/unistd.h` shim 提供（仅 MSVC include 路径注入）。

## 构建

组件模型支持**两种加载方式**，通过 CMake 选项 `CSIM_USE_STATIC_MODELS` 切换：

| 模式 | CMake 选项 | 产物 | 说明 |
|------|-----------|------|------|
| 动态（默认） | `OFF` | 模型 DLL + `libcsim.dll` | 每个模型一个 DLL，运行时由 `ModelLoader` 加载 |
| 静态 | `ON` | 单个 `circuit.exe` | 所有模型直接编译进可执行文件，无模型 DLL |

### 方式一：一键脚本

| 脚本 | 模式 | 构建目录 |
|------|------|---------|
| `build.bat` | MinGW 动态（默认） | `build/` |
| `build_static.bat` | MinGW 静态 | `build_static/` |
| `build_msvc.bat` | MSVC 动态 | `build_msvc/` |
| `build_msvc_static.bat` | MSVC 静态 | `build_msvc_static/` |

双击脚本，或运行 VS Code 的 `build` / `build: static` / `build: msvc` / `build: msvc: static` 任务即可完成 CMake 配置与编译。

脚本会自动把 `D:\msys64\mingw64\bin` 与 `D:\msys64\usr\bin`（bison/flex）加入 PATH。工具链不在默认路径时，请修改脚本顶部的 `MINGW_BIN` / `MSYS_BIN` 变量。

### 方式二：手动命令

```bash
# 动态模式（默认）
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -S . -B build
mingw32-make.exe -C build -j8

# 静态模式
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCSIM_USE_STATIC_MODELS=ON -S . -B build_static
mingw32-make.exe -C build_static -j8
```

### MSVC（Visual Studio 2022）构建

使用 CMake 的 `Visual Studio 17 2022` 生成器，产物输出到 `bin\<配置>\`（如 `bin\Debug\`）。需先将 bison/flex 目录加入 PATH：

```powershell
$env:Path = "D:\msys64\usr\bin;" + $env:Path

# 动态模式（默认）
cmake -G "Visual Studio 17 2022" -A x64 -S . -B build_msvc
cmake --build build_msvc --config Debug -- /m

# 静态模式
cmake -G "Visual Studio 17 2022" -A x64 -DCSIM_USE_STATIC_MODELS=ON -S . -B build_msvc_static
cmake --build build_msvc_static --config Debug -- /m
```

运行 MSVC 版本：

```powershell
bin\Debug\circuit.exe test\case1_op.sp
```

> MSVC 已处理的兼容点：`/utf-8`（源码 UTF-8 中文注释）、`/std:c++17`（结构化绑定）、`compat/unistd.h` shim（flex 生成代码）、`WINDOWS_EXPORT_ALL_SYMBOLS`（csim.dll 导出）、`lib` 前缀 DLL 命名（与 `constants.h` 一致）、`-Wl,--out-implib` 与 `gcov` 等 GCC 专属项已按 `MINGW` 条件隔离。

### Linux 构建

工程代码已按平台隔离（`_WIN32` / `CSIM_IN_WIN32` / `CSIM_IN_UNIX`），Linux 上可直接用 g++ + CMake 编译。**内置的 `third_party/msys` 是 Windows 工具，Linux 上不适用**，需要安装系统 bison/flex：

```bash
# Debian/Ubuntu 示例（其他发行版用对应包管理器）
sudo apt install g++ cmake bison flex

# 动态模式（默认）
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -S . -B build
cmake --build build -j8
./bin/circuit test/case1_op.sp

# 静态模式
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCSIM_USE_STATIC_MODELS=ON -S . -B build_static
cmake --build build_static -j8
```

> Linux 兼容点（已内置处理）：`dlopen`/`dlsym` 动态加载模型 `.so`；`if(UNIX)` 下 csim 自动链接 `dl`（兼容 glibc < 2.34）；构建 RPATH 已指向 `lib/`，动态模式**无需手动设置 `LD_LIBRARY_PATH`**。项目按 GCC 设计（链接 `gcov`、`-Wall`），建议用 g++。

> 注意：编译时需要 `D:\msys64\mingw64\bin` 在 PATH 中（g++ 的子进程需找到汇编器/链接器）。

构建完成后可执行文件位于 `bin/circuit.exe`，所需 DLL 会自动复制到 `bin/` 目录。

> ⚠️ **注意**：`build/` 与 `build_static/` 共享 `bin/` 输出目录。切换模式后请重新构建对应的脚本，否则 `bin/` 中的模型 DLL 可能缺失/不一致（静态构建不生成模型 DLL）。

### VS Code 任务

`.vscode/tasks.json` 提供以下任务（`Ctrl+Shift+B` 触发默认 `build`）：

| 任务 | 说明 |
|------|------|
| `build` | 一键构建（MinGW 动态，默认任务） |
| `build: static` | 一键构建（MinGW 静态） |
| `build: msvc` | 一键构建（MSVC 动态） |
| `build: msvc: static` | 一键构建（MSVC 静态） |
| `clean` / `clean: msvc` / `clean: msvc: static` | 删除对应构建目录 |
| `run: case1_op` / `run: case6_tran_pulse` | 构建后直接运行对应网表（MinGW） |
| `run: msvc case1_op` / `run: msvc: static case1_op` | 构建后直接运行（MSVC） |

`.vscode/launch.json` 已配置 MinGW GDB 调试，`F5` 调试前会自动执行 `build` 任务。

## 使用方式

```bash
# 命令行传入网表文件路径
./bin/circuit.exe test/case1_op.sp
```

程序按网表中的分析指令依次执行，将结果写入 CSV 文件。**CSV 输出到网表所在目录，文件名使用网表基名 + 分析类型后缀**：

| 分析 | 输出文件 | 数值格式 |
|------|----------|----------|
| `.op` | `<网表基名>_op.csv` | 实部 |
| `.dc` | `<网表基名>_dc.csv` | 实部（含扫描变量列） |
| `.ac` | `<网表基名>_ac.csv` | 幅值（含频率列） |
| `.tran` | `<网表基名>_tran.csv` | 实部（含时间列） |

例如运行 `test/case6_tran_pulse.sp` 会在 `test/` 下生成 `case6_tran_pulse_tran.csv`；运行路径为相对路径则输出相对，绝对路径则输出绝对。

CSV 列格式：第一行为列名（`[扫描变量], 探针1, 探针2, ...`，OP 分析无扫描变量列），其后为数据行（`[扫描变量值], 各探针值`）；数值以全精度（17 位有效数字）输出

## 网表语法

### 元件

| 语法 | 说明 | 示例 |
|------|------|------|
| `Rxxx n1 n2 阻值` | 电阻 | `R1 n1 n2 2` |
| `Cxxx n1 n2 容值` | 电容 | `C1 n3 0 1e-11` |
| `Lxxx n1 n2 感值` | 电感 | `L1 n2 n3 1e-10` |
| `Vxxx n+ n- DC 值` | 直流电压源（支持 `AC` / `pulse` / `sin` / `pwl` / `lfsr`） | `V1 n1 0 DC 123 AC 5` |
| `Ixxx n+ n- DC 值` | 直流电流源 | `I1 n1 0 DC 1` |
| `Gxxx n+ n- nc+ nc- 跨导` | VCCS 压控电流源 | `G1 nout 0 nctrl 0 0.5` |
| `Exxx n+ n- nc+ nc- 增益` | VCVS 压控电压源 | `E1 nout 0 nctrl 0 2` |
| `Fxxx n+ n- 控制源 增益` | CCCS 流控电流源 | `F1 nout 0 I1 2` |
| `Hxxx n+ n- 控制源 增益` | CCVS 流控电压源 | `H1 nout 0 I1 2` |
| `Dxxx n+ n- 模型名` | 二极管（需 `.model`） | `D1 out1 out2 MyDiode` |

### 分析指令

```
.op                        # 工作点分析
.dc V1 -10 10 1            # DC 扫描：.dc 源名 起始 终止 步长
.ac lin 101 1e9 10e9       # AC 扫描：.ac 类型 点数 起始频率 终止频率
.tran 1e-8 1e-5            # 瞬态：.tran 时间步长 终止时间
```

### 探针

```
.probe v(n1) v(n2) i(v1)   # v(节点名) 电压探针，i(元件名) 电流探针
```

### 模型（二极管）

```
.model MyDiode D (
+ IS=1e-14
+ CJ0=1e-12
+ VJ=0.7
+ M=0.5
+ TT=12e-9
+ RS=0.5
+ )
```

### 其他

- `**` 或 `*` 开头为注释
- `.end` 结束网表


## 示例网表

`test/` 目录下提供多个示例：

| 文件 | 内容 |
|------|------|
| `case1_op.sp` | 电阻分压网络，DC 扫描 |
| `case2_tran.sp` | RC 电路瞬态分析 |
| `case3_ac.sp` | RLC 电路 AC 扫描 |
| `case6_tran_pulse.sp` | 脉冲源瞬态分析 |
| `diode.sp` | 二极管整流（含 `.model`） |
| `divider_test.sp` | 电阻分压 OP 验证 |
| `ctrl_sources.sp` | 电流源 + VCCS 受控源 |
| `vccs_test.sp` | VCCS 压控电流源 |
| `vcvs_test.sp` | VCVS 压控电压源 |
| `cccs_isc.sp` | CCCS 流控电流源（电流源作控制） |

## 已知限制


- 电流探针需引用元件名（如 `i(v1)`）
- **CCCS/CCVS 的控制源**：库的 CCCS/CCVS 模型通过并联 0V 安培计测量控制电流。用**电流源**作控制时工作正常（见 `cccs_isc.sp`）；用标准 SPICE 语法（`F1 ... Vsense`，控制源为电压源）时，安培计与理想电压源并联会导致矩阵奇异，请改用电流源控制或调整拓扑
- **CCCS 输出极性**：库模型与 VCCS 的输出端子约定相反，若输出极性不符预期，可交换 `Fxxx` 的 n+ / n- 节点
- **IDC 极性**：为符合 SPICE 习惯，`Ixxx n+ n-` 中 n+ 映射到库模型的"正"端（terminal 1）；若需反向电流请交换 n+ / n-

## 测试

`testcases/` 目录包含基于 GoogleTest 的单元测试（默认只启用 `tstOPLinearCircuit.cc`，其余在 `testcases/CMakeLists.txt` 中注释，可按需取消注释）。

通过 CMake 选项 `CSIM_BUILD_TESTS`（默认 `OFF`）开启，并同样支持动态/静态两种模型模式：

```bash
# 动态模式 + 测试
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCSIM_BUILD_TESTS=ON -S . -B build
mingw32-make.exe -C build -j8
bin\tstall.exe

# 静态模式 + 测试
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCSIM_USE_STATIC_MODELS=ON -DCSIM_BUILD_TESTS=ON -S . -B build_static
mingw32-make.exe -C build_static -j8
bin\tstall.exe
```

> 注意：重配置（切换选项）后会清理 `bin/` 中共享的模型 DLL，切换模式后请**完整重新构建**对应目录。
> 部分测试用例可能使用过时的元件 API（如直接配置 `V` 属性），启用前请按当前 API（VDC 用 `value` 字符串）修正。

## License

（本项目未声明许可证，请与作者确认使用条款。）
