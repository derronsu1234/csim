#include "modelmanager.h"

ModelManager::ModelManager() {
    e_R = csim::ModelLoader::load(resistorLibrary);
    e_VDC = csim::ModelLoader::load(VDCLibrary);
    e_C = csim::ModelLoader::load(CapacitorLibrary);
    e_L = csim::ModelLoader::load(InductorLibrary);
    e_PN = csim::ModelLoader::load(PNLibrary);
    e_IDC = csim::ModelLoader::load(IDCLibrary);
    e_VCCS = csim::ModelLoader::load(VCCSLibrary);
    e_VCVS = csim::ModelLoader::load(VCVSLibrary);
    e_CCCS = csim::ModelLoader::load(CCCSLibrary);
    e_CCVS = csim::ModelLoader::load(CCVSLibrary);
}

ModelManager::~ModelManager() {
    delete e_R;
    delete e_VDC;
    delete e_C;
    delete e_L;
    delete e_PN;
    delete e_IDC;
    delete e_VCCS;
    delete e_VCVS;
    delete e_CCCS;
    delete e_CCVS;
}
