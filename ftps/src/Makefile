ftpd: ftps.c preparation.c factory.c taskQue.c threadHandler.c command.c
	gcc $^ -o $@ -pthread -lcrypt -lcrypto -lmysqlclient -I ../include
clean:
	rm ftpd
