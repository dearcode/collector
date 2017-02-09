#include <openssl/md5.h>
#include "global.h"
#include "http_func.h"

int fix_charset_newcpy(char *, char **content);

char cookie[4096] = { 0 };

int timeout = 5;
static MD5_CTX __temp_file_md5;
static file_handle_t __temp_file_handle;

int http_check_prefix(char *src)
{
	if (strncasecmp(src, "http", 4) == 0)
		return MRT_SUC;
	else
		return MRT_ERR;
}

int http_head_find(char *buf, char *key, char *val, uint32_t val_size)
{
	uint32_t v_len;
	char *ps1, *ps2;

	M_csrinl((ps1 = strstr(buf, key)));

	ps1 = ps1 + strlen(key);

	M_csril((ps2 = strchr(ps1, '\r')), "no found \"\\r\"");

	*ps2 = 0;
	v_len = strlen(ps1);
	if (v_len > val_size || v_len < 1) {
		log_error("field length error, len:%u, dest size:%u", v_len, val_size);
		return MRT_ERR;
	}

	strcpy(val, ps1);
	*ps2 = '\r';

	return str_trim(val);
}

int http_url_parse(char *src, char *host, char *page, char *port)
{
	char *phost = src;
	char *ppage = NULL;

	M_cpsril(src);

	if (strncasecmp(src, "https://", 8) == 0)
		phost = src + 8;
	else if (strncasecmp(src, "http://", 7) == 0)
		phost = src + 7;

	if ((ppage = strchr(phost, '/')) != NULL)
		strncpy(host, phost, ppage - phost);
	else
		strcpy(host, phost);

	if (!ppage || (ppage == (src + strlen(src))))
		strcpy(page, "/");
	else
		strcpy(page, ppage);

	if ((phost = strchr(host, ':'))) {
		strcpy(port, phost + 1);
		*phost = 0;
	} else
		strcpy(port, "80");

	log_debug("HOST:%s, PORT:%s, PAGE:%s", host, port, page);

	return MRT_SUC;
}

int uri_chinese_encode(char *link, int llen, string_t * dest)
{
	uint8_t lc = 0, i, j;
	uint8_t bs[] = { 128, 64, 32, 16, 0 };
	char mb[MAX_ID] = { 0 };

	for (i = 0; i < llen; i++) {
		lc = link[i];
		j = 0;
		while (lc & bs[j])
			j++;
		if (j) {
			M_ciril(urlencode(link + i, j, mb, sizeof(mb)), "utf-8 encode error.");
			string_cats(dest, mb);
			i += j - 1;
		} else
			string_catb(dest, link + i, 1);
	}

	return MRT_SUC;
}

int http_make_head(char *host, char *page, char *port, char *referer, string_t * dest)
{
	M_cpsril(host);
	M_cpvril(page);
	M_cpsril(port);

	if (strcmp(page, "/") == 0)
		string_cats(dest, "GET / HTTP/1.1\r\n");
	else {
		if (page[0] == '/')
			string_cats(dest, "GET ");
		else
			string_cats(dest, "GET /");

		M_ciril(uri_chinese_encode(page, strlen(page), dest), "process field:page error.");
		string_cats(dest, " HTTP/1.1\r\n");
	}

	if (referer && *referer) {
		string_cats(dest, "Referer: ");
		string_cats(dest, referer);
		string_cats(dest, "\r\n");
	}

	if (*cookie) {
		string_cats(dest, "Cookie: ");
		string_cats(dest, cookie);
		string_cats(dest, "\r\n");
	}

	string_cats(dest, "Host:");
	string_cats(dest, host);
	if (strcmp(port, "80")) {
		string_cats(dest, ":");
		string_cats(dest, port);
	}

	string_cats(dest, "\r\n");
	string_cats(dest, "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko\r\n");
	string_cats(dest, "Accept: */*\r\n\r\n");

	return MRT_SUC;
}

