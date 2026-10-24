/**
 * @file Ideal DC Voltage Source Model
 */

/*
 *  FastCSIM Copyright (C) 2021 cassuto                                    
 *  This project is free edition{} you can redistribute it and/or          
 *  modify it under the terms of the GNU Lesser General Public             
 *  License(LGPL) as published by the Free Software Foundation{} either     
 *  version 2.1 of the License, or (at your option) any later version.     
 *                                                                         
 *  This project is distributed in the hope that it will be useful,        
 *  but WITHOUT ANY WARRANTY{} without even the implied warranty of        
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      
 *  Lesser General Public License for more details.                        
 */

#include "VDC.h"

namespace csimModel
{

    VDC::VDC(MODELBASE_CONSTRUCTOR_DEF)
        : ModelBase(MODELBASE_CONSTRUCTOR_VAR)
    {
        //property().addProperty("V", Variant(Variant::VariantDouble).setDouble(5.0), nullptr, csimModel::PropertyBag::Required | csimModel::PropertyBag::Write | csimModel::PropertyBag::Read);
        property().addProperty("V", Variant(Variant::VariantDouble).setDouble(5.0), nullptr, csimModel::PropertyBag::Write | csimModel::PropertyBag::Read);
        property().addProperty("value", Variant(Variant::VariantString).setString(""), nullptr, csimModel::PropertyBag::Write | csimModel::PropertyBag::Read);
   
    }

    VDC::~VDC()
    {
    }

    int VDC::configure()
    {
        auto &props = property();
        resizeModel(2, 0, 1);
        m_value= property().getProperty("value").getString();
        m_td = getValueAfterKey(m_value, "td=", 0.00);
        dc_voltage = getValue(m_value, "dc");
        ac_voltage = getValue(m_value, "ac");

        props.setProperty("V", Variant(Variant::VariantDouble).setDouble(dc_voltage ));
        this->setTranValue();
        return 0;
    }

    void VDC::setTranValue(){
        std::string temp = m_value;
        std::vector<std::string> vec = { "sin", "pulse", "pwl","lfsr" };
        std::vector<std::vector<double>> data;
        for (auto ii : vec) {

            if (temp.find(ii) != std::string::npos) {
                m_type = ii;
                if (m_type == "sin") {
                    /*sin ( vl vh freq Time_delay Damping_factor phase)*/
                    data.resize(1);
                    auto extracted = extractContentInsideParentheses(temp, "sin"); // 保存临时对象
                    auto splitResult = splitByChar(extracted, ' ');                 // 传递左值
                    data[0] = string2double(splitResult);
                }
                else if (m_type == "pulse") {
                    data.resize(1);
                    auto extracted = extractContentInsideParentheses(temp, "pulse"); // 保存临时对象
                    auto splitResult = splitByChar(extracted, ' ');                 // 传递左值
                    data[0] = string2double(splitResult);
                }
                else if (m_type == "pwl") {
                    /*pwl(60 0,120 0,130 5,170 5,180 0), r*/
                    auto extracted = extractContentInsideParentheses(temp, "pwl"); // 保存临时对象
                    auto splitResult = splitByChar(extracted, ',');                 // 传递左值				
                    data = string2double2(splitResult);
                }
                else if (m_type == "lfsr") {
                    //m_lfsr
                    data.resize(1);
                    auto value = splitByChar(extractContentInsideParentheses(temp, "lfsr"), ' ');
                    for (int ii = 0; ii < 7; ii++) {
                        data[0].push_back(std::stod(value[ii]));
                    }
                    auto taps = parseStringToVector(value[7]);
                    double rout = 0;
                    if (value.size() > 9)
                        rout = getValueAfterKey(value[8], "rout=", 0);
                    std::string lfsrfile = "xxxx";
                    if (value.size() > 10)
                        lfsrfile = getValueAfterKey(value[9], "rout=", "xxxx");
                    m_lfsr.vl = data[0][0];
                    m_lfsr.vh = data[0][1];
                    m_lfsr.td = data[0][2];
                    m_lfsr.tr = data[0][3];
                    m_lfsr.tf = data[0][4];
                    m_lfsr.rate = data[0][5];
                    m_lfsr.seed = data[0][6];
                    m_lfsr.taps = taps;
                    m_lfsr.rout = rout;
                    m_lfsr.lfsrfile = lfsrfile;
                    m_lfsr.bit_stream = LFSR(m_lfsr.seed, taps);
                }
            }
        }

        if (m_type == "sin") {
            m_sin.vl = data[0][0];
            m_sin.vh = data[0][1];
            m_sin.freq = data[0][2];
            m_sin.Time_delay = data[0][3];
            m_sin.Damping_factor = data[0][4];
            m_sin.phase = data[0][5];
            dc_voltage = (m_sin.vh - m_sin.vl) * sin(2 * 3.14159 * m_sin.freq * 0.0 + m_sin.phase);
        }
        else if (m_type == "pulse") {
            m_pulse.vl = data[0][0];
            m_pulse.vh = data[0][1];
            m_pulse.td = data[0][2];
            m_pulse.tr = data[0][3];
            m_pulse.tf = data[0][4];
            m_pulse.pw = data[0][5];
            m_pulse.per = data[0][6];
            dc_voltage = pulse(0, m_pulse.vl, m_pulse.vh, m_pulse.td, m_pulse.tr, m_pulse.tf, m_pulse.pw, m_pulse.per);
        }
        else if (m_type == "pwl") {
            m_pwl_time = data[0];
            m_pwl_voltage = data[1];
            dc_voltage = m_pwl_voltage[0];
            addValue(m_pwl_time, m_td);
        }
        else if (m_type == "lfsr") {
            auto lfsrdata = lfsr(m_lfsr.vl, m_lfsr.vh, m_lfsr.td, m_lfsr.tr, m_lfsr.tf, m_lfsr.rate, m_lfsr.bit_stream);
            m_pwl_time = std::get<0>(lfsrdata);
            m_pwl_voltage = std::get<1>(lfsrdata);
            dc_voltage = m_pwl_voltage[0];
            addValue(m_pwl_time, m_td);
        }
        


    }

