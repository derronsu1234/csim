/**
 * @file ModelExports.h
 * @brief 组件模型导出宏：支持动态加载(DLL)与静态链接两种模式
 *
 * 每个组件模型 .cc 文件末尾统一使用 CSIM_MODEL_EXPORTS 宏声明导出符号：
 *   - 动态模式（默认，未定义 CSIM_USE_STATIC_MODELS）：
 *     导出标准符号 createModel / deleteModel / descriptor / mdlDescriptors / numMdlDescriptors，
 *     供 ModelLoader 通过 GetProcAddress / dlsym 从 DLL 中查找。
 *   - 静态模式（定义 CSIM_USE_STATIC_MODELS）：
 *     导出带模型名前缀的符号（如 createModel_Resistor / descriptor_Resistor），
 *     避免多个模型静态链接到同一可执行文件时符号冲突，由静态注册表统一登记。
 */
#ifndef CSIM_MODEL_EXPORTS_H_
#define CSIM_MODEL_EXPORTS_H_

#include "csim/model/ModelBase.h"

/* CSIM_MODEL_API 已在 ModelBase.h 中定义（MSVC 动态模式为 __declspec(dllexport)）。 */

#if defined(CSIM_USE_STATIC_MODELS)

#define CSIM_MODEL_EXPORTS(CLS, ID, DESC)                                        \
    extern "C" csimModel::ModelBase *createModel_##CLS(MODELBASE_CONSTRUCTOR_DEF) \
    {                                                                            \
        return new csimModel::CLS(MODELBASE_CONSTRUCTOR_VAR);                    \
    }                                                                            \
    extern "C" void deleteModel_##CLS(csimModel::ModelBase *model)               \
    {                                                                            \
        delete model;                                                            \
    }                                                                            \
    extern const PropertyMdlDescriptor *mdlDescriptors_##CLS = nullptr;          \
    extern const size_t numMdlDescriptors_##CLS = 0;                             \
    extern const ModelDescriptor descriptor_##CLS = { ID, DESC };

#else /* CSIM_USE_STATIC_MODELS */

#define CSIM_MODEL_EXPORTS(CLS, ID, DESC)                                        \
    extern "C" CSIM_MODEL_API csimModel::ModelBase *createModel(MODELBASE_CONSTRUCTOR_DEF) \
    {                                                                            \
        return new csimModel::CLS(MODELBASE_CONSTRUCTOR_VAR);                    \
    }                                                                            \
    extern "C" CSIM_MODEL_API void deleteModel(csimModel::ModelBase *model)      \
    {                                                                            \
        delete model;                                                            \
    }                                                                            \
    CSIM_MODEL_API const PropertyMdlDescriptor *mdlDescriptors = nullptr;         \
    CSIM_MODEL_API const size_t numMdlDescriptors = 0;                           \
    CSIM_MODEL_API const ModelDescriptor descriptor = { ID, DESC };

#endif /* CSIM_USE_STATIC_MODELS */

#endif /* CSIM_MODEL_EXPORTS_H_ */