int http_connect(char *url, char *referer)
{
	int sockfd;
	struct sockaddr_in server_addr;
	struct hostent *r_host;
	string_t *http_head = NULL;
	char host[MAX_ADDR] = { 0 }, port[MAX_PORT] = {
	0}, page[MAX_URL] = {
	0};
	int ssize = 0, csize = 0;

	M_ciril(http_url_parse(url, host, page, port), "http url parse error.");

	if ((r_host = gethostbyname(host)) == NULL) {
		log_error("Gethostname error:%m, host:(%s:%s), page:%s", host, port, page);
		return MRT_ERR;
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		log_error("Socket error:%m, host:(%s:%s) page:%s", host, port, page);
		return MRT_ERR;
	}

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(port));
	server_addr.sin_addr = *((struct in_addr *)r_host->h_addr);

	if (connect(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1) {
		log_error("Connect error:%m, host:(%s:%s) page:%s", host, port, page);
		close(sockfd);
		return MRT_ERR;
	}

	if (!(http_head = string_create(MAX_LINE))) {
		log_error("create string http head error, host:(%s:%s) page:%s.", host, port, page);
		close(sockfd);
		return MRT_ERR;
	}

	if (http_make_head(host, page, port, referer, http_head)) {
		log_error("http make head error, host:(%s:%s) page:%s.", host, port, page);
		string_free(http_head);
		close(sockfd);
		return MRT_ERR;
	}
	//log_debug("head:%s\n", http_head->buf);

	//send head ------------------------
	while (ssize < http_head->len) {
		if ((csize = write(sockfd, http_head->idx, http_head->len - ssize)) == MRT_ERR) {
			log_error("%s:%d send error.%s", __func__, __LINE__, strerror(errno));
			string_free(http_head);
			close(sockfd);
			return MRT_ERR;
		}
		http_head->idx += csize;
		ssize += csize;
	}

	string_free(http_head);

	return sockfd;
}

int http_connect_follow(char *url, char *referer)
{
	int nfd = -1;
	char curl[MAX_URL] = { 0 };
	char http_head[MAX_HEAD] = { 0 };
	int rcode = 0;
	struct timeval tm = { timeout, 0 };

	M_cpsril(url);

	strcpy(curl, url);

	while (1) {
		if ((nfd = http_connect(curl, referer)) == MRT_ERR) {
			log_error("%s http connect error, url:%s", __func__, url);
			return MRT_ERR;
		}
		//recv http head ------------
		s_zero(http_head);
		setsockopt(nfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tm, sizeof(struct timeval));
		if (recv(nfd, http_head, sizeof(http_head), MSG_PEEK) == MRT_ERR) {
			log_error("%s read socket error, %m.", __func__);
			close(nfd);
			return MRT_ERR;
		}

		if (sscanf(http_head, "%*[^ ] %d ", &rcode) != 1) {
			log_error("head error, recv:%s", http_head);
			close(nfd);
			return MRT_ERR;
		}

		if (rcode != 302)
			break;

		if (http_head_find(http_head, "Location:", curl, sizeof(curl)) == MRT_ERR || !*curl) {
			log_error("head no found Localtion, recv:%s", http_head);
			close(nfd);
			return MRT_ERR;
		}

		log_debug("found Location:%s, will follow this Url.", curl);

		close(nfd);

	}

	return nfd;
}

int __get_chunk_length(char **src, int *len)
{
	char *b = *src;

	if (!memcmp(b, "\r\n", 2)) {
		b += 2;
	}

	if (sscanf(b, "%x", len) != 1) {
		printf("b:%s, ret:%d\n", b, sscanf(b, "%x", len));
		log_error("head no found length");
		return MRT_ERR;
	}

	if (*len == 0)
		return 0;

	b = strstr(b, "\r\n");

	if (!b || !memcmp(b, "\r\n0\r\n", 5))
		return 0;

	*src = b + 2;

	printf("len:%d\n", *len);
	return *len;
}

