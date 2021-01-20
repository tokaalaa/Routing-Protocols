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

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

#define NODE_NUMBER	6

NS_LOG_COMPONENT_DEFINE ("My topology");

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

// Construct toplogy
  NodeContainer nodes;
  nodes.Create (NODE_NUMBER);
  NodeContainer n01 = NodeContainer (nodes.Get (0), nodes.Get (1));
  NodeContainer n02 = NodeContainer (nodes.Get (0), nodes.Get (2));
  NodeContainer n12 = NodeContainer (nodes.Get (1), nodes.Get (2));
  NodeContainer n13 = NodeContainer (nodes.Get (1), nodes.Get (3));
  NodeContainer n14 = NodeContainer (nodes.Get (1), nodes.Get (4));
  NodeContainer n24 = NodeContainer (nodes.Get (2), nodes.Get (4));
  NodeContainer n34 = NodeContainer (nodes.Get (3), nodes.Get (4));
  NodeContainer n45 = NodeContainer (nodes.Get (4), nodes.Get (5));

// Install protocol stacks on our nodes
  InternetStackHelper stack;
  stack.Install (nodes);

  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

//create devices for nodes
  NetDeviceContainer d01 = p2p.Install (n01);
  NetDeviceContainer d02 = p2p.Install (n02);
  NetDeviceContainer d12 = p2p.Install (n12);
  NetDeviceContainer d13 = p2p.Install (n13);
  NetDeviceContainer d14 = p2p.Install (n14);
  NetDeviceContainer d24 = p2p.Install (n24);
  NetDeviceContainer d34 = p2p.Install (n34);
  NetDeviceContainer d45 = p2p.Install (n45);

// Associate the devices on our nodes with IP addresses
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i01 = ipv4.Assign (d01);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i02 = ipv4.Assign (d02);

  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i12 = ipv4.Assign (d12);

  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i13 = ipv4.Assign (d13);

  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer i14 = ipv4.Assign (d14);

  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer i24 = ipv4.Assign (d24);

  ipv4.SetBase ("10.1.7.0", "255.255.255.0");
  Ipv4InterfaceContainer i34 = ipv4.Assign (d34);

  ipv4.SetBase ("10.1.8.0", "255.255.255.0");
  Ipv4InterfaceContainer i45 = ipv4.Assign (d45);

  // Establish Routes and print them.
  // print table in file
  Ipv4GlobalRoutingHelper table;
  table.PopulateRoutingTables ();

  // Application layer to test topology.
  UdpEchoServerHelper echoServer (9);// takes port number of the server

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (5));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (i45.GetAddress (1), 9);// takes the ip address & the port number of the server.
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));// the maximum number of packets we allow it to send during the simulati
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));// tells the client how long to wait between packets.
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));// tells the client how large its packet payloads should be.

  ApplicationContainer clientApps0 = echoClient.Install (nodes.Get (0));
  clientApps0.Start (Seconds (2.0));
  clientApps0.Stop (Seconds (10.0));

/**  ApplicationContainer clientApps1 = echoClient.Install (nodes.Get (1));
  clientApps1.Start (Seconds (3.0));
  clientApps1.Stop (Seconds (10.0));

  ApplicationContainer clientApps2 = echoClient.Install (nodes.Get (2));
  clientApps2.Start (Seconds (4.0));
  clientApps2.Stop (Seconds (10.0));

  ApplicationContainer clientApps3 = echoClient.Install (nodes.Get (3));
  clientApps3.Start (Seconds (5.0));
  clientApps3.Stop (Seconds (10.0));

  ApplicationContainer clientApps4 = echoClient.Install (nodes.Get (4));
  clientApps4.Start (Seconds (6.0));
  clientApps4.Stop (Seconds (10.0));*/

  Ptr<OutputStreamWrapper> routingStreamOut = Create<OutputStreamWrapper>("routing-tables.routes" , std::ios::out);
  table.PrintRoutingTableAllAt(Seconds(0) , routingStreamOut);

 // Animation of our system using NetAnim.
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  AnimationInterface anim ("topology.xml");

     // Set position for nodes

  Ptr<ConstantPositionMobilityModel> s1 = nodes.Get (0)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s2 = nodes.Get (1)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s3 = nodes.Get (2)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s4 = nodes.Get (3)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s5 = nodes.Get (4)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s6 = nodes.Get (5)->GetObject<ConstantPositionMobilityModel> ();

  s1->SetPosition (Vector ( 0.0, 10.0, 0  ));
  s2->SetPosition (Vector ( 20.0, 20.0, 0  ));
  s3->SetPosition (Vector ( 20.0, 0.0, 0  ));
  s4->SetPosition (Vector ( 30.0, 20.0, 0  ));
  s5->SetPosition (Vector ( 30.0, 0.0, 0  ));
  s6->SetPosition (Vector ( 40.0, 0.0, 0  ));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

