#ifndef MODELMANAGER_H
#define MODELMANAGER_H
#include "csim/internal/ModelLoader.h"
#include "constants.h"
class ModelManager {
public:
    ModelManager();
    ~ModelManager();
    csim::ModelEntry* getResistor() { return e_R; }
    csim::ModelEntry* getVDC() { return e_VDC; }
    csim::ModelEntry* getCapacitor() { return e_C; }
    csim::ModelEntry* getInductor() { return e_L; }
    csim::ModelEntry* getDiode() { return e_PN; }
    csim::ModelEntry* getIDC() { return e_IDC; }
    csim::ModelEntry* getVCCS() { return e_VCCS; }
    csim::ModelEntry* getVCVS() { return e_VCVS; }
    csim::ModelEntry* getCCCS() { return e_CCCS; }
    csim::ModelEntry* getCCVS() { return e_CCVS; }
    
private:
    csim::ModelEntry* e_R;
    csim::ModelEntry* e_VDC;
    csim::ModelEntry* e_C;
    csim::ModelEntry* e_L;
    csim::ModelEntry* e_PN;
    csim::ModelEntry* e_IDC;
    csim::ModelEntry* e_VCCS;
    csim::ModelEntry* e_VCVS;
    csim::ModelEntry* e_CCCS;
    csim::ModelEntry* e_CCVS;
};
#endif // MODELMANAGER_H