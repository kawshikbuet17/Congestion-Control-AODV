/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SecondScriptExample");

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nNodes = 20;
  int nFlows = 20;
  int nPPS = 100;
  int packetSize = 4096;

  CommandLine cmd (__FILE__);
  cmd.AddValue ("nNodes", "Number of CSMA nodes/devices", nNodes);
  cmd.AddValue("nFlows", "Number of total flows", nFlows);
  cmd.AddValue("nPPS", "Number of packets per second", nPPS);
  cmd.AddValue("packetSize", "Packet Size", packetSize);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);

  cmd.Parse (argc,argv);

  if (verbose)
    {
      ;
      // LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      // LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  nNodes = nNodes == 0 ? 1 : nNodes;
  int nLeft = nNodes/2 - 1;
  int nRight = nNodes/2 -1;
  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  //---Left Side---
  NodeContainer csmaNodes;
  csmaNodes.Add(p2pNodes.Get(0));
  csmaNodes.Create (nLeft);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("1Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (MicroSeconds (100)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);
  
  //---Right Side---
  NodeContainer csmaNodes2;
  csmaNodes2.Add(p2pNodes.Get(1));
  csmaNodes2.Create (nRight);

  CsmaHelper csma2;
  csma2.SetChannelAttribute ("DataRate", StringValue ("1Mbps"));
  csma2.SetChannelAttribute ("Delay", TimeValue (MicroSeconds (100)));

  NetDeviceContainer csmaDevices2;
  csmaDevices2 = csma2.Install (csmaNodes2);

  InternetStackHelper stack;
  stack.Install (csmaNodes);
  stack.Install(csmaNodes2);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces2;
  csmaInterfaces2 = address.Assign (csmaDevices2);

  uint32_t rxPacketsum = 0;
  double Delaysum = 0; 
  uint32_t txPacketsum = 0;
  uint32_t txBytessum = 0;
  uint32_t rxBytessum = 0;
  uint32_t txTimeFirst = 0;
  uint32_t rxTimeLast = 0;
  uint32_t lostPacketssum = 0;

  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();

  int startTime = 1;
  int endTime = 30;

  srand(time(0));
  for(int i=0; i<nFlows; i++){
    int server = rand()%nLeft;
    int client = rand()%nRight;
    int port = 10+i;

    UdpEchoServerHelper echoServer (port);
    ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (server));
    serverApps.Start (Seconds (startTime));

    UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (server), port);
    echoClient.SetAttribute ("MaxPackets", UintegerValue (0));
    echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0/nPPS)));
    echoClient.SetAttribute ("PacketSize", UintegerValue (packetSize));

    ApplicationContainer clientApps = echoClient.Install (csmaNodes2.Get (client));
    clientApps.Start (Seconds (startTime+1));
  }


  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  Simulator::Stop (Seconds (endTime));
  Simulator::Run ();



  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
  {
    Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
    if(t.sourcePort==654){
      continue;
    }

    rxPacketsum += i->second.rxPackets;
    txPacketsum += i->second.txPackets;
    txBytessum += i->second.txBytes;
    rxBytessum += i->second.rxBytes;
    Delaysum += i->second.delaySum.GetSeconds();
    lostPacketssum += i->second.lostPackets;
    
    if(txTimeFirst == 0)
    {
      txTimeFirst = i->second.timeFirstTxPacket.GetSeconds();
    }
    
    rxTimeLast = i->second.timeLastRxPacket.GetSeconds();
  }
  std::string flowFileName ("taskA_wired_topology");
  monitor->SerializeToXmlFile ((flowFileName + ".flowmon").c_str(), false, false);

  double timeDiff = (rxTimeLast - txTimeFirst);
  std::cout << "\n\n";
  std::cout << "Total Tx Packets: " << txPacketsum << "\n";
  std::cout << "Total Rx Packets: " << rxPacketsum << "\n";
  std::cout << "Total Packets Lost: " << lostPacketssum << "\n";
  std::cout << "Throughput: " << ((rxBytessum * 8.0) / timeDiff)/1024<<" Kbps"<<"\n";
  std::cout << "Packets Delivery Ratio: " << (double)((rxPacketsum * 100.0) /txPacketsum) << "%" << "\n";
  std::cout << "Packets Loss Ratio: " << (double)((lostPacketssum * 100.0) /txPacketsum) << "%" << "\n";
  std::cout << "Avg End to End Delay: " << Delaysum/rxPacketsum << "\n";

  std::ofstream myfile;
  myfile.open ("Plot-Data.txt", std::ios::app);
  myfile
        <<nNodes<<" "
        // <<nFlows<<" "
        // <<nPPS<<" "
        <<((rxBytessum * 8.0) / timeDiff)/1024<<" "
        << Delaysum/rxPacketsum <<" "
        <<(double)((rxPacketsum * 100) /txPacketsum)<<" "
        <<(double)((lostPacketssum * 100) /txPacketsum)
        <<std::endl;
  myfile.close();

  Simulator::Destroy ();
  return 0;
}
