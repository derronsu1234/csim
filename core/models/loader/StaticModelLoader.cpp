#include "StaticModelLoader.h"
#include "csim/utils/string.h"
#include "csim/utils/errors.h"
#include <vector>

namespace csim
{
    StaticModelEntry::StaticModelEntry(csimModel::pfnCreateModel_t pfnCreate,
                                      csimModel::pfnDeleteModel_t pfnDelete,
                                      const ModelDescriptor *descriptor,
                                      const PropertyMdlDescriptor *mdlDescriptors,
                                      size_t numMdlDescriptors)
        : m_pfnCreate(pfnCreate),
          m_pfnDelete(pfnDelete),
          m_descriptor(descriptor),
          m_mdlDescriptors(mdlDescriptors),
          m_numMdlDescriptors(numMdlDescriptors)
    {
        buildMdlIndexs();
    }

    StaticModelEntry::~StaticModelEntry()
    {
        // 静态模型无需释放DLL句柄
    }


    csimModel::ModelBase *StaticModelEntry::createInstance(MODELBASE_CONSTRUCTOR_DEF) const
    {
        return m_pfnCreate(MODELBASE_CONSTRUCTOR_VAR);
    }

    void StaticModelEntry::deleteInstance(csimModel::ModelBase *model) const
    {
        m_pfnDelete(model);
    }

    int StaticModelEntry::buildMdlIndexs()
    {
        std::string mdlName, propName;
        for (size_t i = 0; i < m_numMdlDescriptors; i++)
        {
            mdlName = toUpper(m_mdlDescriptors[i].name);
            if (m_mdls.find(mdlName) != m_mdls.end())
                return CERR_MDL_EXISTING;
            MdlEntry &mdl = m_mdls[mdlName];
            mdl.desc = &m_mdlDescriptors[i];

            for (size_t j = 0; j < mdl.desc->numEntries; j++)
            {
                const PropertyMdlPropDescriptor *prop = &mdl.desc->entries[j];
                propName = toUpper(prop->name);
                if (mdl.props.find(propName) != mdl.props.end())
                    return CERR_MDL_PROP_EXISTING;
                mdl.props[propName] = prop;
            }
        }
        return 0;
    }


    int StaticModelEntry::getMdlEntry(const char *mdl, const MdlEntry **out) const
    {
        std::string mdlName = toUpper(mdl);
        if (m_mdls.find(mdlName) == m_mdls.end())
            return CERR_NO_SUCH_MDL;
        *out = &m_mdls.at(mdlName);
        return 0;
    }

    csimModel::PropertyMdl *StaticModelEntry::MdlEntry::createInstance() const
    {
        return desc->pfnCreate();
    }

    void StaticModelEntry::MdlEntry::deleteInstance(csimModel::PropertyMdl *mdl) const
    {
        desc->pfnDelete(mdl);
    }

    const PropertyMdlPropDescriptor *StaticModelEntry::MdlEntry::getProperty(const char *name) const
    {
        std::string uniname = toUpper(name);
        if (props.find(uniname) == props.end())
            return nullptr;
        return props.at(uniname);
    }








 
}