int __get_http_page(char *url, char *referer, string_t * data)
{
	int nfd = -1, type = 0, rsize = 0, csize = 0, cLen = 0, psize, ls, lr;
	char bbuf[M_16KB] = { 0 }, hlen[32] = {
	0}, code[64] = {
	0}, *lbuf = NULL, *pstart = NULL, *s = NULL;

	pstart = bbuf;

	if ((nfd = http_connect_follow(url, referer)) == MRT_ERR) {
		log_error("http_connect_follow error, url:%s", url);
		return MRT_ERR;
	}

	rsize = read(nfd, bbuf, sizeof(bbuf) - 1);
	if (rsize == MRT_ERR) {
		log_error("read socket error:%m, url:%s", url);
		close(nfd);
		return MRT_ERR;
	}
	bbuf[rsize] = 0;

	if (!(pstart = str_jump_tag(pstart, "\r\n\r\n"))) {
		log_error("No found \"\\r\\n\\r\\n\", url:%s.", url);
		close(nfd);
		return MRT_ERR;
	}

	http_head_find(bbuf, "Cookie:", cookie, sizeof(cookie) - 1);

	if (http_head_find(bbuf, "Content-Length:", hlen, sizeof(hlen) - 1) == MRT_ERR) {
		if (http_head_find(bbuf, "Transfer-Encoding", code, sizeof(code) - 1) == MRT_ERR) {
			type = 2;
		} else {
			type = 1;
		}
	} else {
		cLen = atoi(hlen);
	}

	rsize -= pstart - bbuf;

	printf("%s\n", bbuf);
	do {
		s = pstart;
		//这个循环里不能用continue;
		pstart[rsize] = 0;

		switch (type) {
		case 2:
			string_catb(data, pstart, rsize);
			break;
		case 0:
			string_catb(data, pstart, rsize);
			if (data->len >= cLen) {
				log_debug("recv success, content-length:%d recv size:%d", cLen, data->len);
				data->len = cLen;
				data->str[cLen] = 0;
				goto recv_over_success;
			}
			break;
		case 1:
			if (rsize == 2 && *pstart == '\r' && *(pstart + 1) == '\n')
				goto next_loop;
			switch (__get_chunk_length(&pstart, &csize)) {
			case 0:
				goto recv_over_success;
			case -1:
				printf("%s\n", data->str);
				exit(0);
				log_error("get chunk length error, body:%s", pstart);
				close(nfd);
				return MRT_ERR;
			}
			psize = rsize - (pstart - s);
			printf("psize:%d, len:%jd, csize:%d\n", psize, strlen(pstart), csize);
			if (psize < csize) {
less_then_label:		ls = csize - psize;
				lr = 0;
				lbuf = M_alloc(ls);
				if (!lbuf) {
					log_error("malloc size:%d error:%m", ls);
					close(nfd);
					return MRT_ERR;
				}

				while (lr < ls && (rsize = socket_read_wait(nfd, lbuf + lr, ls - lr, 5)) != -1) {
					printf("recv size:%d, bsize:%d\n", rsize, ls - lr);
					lr += rsize;
				}

				string_catb(data, pstart, psize);
				string_catb(data, lbuf, ls);
				M_free(lbuf);
			} else if (psize == csize) {
				string_catb(data, pstart, psize);
			} else {
				while (psize > csize) {
					string_catb(data, pstart, csize);

					pstart += csize;
					psize = rsize - (pstart - s);

					if (psize == 2 && *pstart == '\r' && *(pstart + 1) == '\n')
						goto next_loop;

					switch (__get_chunk_length(&pstart, &csize)) {
					case 0:
						goto recv_over_success;
					case -1:
						printf("%s\n", data->str);
						exit(0);
						log_error("get chunk length error, body:%s", pstart);
						close(nfd);
						return MRT_ERR;
					}
					psize = rsize - (pstart - s);
				}
				goto less_then_label;
			}

			break;
		}		//switch
next_loop:	pstart = bbuf;
	} while ((rsize = socket_read_wait(nfd, bbuf, sizeof(bbuf) - 1, 5)) > 0);

	close(nfd);
recv_over_success:

	log_info("recv %s success, recv size:%d.", url, data->len);

	return MRT_SUC;
}

int http_recv_page(char *url, html_page_t * page, int parse)
{
	int type = 0, idx;

	if (__get_http_page(url, NULL, &page->data) == MRT_ERR) {
		return MRT_ERR;
	}

	snprintf(page->url, sizeof(page->url), "%s", url);

	for (idx = 0; idx < page->data.len; idx++) {
		switch (page->data.str[idx]) {
		case '<':
			type = 1;
			break;
		case '>':
			type = 0;
			break;
		case '\'':
			if (type == 1)
				page->data.str[idx] = '\"';
			break;
		}
	}

	if (parse == 1)
		M_ciril(html_head_parse(page), "html_head_parse return error.");

	return MRT_OK;
}

void http_free_page(html_page_t * page)
{
	if (!page)
		return;

	string_free(&page->data);
	string_free(&page->charset);
	string_free(&page->base);
}

int __temp_file_open()
{
	s_zero(__temp_file_handle);

	MD5_Init(&__temp_file_md5);

	if (file_open_temp("userData/tmp/", &__temp_file_handle) == MRT_ERR) {
		log_error("create tmp file error:%s", get_error());
		return MRT_ERR;
	}

	return MRT_SUC;
}

