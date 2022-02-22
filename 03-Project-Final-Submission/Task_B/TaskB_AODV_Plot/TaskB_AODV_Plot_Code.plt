set terminal pdf
set output "TaskB_AODV_Plot.pdf"

set title "Number of Packet Loss vs Num of Nodes"
set xlabel "Number of Nodes"
set ylabel "Number of Packet Loss"
plot "aodv-data.txt" using 1:3 with lines title "AODV", "cc-aodv-data.txt" using 1:3 with lines title "CC-AODV"

set title "Throughput vs Num of Nodes"
set xlabel "Number of Nodes"
set ylabel "Throughput (Kbps)"
plot "aodv-data.txt" using 1:4 with lines title "AODV", "cc-aodv-data.txt" using 1:4 with lines title "CC-AODV"


set title "End-to-End Delay vs Num of Nodes"
set xlabel "Number of Nodes"
set ylabel "End to End Delay (Second)"
plot "aodv-data.txt" using 1:5 with lines title "AODV", "cc-aodv-data.txt" using 1:5 with lines title "CC-AODV"


set title "Packet Delivery Ratio vs Num of Nodes"
set xlabel "Number of Nodes"
set ylabel "Packet Delivery Ratio (%)"
plot "aodv-data.txt" using 1:6 with lines title "AODV", "cc-aodv-data.txt" using 1:6 with lines title "CC-AODV"


set title "Packet Loss Ratio vs Num of Nodes"
set xlabel "Number of Nodes"
set ylabel "Packet Loss Ratio (%)"
plot "aodv-data.txt" using 1:7 with lines title "AODV", "cc-aodv-data.txt" using 1:7 with lines title "CC-AODV"
