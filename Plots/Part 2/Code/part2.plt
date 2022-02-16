set terminal png
set output "channel/ChannelRate_2Mbps.png"
set title "Congestion Window Size vs. Time (ChannelRate_2Mbps)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
set datafile separator ","
plot "channel/ChannelRate_2Mbps.csv" using 1:2 with lines title "cwnd"

set terminal png
set output "channel/ChannelRate_4Mbps.png"
set title "Congestion Window Size vs. Time (ChannelRate_4Mbps)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "channel/ChannelRate_4Mbps.csv" using 1:2 with lines title "cwnd"

set terminal png
set output "channel/ChannelRate_10Mbps.png"
set title "Congestion Window Size vs. Time (ChannelRate_10Mbps)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "channel/ChannelRate_10Mbps.csv" using 1:2 with lines title "cwnd"

set terminal png
set output "channel/ChannelRate_20Mbps.png"
set title "Congestion Window Size vs. Time (ChannelRate_20Mbps)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "channel/ChannelRate_20Mbps.csv" using 1:2 with lines title "cwnd"

set terminal png
set output "channel/ChannelRate_50Mbps.png"
set title "Congestion Window Size vs. Time (ChannelRate_50Mbps)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "channel/ChannelRate_50Mbps.csv" using 1:2 with lines title "cwnd"

set terminal png
set output "app/ApplicationRate_500Kbps.png"
set title "Congestion Window Size vs. Time (ApplicationRate_500Kbps)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "app/ApplicationRate_500Kbps.csv" using 1:2 with lines title "cwnd"

set terminal png
set output "app/ApplicationRate_1Mbps.png"
set title "Congestion Window Size vs. Time (ApplicationRate_1Mbps)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "app/ApplicationRate_1Mbps.csv" using 1:2 with lines title "cwnd"

set terminal png
set output "app/ApplicationRate_2Mbps.png"
set title "Congestion Window Size vs. Time (ApplicationRate_2Mbps)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "app/ApplicationRate_2Mbps.csv" using 1:2 with lines title "cwnd"

set terminal png
set output "app/ApplicationRate_4Mbps.png"
set title "Congestion Window Size vs. Time (ApplicationRate_4Mbps)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "app/ApplicationRate_4Mbps.csv" using 1:2 with lines title "cwnd"

set terminal png
set output "app/ApplicationRate_10Mbps.png"
set title "Congestion Window Size vs. Time (ApplicationRate_10Mbps)"
set xlabel "Time (Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "app/ApplicationRate_10Mbps.csv" using 1:2 with lines title "cwnd"
