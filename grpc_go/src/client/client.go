package main

import (
	"context"
	"fmt"
	"google.golang.org/grpc"
	"grpc_test/pb"
	"io"
	"log"
	"time"
)

const (
	address  = "localhost:4306"
	name = "helloworld"
)

func hello_once (){
	conn,err := grpc.Dial(address, grpc.WithInsecure(), grpc.WithBlock())
	if err != nil {
		log.Fatalln("Dial err")
	}
	defer conn.Close()
	c := pb.NewGreeterClient(conn)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second)
	defer cancel()
	req := pb.HelloRequest{Name:name}
	r,err := c.SayHello(ctx, &req)
	if err != nil {
		log.Fatalln("revoke err, %v",err )
	}
	log.Print("success %s\n",r.GetMessage())
}

func hello_all (){
	conn,err := grpc.Dial(address, grpc.WithInsecure(), grpc.WithBlock())
	if err != nil {
		log.Fatalln("Dial err")
	}
	defer conn.Close()
	ctx, cancel := context.WithTimeout(context.Background(), time.Second)
	defer cancel()
	client := pb.NewGreeterClient(conn)
	stream, err := client.SayHelloAll(ctx)
	if err != nil {
		log.Fatalf("revoke err, %v",err )
	}
	waitc := make(chan string)
	go func() {
		for {
			get, err := stream.Recv()
			if err == io.EOF {
				return
			}
			if get != nil{
				waitc <- get.GetMessage()
				//log.Printf("%s\n", get.Message)
			}
		}
	}()

	for i:=0;i<100;i++ {
		stream.Send(&pb.HelloRequest{
			Name: name,
		})
	}
	err = stream.CloseSend()
	if err != nil {
		return
	}
	for  st ,ok := <- waitc; ok ; st = <- waitc{
		fmt.Printf("%s",st);
	}
}

func main () {
	hello_all()
}