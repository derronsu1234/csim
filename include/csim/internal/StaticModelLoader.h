// SPDX-License-Identifier: LGPL-2.1-or-later
// StaticModelLoader.h - 专门的静态模型加载器
#ifndef CSIM_STATIC_MODELLOADER_H_
#define CSIM_STATIC_MODELLOADER_H_

#include <map>
#include <string>
#include "csim/model/ModelBase.h"

namespace csimModel
{
    class ModelBase;
}

namespace csim
{        

    class Circuit;

    // 静态模型描述符
    struct StaticModelDescriptor
    {
        const char* name;
        csimModel::pfnCreateModel_t pfnCreate;
        csimModel::pfnDeleteModel_t pfnDelete;
        const ModelDescriptor* descriptor;
        const PropertyMdlDescriptor* mdlDescriptors;
        size_t numMdlDescriptors;
    };

    // Resistor模型
    csimModel::ModelBase *createModel_Resistor(MODELBASE_CONSTRUCTOR_DEF);
    void deleteModel_Resistor(csimModel::ModelBase *model){}
    ModelDescriptor descriptor_Resistor;
    PropertyMdlDescriptor *mdlDescriptors_Resistor;
    size_t numMdlDescriptors_Resistor;

    // VDC模型
    csimModel::ModelBase *createModel_VDC(MODELBASE_CONSTRUCTOR_DEF);
    void deleteModel_VDC(csimModel::ModelBase *model){}
    ModelDescriptor descriptor_VDC;
    PropertyMdlDescriptor *mdlDescriptors_VDC;
    size_t numMdlDescriptors_VDC;

    // Capacitor模型
    csimModel::ModelBase *createModel_Capacitor(MODELBASE_CONSTRUCTOR_DEF);
    void deleteModel_Capacitor(csimModel::ModelBase *model){}
     ModelDescriptor descriptor_Capacitor;
     PropertyMdlDescriptor *mdlDescriptors_Capacitor;
     size_t numMdlDescriptors_Capacitor;

    std::map<std::string, StaticModelDescriptor> m_s_staticModels;
    bool m_s_initialized;
    // 静态模型条目类
    class StaticModelEntry
    {
    public:
        StaticModelEntry(csimModel::pfnCreateModel_t pfnCreate, 
                        csimModel::pfnDeleteModel_t pfnDelete,
                        const ModelDescriptor *descriptor,
                        const PropertyMdlDescriptor *mdlDescriptors,
                        size_t numMdlDescriptors);
        ~StaticModelEntry();

        csimModel::ModelBase *createInstance(MODELBASE_CONSTRUCTOR_DEF) const;
        void deleteInstance(csimModel::ModelBase *model) const;

        inline const ModelDescriptor *descriptor() const
        {
            return m_descriptor;
        }

        inline size_t numMdlDescriptors() const
        {
            return m_numMdlDescriptors;
        }

        inline const PropertyMdlDescriptor *mdlDescriptors(size_t i) const
        {
            return m_mdlDescriptors + i;
        }

        class MdlEntry
        {
        public:
            csimModel::PropertyMdl *createInstance() const;
            void deleteInstance(csimModel::PropertyMdl *mdl) const;
            const PropertyMdlPropDescriptor *getProperty(const char *name) const;

        public:
            const PropertyMdlDescriptor *desc;
            std::map<std::string, const PropertyMdlPropDescriptor *> props;
        };

        int getMdlEntry(const char *mdl, const MdlEntry **out) const;

    private:
        int buildMdlIndexs();

    private:
        csimModel::pfnCreateModel_t m_pfnCreate;
        csimModel::pfnDeleteModel_t m_pfnDelete;
        const ModelDescriptor *m_descriptor;
        const PropertyMdlDescriptor *m_mdlDescriptors;
        size_t m_numMdlDescriptors;
        std::map<std::string, MdlEntry> m_mdls;
    };

    // 静态模型加载器
    class StaticModelLoader
    {
    public:

        // 注册静态模型
        static void registerModel(const char* name, const StaticModelDescriptor& desc)    {
            std::string nameStr = name;
            m_s_staticModels[nameStr] = desc;    
        }
        static void initializeModels()   {
            if (m_s_initialized) return;
            
            // 注册Resistor模型
            StaticModelDescriptor resistorDesc = {
                "resistorLibrary",
                createModel_Resistor,
                deleteModel_Resistor,
                &descriptor_Resistor,
                mdlDescriptors_Resistor,
                numMdlDescriptors_Resistor
            };
            registerModel("resistorLibrary", resistorDesc);

            // 注册VDC模型
            StaticModelDescriptor vdcDesc = {
                "VDCLibrary",
                createModel_VDC,
                deleteModel_VDC,
                &descriptor_VDC,
                mdlDescriptors_VDC,
                numMdlDescriptors_VDC
            };
            registerModel("VDCLibrary", vdcDesc);

            // 注册Capacitor模型
            StaticModelDescriptor capDesc = {
                "CapacitorLibrary",
                createModel_Capacitor,
                deleteModel_Capacitor,
                &descriptor_Capacitor,
                mdlDescriptors_Capacitor,
                numMdlDescriptors_Capacitor
            };
            registerModel("CapacitorLibrary", capDesc);
            

            m_s_initialized = true;

        };



        // 加载静态模型
        static StaticModelEntry* load(const char* modelName)    {
            std::cout<<"load model: "<<modelName<<std::endl;
            if (!m_s_initialized) {
                initializeModels();
            }

            std::string nameStr = modelName;
            auto it = m_s_staticModels.find(nameStr);
            if (it == m_s_staticModels.end()) {
                return nullptr;
            }

            const StaticModelDescriptor& desc = it->second;
            return new StaticModelEntry(desc.pfnCreate, desc.pfnDelete, desc.descriptor,
                                    desc.mdlDescriptors, desc.numMdlDescriptors);
        }

        // 检查模型是否存在
        static bool hasModel(const char* modelName){
            if (!m_s_initialized) {
                initializeModels();
            }

            std::string nameStr = modelName;
            return m_s_staticModels.find(nameStr) != m_s_staticModels.end();           
        }

               

        // 获取所有已注册的模型名称
        static std::vector<std::string> getRegisteredModels(){
            if (!m_s_initialized) {
                initializeModels();
            }

            std::vector<std::string> models;
            for (const auto& pair : m_s_staticModels) {
                models.push_back(pair.first);
            }
            return models;

        }

    };

}

#endif // CSIM_STATIC_MODELLOADER_H_