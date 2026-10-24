

R1 n1 n2 2
R2 n2 n3 1
**L3 n3 0 1e-9
L1 n2 n3 10e-9
C1 n3 0 10e-11
VIN1 n1 0 DC 0 AC 1 pulse(0 1 0 0.1e-9 0.1e-9 0.9e-9 2e-9)

.probe v(n1)  v(n2)  v(n3)  i(VIN1)
.tran 0.1e-9 10e-9
.option SolverType = ngspice 
.end