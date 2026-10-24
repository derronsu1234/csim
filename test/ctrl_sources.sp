** 电流源 + VCCS 受控源测试
** I1 注入 1A 到 n1；G1 受控源跨导 0.5，控制电压 V(nctrl)=2V -> 输出 1A 到 nout
V1 nctrl 0 DC 2
I1 n1 0 DC 1
R1 n1 0 2
R2 nout 0 2
G1 nout 0 nctrl 0 0.5

.probe v(n1) v(nout) i(I1)
.op

.end
