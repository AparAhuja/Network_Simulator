#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/stats-module.h"
#include "iostream"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Part3TEST");

class MyApp : public Application
{
public:
  MyApp ();
  virtual ~MyApp ();

  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId (void);
  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp ()
{
  m_socket = 0;
}

/* static */
TypeId MyApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("MyApp")
    .SetParent<Application> ()
    .SetGroupName ("Tutorial")
    .AddConstructor<MyApp> ()
    ;
  return tid;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  if (InetSocketAddress::IsMatchingType (m_peer))
    {
      m_socket->Bind ();
    }
  else
    {
      m_socket->Bind6 ();
    }
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}
static int noOfSentPkts = 0;

void
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);
  noOfSentPkts++;
  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  //  NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "," << oldCwnd << "," << newCwnd << std::endl;
}

static int noOfDroppedPkts = 0;
static int drop1=0,drop2=0,drop3=0;
static void RxDrop (Ptr<PcapFileWrapper> file, Ptr<const Packet> p)
{  
  //  NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
  file->Write (Simulator::Now (), p);
  std::string s = p->ToString();
  if(s.find("> 8080") < s.size())
    drop1++;
  if(s.find("> 8081") < s.size())
    drop2++;
  if(s.find("> 8082") < s.size())
    drop3++;
  noOfDroppedPkts++;
}

