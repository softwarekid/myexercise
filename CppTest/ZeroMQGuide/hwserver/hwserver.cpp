//
//  Hello World server in C++
//  Binds REP socket to tcp://*:5555
//  Expects "Hello" from client, replies with "World"
//
#include <zmq.hpp>
#include <string>
#include <iostream>

// linux系统下sleep()函数包含在<unistd.h>
//#include <unistd.h>
// windows系统下Sleep()函数包含在<windows.h>
#include <windows.h>
// 注意两者单位的不同，sleep(1) <==>Sleep(1000) <==> 挂起1秒

int main ()
{
    //  Prepare our context and socket
    zmq::context_t context ( 1 );
    zmq::socket_t socket ( context, ZMQ_REP );
    socket.bind ( "tcp://*:5555" );

    while ( true )
    {
        zmq::message_t request;

        //  Wait for next request from client
        socket.recv ( &request );
        std::cout << "Received Hello" << std::endl;

        //  Do some 'work'
        //sleep (1);
        Sleep ( 1000 );

        //  Send reply back to client
        zmq::message_t reply ( 5 );
        memcpy ( ( void* ) reply.data (), "World", 5 );
        socket.send ( reply );
    }
    return 0;
}
