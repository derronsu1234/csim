** CCCS 用电流源作控制（与库自身测试一致）
** I1 注入 5mA；F1 = 2 * I(I1) = 10mA 到 nout -> V(nout)=10V
I1 in 0 DC 5m
F1 nout 0 I1 2
R1 nout 0 1k

.probe v(in) v(nout)
.op

.end
