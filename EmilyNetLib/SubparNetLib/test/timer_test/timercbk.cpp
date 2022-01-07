#include "TcpServer.h"
#include "TcpSocket.h"
#include "InetAddress.h"
#include "EventScheduler.h"
#include "Time.h"
#include "Log.h"
#include <string>
#include <iostream>
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
	FILE_LOG(logINFO)<<__FILE__<<" "<<__LINE__<<" now : "<<Time::GetFormatNowTime(Time::NowTimeUs());
    scheduler->RegisterPeriodicTimer(8000000, print2);
    scheduler->RegisterPeriodicTimer(5000000, print1);
    scheduler->RegisterPeriodicTimer(3000000, print4);
    scheduler->RegisterOnceTimer(10000000, print3);
	
    FILE_LOG(logINFO)<<__FILE__<<" "<<__LINE__<<" ";
    //scheduler->GetQueuePtr()->GetMonitor().PrintQueue();
    TcpServer server(server_name, argv[2], scheduler);
    FILE_LOG(logINFO)<<"TcpServer server("<<argv[1]<<", "<<argv[2]<<")";
    server.ListenToThePort();
    FILE_LOG(logINFO)<<"file: "<<__FILE__<<"line: "<<__LINE__;
    server.EventDrive();
    
}
