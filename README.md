# webServer

1. 使用reactor模式，采用oneloop per thread的设计架构实现的webserver
2. 支持epoll_lt 和epoll_et模式，通过编译宏来切换
3. 使用map实现了定时器；
4. 使用c++11封装了eventloop thread和eventloop threadpool
