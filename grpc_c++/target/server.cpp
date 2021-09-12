//
// Created by gs on 2021/9/12.
//

#include "../header/test.grpc.pb.h"
#include <iostream>
#include <string>
#include <grpcpp/grpcpp.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::ServerReaderWriter;
using grpc_test::Greeter;
using grpc_test::HelloReply;
using grpc_test::HelloRequest;


//继承Greeter类的Service类,这个类是继承了grpc::Service类的子类
class GreeterServiceImpl final : public Greeter::Service{

    //调用通过request获取请求信息，通过response返回信息。
    Status SayHello(ServerContext* context, const HelloRequest* request, HelloReply* response) override{
        std::string prefix = "hello";
        //这里就是返回值了
        response->set_message(prefix + request->name());
        return Status::OK;
    }


    //这个用来打印日志使用。
    void print (std::string temp){
        char *a = new char[4];
        std::cout<<temp<<a<<std::endl;
    }
    //双向流式rpc服务
    Status SayHelloAll(ServerContext* context, ServerReaderWriter< HelloReply, HelloRequest>* stream) override{
        grpc_test::HelloReply rep;
        grpc_test::HelloRequest req;
        std::string str;
        char *a = new char[4];
        int count = 0;
        while(stream->Read(&req)){
            str = req.name();
            sprintf(a,"%3d\n",count++);
            str += a;
            print(str);
            rep.set_message(str);
            stream->Write(rep);
        }
        return Status::OK;
    }

public:
    ~GreeterServiceImpl() override= default;
    GreeterServiceImpl() = default;
    bool Run (){
        std::string address = "localhost:4306";
        //不知道
        grpc::EnableDefaultHealthCheckService(true);
        //这个就是相当于添加了一些启动服务的options
        ServerBuilder builder;
        builder.AddListeningPort(address, grpc::InsecureServerCredentials());
        builder.RegisterService(this);
        //启动服务
        std::unique_ptr<Server> server(builder.BuildAndStart());
        std::cout<<"start successful listening on "<<address<<std::endl;
        //保持。
        server->Wait();
        return true;
    }
};

int main (){
    GreeterServiceImpl service;
    service.Run();
}


