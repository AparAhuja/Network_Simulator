set terminal png
set output "config1/TcpNewReno_N1.png"
set title "Congestion Window Size vs. Time (Connection 1)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
set datafile separator ","
plot "config1/TcpNewReno_N1.csv" using 1:2 with lines title "cwnd"

set terminal png
set output "config1/TcpNewReno_N2.png"
set title "Congestion Window Size vs. Time (Connection 2)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
set datafile separator ","
plot "config1/TcpNewReno_N2.csv" using 1:2 with lines title "cwnd"

set terminal png
set output "config1/TcpNewReno_N3.png"
set title "Congestion Window Size vs. Time (Connection 3)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
set datafile separator ","
plot "config1/TcpNewReno_N3.csv" using 1:2 with lines title "cwnd"


set terminal png
set output "config2/TcpHybrid_N1.png"
set title "Congestion Window Size vs. Time (Connection 1)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
set datafile separator ","
plot "config2/TcpHybrid_N1.csv" using 1:2 with lines title "cwnd"

set terminal png
set output "config2/TcpHybrid_N2.png"
set title "Congestion Window Size vs. Time (Connection 2)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
set datafile separator ","
plot "config2/TcpHybrid_N2.csv" using 1:2 with lines title "cwnd"

set terminal png
set output "config2/TcpHybrid_N3.png"
set title "Congestion Window Size vs. Time (Connection 3)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
set datafile separator ","
plot "config2/TcpHybrid_N3.csv" using 1:2 with lines title "cwnd"




set terminal png
set output "config3/TcpNewRenoCSE_N1.png"
set title "Congestion Window Size vs. Time (Connection 1)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
set datafile separator ","
plot "config3/TcpNewRenoCSE_N1.csv" using 1:2 with lines title "cwnd"

set terminal png
set output "config3/TcpNewRenoCSE_N2.png"
set title "Congestion Window Size vs. Time (Connection 2)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
set datafile separator ","
plot "config3/TcpNewRenoCSE_N2.csv" using 1:2 with lines title "cwnd"

set terminal png
set output "config3/TcpNewRenoCSE_N3.png"
set title "Congestion Window Size vs. Time (Connection 3)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
set datafile separator ","
plot "config3/TcpNewRenoCSE_N3.csv" using 1:2 with lines title "cwnd"

