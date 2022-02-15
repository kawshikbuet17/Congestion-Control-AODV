set terminal pdf
set output "kkp_report.pdf"
set title "Receive Rate"
set xlabel "Simulation Time (Seconds)"
set ylabel "Receive Rate"
plot "kkp_AODV_data" using 1:2 with lines title "AODV_R", "kkp_OLSR_data" using 1:2 with lines title "OLSR_R", "kkp_DSDV_data" using 1:2 with lines title "DSDV_R",  "kkp_DSR_data" using 1:2 with lines title "DSR_R"
