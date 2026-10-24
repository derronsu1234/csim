// SPDX-License-Identifier: LGPL-2.1-or-later
#ifndef MODEL_VDC_H_
#define MODEL_VDC_H_

#include "csim/model/ModelBase.h"

struct sin_signal {  
	double vl;
	double vh;
	double freq;
	double Time_delay;
	double Damping_factor;
	double phase;
};  

struct pulse_signal {  
	//pulse(v1 v2 td tr tf pw per)
	double vl;
	double vh;
	double td;
	double tr;
	double tf;
	double pw;
	double per;
}; 

struct lfsr_signal {
	//lfsr(vlow vhigh tdelay trise tfall rate seed taps rout=val lfsrfile='filename')
	double vl;
	double vh;
	double td;
	double tr;
	double tf;
	double rate;
	int seed;
	std::vector<int> taps;
	double rout;
	std::string lfsrfile;
	std::vector<int> bit_stream;
};

namespace csimModel
{

    class VDC : public ModelBase
    {
    public:
        VDC(MODELBASE_CONSTRUCTOR_DEF);
        virtual ~VDC();

    public:
        virtual int configure();
        virtual int prepareDC();
        virtual int prepareAC();
        virtual int prepareTR();
        virtual int iterateDC();
        virtual int iterateAC(double omega);
        virtual int iterateTR(double tTime);
        void setTranValue();
    };
    std::string  m_value;
    std::string  m_type ;
    double  dc_voltage;
    double  ac_voltage;
    double m_td = 0.00;
	sin_signal m_sin;
	pulse_signal m_pulse;
	lfsr_signal m_lfsr;
	std::vector<double> m_pwl_time;
	std::vector<double> m_pwl_voltage;
}

#endif // MODEL_VDC_H_