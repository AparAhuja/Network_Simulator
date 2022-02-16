#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/stats-module.h"
#include "iostream"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Part2");

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

static void RxDrop (Ptr<PcapFileWrapper> file, Ptr<const Packet> p)
{
  //  NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
  file->Write (Simulator::Now (), p);
  noOfDroppedPkts++;
}


void channalDataRate (std::string s)
{
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));

  NodeContainer nodes;
  nodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue (s));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("3ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetAttribute ("ErrorRate", DoubleValue (0.00001));
  devices.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

  InternetStackHelper stack;
  stack.Install (nodes);

  uint16_t sinkPort = 8080;
  Address sinkAddress;
  Address anyAddress;
  std::string probeType;
  std::string tracePath;
  
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign (devices);
  sinkAddress = InetSocketAddress (interfaces.GetAddress (1), sinkPort);
  anyAddress = InetSocketAddress (Ipv4Address::GetAny (), sinkPort);
  probeType = "ns3::Ipv4PacketProbe";
  tracePath = "/NodeList/*/$ns3::Ipv4L3Protocol/Tx";
  
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", anyAddress);
  ApplicationContainer sinkApps = packetSinkHelper.Install (nodes.Get (1));
  sinkApps.Start (Seconds (1.));
  sinkApps.Stop (Seconds (30.));

  Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());

  Ptr<MyApp> app = CreateObject<MyApp> ();
  app->Setup (ns3TcpSocket, sinkAddress, 3000, 999999, DataRate ("2Mbps"));
  nodes.Get (0)->AddApplication (app);
  app->SetStartTime (Seconds (1.));
  app->SetStopTime (Seconds (30.));

  AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream ("part2/channel/ChannelRate_" + s + ".csv");
  ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));

  PcapHelper pcapHelper;
  Ptr<PcapFileWrapper> file = pcapHelper.CreateFile ("part2/channel/ChannelRate_" + s + ".pcap", std::ios::out, PcapHelper::DLT_PPP);
  devices.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop, file));

  Simulator::Stop (Seconds (30));
  Simulator::Run ();
  Simulator::Destroy ();
}


void applicationDataRate (std::string s)
{
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));

  NodeContainer nodes;
  nodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("6Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("3ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetAttribute ("ErrorRate", DoubleValue (0.00001));
  devices.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

  InternetStackHelper stack;
  stack.Install (nodes);

  uint16_t sinkPort = 8080;
  Address sinkAddress;
  Address anyAddress;
  std::string probeType;
  std::string tracePath;
  
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign (devices);
  sinkAddress = InetSocketAddress (interfaces.GetAddress (1), sinkPort);
  anyAddress = InetSocketAddress (Ipv4Address::GetAny (), sinkPort);
  probeType = "ns3::Ipv4PacketProbe";
  tracePath = "/NodeList/*/$ns3::Ipv4L3Protocol/Tx";
  
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", anyAddress);
  ApplicationContainer sinkApps = packetSinkHelper.Install (nodes.Get (1));
  sinkApps.Start (Seconds (1.));
  sinkApps.Stop (Seconds (30.));

  Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());

  Ptr<MyApp> app = CreateObject<MyApp> ();
  app->Setup (ns3TcpSocket, sinkAddress, 3000, 999999, DataRate (s));
  nodes.Get (0)->AddApplication (app);
  app->SetStartTime (Seconds (1.));
  app->SetStopTime (Seconds (30.));

  AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream ("part2/app/ApplicationRate_" + s + ".csv");
  ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));

  PcapHelper pcapHelper;
  Ptr<PcapFileWrapper> file = pcapHelper.CreateFile ("part2/app/ApplicationRate_" + s + ".pcap", std::ios::out, PcapHelper::DLT_PPP);
  devices.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop, file));

  Simulator::Stop (Seconds (30));
  Simulator::Run ();
  Simulator::Destroy ();
}


int main (int argc, char *argv[])
{
    system("[ -d part2 ] ||mkdir part2");
    system("[ -d part2/app ] ||mkdir part2/app");
    system("[ -d part2/channel ] ||mkdir part2/channel");
    int cmdarg = 1;
    CommandLine cmd;
    cmd.AddValue ("useRate", "useRate", cmdarg);
    cmd.Parse (argc, argv);
   
    std::string s;
    if(cmdarg == 1)
    {channalDataRate("2Mbps");
    s = "Channel Data Rate = 2 Mbps";}
    if(cmdarg == 2)
    {channalDataRate("4Mbps");
    s = "Channel Data Rate = 4 Mbps";}
    if(cmdarg == 3)
    {channalDataRate("10Mbps");
    s = "Channel Data Rate = 10 Mbps";}
    if(cmdarg == 4)
    {channalDataRate("20Mbps");
    s = "Channel Data Rate = 20 Mbps";}
    if(cmdarg == 5)
    {channalDataRate("50Mbps");
    s = "Channel Data Rate = 50 Mbps";}
    if(cmdarg == 6)
    {applicationDataRate("500Kbps");
    s = "Application Data Rate = 0.5 Mbps";}
    if(cmdarg == 7)
    {applicationDataRate("1Mbps");
    s = "Application Data Rate = 1 Mbps";}
    if(cmdarg == 8)
    {applicationDataRate("2Mbps");
    s = "Application Data Rate = 2 Mbps";}
    if(cmdarg == 9)
    {applicationDataRate("4Mbps");
    s = "Application Data Rate = 4 Mbps";}
    if(cmdarg == 10)
    {applicationDataRate("10Mbps");
    s = "Application Data Rate = 10 Mbps";}
    
    NS_LOG_UNCOND ("Testing with " << s << ".");
    NS_LOG_UNCOND ("Number of Packets dropped = " << noOfDroppedPkts);
    NS_LOG_UNCOND ("Number of Packets sent = " << noOfSentPkts << "\n");
  
    return 0;
}
