set terminal pdf
set output "aodv_report.pdf"
set title "nWifi = 50"
set xlabel "nSinks"
set ylabel "Quantity"
plot "DATA_NSINKS.txt" using 1:2 with lines title "Tx Packets", "DATA_NSINKS.txt" using 1:3 with lines title "Rx Packets", "DATA_NSINKS.txt" using 1:4 with lines title "Packets Lost",  "DATA_NSINKS.txt" using 1:4 with lines title "Throughput", "DATA_NSINKS.txt" using 1:5 with lines title "Packet Loss Ratio", "DATA_NSINKS.txt" using 1:6 with lines title "Packet Delivery Ratio", "DATA_NSINKS.txt" using 1:7 with lines title "Avg End to End Delay"