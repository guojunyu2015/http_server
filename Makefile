SERVER_OBJS=pub_deal.o http_server.o

http_server:$(SERVER_OBJS)
	@echo "开始编译http_server"
	cc -o http_server $(SERVER_OBJS) -lpthread
	@echo "http_server编译完成"

http_server.o:http_server.c pub_include.h
	cc -c http_server.c

pub_deal.o:pub_deal.c pub_include.h
	cc -c pub_deal.c

clean:
	rm -f $(SERVER_OBJS)