/**
 * @file StaticLoad.h
 * @brief 静态模型加载入口（仅在 CSIM_USE_STATIC_MODELS 静态模式下使用）
 */
#ifndef CSIM_STATIC_LOAD_H_
#define CSIM_STATIC_LOAD_H_

namespace csim
{
    class ModelEntry;

    /**
     * @brief 按库名常量（如 "libcsimModelResistor.dll"）在静态注册表中查找模型入口
     * @param filename 库名常量
     * @retval nullptr 未找到
     * @retval 指向静态编译的 ModelEntry 的指针
     */
    ModelEntry *StaticLoad(const char *filename);
}

#endif // CSIM_STATIC_LOAD_H_
