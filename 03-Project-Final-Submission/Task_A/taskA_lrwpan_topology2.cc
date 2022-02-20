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
      NS_LOG_UNCOND ("**********" << Simulator::Now ().As (Time::S) << " | Transmission successfully sent");
    }
}


int main (int argc, char** argv)
{

  bool verbose = false;
  int nNodes = 3;

  CommandLine cmd (__FILE__);
  cmd.AddValue("nNodes", "the number of nodes", nNodes);
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
                                 "DeltaX", DoubleValue (20),
                                 "DeltaY", DoubleValue (20),
                                 "GridWidth", UintegerValue (2),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  LrWpanHelper lrWpanHelper;
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

  // Fake PAN association, coordinator assignment, short address assignment and initialization
  // of beacon-enabled mode in 802.15.4-2011.
  // This is needed because the lr-wpan module does not provide (yet)
  // a full PAN association procedure.

  // AssociateToBeaconPan (devices, PAN ID, Coordinator Address, Beacon Order, Superframe Order)

  // Must be careful not setting the beacon order (BO) and the superframe order (SO) too far apart
  // or the ping reply (ICMPV6 echo reply) can time out during the inactive period of the superframe.
  // A full time table of the BO/SO time equivalence can be found at the end of this document.
  // The current configuration is BO = 14, SO = 13 :

  //           Contention Access Period (CAP)                           Inactive
  //              (125.82912 secs)                                     (125.82088)
  //   |---------------------------------------------|-------------------------------------------|
  // Beacon                                                                                   Beacon
  //                            Beacon Interval = 251.65 secs
  //   |-----------------------------------------------------------------------------------------|

  //lrWpanHelper.AssociateToBeaconPan (lrwpanDevices, 0, Mac16Address ("00:01"), 14,13);


  InternetStackHelper stack;
  stack.Install (nodes);

  SixLowPanHelper sixlowpan;
  NetDeviceContainer devices = sixlowpan.Install (lrwpanDevices);
  for (uint32_t i = 0; i < devices.GetN (); i++)
    {
      Ptr<NetDevice> dev = devices.Get (i);
      dev->SetAttribute ("UseMeshUnder", BooleanValue (true));
      dev->SetAttribute ("MeshUnderRadius", UintegerValue (10));
    }

  Ipv6AddressHelper ipv6;
  ipv6.SetBase (Ipv6Address ("2001:f00d::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer deviceInterfaces;
  deviceInterfaces = ipv6.Assign (devices);
  deviceInterfaces.SetForwarding (0, true);
  deviceInterfaces.SetDefaultRouteInAllNodes (0);

  uint32_t rxPacketsum = 0;
  double Delaysum = 0; 
  double rxTimeSum = 0, txTimeSum = 0;
  uint32_t txPacketsum = 0;
  uint32_t txBytessum = 0;
  uint32_t rxBytessum = 0;
  uint32_t txTimeFirst = 0;
  uint32_t rxTimeLast = 0;
  uint32_t lostPacketssum = 0;

  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (30.0));

  UdpEchoClientHelper echoClient (deviceInterfaces.GetAddress(0,0), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (10));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (nNodes/2));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (30.0));

  Simulator::Stop (Seconds (30));

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
    lostPacketssum += i->second.lostPackets;
    Delaysum += i->second.delaySum.GetSeconds();
  }

  std::string flowFileName ("lr-wpan2");
  monitor->SerializeToXmlFile ((flowFileName + ".flowmon").c_str(), false, false);

    uint64_t timeDiff = (rxTimeLast - txTimeFirst);
  double timeDiff2 = (rxTimeSum - txTimeSum) / rxPacketsum;

  std::cout << "\n\n";
  std::cout << "Total Tx Packets: " << txPacketsum << "\n";
  std::cout << "Total Rx Packets: " << rxPacketsum << "\n";
  std::cout << "Total Packets Lost: " << (txPacketsum - rxPacketsum) << "\n";
  std::cout << "Average Round trip time of Packet: " << timeDiff2 << "\n";
  std::cout << "Throughput: " << ((rxBytessum * 8.0) / timeDiff)/1024<<" Kbps"<<"\n";
  std::cout << "Packets Loss Ratio: " << (((txPacketsum - rxPacketsum) * 100) /txPacketsum) << "%" << "\n";
  std::cout << "Packets Delivery Ratio: " << ((rxPacketsum * 100) /txPacketsum) << "%" << "\n";
  std::cout << "Avg End to End Delay: " << Delaysum/rxPacketsum << "\n";
  
  Simulator::Destroy ();

}

// BO/SO values to time equivalence
// These times are only valid for a 250kbps O-QPSK modulation,
// times differ with other modulation configurations.

// +------------------------+
// | BO/SO |  Time (secs)   |
// +------------------------+
// |   0   | 0.01536 secs   |
// |   1   | 0.03072 secs   |
// |   2   | 0.06144 secs   |
// |   3   | 0.12288 secs   |
// |   4   | 0.24576 secs   |
// |   5   | 0.49152 secs   |
// |   6   | 0.98304 secs   |
// |   7   | 1.96608 secs   |
// |   8   | 3.93216 secs   |
// |   9   | 7.86432 secs   |
// |   10  | 15.72864 secs  |
// |   11  | 31.45728 secs  |
// |   12  | 62.91456 secs  |
// |   13  | 125.82912 secs |
// |   14  | 251.65 secs    |
// +------------------------+
