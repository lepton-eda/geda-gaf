v 20191003 2
C 40000 40000 0 0 0 title-B.sym
C 54600 46500 1 0 0 resistor-1.sym
{
T 54900 46900 5 10 0 0 0 0 1
device=RESISTOR
T 54800 46800 5 10 1 1 0 0 1
refdes=R1
T 54900 46200 5 10 1 1 0 0 1
value=470
}
C 52400 46400 1 0 0 diode-1.sym
{
T 52800 47000 5 10 0 0 0 0 1
device=DIODE
T 52700 46900 5 10 1 1 0 0 1
refdes=D1
T 52400 46400 5 10 0 1 0 0 1
model=IS=0.1PA, RS=16 CJO=2PF TT=12N BV=100 IBV=0.1PA
T 52500 46100 5 10 1 1 0 0 1
model-name=D1N4148
}
C 55400 46300 1 0 0 gnd-1.sym
N 53300 46600 54600 46600 4
C 51600 45700 1 90 0 inductor-1.sym
{
T 51100 45900 5 10 0 0 90 0 1
device=INDUCTOR
T 51300 45900 5 10 1 1 90 0 1
refdes=L1
T 50900 45900 5 10 0 0 90 0 1
symversion=0.1
T 51600 45700 5 10 1 1 0 0 1
value=12u
}
C 50700 45700 1 90 0 resistor-1.sym
{
T 50300 46000 5 10 0 0 90 0 1
device=RESISTOR
T 50400 45900 5 10 1 1 90 0 1
refdes=R2
T 50700 45700 5 10 1 1 0 0 1
value=10k
}
C 49800 45700 1 90 0 capacitor-1.sym
{
T 49100 45900 5 10 0 0 90 0 1
device=CAPACITOR
T 49300 45900 5 10 1 1 90 0 1
refdes=C1
T 48900 45900 5 10 0 0 90 0 1
symversion=0.1
T 49800 45700 5 10 1 1 0 0 1
value=18p
}
N 52400 46600 49600 46600 4
C 49500 45400 1 0 0 gnd-1.sym
N 51500 45000 51500 45700 4
N 50600 45000 50600 45700 4
C 48700 46500 1 0 0 inductor-1.sym
{
T 48900 47000 5 10 0 0 0 0 1
device=INDUCTOR
T 48900 46800 5 10 1 1 0 0 1
refdes=L2
T 48900 47200 5 10 0 0 0 0 1
symversion=0.1
T 48900 46400 5 10 1 1 0 0 1
value=8.2u
}
C 47800 46400 1 0 0 capacitor-1.sym
{
T 48000 47100 5 10 0 0 0 0 1
device=CAPACITOR
T 48000 46900 5 10 1 1 0 0 1
refdes=C2
T 48000 47300 5 10 0 0 0 0 1
symversion=0.1
T 48200 46100 5 10 1 1 0 0 1
value=27p
}
C 54800 45000 1 270 0 voltage-3.sym
{
T 55500 44800 5 8 0 0 270 0 1
device=VOLTAGE_SOURCE
T 55300 44700 5 10 1 1 270 0 1
refdes=V1
T 55300 44100 5 10 1 1 0 0 1
value=9
}
C 53000 44900 1 0 0 resistor-1.sym
{
T 53300 45300 5 10 0 0 0 0 1
device=RESISTOR
T 53200 45200 5 10 1 1 0 0 1
refdes=R3
T 53300 44600 5 10 1 1 0 0 1
value=100
}
N 55000 45000 53900 45000 4
N 47600 45000 53000 45000 4
C 54900 43800 1 0 0 gnd-1.sym
C 50500 43800 1 0 0 gnd-1.sym
C 51400 43800 1 0 0 gnd-1.sym
C 50800 44100 1 90 0 capacitor-1.sym
{
T 50100 44300 5 10 0 0 90 0 1
device=CAPACITOR
T 50300 44300 5 10 1 1 90 0 1
refdes=C3
T 49900 44300 5 10 0 0 90 0 1
symversion=0.1
T 50800 44100 5 10 1 1 0 0 1
value=10u
}
C 51700 44100 1 90 0 capacitor-1.sym
{
T 51000 44300 5 10 0 0 90 0 1
device=CAPACITOR
T 51200 44300 5 10 1 1 90 0 1
refdes=C4
T 50800 44300 5 10 0 0 90 0 1
symversion=0.1
T 51700 44100 5 10 1 1 0 0 1
value=.047u
}
C 48000 45700 1 90 0 capacitor-1.sym
{
T 47300 45900 5 10 0 0 90 0 1
device=CAPACITOR
T 47500 45900 5 10 1 1 90 0 1
refdes=C5
T 47100 45900 5 10 0 0 90 0 1
symversion=0.1
T 48000 45700 5 10 1 1 0 0 1
value=56p
}
C 47100 45700 1 90 0 inductor-1.sym
{
T 46600 45900 5 10 0 0 90 0 1
device=INDUCTOR
T 46800 45900 5 10 1 1 90 0 1
refdes=L3
T 46400 45900 5 10 0 0 90 0 1
symversion=0.1
T 47100 45700 5 10 1 1 0 0 1
value=3.9u
}
N 47800 46600 47000 46600 4
C 47700 45400 1 0 0 gnd-1.sym
C 46900 45400 1 0 0 gnd-1.sym
C 46100 46400 1 0 0 capacitor-1.sym
{
T 46300 47100 5 10 0 0 0 0 1
device=CAPACITOR
T 46300 46900 5 10 1 1 0 0 1
refdes=C6
T 46300 47300 5 10 0 0 0 0 1
symversion=0.1
T 46200 46200 5 10 1 1 0 0 1
value=27p
}
C 45200 46500 1 0 0 inductor-1.sym
{
T 45400 47000 5 10 0 0 0 0 1
device=INDUCTOR
T 45400 46800 5 10 1 1 0 0 1
refdes=L4
T 45400 47200 5 10 0 0 0 0 1
symversion=0.1
T 45500 46300 5 10 1 1 0 0 1
value=8.2u
}
C 46700 44900 1 0 0 resistor-1.sym
{
T 47000 45300 5 10 0 0 0 0 1
device=RESISTOR
T 46900 45200 5 10 1 1 0 0 1
refdes=R4
T 47000 44700 5 10 1 1 0 0 1
value=100
}
C 49000 44100 1 90 0 resistor-1.sym
{
T 48600 44400 5 10 0 0 90 0 1
device=RESISTOR
T 48700 44300 5 10 1 1 90 0 1
refdes=R5
T 49000 44100 5 10 1 1 0 0 1
value=10k
}
C 48800 43800 1 0 0 gnd-1.sym
C 45400 45700 1 90 0 capacitor-1.sym
{
T 44700 45900 5 10 0 0 90 0 1
device=CAPACITOR
T 44900 45900 5 10 1 1 90 0 1
refdes=C7
T 44500 45900 5 10 0 0 90 0 1
symversion=0.1
T 45400 45700 5 10 1 1 0 0 1
value=18p
}
C 44600 45700 1 90 0 resistor-1.sym
{
T 44200 46000 5 10 0 0 90 0 1
device=RESISTOR
T 44300 45900 5 10 1 1 90 0 1
refdes=R6
T 44600 45700 5 10 1 1 0 0 1
value=10k
}
C 44100 45700 1 90 0 inductor-1.sym
{
T 43600 45900 5 10 0 0 90 0 1
device=INDUCTOR
T 43800 45900 5 10 1 1 90 0 1
refdes=L5
T 43400 45900 5 10 0 0 90 0 1
symversion=0.1
T 44100 45700 5 10 1 1 0 0 1
value=12u
}
N 43500 46600 45200 46600 4
C 44200 44100 1 90 0 capacitor-1.sym
{
T 43500 44300 5 10 0 0 90 0 1
device=CAPACITOR
T 43700 44300 5 10 1 1 90 0 1
refdes=C8
T 43300 44300 5 10 0 0 90 0 1
symversion=0.1
T 44200 44100 5 10 1 1 0 0 1
value=.047u
}
C 45200 44100 1 90 0 capacitor-1.sym
{
T 44500 44300 5 10 0 0 90 0 1
device=CAPACITOR
T 44700 44300 5 10 1 1 90 0 1
refdes=C9
T 44300 44300 5 10 0 0 90 0 1
symversion=0.1
T 45200 44100 5 10 1 1 0 0 1
value=10u
}
N 46700 45000 44000 45000 4
N 44000 45000 44000 45700 4
C 44900 43800 1 0 0 gnd-1.sym
C 43900 43800 1 0 0 gnd-1.sym
C 44400 45400 1 0 0 gnd-1.sym
C 45100 45400 1 0 0 gnd-1.sym
C 43500 46800 1 180 0 diode-1.sym
{
T 43100 46200 5 10 0 0 180 0 1
device=DIODE
T 43200 46300 5 10 1 1 180 0 1
refdes=D2
T 43500 46800 5 10 0 1 0 0 1
model=IS=0.1PA, RS=16 CJO=2PF TT=12N BV=100 IBV=0.1PA
T 43500 46800 5 10 1 1 0 0 1
model-name=D1N4148
}
C 42700 44800 1 90 0 resistor-1.sym
{
T 42300 45100 5 10 0 0 90 0 1
device=RESISTOR
T 42400 45000 5 10 1 1 90 0 1
refdes=R7
T 42700 44800 5 10 1 1 0 0 1
value=470
}
N 42600 45700 42600 46600 4
C 42500 44500 1 0 0 gnd-1.sym
C 41000 46300 1 90 0 capacitor-1.sym
{
T 40300 46500 5 10 0 0 90 0 1
device=CAPACITOR
T 40500 46500 5 10 1 1 90 0 1
refdes=C10
T 40100 46500 5 10 0 0 90 0 1
symversion=0.1
T 40900 46900 5 10 1 1 0 0 1
value=.047u
}
C 41900 46300 1 90 0 capacitor-1.sym
{
T 41200 46500 5 10 0 0 90 0 1
device=CAPACITOR
T 41400 46500 5 10 1 1 90 0 1
refdes=C11
T 41000 46500 5 10 0 0 90 0 1
symversion=0.1
T 42000 46600 5 10 1 1 0 0 1
value=10u
}
N 40800 46300 42600 46300 4
C 41300 48800 1 0 0 voltage-3.sym
{
T 41500 49500 5 8 0 0 0 0 1
device=VOLTAGE_SOURCE
T 41600 49300 5 10 1 1 0 0 1
refdes=V2
T 41500 48500 5 10 1 1 0 0 1
value=ac 1u
}
N 40800 47200 41700 47200 4
N 41300 49000 41300 47200 4
C 42100 48700 1 0 0 gnd-1.sym
T 50300 40800 9 10 1 0 0 0 1
frg_bandpasstest
C 53000 49700 1 0 0 spice-include-1.sym
{
T 53100 50000 5 10 0 1 0 0 1
device=include
T 53100 50100 5 10 1 1 0 0 1
refdes=A1
T 53500 49800 5 10 1 1 0 0 1
file=./ngspice.cmd
}