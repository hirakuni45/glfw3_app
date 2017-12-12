#include <iostream>

int main(int argc, char* argv[])
{


}



#if 0
 extern "C" {
 #include <sys/socket.h>
 }

#include <boost/asio.hpp>

typedef boost::asio::detail::socket_option::boolean<IPPROTO_IP, IP_PKTINFO> packet_info;

template <typename ConstBufferSequence>
inline std::size_t sendmsg(
    boost::asio::ip::udp::socket & socket,
    boost::asio::ip::udp::endpoint const & endpoint,
    ConstBufferSequence buffer,
    struct ::in_addr const & address,
    int if_index,
    boost::asio::socket_base::message_flags flags,
    boost::system::error_code & ec)
{
    struct iovec iov = {
        .iov_base = const_cast<char *>(boost::asio::buffer_cast<char const *>(*buffer.begin())),
        .iov_len = boost::asio::buffer_size(*buffer.begin())
    };

    char cbuf[CMSG_LEN(sizeof(struct ::in_pktinfo))];
    struct msghdr msg = {
        .msg_name = const_cast<struct ::sockaddr *>(endpoint.data()),
        .msg_namelen = static_cast<::socklen_t>(endpoint.size()),
        .msg_iov = &iov,
        .msg_iovlen = 1,
        .msg_control = cbuf,
        .msg_controllen = CMSG_LEN(sizeof(struct ::in_pktinfo)),
        .msg_flags = flags,
    };

    struct cmsghdr * cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = IPPROTO_IP;
    cmsg->cmsg_type = IP_PKTINFO;
    cmsg->cmsg_len = CMSG_LEN(sizeof(struct ::in_pktinfo));

    struct in_pktinfo * pktinfo = reinterpret_cast<struct ::in_pktinfo *>(CMSG_DATA(cmsg));
    pktinfo->ipi_ifindex = if_index;
    pktinfo->ipi_spec_dst = address;
    pktinfo->ipi_addr = reinterpret_cast<struct ::sockaddr_in const *>(endpoint.data())->sin_addr;

    ssize_t rc = ::sendmsg(socket.native_handle(), &msg, flags);
    if (rc < 0) {
        rc = 0;
        ec = boost::system::error_code(errno, boost::system::system_category());
    }

    return rc;
}


int main(int argc, char ** argv)
{
    struct in_addr address;
    address.s_addr = inet_addr(argv[1]);
    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket socket(io_service);
    socket.open(boost::asio::ip::udp::v4());
    socket.set_option(boost::asio::socket_base::broadcast(true));
    socket.set_option(packet_info(true));

    boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::address_v4::broadcast(), 12345);
    boost::system::error_code ec;

    sendmsg(
        socket,
        endpoint,
        boost::asio::buffer("hello", 5),
        address,
        atoi(argv[2]),
        0,
        ec
    );

    std::cout << ec.message() << std::endl;
}
#endif
