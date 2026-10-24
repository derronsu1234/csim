** VCCS 受控源测试：G1 输出电流 = 0.5 * V(nctrl)
** 理论值：V(nctrl)=2V -> Iout=1A -> V(nout)=2V
V1 nctrl 0 DC 2
R1 nout 0 2
G1 nout 0 nctrl 0 0.5

.probe v(nctrl) v(nout) i(V1)
.op

.end
