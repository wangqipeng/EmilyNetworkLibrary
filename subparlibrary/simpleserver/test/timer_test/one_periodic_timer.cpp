#include "TcpServer.h"
#include "TcpSocket.h"
#include "InetAddress.h"
#include "EventScheduler.h"
#include "Log.h"
#include <string>
using namespace std;
using namespace ACache;

void print1()
{
    cout<<"<<<<>>>>>>hello, world, i'm the first print!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
}

void print2()
{

    cout<<"<<<<>>>>>>hello, world, i'm the second print!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
}

void print3()
{

    cout<<"<<<<>>>>>>hello, world, i'm the third print!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
}

void print4()
{

    cout<<"<<<<>>>>>>hello, world, i'm the forth print!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
}

int main(int argc, char **argv)
{ 
    FILELog::ReportingLevel() = FILELog::FromString(argv[3] ? argv[3]:"DEBUG");
    string server_name(argv[1]);
    FILE_LOG(logINFO)<<"server_name("<<argv[1]<<")"<<"port:"<<argv[2];
    EventScheduler *scheduler = new EventScheduler;
    //scheduler->RunCallbackPeriodic(13000, print3);
    scheduler->RunCallbackPeriodic(5000, print1);
    //scheduler->RunCallbackPeriodic(18000, print4);
    //scheduler->RunCallbackAfter(10000, print2);
	
    FILE_LOG(logINFO)<<__FILE__<<" "<<__LINE__<<" ";
    scheduler->GetQueuePtr()->GetQueue().PrintQueue();
    TcpServer server(server_name, argv[2], scheduler);
    FILE_LOG(logINFO)<<"TcpServer server("<<argv[1]<<", "<<argv[2]<<")";
    server.ListenToThePort();
    FILE_LOG(logINFO)<<"file: "<<__FILE__<<"line: "<<__LINE__;
    server.EventDrive();
    
}
