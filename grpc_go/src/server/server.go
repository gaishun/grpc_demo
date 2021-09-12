package main

import (
	"context"
	"fmt"
	"google.golang.org/grpc"
	pb "grpc_test/pb"
	"io"
	"log"
	"net"
)

//还可以放点其他的东西在创建server的时候可以初始化本地信息，然后后面执行远程过程调用的时候也可以使用。
type server struct {
	pb.UnimplementedGreeterServer
}


/**
SayHello(context.Context, *HelloRequest) (*HelloReply, error)
SayHelloAll(Greeter_SayHelloAllServer) error
*/

func (s* server) SayHello(ctx context.Context, req *pb.HelloRequest) (*pb.HelloReply, error){

	fmt.Printf("%s req, %s once\n",req , req.Name)
	resp := pb.HelloReply{
		Message: "adfadf",
	}
	return &resp, nil
}

func (s *server) SayHelloAll(req pb.Greeter_SayHelloAllServer) error {
	for i:=0;i<10000;i++ {
		t, err := req.Recv();
		if err == io.EOF{
			break
		}
		if err != nil{
			log.Fatalln("Recv err")
		}
		fmt.Printf("%s req, %s time %d\n",t ,t.Name , i)
		req.Send(&pb.HelloReply{Message: fmt.Sprintf("reply %v\n", i)})
	}
	return nil
}


func main (){
	lis,err  := net.Listen("tcp","localhost:4306")
	if err!=nil{
		log.Fatalln("listen err");
	}
	s := grpc.NewServer()
	pb.RegisterGreeterServer(s, &server{})

	err=s.Serve(lis)
	if err!=nil {
		fmt.Println("start err")
	}
	fmt.Println("start")


}