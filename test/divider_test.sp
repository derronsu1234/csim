** 电阻分压测试：V1=3V, R1=1Ω, R2=2Ω -> V(n2) 应为 2V
V1 n1 0 DC 3
R1 n1 n2 1
R2 n2 0 2

.probe v(n1) v(n2)
.op

.end
