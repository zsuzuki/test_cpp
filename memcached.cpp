//
// memcached 接続テスト
//
#include <stdio.h>

#include <string>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

//
//
//

//
//
//
int
main(int argc, char** argv)
{
  if (argc < 4)
  {
    printf("usage: %s host port\n", argv[0]);
    return 1;
  }

  std::string hostname = argv[1];
  std::string service  = argv[2];
  std::string message  = argv[3];

  struct addrinfo hints, *res;
  struct in_addr  addr;

  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family   = AF_INET;
  hints.ai_flags    = 0;
  hints.ai_protocol = 0;

  auto err = getaddrinfo(hostname.c_str(), service.c_str(), &hints, &res);
  if (err != 0)
  {
    printf("address info error: %d\n", err);
    return 1;
  }

  addr.s_addr = ((struct sockaddr_in*)(res->ai_addr))->sin_addr.s_addr;

  printf("ip addres: %s\n", inet_ntoa(addr));

  auto sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (sock < 0)
  {
    perror("socket");
    return 1;
  }

  if (connect(sock, res->ai_addr, res->ai_addrlen) != 0)
  {
    perror("connect");
    return 1;
  }

  {
    // write header
    int  len = message.length();
    char head[256];
    snprintf(head, sizeof(head), "set key 0 0 %d\r\n", len);
    send(sock, head, strlen(head), 0);
    // write body
    std::string body = message + "\r\n";
    send(sock, body.c_str(), body.length(), 0);
    // result
    char cmd_res[256];
    int  res_len = recv(sock, cmd_res, sizeof(cmd_res), 0);
    printf("--- command result[%d] ---\n%s", res_len, cmd_res);

    const char* getter = "get key\r\n";
    char        get_buff[1024];
    send(sock, getter, strlen(getter), 0);
    int rlen = recv(sock, get_buff, sizeof(get_buff), 0);
    printf("--- result[%d] ---\n%s", rlen, get_buff);
  }
  printf("done\n");

  close(sock);

  // 取得した情報を解放
  freeaddrinfo(res);

  return 0;
}
//
