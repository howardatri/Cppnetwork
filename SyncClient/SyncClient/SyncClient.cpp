﻿
#include <iostream>
#include <boost/asio.hpp>
#include <thread>
using namespace std;
using namespace boost::asio::ip;
const int MAX_LENGTH = 1024 * 2;
const int HEAD_LENGTH = 2;
int main()
{
	try {
		//创建上下文服务
		boost::asio::io_context   ioc;
		//构造endpoint
		tcp::endpoint  remote_ep(address::from_string("127.0.0.1"), 10086);
		tcp::socket  sock(ioc);
		boost::system::error_code   error = boost::asio::error::host_not_found; ;
		sock.connect(remote_ep, error);
		if (error) {
			cout << "connect failed, code is " << error.value() << " error msg is " << error.message();
			return 0;
		}

		thread send_thread([&sock] {
			for (;;) {
				this_thread::sleep_for(std::chrono::milliseconds(2));
				const char* request = "hello world!";
				size_t request_length = strlen(request);
				char send_data[MAX_LENGTH] = { 0 };
				memcpy(send_data, &request_length, 2);
				memcpy(send_data + 2, request, request_length);
				boost::asio::write(sock, boost::asio::buffer(send_data, request_length + 2));
			}
			});

		thread recv_thread([&sock] {
			for (;;) {
				this_thread::sleep_for(std::chrono::milliseconds(2));
				cout << "begin to receive..." << endl;
				char reply_head[HEAD_LENGTH];
				size_t reply_length = boost::asio::read(sock, boost::asio::buffer(reply_head, HEAD_LENGTH));
				short msglen = 0;
				memcpy(&msglen, reply_head, HEAD_LENGTH);
				char msg[MAX_LENGTH] = { 0 };
				size_t  msg_length = boost::asio::read(sock, boost::asio::buffer(msg, msglen));

				std::cout << "Reply is: ";
				std::cout.write(msg, msglen) << endl;
				std::cout << "Reply len is " << msglen;
				std::cout << "\n";
			}
			});

		send_thread.join();
		recv_thread.join();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << endl;
	}
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
