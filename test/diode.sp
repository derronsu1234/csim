
V1 in 0 DC 123 AC 5
R1 in out1 1e3
.model MyDiode D (
+ IS=1e-14
+ CJ0=1e-12
+ VJ=0.7
+ M=0.5
+ TT=12e-9
+ RS=0.5
+ )

D1 out1 out2 MyDiode
V2 out2 0 0

.op
**.dc V1 0 10 0.1

.probe v(in) v(out1)
 
.end