static std::string config;
void analyzeProtocol (std::string s)
{
  noOfSentPkts = 0;
  noOfDroppedPkts = 0;
  Packet::EnablePrinting();
  NodeContainer n1, n2;
  n1.Create (2); // N1 and N3
  n2.Create (1); // N2
  n2.Add (n1.Get (1)); // add N3 to n2
  
  if(s == "TcpNewReno")
    {Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::" + s));
    config = "config1/";}
  else if(s == "TcpNewRenoCSE")
    {Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::" + s));
    config = "config3/";}
  
  PointToPointHelper pointToPoint1, pointToPoint2;
  pointToPoint1.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint1.SetChannelAttribute ("Delay", StringValue ("3ms"));
  
  pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("9Mbps"));
  pointToPoint2.SetChannelAttribute ("Delay", StringValue ("3ms"));

  NetDeviceContainer devices1, devices2;
  devices1 = pointToPoint1.Install (n1);
  devices2 = pointToPoint2.Install (n2);

  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetAttribute ("ErrorRate", DoubleValue (0.00001));
  Ptr<RateErrorModel> em1 = CreateObject<RateErrorModel> ();
  em1->SetAttribute ("ErrorRate", DoubleValue (0.00001));
  devices1.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
  devices2.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em1));

  InternetStackHelper stack;
  stack.Install (n1);
  stack.Install (n2.Get (0));

  uint16_t sinkPort = 8080, sinkPort2 = 8081, sinkPort3 = 8082;
  Address sinkAddress, sinkAddress2, sinkAddress3;
  Address anyAddress, anyAddress2, anyAddress3;
  std::string probeType;
  std::string tracePath;
  
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces1 = address.Assign (devices1);
  address.SetBase ("11.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces2 = address.Assign (devices2);
  
  sinkAddress = InetSocketAddress (interfaces1.GetAddress (1), sinkPort);
  anyAddress = InetSocketAddress (Ipv4Address::GetAny (), sinkPort);
  
  sinkAddress3 = InetSocketAddress (interfaces1.GetAddress (1), sinkPort3);
  anyAddress3 = InetSocketAddress (Ipv4Address::GetAny (), sinkPort3);
  
  sinkAddress2 = InetSocketAddress (interfaces2.GetAddress (1), sinkPort2);
  anyAddress2 = InetSocketAddress (Ipv4Address::GetAny (), sinkPort2);
  
  probeType = "ns3::Ipv4PacketProbe";
  tracePath = "/NodeList/*/$ns3::Ipv4L3Protocol/Tx";
  
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", anyAddress), packetSinkHelper2 ("ns3::TcpSocketFactory", anyAddress2), packetSinkHelper3 ("ns3::TcpSocketFactory", anyAddress3);
  ApplicationContainer sinkApps = packetSinkHelper.Install (n1.Get (1));
  ApplicationContainer sinkApps2 = packetSinkHelper2.Install (n2.Get (1));
  ApplicationContainer sinkApps3 = packetSinkHelper3.Install (n1.Get (1));
  sinkApps.Start (Seconds (1.));
  sinkApps.Stop (Seconds (20.));
  sinkApps2.Start (Seconds (15.));
  sinkApps2.Stop (Seconds (30.));
  sinkApps3.Start (Seconds (5.));
  sinkApps3.Stop (Seconds (25.));
  Ptr<Socket> ns3TcpSocket, ns3TcpSocket2, ns3TcpSocket3;

  if (s == "TcpHybrid"){
    // TypeId tid1 = TypeId::LookupByName ("ns3::TcpNewReno"), tid2 = TypeId::LookupByName ("ns3::TcpNewRenoCSE");
    std::stringstream nodeId1, nodeId2;
    nodeId1 << n1.Get (0)->GetId ();
    nodeId2 << n2.Get (0)->GetId ();
    std::string specificNode = "/NodeList/" + nodeId1.str () + "/$ns3::TcpL4Protocol/SocketType";
    Config::Set (specificNode, StringValue ("ns3::TcpNewReno"));
    ns3TcpSocket = Socket::CreateSocket (n1.Get (0), TcpSocketFactory::GetTypeId ());
    ns3TcpSocket2 = Socket::CreateSocket (n1.Get (0), TcpSocketFactory::GetTypeId ());
    std::string specificNode2 = "/NodeList/" + nodeId2.str () + "/$ns3::TcpL4Protocol/SocketType";
    Config::Set (specificNode2, StringValue ("ns3::TcpNewRenoCSE"));
    ns3TcpSocket3 = Socket::CreateSocket (n2.Get (0), TcpSocketFactory::GetTypeId ());
    config = "config2/";
  }
  else{
    ns3TcpSocket = Socket::CreateSocket (n1.Get (0), TcpSocketFactory::GetTypeId ());
    ns3TcpSocket2 = Socket::CreateSocket (n1.Get (0), TcpSocketFactory::GetTypeId ());
    ns3TcpSocket3 = Socket::CreateSocket (n2.Get (0), TcpSocketFactory::GetTypeId ());
  }

  Ptr<MyApp> app = CreateObject<MyApp> ();
  app->Setup (ns3TcpSocket, sinkAddress, 3000, 999999, DataRate ("1500Kbps"));
  n1.Get (0)->AddApplication (app);
  app->SetStartTime (Seconds (1.));
  app->SetStopTime (Seconds (20.));
  
  Ptr<MyApp> app2 = CreateObject<MyApp> ();
  app2->Setup (ns3TcpSocket2, sinkAddress3, 3000, 999999, DataRate ("1500Kbps"));
  n1.Get (0)->AddApplication (app2);
  app2->SetStartTime (Seconds (5.));
  app2->SetStopTime (Seconds (25.));
  
  Ptr<MyApp> app3 = CreateObject<MyApp> ();
  app3->Setup (ns3TcpSocket3, sinkAddress2, 3000, 999999, DataRate ("1500Kbps"));
  n2.Get (0)->AddApplication (app3);
  app3->SetStartTime (Seconds (15.));
  app3->SetStopTime (Seconds (30.));

  AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream ("part3/" + config + s + "_N1.csv");
  ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));

  AsciiTraceHelper asciiTraceHelper2;
  Ptr<OutputStreamWrapper> stream2 = asciiTraceHelper2.CreateFileStream ("part3/"+ config + s+"_N2.csv");
  ns3TcpSocket2->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream2));

  AsciiTraceHelper asciiTraceHelper3;
  Ptr<OutputStreamWrapper> stream3 = asciiTraceHelper3.CreateFileStream ("part3/"+ config + s+"_N3.csv");
  ns3TcpSocket3->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream3));

  PcapHelper pcapHelper;
  Ptr<PcapFileWrapper> file = pcapHelper.CreateFile ("part3/" + config + s+"_Part3.pcap", std::ios::out, PcapHelper::DLT_PPP);
  devices1.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop, file));
  devices2.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop, file));

  Simulator::Stop (Seconds (30));
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_UNCOND ("Testing " << s << " protocol.");
  NS_LOG_UNCOND ("Number of Packets dropped by connection 1 = " << drop1);
  NS_LOG_UNCOND ("Number of Packets dropped by connection 2 = " << drop3);
  NS_LOG_UNCOND ("Number of Packets dropped by connection 3 = " << drop2);
  NS_LOG_UNCOND ("Total Number of Packets dropped = " << noOfDroppedPkts);
  NS_LOG_UNCOND ("Total Number of Packets sent = " << noOfSentPkts << "\n");
}


int main (int argc, char *argv[])
{
   system("[ -d part3 ] || mkdir part3");
   system("[ -d part3/config1 ] || mkdir part3/config1");
   system("[ -d part3/config2 ] || mkdir part3/config2");
   system("[ -d part3/config3 ] || mkdir part3/config3");
   
   int cmdarg = 3;
   CommandLine cmd;
   cmd.AddValue ("config", "Configuration", cmdarg);
   cmd.Parse (argc, argv);
   if(cmdarg == 1)
   analyzeProtocol("TcpNewReno");
   if(cmdarg == 2)
   analyzeProtocol("TcpHybrid");
   if(cmdarg == 3)
   analyzeProtocol("TcpNewRenoCSE");
   return 0;
}

