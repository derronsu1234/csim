** include 相对路径 + .END 大小写测试
V1 n1 0 DC 123 AC 5
.include "inc_sub.inc"
R2 n2 0 7

.probe v(n1) v(n2)
.op

.END
