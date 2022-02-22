# Vary Num of Nodes
set terminal pdf
set output "TaskA_Wireless_LowRate_Plot.pdf"
set title "TaskA Wireless LowRate (Throughput vs Num of Nodes) \nNumber of Flows = 100 \nNumber of Packets Per Second = 100 \nPacket Size = 4096 byte\nTX Range = 5"
set xlabel "Number of Nodes"
set ylabel "Throughput (Kbps)"
plot "VaryNodes.txt" using 1:2 with lines title "Throughput"


set title "TaskA Wireless LowRate (End-to-End Delay vs Num of Nodes) \nNumber of Flows = 100 \nNumber of Packets Per Second = 100 \nPacket Size = 4096 byte\nTX Range = 5"
set xlabel "Number of Nodes"
set ylabel "End to End Delay (Second)"
plot "VaryNodes.txt" using 1:3 with lines title "End to End Delay"


set title "TaskA Wireless LowRate (Packet Delivery Ratio vs Num of Nodes) \nNumber of Flows = 100 \nNumber of Packets Per Second = 100 \nPacket Size = 4096 byte\nTX Range = 5"
set xlabel "Number of Nodes"
set ylabel "Packet Delivery Ratio (%)"
plot "VaryNodes.txt" using 1:4 with lines title "Packet Delivery Ratio (%)"


set title "TaskA Wireless LowRate (Packet Loss Ratio vs Num of Nodes) \nNumber of Flows = 100 \nNumber of Packets Per Second = 100 \nPacket Size = 4096 byte\nTX Range = 5"
set xlabel "Number of Nodes"
set ylabel "Packet Loss Ratio (%)"
plot "VaryNodes.txt" using 1:5 with lines title "Packet Loss Ratio (%)"





# Vary Num of Flows
set title "TaskA Wireless LowRate (Throughput vs Num of Flows) \nNumber of Nodes = 50 \nNumber of Packets Per Second = 100 \nPacket Size = 4096 byte\nTX Range = 5"
set xlabel "Number of Flows"
set ylabel "Throughput (Kbps)"
plot "VaryFlows.txt" using 1:2 with lines title "Throughput"


set title "TaskA Wireless LowRate (End-to-End Delay vs Num of Flows) \nNumber of Nodes = 50 \nNumber of Packets Per Second = 100 \nPacket Size = 4096 byte\nTX Range = 5"
set xlabel "Number of Flows"
set ylabel "End to End Delay (Second)"
plot "VaryFlows.txt" using 1:3 with lines title "End to End Delay"


set title "TaskA Wireless LowRate (Packet Delivery Ratio vs Num of Flows) \nNumber of Nodes = 50 \nNumber of Packets Per Second = 100 \nPacket Size = 4096 byte\nTX Range = 5"
set xlabel "Number of Flows"
set ylabel "Packet Delivery Ratio (%)"
plot "VaryFlows.txt" using 1:4 with lines title "Packet Delivery Ratio (%)"


set title "TaskA Wireless LowRate (Packet Loss Ratio vs Num of Flows) \nNumber of Nodes = 50 \nNumber of Packets Per Second = 100 \nPacket Size = 4096 byte\nTX Range = 5"
set xlabel "Number of Flows"
set ylabel "Packet Loss Ratio (%)"
plot "VaryFlows.txt" using 1:5 with lines title "Packet Loss Ratio (%)"




# Vary Num of Packets Per Second
set title "TaskA Wireless LowRate (Throughput vs Num of Packets Per Second) \nNumber of Nodes = 50 \nNumber of Flows = 50 \nPacket Size = 4096 byte\nTX Range = 5"
set xlabel "Number of Packets Per Second"
set ylabel "Throughput (Kbps)"
plot "VaryPacketsPerSecond.txt" using 1:2 with lines title "Throughput"


set title "TaskA Wireless LowRate (End-to-End Delay vs Num of Packets Per Second) \nNumber of Nodes = 50 \nNumber of Flows = 50 \nPacket Size = 4096 byte\nTX Range = 5"
set xlabel "Number of Packets Per Second"
set ylabel "End to End Delay (Second)"
plot "VaryPacketsPerSecond.txt" using 1:3 with lines title "End to End Delay"


set title "TaskA Wireless LowRate (Packet Delivery Ratio vs Num of Packets Per Second) \nNumber of Nodes = 50 \nNumber of Flows = 50 \nPacket Size = 4096 byte\nTX Range = 5"
set xlabel "Number of Packets Per Second"
set ylabel "Packet Delivery Ratio (%)"
plot "VaryPacketsPerSecond.txt" using 1:4 with lines title "Packet Delivery Ratio (%)"


set title "TaskA Wireless LowRate (Packet Loss Ratio vs Num of Packets Per Second) \nNumber of Nodes = 50 \nNumber of Flows = 50 \nPacket Size = 4096 byte\nTX Range = 5"
set xlabel "Number of Packets Per Second"
set ylabel "Packet Loss Ratio (%)"
plot "VaryPacketsPerSecond.txt" using 1:5 with lines title "Packet Loss Ratio (%)"




# Vary TX Range
set title "TaskA Wireless LowRate (Throughput vs TX Range) \nNumber of Nodes = 50 \nNumber of Flows = 50 \nPacket Size = 4096 byte"
set xlabel "Number of TX Range"
set ylabel "Throughput (Kbps)"
plot "VaryTxRange.txt" using 1:2 with lines title "Throughput"


set title "TaskA Wireless LowRate (End-to-End Delay vs TX Range) \nNumber of Nodes = 50 \nNumber of Flows = 50 \nPacket Size = 4096 byte"
set xlabel "Number of TX Range"
set ylabel "End to End Delay (Second)"
plot "VaryTxRange.txt" using 1:3 with lines title "End to End Delay"


set title "TaskA Wireless LowRate (Packet Delivery Ratio vs TX Range) \nNumber of Nodes = 50 \nNumber of Flows = 50 \nPacket Size = 4096 byte"
set xlabel "Number of TX Range"
set ylabel "Packet Delivery Ratio (%)"
plot "VaryTxRange.txt" using 1:4 with lines title "Packet Delivery Ratio (%)"


set title "TaskA Wireless LowRate (Packet Loss Ratio vs TX Range) \nNumber of Nodes = 50 \nNumber of Flows = 50 \nPacket Size = 4096 byte"
set xlabel "Number of TX Range"
set ylabel "Packet Loss Ratio (%)"
plot "VaryTxRange.txt" using 1:5 with lines title "Packet Loss Ratio (%)"