int __temp_file_write(char *dat, int len)
{
	MD5_Update(&__temp_file_md5, dat, len);

	if (file_buffer_write(&__temp_file_handle, dat, len) == MRT_ERR) {
		log_error("file_buffer_write error:%s", get_error());
		return MRT_ERR;
	}

	return MRT_SUC;
}

int __temp_file_close(char *url, string_t * new_path)
{
	unsigned char md[16];
	char md5_str[33] = { 0 }, *pstr = NULL;
	int i = 0, iret = MRT_SUC;

	file_close(&__temp_file_handle);

	do {
		if (!url || !*url)
			break;

		MD5_Final(md, &__temp_file_md5);

		for (i = 0; i < 16; i++)
			sprintf(md5_str + strlen(md5_str), "%02x", md[i]);

		pstr = strrchr(url, '.');
		if (!pstr || !*pstr || strlen(pstr) > 4) {
			log_error("url format error, url:%s", url);
			iret = MRT_ERR;
			break;
		}

		string_printf(new_path, "userData/imageUniq/%s%s", md5_str, pstr);

		if (link(__temp_file_handle.from, new_path->str) == MRT_ERR) {
			if (errno != EEXIST) {
				log_error("link file error:%m, from:%s to:%s", __temp_file_handle.from, new_path->str);
				iret = MRT_ERR;
			}
		}

	} while (0);

	file_delete(__temp_file_handle.from);

	return iret;
}

int http_recv_file(char *url, char *referer, string_t * new_path)
{
	string_t *data = string_new(M_16KB, "");

	if (__get_http_page(url, referer, data) == MRT_ERR) {
		return MRT_ERR;
	}

	M_ciril(__temp_file_open(), "__temp_file_open error");

	if (__temp_file_write(data->str, data->len) == MRT_ERR) {
		log_error("http_recv_write_temp error, url:%s", url);
		__temp_file_close(NULL, NULL);
		return MRT_ERR;
	}

	if (__temp_file_close(url, new_path) == MRT_ERR) {
		log_error("http_recv_close_temp error");
		return MRT_ERR;
	}

	log_info("Recv %s success, length:%d.", new_path->str, data->len);

	return MRT_OK;
}

int http_recv_html(char *url, char *fname)
{
	int nfd = -1, ffd = -1;
	char http_head[MAX_HEAD] = { 0 };
	char val[MAX_LINE * 8] = { 0 };
	int rsize = 0, csize = 0;
	char *pstart = http_head, *ps1;

	if ((nfd = http_connect_follow(url, NULL)) == MRT_ERR) {
		log_error("%s http connect error, url:%s", __func__, url);
		return MRT_ERR;
	}
	//recv http head ------------
	if ((csize = read(nfd, http_head, sizeof(http_head))) == MRT_ERR) {
		log_error("%s read socket error, %m.", __func__);
		close(nfd);
		return MRT_ERR;
	}

	M_ciril((ffd = create_file(fname)), "create file error, file:%s, %s", fname, get_error());

	if ((ps1 = strstr(pstart, "\r\n\r\n")) == NULL) {
		log_error("%s no found \\r\\n\\r\\n, url:%s.", __func__, url);
		close(nfd);
		close(ffd);
		return MRT_ERR;
	}

	rsize = csize - (ps1 - pstart + 4);
	write(ffd, ps1 + 4, rsize);

	s_zero(val);
	while ((csize = read(nfd, val, sizeof(val))) > 0) {
		write(ffd, val, csize);
		rsize += csize;
		s_zero(val);
	}

	close(nfd);
	close(ffd);

	log_info("Recv file success, fname:%s, size:%d.", fname, rsize);

	return MRT_SUC;
}

int http_get_parameter(char *src, char *key, char *val)
{
	char *pkey, *pval;
	char tkey[MAX_ID] = { 0 };

	sprintf(tkey, "%s=", key);
	if (!(strncmp(src, tkey, strlen(tkey)))) {
		pval = src + strlen(tkey);
		if ((pkey = strchr(pval, '&'))) {
			*pkey = 0;
			sprintf(val, "%s", pval);
			*pkey = '&';
		} else {
			sprintf(val, "%s", pval);
		}

		return MRT_SUC;
	}

	sprintf(tkey, "&%s=", key);

	if ((pkey = strstr(src, tkey))) {
		pval = pkey + strlen(tkey);
		if ((pkey = strchr(pval, '&'))) {
			*pkey = 0;
			sprintf(val, "%s", pval);
			*pkey = '&';
		} else {
			sprintf(val, "%s", pval);
		}
		return MRT_SUC;
	}

	return MRT_ERR;
}
