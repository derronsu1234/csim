

V1 n1 0 DC 123 AC 5
R1 n1 n2 2
R2 n2 n3 7 
R3 n3 0 1


.probe v(n1) v(n2) v(n3)  i(v1)
**.op
.dc V1 -10 10 1

.end