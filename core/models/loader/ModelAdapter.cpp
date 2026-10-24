#include "ModelAdapter.h"

namespace csim
{
    ModelEntry* ModelAdapter::load(const char* modelName)
    {
        // 首先尝试静态加载
        ModelEntry* entry = loadStatic(modelName);
        if (entry != nullptr) {
            return entry;
        }
        
        // 如果静态加载失败，尝试动态加载
        return loadDynamic(modelName);
    }

    ModelEntry* ModelAdapter::loadStatic(const char* modelName)
    {
        StaticModelEntry* staticEntry = StaticModelLoader::load(modelName);
        if (staticEntry == nullptr) {
            return nullptr;
        }
        return wrapStaticEntry(staticEntry);
    }

    ModelEntry* ModelAdapter::loadDynamic(const char* filename)
    {
        return ModelLoader::load(filename);
    }

    ModelEntry* ModelAdapter::wrapStaticEntry(StaticModelEntry* staticEntry)
    {
        // 创建一个ModelEntry包装器，内部持有StaticModelEntry
        // 这里需要扩展ModelEntry类以支持包装StaticModelEntry
        // 为简化，假设我们直接创建一个新的ModelEntry
        return new ModelEntry(
            staticEntry->createInstance,
            staticEntry->deleteInstance,
            staticEntry->descriptor(),
            staticEntry->mdlDescriptors(0),
            staticEntry->numMdlDescriptors()
        );
    }
}