#ifndef CSIM_MODEL_ADAPTER_H_
#define CSIM_MODEL_ADAPTER_H_

#include "ModelLoader.h"
#include "StaticModelLoader.h"

namespace csim
{
    // 统一的模型加载适配器
    class ModelAdapter
    {
    public:
        // 统一的加载接口，自动选择静态或动态加载
        static ModelEntry* load(const char* modelName);
        
        // 显式指定加载方式
        static ModelEntry* loadStatic(const char* modelName);
        static ModelEntry* loadDynamic(const char* filename);

    private:
        // 将StaticModelEntry转换为ModelEntry
        static ModelEntry* wrapStaticEntry(StaticModelEntry* staticEntry);
    };
}

#endif // CSIM_MODEL_ADAPTER_H_