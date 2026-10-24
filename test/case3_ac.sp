
**.option post=2
R1 n1 n2 1
L1 n2 n3 1e-10
C1 n3 0 1e-11

VIN1 n1 0 DC 1 AC 5

.probe v(n2)  v(n1) v(n3) 
.ac lin 101 1e9 10e9
.option SolverType = ngspice 


.end