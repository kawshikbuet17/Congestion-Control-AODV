/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2020 Ritsumeikan University, Shiga, Japan
 *
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
 *
 * Author: Alberto Gallegos Ramonet <ramonet@fc.ritsumei.ac.jp>
 */


#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/mobility-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/propagation-module.h"
#include "ns3/sixlowpan-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"


#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"



using namespace ns3;


static void dataSentMacConfirm (McpsDataConfirmParams params)
{
  // In the case of transmissions with the Ack flag activated, the transaction is only
  // successful if the Ack was received.
  if (params.m_status == LrWpanMcpsDataConfirmStatus::IEEE_802_15_4_SUCCESS)
    {
      ;//NS_LOG_UNCOND ("**********" << Simulator::Now ().As (Time::S) << " | Transmission successfully sent");
    }
}

// void PrintMyData(std::string s){
//   std::cout<<s<<std::endl;
// }


int main (int argc, char** argv)
{

  bool verbose = false;
  int nNodes = 3;
  int nFlows = 3;
  int nPPS = 100;
  int txRange = 3;

  CommandLine cmd (__FILE__);
  cmd.AddValue("nNodes", "the number of nodes", nNodes);
  cmd.AddValue("nFlows", "the number of flows", nFlows);
  cmd.AddValue("nPPS", "the number of packets per second", nPPS);
  cmd.AddValue("txRange", "coverage area", txRange);
  cmd.AddValue ("verbose", "turn on log components", verbose);
  cmd.Parse (argc, argv);

  if (verbose)
    {
      LogComponentEnableAll (LOG_PREFIX_TIME);
      LogComponentEnableAll (LOG_PREFIX_FUNC);
      LogComponentEnable ("LrWpanMac", LOG_LEVEL_INFO);
      LogComponentEnable ("LrWpanCsmaCa", LOG_LEVEL_INFO);
      LogComponentEnable ("LrWpanHelper", LOG_LEVEL_ALL);
      LogComponentEnable ("Ping6Application", LOG_LEVEL_INFO);
    }


  NodeContainer nodes;
  nodes.Create (nNodes);

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (3),
                                 "DeltaY", DoubleValue (3),
                                 "GridWidth", UintegerValue (4),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  Config::SetDefault("ns3::RangePropagationLossModel::MaxRange", DoubleValue(txRange));
  Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel>();
  Ptr<RangePropagationLossModel> propModel = CreateObject<RangePropagationLossModel>();
  Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel>();
  channel->AddPropagationLossModel(propModel);
  channel->SetPropagationDelayModel(delayModel);

  LrWpanHelper lrWpanHelper;
  lrWpanHelper.SetChannel(channel);
  // Add and install the LrWpanNetDevice for each node
  NetDeviceContainer lrwpanDevices = lrWpanHelper.Install (nodes);
  lrWpanHelper.AssociateToPan (lrwpanDevices, 0);

  Ptr<LrWpanNetDevice> dev[nNodes];
  McpsDataConfirmCallback cb1;
  for(int i=0; i<nNodes; i++){
        dev[i] = lrwpanDevices.Get (i)->GetObject<LrWpanNetDevice> ();
        cb1 = MakeCallback (&dataSentMacConfirm);
        dev[i]->GetMac ()->SetMcpsDataConfirmCallback (cb1);
  }


  InternetStackHelper stack;
  stack.Install (nodes);

  SixLowPanHelper sixlowpan;
  NetDeviceContainer devices = sixlowpan.Install (lrwpanDevices);

  Ipv6AddressHelper ipv6;
  ipv6.SetBase (Ipv6Address ("2001:f00d::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer deviceInterfaces;
  deviceInterfaces = ipv6.Assign (devices);
  deviceInterfaces.SetForwarding (0, true);
  deviceInterfaces.SetDefaultRouteInAllNodes (0);

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

  srand(time(0));
  int startTime = 1;
  int endTime = 30;
  for(int i=0; i<nFlows; i++){
    int server = rand()%nNodes;
    int client = rand()%nNodes;
    int port = 10+i;
    while(server==client){
      client = rand()%nNodes;
    }

    UdpEchoServerHelper echoServer (port);
    ApplicationContainer serverApps = echoServer.Install (nodes.Get (server));
    serverApps.Start (Seconds (startTime));

    UdpEchoClientHelper echoClient (deviceInterfaces.GetAddress(server,0), port);
    echoClient.SetAttribute ("MaxPackets", UintegerValue (0));
    echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0/nPPS)));
    echoClient.SetAttribute ("PacketSize", UintegerValue (60));

    ApplicationContainer clientApps = echoClient.Install (nodes.Get (client));
    clientApps.Start (Seconds (startTime+1));
  }

  Simulator::Stop (Seconds (endTime));

  Simulator::Run ();

  Ptr<Ipv6FlowClassifier> classifier = DynamicCast<Ipv6FlowClassifier> (flowmon.GetClassifier6 ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
  {
    Ipv6FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);

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

  std::string flowFileName ("taskA-lrwpan-ipv6");
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
        // <<nNodes<<" "
        // <<nFlows<<" "
        // <<nPPS<<" "
        <<txRange<<" "
        <<((rxBytessum * 8.0) / timeDiff)/1024<<" "
        << Delaysum/rxPacketsum <<" "
        <<(double)((rxPacketsum * 100) /txPacketsum)<<" "
        <<(double)((lostPacketssum * 100) /txPacketsum)
        <<std::endl;
  myfile.close();

  Simulator::Destroy ();
  return 0;

}


