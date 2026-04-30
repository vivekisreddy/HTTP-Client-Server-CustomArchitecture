all: http_server http_client

http_server: http_server.c
	gcc http_server.c -o http_server

http_client: http_client.c
	gcc http_client.c -o http_client

clean:
	rm -f http_server http_client
