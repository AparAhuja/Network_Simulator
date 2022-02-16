set terminal png
set output "TcpNewReno.png"
set title "Congestion Window Size vs. Time (NewReno)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
set datafile separator ","
plot "TcpNewReno.csv" using 1:2 with lines title "cwnd"

set terminal png
set output "TcpHighSpeed.png"
set title "Congestion Window Size vs. Time (HighSpeed)"
set xlabel "Time (Seconds)"
plot "TcpHighSpeed.csv" using 1:2 with lines title "cwnd"

set terminal png
set output "TcpVeno.png"
set title "Congestion Window Size vs. Time (Veno)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "TcpVeno.csv" using 1:2 with lines title "cwnd"

set terminal png
set output "TcpVegas.png"
set title "Congestion Window Size vs. Time (Vegas)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "TcpVegas.csv" using 1:2 with lines title "cwnd"

