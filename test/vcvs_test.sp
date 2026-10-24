** VCVS 受控电压源测试：E1 = 2 * V(nctrl)，V(nctrl)=2V -> V(nout)=4V
V1 nctrl 0 DC 2
R1 nout 0 1000
E1 nout 0 nctrl 0 2

.probe v(nctrl) v(nout)
.op

.end
