

R1 n1 n2 1000
C1 n2 0 4.7E-6
VIN1 n1 0 1

.probe v(n1)  v(n2)
.tran 1e-8 1e-5

.end