

#include <jet2/Common.hpp>
#include <jet2/Functor.hpp>
#include <jet2/Attr.hpp>
#include <jet2/Writer.hpp>
#include <jet2/Reader.hpp>

using namespace jet2;

struct Message { 
    Attr<int> number;
    Attr<std::string> string;

    SERIALIZED(number, string);
};


void readmsg(Ptr<Functor> in) {
    try {
        auto msg = std::make_shared<Message>();
        while (true) {
            in->val(msg);
            std::cout << "msg: " << msg->string() << std::endl;
            if (msg->string()=="bob") {
                exit(0);
            }
        }
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

        coro::start(std::bind(readmsg, in));
        coro::yield();


        auto msg = std::make_shared<Message>();
        msg->string = std::string("bob");
        msg->number = 1;
        out->val(msg);
        writer->flush();

        coro::sleep(coro::Time::sec(10));
        //assert(msg->number() == 42);
        //assert(msg->string() == "hello world");
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

        coro::start(std::bind(readmsg, in));
        coro::yield();

        auto msg = std::make_shared<Message>();
        for (int i = 0; i < 1; ++i) {
            msg->string = std::string("hello world");
            msg->number = 42;
            out->val(msg);
            writer->flush();
        }
        coro::sleep(coro::Time::sec(10));

        //assert(msg->number() == 1);
        //assert(msg->string() == "bob");
    } catch (coro::SystemError const& ex) {
        std::cout << ex.what() << std::endl;
    }
}


void run() {

    Ptr<coro::Socket> socket(new coro::Socket);
    Ptr<Functor> writer(new WriteFunctor<coro::Socket>(socket));


    Ptr<Message> foo(new Message);
    writer->val(foo);


    exit(0);
}

int main() {
    //coro::start(run);
    coro::start(server);
    coro::start(client);

    coro::run();
    return 0;

}
