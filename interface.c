#include <stdio.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <string.h>

int main()
{
    int rtnetlink_socket;

    struct sockaddr_nl sa;
    struct nlmsghdr *nh;
    struct msghdr msg;
    struct iovec iov;
    char buf[100000]; 

    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    sa.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;


    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);

    msg.msg_name = (void *)&(sa);
    msg.msg_namelen = sizeof(sa);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_flags = 0;

    if ((rtnetlink_socket = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) == -1) {
        printf("error: %s", strerror(errno));
        return -1;
    }
    printf("Create socket\n");

    if (bind(rtnetlink_socket, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
        printf("error: %s", strerror(errno));
        return -1;
    }
    printf("Bind\n");

    int len;

    if ((len = recvmsg(rtnetlink_socket, &msg, 0)) == -1) {
        printf("recv mesg error\n");
        return -1;
    }

    printf("recvmesg\n");
    
    for (nh = (struct nlmsghdr *) buf; NLMSG_OK(nh, len); nh = NLMSG_NEXT (nh, len)) {

        if (nh->nlmsg_type == RTM_NEWLINK) {
            printf("new route\n");
        }

         if (nh->nlmsg_type == RTM_DELLINK) {
            printf("remove route\n");
        }

    }
    
    return 0;
}
