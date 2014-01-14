

#include <jet2/Common.hpp>
#include <jet2/Functor.hpp>
#include <jet2/Attr.hpp>
#include <jet2/Writer.hpp>
#include <jet2/Reader.hpp>

using namespace jet2;

struct Message { 
    Attr<int> type;
    Attr<int> number;
    Attr<std::string> string;

    CONSTRUCT(type);
    SERIALIZED(number, string);
};


void readmsg(Ptr<Functor> in, std::string const expected) {
    try {
        auto msg = std::make_shared<Message>();
        in->val(msg);
        std::cout << "msg: " << msg->string() << std::endl;
        assert(msg->string()==expected);
    } catch (coro::SystemError const& ex) {
        std::cout << ex.what() << std::endl;
        exit(1);
    }
}

void server() {
    try {
        auto ls = std::make_shared<coro::Socket>();
        ls->setsockopt(SOL_SOCKET, SO_REUSEADDR, 1);
        ls->bind(coro::SocketAddr("127.0.0.1", 9090));
        ls->listen(10);

        auto sd = ls->accept();
        auto writer = std::make_shared<Writer<coro::Socket>>(sd);
        auto reader = std::make_shared<Reader<coro::Socket>>(sd);
        auto out = Ptr<Functor>(new WriteFunctor<Writer<coro::Socket>>(writer));
        auto in = Ptr<Functor>(new ReadFunctor<Reader<coro::Socket>>(reader));

        auto msg = std::make_shared<Message>();
        msg->string = std::string("bob");
        msg->number = 1;
        out->val(msg);
        writer->flush();
        std::cout << "sent" << std::endl;
        readmsg(in, "hello world");
        std::cout << "received" << std::endl;
    } catch (coro::SystemError const& ex) {
        std::cout << ex.what() << std::endl;
        exit(1);
    }
}

void client() {
    try {
        auto sd = std::make_shared<coro::Socket>();
        auto writer = std::make_shared<Writer<coro::Socket>>(sd);
        auto reader = std::make_shared<Reader<coro::Socket>>(sd);
        auto out = Ptr<Functor>(new WriteFunctor<Writer<coro::Socket>>(writer));
        auto in = Ptr<Functor>(new ReadFunctor<Reader<coro::Socket>>(reader));
        sd->connect(coro::SocketAddr("127.0.0.1", 9090));

        auto msg = std::make_shared<Message>();
        for (int i = 0; i < 1; ++i) {
            msg->string = std::string("hello world");
            msg->number = 42;
            out->val(msg);
            writer->flush();
        }
        std::cout << "sent" << std::endl;
        readmsg(in, "bob");
        std::cout << "received" << std::endl;
    } catch (coro::SystemError const& ex) {
        std::cout << ex.what() << std::endl;
    }
}

int main() {
    //coro::start(run);
    auto serverCoro = coro::start(server);
    auto clientCoro = coro::start(client);

    coro::run();
    return 0;

}
