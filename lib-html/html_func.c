#include "global.h"
#include "http_func.h"
int fix_charset_newcpy(char *, char **content);
int html_fetch_img_ex(char *src, char **dest)
{
	char *ipart = NULL, *dpart;
	if ((last_gets(src, "<img", ">", &ipart) == MRT_ERR) && (last_gets(src, "<IMG", ">", &ipart) == MRT_ERR)) {
		log_error("No found img tags, error:%s.", get_error());
		return MRT_ERR;
	}
	if (last_gets(ipart, "src=\"", "\"", &dpart) == MRT_ERR && last_gets(src, "src='", "'", &dpart) == MRT_ERR) {
		log_error("No found img tags, error:%s.", get_error());
		return MRT_ERR;
	}
	*dest = dpart + 5;
	return MRT_SUC;
}

int html_fetch_img(char *src, char *url, int url_size)
{
	if (comm_gets(src, "src=\"", "\"", url, url_size) == MRT_ERR
	    && comm_gets(src, "src='", "'", url, url_size) == MRT_ERR) {
		log_error("No found href tags href, src:%s.", src);
		return MRT_ERR;
	}
	return MRT_SUC;
}

int html_fetch_href_ex(char *src, char **url, char **caption)
{
	char *ipart = NULL;
	if (last_gets(src, "<a ", "</a>", &ipart) == MRT_ERR) {
		log_error("No found img tags, error:%s.", get_error());
		return MRT_ERR;
	}
	if (move_cut_gets(&ipart, "href=\"", "\"", url) == MRT_ERR
	    && move_cut_gets(&ipart, "href='", "'", url) == MRT_ERR) {
		log_error("No found href tags href, error:%s.", get_error());
		return MRT_ERR;
	}
	if (!caption)
		return MRT_SUC;
	*caption = strchr(ipart, '>');
	return MRT_SUC;
}

//int html_fetch_href(char *src, char *url, int url_size, char *caption, int caption_size)
int html_fetch_href(string_t * src, string_t * url, string_t * caption)
{
	M_cpvril(src);
	if (string_fetch(src, "href=\"", "\"", url) == MRT_ERR) {
		if (string_fetch(src, "href='", "'", url) == MRT_ERR) {
			if (string_fetch(src, "href=", " ", url) == MRT_ERR) {
				log_error("No found href tags href, src:%s.", src->str);
				return MRT_ERR;
			}
		}
	}
	if (!caption)
		return MRT_SUC;
	if (string_fetch(src, ">", "<", caption)) {
		log_error("No found </a>, src:%s", src->str);
		return MRT_ERR;
	}
	return MRT_SUC;
}

//int html_part_fetch_href(char *src, char *part_start, char *part_end, char *url, int url_size, char *caption, int caption_size)
int html_part_fetch_href(string_t * src, char *part_start, char *part_end, string_t * url, string_t * caption)
{
	string_t part;
	M_cpvril(src);
	s_zero(part);
	if (string_fetch(src, part_start, part_end, &part) == MRT_ERR) {
		log_error("string_fetch error:%s", get_error());
		return MRT_ERR;
	}
	if (html_fetch_href(&part, url, caption) == MRT_ERR) {
		string_free(&part);
		return MRT_ERR;
	}
	string_free(&part);
	return MRT_SUC;
}

int html_move_fetch_href(string_t * src, string_t * url, string_t * caption)
{
	char *pe = NULL;
	if (string_move_fetch(src, "href=\"", "\"", url)) {
		if (string_move_fetch(src, "href='", "'", url)) {
			if (string_move_fetch(src, "href=", " ", url)) {
				log_error("string_move_fetch error:%s.", get_error());
				return MRT_ERR;
			}
		}
	}
	if ((pe = memchr(url->str, '>', url->len))) {
		*pe = 0;
		url->len -= pe - url->str;
	}
	if (!caption)
		return MRT_SUC;
	if (string_move_fetch(src, ">", "</a>", caption)) {
		log_error("string_move_fetch error:%s", get_error());
		return MRT_ERR;
	}
	if (string_ltrim(caption) == MRT_ERR) {
		log_error("string_ltrim caption error:%s", get_error());
		return MRT_ERR;
	}
	if (string_rtrim(caption) == MRT_ERR) {
		log_error("string_ltrim caption error:%s", get_error());
		return MRT_ERR;
	}
	return MRT_SUC;
}

int html_fix_charset(html_page_t * page, char *charset)
{

	//string_t new_data;
	M_cpvril(page);
	M_cpsril(charset);

	//s_zero(new_data);

	//当前页面的编码与charset参数不一至时转换
	if (*page->charset.str && strcasecmp(page->charset.str, charset)) {
		if (charset_convert_string(page->charset.str, charset, &page->data) == MRT_ERR) {
			log_error("Charset convert error, %s", get_error());
			return MRT_ERR;
		}

		/*
		   if(string_copy(&page->data, &new_data) == MRT_ERR)
		   {
		   log_error("string_copy error:%s", get_error());
		   string_free(&new_data);
		   return MRT_ERR;
		   }

		   string_free(&new_data);
		 */
	}
	return MRT_SUC;
}

