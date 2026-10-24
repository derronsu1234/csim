# 第三方组件与许可声明

本仓库包含第三方作品。各部分的著作权与许可条件归其各自权利人所有；本项目的自有代码
以根目录 [LICENSE](LICENSE)（GNU LGPL 2.1 或更高版本）授权。本文件用于满足各许可证的
署名与声明要求。

---

## 1. FastCSIM

本项目是 **FastCSIM** 的衍生作品。`core/`、`include/csim/` 下的多数源文件保留了原始文件头：

```
FastCSIM Copyright (C) 2021 cassuto
This project is free edition; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License(LGPL) as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
```

这些文件继续以 **LGPL-2.1-or-later** 授权。本项目对其所做的修改未删除、未替换任何原始
版权或许可声明（仅修正了文件头中因模板替换产生的 `{}` → `;` 排版字符）。再分发时请一并
保留这些声明。

## 2. BSIM3v3.2（加州大学伯克利分校）

`core/models/bsim3v32/` 下的 BSIM3v3.2 模型代码源自 University of California, Berkeley
的 BSIM Research Group（Copyright 2001 Regents of the University of California）。
完整条款见 [`core/models/bsim3v32/B3TERMS_OF_USE`](core/models/bsim3v32/B3TERMS_OF_USE)，
其中与再分发相关的要点：

- 不得就加州大学代码本身收费（可为新增、扩展或支持服务收费）；
- 任何基于该软件的产品，须在产品文档中致谢 UC Berkeley BSIM Research Group；
- 分发时须保留软件上的版权声明，并遵守美国政府关于再分发/出口的限制。

## 3. Boost.Regex

`include/boost/` 为 Boost.Regex 的头文件子集（Copyright (c) 1998-2020 John Maddock 等），
遵循 **Boost Software License 1.0**：可自由使用、修改、再分发，源码分发需保留版权声明，
二进制分发需在文档或材料中复现该声明。

## 4. GoogleTest

`testcases/googletest/` 为 GoogleTest（Copyright 2008, Google Inc.），遵循
**BSD 3-Clause**，全文见 [`testcases/googletest/LICENSE`](testcases/googletest/LICENSE)。

## 5. 内置构建工具（third_party/msys）

`third_party/msys/usr/bin/` 打包了 MSYS2 构建的下列 GPL 程序，仅作为**构建期工具**使用
（`CMakeLists.txt` 在 `find_package(BISON/FLEX)` 前优先使用它们，因此 Windows 上无需安装
MSYS2）：

| 程序 | 版本 | 许可 |
|------|------|------|
| GNU Bison | 3.8.2 | GPL-3.0-or-later |
| GNU Flex | 2.6.4 | GPL-2.0-or-later（生成代码有专门例外） |
| GNU M4 | 1.4.21 | GPL-3.0-or-later |
| GNU sed | 4.9 | GPL-3.0-or-later |
| MSYS2 运行时（`msys-2.0.dll`、`msys-iconv-2.dll`、`msys-intl-8.dll`） | MSYS2 | GPL-2.0-or-later |
| bison 运行时骨架与 m4sugar（`usr/share/bison/`） | 随 Bison 3.8.2 | GPL-3.0-or-later |

**关于 GPL 源代码的书面要约**：上述程序以二进制形式随本仓库分发。作为分发者，本项目声明：
任何人均可向维护者索取（或从下列位置取得）对应版本的完整源代码，费用不超过分发成本：

- MSYS2 打包与构建脚本：<https://github.com/msys2/MSYS2-packages>
- GNU Bison：<https://ftp.gnu.org/gnu/bison/>
- GNU Flex：<https://github.com/westes/flex/releases>
- GNU M4：<https://ftp.gnu.org/gnu/m4/>
- GNU sed：<https://ftp.gnu.org/gnu/sed/>

**对本项目授权无影响**：这些程序只在构建期被调用。Bison/Flex 生成的解析器代码适用其
「生成代码例外」条款（Bison 的 `yacc.c` 等骨架明确声明生成物不受 GPL 约束），不会被链接
进 `circuit` 可执行文件，也不会使本项目整体转为 GPL。

## 6. 运行时 DLL（lib/）

`lib/libgcc_s_seh-1.dll`、`lib/libstdc++-6.dll`、`lib/libwinpthread-1.dll` 来自 GCC/MinGW-w64
运行库（GPL-3.0-with-GCC-exception 与 libstdc++ 的运行时例外），构建时由 CMake 复制到 `bin/`，
随可执行文件分发时适用其**运行时例外**条款，不要求公开你的源代码。
