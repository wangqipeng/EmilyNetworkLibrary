#include "EchoServer.h"
#include "Log.h"
#include <string>
using namespace std;
using namespace ACache;


int main(int argc, char **argv)
{ 
    FILELog::ReportingLevel() = FILELog::FromString(argv[3] ? argv[3]:"DEBUG");
    string server_name(argv[1]);
    FILE_LOG(logINFO)<<"server_name("<<argv[1]<<")"<<"port:"<<argv[2];
    FILE_LOG(logINFO)<<__FILE__<<" "<<__LINE__<<" ";
    EventScheduler* scheduler = new EventScheduler; 

    EchoServer server(server_name, argv[2], scheduler);
    FILE_LOG(logINFO)<<"TcpServer server("<<argv[1]<<", "<<argv[2]<<")";
    FILE_LOG(logINFO)<<"file: "<<__FILE__<<"line: "<<__LINE__;
    server.start();
    server.EventDrive();
}