int fix_charset_newcpy(char *src_charset, char **content)
{
	char *cpt = NULL;
	int32_t nlen;
	M_cpsril(*content);
	if (src_charset && *src_charset) {
		nlen = strlen(*content) * 3;
		M_cvril((cpt = M_alloc(nlen)), "M_alloc error, %m");
		if (charset_convert(src_charset, "UTF-8", *content, strlen(*content), cpt, nlen - 1) == MRT_ERR) {
			log_error("Content charset convert error, from:%s, to:UTF-8", src_charset);
			M_free(cpt);
			return MRT_ERR;
		}
		M_free(*content);
		*content = cpt;
	}
	return MRT_SUC;
}

int html_mark_filter(mrt_filter_t * filter, string_t * src)
{
	int stat = 0, in_size, l_size;
	char *pmb = NULL, *pme = NULL;
	int blen, elen;
	string_t nsrc;
	mrt_mark_t *pmk;
	M_cpvril(src);
	in_size = src->len;

	//删除要指定的段
	LIST_FOREACH(pmk, node, &filter->remove_list, head) {
		blen = strlen(pmk->begin);
		elen = strlen(pmk->end);
		while ((pmb = strstr(src->str, pmk->begin))) {
			if (!(pmb + blen) || !*(pmb + blen))
				break;
			if (!(pme = strstr(pmb + blen, pmk->end)))
				break;
			pme += elen;
			if (!pme || !*pme)
				break;
			l_size = src->len - (pme - src->str);
			memmove(pmb, pme, l_size);
			src->len -= pme - pmb;
			src->str[src->len] = 0;
		}
	}
	s_zero(nsrc);

	//替换指定的内容
	LIST_FOREACH(pmk, node, &filter->replace_list, head) {
		while ((pmb = strstr(src->str, pmk->begin)) && *pmb) {
			if (string_copyb(&nsrc, src->str, pmb - src->str) == MRT_ERR) {
				log_error("string_copyb nsrc error:%s", get_error());
				return MRT_ERR;
			}
			if (string_cats(&nsrc, pmk->end) == MRT_ERR) {
				log_error("string_copyb nsrc error:%s", get_error());
				return MRT_ERR;
			}
			if (pmb + strlen(pmk->begin) && *(pmb + strlen(pmk->begin))) {
				if (string_cats(&nsrc, pmb + strlen(pmk->begin)) == MRT_ERR) {
					log_error("string_cats nsrc error:%s", get_error());
					string_free(&nsrc);
					return MRT_ERR;
				}
			}
			if (string_copy(src, &nsrc) == MRT_ERR) {
				log_error("string_copy src error:%s", get_error());
				string_free(&nsrc);
				return MRT_ERR;
			}
		}
	}
	for (l_size = 0; l_size < src->len; l_size++) {
		switch (src->str[l_size]) {
		case '\n':
		case '\r':
		case '\t':
		case ' ':
			if (stat == 0)
				continue;
			break;
		case '\'':
			if (stat == 1)
				src->str[l_size] = '\"';
			break;
		case '<':
			stat = 1;
			break;
		case '>':
			stat = 0;
			break;
		}
	}
	string_free(&nsrc);
	log_debug("filter result, In size:%d, Out size:%d", in_size, src->len);
	return MRT_SUC;
}

int html_string_format(string_t * src)
{
	int i = 0, stat = 0;
	for (i = 0; i < src->len; i++) {
		switch (src->str[i]) {
		case '<':
			stat = 1;
			break;
		case '>':
			stat = 0;
			break;
		case '\'':
			if (stat == 1)
				src->str[i] = '\"';
			break;
		default:
			if (stat == 1) {
				if (isupper(src->str[i]))
					src->str[i] -= 32;
			}
		}
	}
	return MRT_SUC;
}

int html_format(mrt_page_t * page, char *data)
{
	int len = 0;
	char *ps, *ps1, *ps2 = NULL;
	if (page->charset && *page->charset && strncasecmp(page->charset, "utf", 3)) {
		M_ciril(fix_charset_newcpy(page->charset, &data), "fix charset error.");
	}
	ps = data;
	while ((ps1 = strchr(ps, '<'))) {
		if ((ps2 = strchr(ps1, '>'))) {
			if (strncasecmp(ps1 + 1, "P>", 2) == 0) {
				ps = ps2 + 1;
				continue;
			}
			if (strncasecmp(ps1 + 1, "br>", 3) == 0) {
				ps = ps2 + 1;
				continue;
			}
			len = strlen(ps2) - 1;
			memmove(ps1, ps2 + 1, len);
			*(ps1 + len) = 0;
		}
	}
	str_trim(data);
	return MRT_SUC;
}

