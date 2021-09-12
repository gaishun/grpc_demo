//
// Created by gs on 2021/9/12.
//
#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <thread>
#include "../header/test.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using grpc_test::Greeter;
using grpc_test::HelloRequest;
using grpc_test::HelloReply;

//自己创建一个类，里面将grpc文件里面的Service_name的Stub作为成员变量进行使用。
//这个stud现在理解就是作为整个client的一个载体，放在任何一个类中都可以。
class GreeterClient {
    std::unique_ptr<Greeter::Stub> _stub;
public:
    //用一个channel来构建这个客户端，也就是用一个连接来实例化这个类。
    GreeterClient(std::shared_ptr<grpc::Channel> channel):
    _stub(Greeter::NewStub(channel)){};
    
    std::string SayHello (const std::string &user){
        grpc_test::HelloRequest request;
        request.set_name(user);
        grpc_test::HelloReply rep;
        //这些ctx应该有什么作用，但是咱也不知道，等查一下官方的文档。
        grpc::ClientContext ctx;

        Status status = _stub->SayHello(&ctx, request, &rep);

        if (status.ok()){
            return rep.message();
        }else {
            std::cout<<"SayHello err"<<std::endl;
            return "RPC failed";
        }
    }

    std::string SayHelloAll (){
        grpc::ClientContext ctx;
        std::shared_ptr<grpc::ClientReaderWriter<grpc_test::HelloRequest,grpc_test::HelloReply >> stream(
                _stub->SayHelloAll(&ctx)
                );
        //用一个线程来执行读操作，没有新内容的时候这个线程是一直阻塞的。
        std::thread Read([stream](){
            grpc_test::HelloReply rep;
            while(stream->Read(&rep)){
                std::cout<<rep.message()<<std::endl;
            }
        });
        grpc_test::HelloRequest req;
        req.set_name("hello world");
        for(int i=0;i<100;i++){
            stream->Write(req);
        }
        stream->WritesDone();//写操作完成
        Read.join();//等待Read线程结束
        grpc::Status status = stream->Finish();//关闭流
        if (!status.ok()){
          std::cout<<"client finish stream failed, msg is "<<status.error_message()<<std::endl;
          return "client finish stream failed";
        }
        return "success";
    }
};





int main (){
    //访问路径
    std::string target_str = "localhost:4306";
    //这里的channel我理解就是一个连接的意思
    GreeterClient greeter(
            grpc::CreateChannel(target_str,grpc::InsecureChannelCredentials()));
    std::string user = "world";
    //请求
    std::string rep = greeter.SayHello(user);
    std::string rep2 = greeter.SayHelloAll();
    std::cout<<"get reply : "<<rep<<"\nget reply2 : "<<rep2<<std::endl;
}







