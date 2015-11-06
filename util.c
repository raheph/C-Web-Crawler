#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "util.h"

typedef struct {
	char * host;
	char * port;
	char * resource;
} url;

int parseURL(const char* urlS, url* urlU) {
	int hostIndex, portIndex, resourceIndex, len;
	char * tmp;

	tmp = strstr(urlS, "://");
	if (tmp != NULL)
		hostIndex = tmp - urlS + 3;
	else
		hostIndex = 0;

	resourceIndex = hostIndex + 1;
	while (urlS[resourceIndex] != ':' && urlS[resourceIndex] != '/' && urlS[resourceIndex] != 0)
		resourceIndex++;
	if (urlS[resourceIndex] == 0)
		return 0;
	if (urlS[resourceIndex] == ':') {
		portIndex = ++resourceIndex;
		while (urlS[resourceIndex] != '/' && urlS[resourceIndex] != 0)
			resourceIndex++;
		if (urlS[resourceIndex] == 0)
				return 0;
	} else
		portIndex = -1;

	len = resourceIndex + 1;
	while (urlS[len] != 0)
		len++;
	if (portIndex == -1) {
		urlU->host = (char*)malloc(resourceIndex - hostIndex + 1);
		memcpy(urlU->host, &urlS[hostIndex], resourceIndex - hostIndex);
		urlU->host[resourceIndex - hostIndex] = 0;
		urlU->port = NULL;
	} else {
		urlU->host = (char*)malloc(portIndex - hostIndex);
		memcpy(urlU->host, &urlS[hostIndex], portIndex - hostIndex - 1);
		urlU->host[portIndex - hostIndex - 1] = 0;
		urlU->port = (char*)malloc(resourceIndex - portIndex + 1);
		memcpy(urlU->port, &urlS[portIndex], resourceIndex - portIndex);
		urlU->port[resourceIndex - portIndex] = 0;
	}
	urlU->resource = (char*)malloc(len - resourceIndex + 1);
	memcpy(urlU->resource, &urlS[resourceIndex], len - resourceIndex);
	urlU->resource[len - resourceIndex] = 0;

	return 1;
}

int getSocketForURL(const url* url) {
	struct addrinfo hints, * server, * tmp;
	int error, sock = -1;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	error = getaddrinfo(url->host, url->port == NULL ? "http" : url->port, &hints, &server);
	if (error != 0)
		fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(error));
	else {
		for (tmp = server; tmp != NULL && sock == -1; tmp = tmp->ai_next) {
			if ((sock = socket(server->ai_family, server->ai_socktype, server->ai_protocol)) == -1)
				fprintf(stderr, "socket(): %s\n", strerror(errno));
			else if (connect(sock, server->ai_addr, server->ai_addrlen) == -1) {
				fprintf(stderr, "connect(): %s\n", strerror(errno));
				close(sock);
				sock = -1;
			}
	    }
	}

	freeaddrinfo(server);
	return sock;
}

char* getResponse(int socket) {
	int bufferSize = 1000, bufferUsed = 0, read;
	char * buffer = (char*)malloc(bufferSize), * tmp;

	while ((read = recv(socket, &buffer[bufferUsed], bufferSize - bufferUsed - 1, 0)) > 0) {
		bufferUsed += read;
		if (bufferUsed == bufferSize - 1) {
			tmp = (char*)malloc(bufferSize * 2);
			memcpy(tmp, buffer, bufferUsed);
			free(buffer);
			buffer = tmp;
			bufferSize *= 2;
		}
	}
	if (read == -1) {
		fprintf(stderr, "recv(): %s\n", strerror(errno));
		free(buffer);
		return NULL;
	}
	buffer[bufferUsed] = 0;
	return buffer;
}

const char * HTTP_REQUEST = "%s %s HTTP/1.0\r\nHost: %s\r\n\r\n";

int getMIMEType(const char* urlS, char* buffer, int bufferSize) {
	url urlU;
	int socket, len, rv = 1;
	char * request, * response = NULL, * left, * right;

	if (!parseURL(urlS, &urlU))
		return 0;
	if ((socket = getSocketForURL(&urlU)) == -1)
		return 0;

	request = (char*)malloc(strlen(HTTP_REQUEST) + strlen(urlU.host) + strlen(urlU.resource));
	sprintf(request, HTTP_REQUEST, "HEAD", urlU.resource, urlU.host);
	len = strlen(request);
	if (send(socket, request, len, 0) != len) {
		fprintf(stderr, "send(): %s\n", strerror(errno));
		rv = 0;
	} else if ((response = getResponse(socket)) == NULL)
		rv = 0;
	else {
		left = strstr(response, "Content-Type: ");
		if (left == NULL)
			rv = 0;
		else {
			right = strstr(left + 14, "\r\n");
			if (right == NULL)
				rv = 0;
			else {
				len = right - (left + 14);
				if (len > bufferSize - 1)
					len = bufferSize - 1;
				memcpy(buffer, left + 14, len);
				buffer[len] = 0;
			}
		}
	}

	close(socket);
	free(urlU.host);
	free(urlU.port);
	free(urlU.resource);
	free(request);
	free(response);
	return rv;
}

char* getContent(const char* urlS) {
	url urlU;
	int socket, len;
	char * request, * response = NULL, * left, * content = NULL;

	if (!parseURL(urlS, &urlU))
		return NULL;
	if ((socket = getSocketForURL(&urlU)) == -1)
		return NULL;

	request = (char*)malloc(strlen(HTTP_REQUEST) + strlen(urlU.host) + strlen(urlU.resource));
	sprintf(request, HTTP_REQUEST, "GET", urlU.resource, urlU.host);
	len = strlen(request);
	if (send(socket, request, len, 0) != len)
		fprintf(stderr, "send(): %s\n", strerror(errno));
	else if ((response = getResponse(socket)) != NULL) {
		left = strstr(response, "\r\n\r\n");
		if (left != NULL) {
			len = strlen(response) - (left + 4 - response);
			content = (char*)malloc(len + 1);
			memcpy(content, left + 4, len);
			content[len] = 0;
		}
	}

	close(socket);
	free(urlU.host);
	free(urlU.port);
	free(urlU.resource);
	free(request);
	free(response);
	return content;
}
