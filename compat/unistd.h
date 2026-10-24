/**
 * @file unistd.h
 * @brief MSVC 兼容 shim：flex 生成的词法分析器需要 <unistd.h>。
 *
 * MSVC 不提供 unistd.h。此文件将 POSIX 常用函数名映射到 CRT 对应函数，
 * 仅在 MSVC 构建时通过 include 路径注入（见 core/CMakeLists.txt）。
 * MinGW / Linux 使用系统自带的 unistd.h，不会包含本文件。
 */
#ifndef CSIM_COMPAT_UNISTD_H_
#define CSIM_COMPAT_UNISTD_H_

#include <io.h>
#include <process.h>
#include <stdio.h>

/* 终端检测：flex 生成的 YY_INTERACTIVE 判断用到 */
#ifndef isatty
#define isatty _isatty
#endif

/* 文件/进程操作 */
#ifndef unlink
#define unlink _unlink
#endif

#ifndef access
#define access _access
#endif

#ifndef getpid
#define getpid _getpid
#endif

#endif /* CSIM_COMPAT_UNISTD_H_ */
