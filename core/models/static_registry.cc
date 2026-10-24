// SPDX-License-Identifier: LGPL-2.1-or-later
/**
 * @file static_registry.cc
 * @brief 静态模型注册表：把库名常量映射到静态编译的模型入口。
 *        仅在定义 CSIM_USE_STATIC_MODELS 时编译（静态链接模式）。
 */
#if defined(CSIM_USE_STATIC_MODELS)

#include "csim/model/ModelBase.h"
#include "csim/internal/ModelLoader.h"
#include "csim/internal/StaticLoad.h"
/* 注意：必须用尖括号 <constants.h>，避免在本目录(core/models/)中被大小写不敏感地
   解析成 Constants.h（模型常量头文件） */
#include <constants.h>
#include <string>

namespace csimModel
{
    class ModelBase;
}

/* 声明各模型在各自 .cc 中通过 CSIM_MODEL_EXPORTS 宏生成的静态符号 */
#define CSIM_MODEL_EXPORT_DECL(CLS)                                                \
    extern "C" csimModel::ModelBase *createModel_##CLS(MODELBASE_CONSTRUCTOR_DEF); \
    extern "C" void deleteModel_##CLS(csimModel::ModelBase *model);                \
    extern const ModelDescriptor descriptor_##CLS;                                 \
    extern const PropertyMdlDescriptor *mdlDescriptors_##CLS;                      \
    extern const size_t numMdlDescriptors_##CLS;

CSIM_MODEL_EXPORT_DECL(Resistor)
CSIM_MODEL_EXPORT_DECL(VDC)
CSIM_MODEL_EXPORT_DECL(VAC)
CSIM_MODEL_EXPORT_DECL(IDC)
CSIM_MODEL_EXPORT_DECL(IAC)
CSIM_MODEL_EXPORT_DECL(VCVS)
CSIM_MODEL_EXPORT_DECL(CCVS)
CSIM_MODEL_EXPORT_DECL(VCCS)
CSIM_MODEL_EXPORT_DECL(CCCS)
CSIM_MODEL_EXPORT_DECL(PN)
CSIM_MODEL_EXPORT_DECL(Capacitor)
CSIM_MODEL_EXPORT_DECL(Inductor)
CSIM_MODEL_EXPORT_DECL(BSIM3V32)

namespace csim
{
    ModelEntry *StaticLoad(const char *filename)
    {
        struct RegEntry
        {
            const char *key;
            csimModel::pfnCreateModel_t create;
            csimModel::pfnDeleteModel_t del;
            const ModelDescriptor *desc;
            const PropertyMdlDescriptor *mdlDesc;
            size_t numMdl;
        };
        static const RegEntry table[] = {
            {resistorLibrary, createModel_Resistor, deleteModel_Resistor, &descriptor_Resistor, mdlDescriptors_Resistor, numMdlDescriptors_Resistor},
            {VDCLibrary, createModel_VDC, deleteModel_VDC, &descriptor_VDC, mdlDescriptors_VDC, numMdlDescriptors_VDC},
            {VACLibrary, createModel_VAC, deleteModel_VAC, &descriptor_VAC, mdlDescriptors_VAC, numMdlDescriptors_VAC},
            {IDCLibrary, createModel_IDC, deleteModel_IDC, &descriptor_IDC, mdlDescriptors_IDC, numMdlDescriptors_IDC},
            {IACLibrary, createModel_IAC, deleteModel_IAC, &descriptor_IAC, mdlDescriptors_IAC, numMdlDescriptors_IAC},
            {VCVSLibrary, createModel_VCVS, deleteModel_VCVS, &descriptor_VCVS, mdlDescriptors_VCVS, numMdlDescriptors_VCVS},
            {CCVSLibrary, createModel_CCVS, deleteModel_CCVS, &descriptor_CCVS, mdlDescriptors_CCVS, numMdlDescriptors_CCVS},
            {VCCSLibrary, createModel_VCCS, deleteModel_VCCS, &descriptor_VCCS, mdlDescriptors_VCCS, numMdlDescriptors_VCCS},
            {CCCSLibrary, createModel_CCCS, deleteModel_CCCS, &descriptor_CCCS, mdlDescriptors_CCCS, numMdlDescriptors_CCCS},
            {PNLibrary, createModel_PN, deleteModel_PN, &descriptor_PN, mdlDescriptors_PN, numMdlDescriptors_PN},
            {CapacitorLibrary, createModel_Capacitor, deleteModel_Capacitor, &descriptor_Capacitor, mdlDescriptors_Capacitor, numMdlDescriptors_Capacitor},
            {InductorLibrary, createModel_Inductor, deleteModel_Inductor, &descriptor_Inductor, mdlDescriptors_Inductor, numMdlDescriptors_Inductor},
            {BSIM3V32Library, createModel_BSIM3V32, deleteModel_BSIM3V32, &descriptor_BSIM3V32, mdlDescriptors_BSIM3V32, numMdlDescriptors_BSIM3V32},
        };

        for (size_t i = 0; i < sizeof(table) / sizeof(table[0]); i++)
        {
            if (std::string(filename) == table[i].key)
            {
                return new ModelEntry(nullptr, table[i].create, table[i].del,
                                      table[i].desc, table[i].mdlDesc, table[i].numMdl);
            }
        }
        return nullptr;
    }
} // namespace csim

#endif /* CSIM_USE_STATIC_MODELS */
