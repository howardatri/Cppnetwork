#include<boost/asio.hpp>
#include <set>
#include<memory>
#include <iostream>
using boost::asio::ip::tcp;
const int MAX_LENGTH = 1024;
typedef std::shared_ptr<tcp::socket> socket_ptr;
std::set<std::shared_ptr<std::thread>> thread_set;
using namespace std;

void session(socket_ptr sock) {
	try {
		for (;;) {
			char data[MAX_LENGTH];
			memset(data, '\0', MAX_LENGTH);
			boost::system::error_code error;
			size_t length = sock->read_some(boost::asio::buffer(data, MAX_LENGTH), error);
			if(error == boost::asio::error::eof){
				std::cout << "Client disconnected." << std::endl;
				break;
			}
			else if (error) {
				throw boost::system::system_error(error);
			}
			cout << "receive from " << sock->remote_endpoint().address().to_string() << endl;
			cout << "receive data is: " << data << endl;
			//回传给对方
			boost::asio::write(*sock, boost::asio::buffer(data, length));
		}
	}
	catch (exception& e) {
		std::cerr << "Exception in thread: " << e.what() << std::endl;
	}
}

void server(boost::asio::io_context& io_context, unsigned short port_num) {
	tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), port_num));
	for (;;) {
		socket_ptr socket(new tcp::socket(io_context));
		a.accept(*socket);
		auto t = std::make_shared<std::thread>(session, socket);
		thread_set.insert(t);
	}
}

int main()
{
	try {
		boost::asio::io_context ioc;
		server(ioc, 10086);
		//主线程不能直接退出
		for (auto& t : thread_set) {
			t->join();
		}
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}

}