int html_fix_caption(html_page_t * page, string_t * caption)
{
	int all_len = 0, len = 0;
	char *ps, *ps1, *ps2 = NULL;
	ps = caption->str;
	while ((ps1 = strchr(ps, '<'))) {
		all_len = strlen(caption->str);
		if ((ps2 = strchr(ps1, '>'))) {
			len = ps2 - ps1;
			memmove(ps1, ps2 + 1, strlen(ps2));
			memset(ps + all_len - len, 0, len);
		}
	}
	caption->len = strlen(caption->str);
	return MRT_SUC;
}

int html_fix_url_ex(mrt_page_t * page, char **url)
{
	char *ps1, *ps2, *ps3, *ps4;
	char turl[MAX_URL] = { 0 };
	char tbase[MAX_URL] = { 0 };
	if (http_check_prefix(*url))	//不是以http开头的
	{
		strcpy(tbase, page->base);
		ps1 = *url;
		ps3 = tbase;
		if (*url[0] == '/') {
			ps1 = page->base;
			ps1 = str_jump_tag(ps1, "://");
			if ((ps3 = strchr(ps1, '/'))) {
				*ps3 = 0;
				sprintf(turl, "%s%s", page->base, *url);
				*ps3 = '/';
			}

			else
				sprintf(turl, "%s%s", page->base, *url);
			*url = str_newcpy(turl, strlen(turl));
			return MRT_SUC;
		}
		while ((ps2 = strstr(ps1, "../"))) {
			ps1 += 3;
			if ((ps4 = strrchr(ps3, '/')))
				*ps4 = 0;
		}
		ps1 = (*ps1 == '.') ? ps1 + 1 : ps1;
		if (*ps1 == '/')
			sprintf(turl, "%s%s", ps3, ps1);

		else
			sprintf(turl, "%s/%s", ps3, ps1);
		*url = str_newcpy(turl, strlen(turl));
		return MRT_SUC;
	}
	return MRT_SUC;
}

int html_fix_url(html_page_t * page, string_t * url)
{
	char *pstr, *purl, *sep = "";
	char tbase[MAX_URL] = { 0 };
	char turl[MAX_URL] = { 0 };
	if (http_check_prefix(url->str) == MRT_SUC)	//以http开头的
		return MRT_SUC;
	snprintf(tbase, sizeof(tbase), "%s", page->base.str);
	snprintf(turl, sizeof(turl), "%s", url->str);
	purl = turl;
	if (purl[0] == '.' && purl[1] == '/')
		purl += 2;

	//如果是以/开始的，找到base的第一个/之后追加
	if (purl[0] == '/') {
		pstr = str_jump_tag(tbase, "://");
		if ((pstr = strchr(pstr, '/')))
			*pstr = 0;
	}

	else {
		while (strstr(purl, "../")) {
			purl += 3;
			if ((pstr = strrchr(tbase, '/')))
				*pstr = 0;
		}
	}
	if ((tbase[strlen(tbase) - 1] != '/' && purl[0] != '/'))
		sep = "/";
	if (string_printf(url, "%s%s%s", tbase, sep, purl) == MRT_ERR) {
		log_error("string_copys error.");
		return MRT_ERR;
	}
	return MRT_SUC;
}

int html_fetch_base(string_t * src, html_page_t * page)
{
	char *pstr = NULL;
	M_cpvril(src);
	if (html_part_fetch_href(src, "<base", ">", &page->base, NULL) == MRT_ERR)	//没有base标记
	{
		pstr = strrchr(page->url, '/');
		if (!pstr || !*pstr) {
			log_error("url no found '/', url:%s", page->url);
			return MRT_ERR;
		}
		*pstr = 0;
		if (string_copys(&page->base, page->url) == MRT_ERR) {
			log_error("string_copys error:%s", get_error());
			*pstr = '/';
			return MRT_ERR;
		}
		*pstr = '/';
	}
	return MRT_SUC;
}

int html_fetch_charset(string_t * src, html_page_t * page)
{
	M_cpvril(src);
	if (string_fetch(src, "charset=\"", "\"", &page->charset) == MRT_ERR) {
		if (string_fetch(src, "charset=", "\"", &page->charset) == MRT_ERR) {
			log_error("no found charset, src:%s", src->str);
			return MRT_ERR;
		}
	}
	if (!strncasecmp(page->charset.str, "gb", 2)) {
		if (string_printf(&page->charset, "GB18030") == MRT_ERR) {
			log_error("string_printf error:%s", get_error());
			return MRT_ERR;
		}
	}
	return MRT_SUC;
}

int html_head_parse(html_page_t * page)
{
	string_t part;
	s_zero(part);
	if (string_fetch(&page->data, "<head>", "</head>", &part) == MRT_ERR) {
		log_error("no found head begin or end, url:%s, body:%s", page->url, page->data.str);
		return MRT_ERR;
	}
	if (html_fetch_base(&part, page) == MRT_ERR) {
		log_error("fetch base error, url:%s", page->url);
		string_free(&part);
		return MRT_ERR;
	}
	if (html_fetch_charset(&part, page) == MRT_ERR) {
		log_error("fetch charset error, url:%s", page->url);
	}
	log_info("charset:%s", page->charset.str);
	string_free(&part);
	return MRT_SUC;
}