    int VDC::prepareDC()
    {
        return 0;
    }
    int VDC::prepareAC()
    {
        return 0;
    }
    int VDC::prepareTR()
    {
        return 0;
    }

    int VDC::iterateDC()
    {   
        double V = property().getProperty("V").getDouble();
        unsigned int k = getBranch(0);
        addB(getNode(0), k, +1.0);
        addB(getNode(1), k, -1.0);
        addC(k, getNode(0), 1.0), addC(k, getNode(1), -1.0);
        addE(k, V);
        return 0;
    }

    int VDC::iterateAC(double omega)
    {
        std::string Vstr= property().getProperty("value").getString();
        double V = getValue(Vstr, "ac");
        unsigned int k = getBranch(0);
        addB(getNode(0), k, +1.0);
        addB(getNode(1), k, -1.0);
        addC(k, getNode(0), 1.0), addC(k, getNode(1), -1.0);
        addE(k, V); /* Remove any DC offset */
        return 0;
    }

    int VDC::iterateTR(double tTime)
    {
        double v;
        if (m_type == "sin") {
            v = (m_sin.vh - m_sin.vl) * sin(2 * 3.14159 * m_sin.freq * tTime + m_sin.phase) * exp(-tTime * m_sin.Damping_factor);
        }
        else if (m_type == "pulse") {

            v = pulse(tTime, m_pulse.vl, m_pulse.vh, m_pulse.td, m_pulse.tr, m_pulse.tf, m_pulse.pw, m_pulse.per);
        }
        else if (m_type == "pwl") {

            v = interp1(m_pwl_time, m_pwl_voltage,tTime) ;
        }
        else if (m_type == "lfsr") {

            v = interp1(m_pwl_time, m_pwl_voltage,tTime) ;
        }
        else {
            v =dc_voltage;

        }
        unsigned int k = getBranch(0);
        addB(getNode(0), k, +1.0);
        addB(getNode(1), k, -1.0);
        addC(k, getNode(0), 1.0), addC(k, getNode(1), -1.0);
        addE(k, v);
        return 0;
        //return iterateDC();
    }

}

#include "csim/model/ModelExports.h"

CSIM_MODEL_EXPORTS(VDC, "VDC", "Generic ideal DC Voltage Source")
