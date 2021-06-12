/*

	lang-string.c

	Dynamic string class

*/

#include <string.h>
#include <ctype.h>
#include <memory.h>
#include "inc/bob.h"


#define MAX_LEN MAX_STRING_LEN

#define STR_CHECK(p) check(p,__FUNCTION__)

static void check(PSTRING p, LPCSTR func) {
	ptrchk(p, func);
	ptrchk(p->data, func);
	if (p->len < 0) {
		fprintf(stderr, "Negative string length in %s(). Dying...\n", func);
		last_call(1);
	}
	if (p->len > MAX_LEN) {
		fprintf(stderr, "String too long in %s(). Dying...\n", func);
		last_call(1);
	}
}

static inline LPSTR alloc_buf(int count) {
	LPSTR sz = (LPSTR)malloc(count+1);
	PTR_CHECK(sz);
	return sz;
}

static inline PSTRING alloc_str() {
	PSTRING p = (PSTRING)malloc(sizeof(String));
	PTR_CHECK(p);
	return p;
}

int GetStringLength(PSTRING p) {
	STR_CHECK(p);
	return (p->len);
}

PSTRING CreateString(LPCSTR src) {
	PSTRING p = alloc_str();
	p->data = strdup(src);
	PTR_CHECK(p->data);
	p->len = strlen(p->data);
	return p;
}

void DestroyString(PSTRING p) {
	STR_CHECK(p);
	free((void*)(p->data));
	free(p);
}

PSTRING CopyString(PSTRING src) {
	STR_CHECK(src);
	return CreateString(src->data);
}

PSTRING AppendString(PSTRING str, LPCSTR add) {
	STR_CHECK(str);
	PTR_CHECK(add);
	int len1 = str->len;
	int len2 = strlen(add);
	int len = len1 + len2;
	LPSTR buf = alloc_buf(len);
	memcpy(buf, str->data, len1);
	memcpy(buf + len1, add, len2 + 1);
	PSTRING p = alloc_str();
	p->data = buf;
	p->len = len;
	return p;
}

PSTRING CopySubString(PSTRING src, int first, int count) {
	if (first < 0) {
		count += first;
		first = 0;
	}
	if (count < 1) {
		return CreateString("");
	}
	STR_CHECK(src);
	int len = src->len;
	int left = len - first;
	if (left < 1) {
		return CreateString("");
	}
	count = MIN(count, left);
	LPSTR buf = alloc_buf(count);
	memcpy(buf, src->data+first, count+1);
	PSTRING p = alloc_str();
	p->data = buf;
	p->len = count;
	return p;
}

PSTRING LeftSubString(PSTRING src, int count) {
	STR_CHECK(src);
	return CopySubString(src, 0, count);
}

PSTRING RightSubString(PSTRING src, int count) {
	STR_CHECK(src);
	int first = GetStringLength(src) - count;
	return CopySubString(src, first, count);
}

PSTRING CutSubString(PSTRING src, int first, int count) {
	STR_CHECK(src);
	PSTRING sub = CopySubString(src, first, count);
	DeleteSubString(src, first, count);
	return sub;
}

PSTRING DeleteSubString(PSTRING src, int first, int count) {
	STR_CHECK(src);
	if (count < 1) return CopyString(src);
	int last = first + count;
	PSTRING left = LeftSubString(src, first);
	PSTRING right = RightSubString(src, src->len - first + count);
	PSTRING both = AppendString(left, right->data);
	DestroyString(left);
	DestroyString(right);
	return both;
}

PSTRING InsertSubString(PSTRING src, int first, LPCSTR ins) {
	STR_CHECK(src);
	PTR_CHECK(ins);
	int len = src->len;
	if (first < 0) return CopyString(src);
	if (first >= len) return CopyString(src);
	PSTRING left = LeftSubString(src, first);
	PSTRING right = CopySubString(src, first, len);
	PSTRING half = AppendString(left, ins);
	PSTRING full = AppendString(half, right->data);
	DestroyString(half);
	DestroyString(left);
	DestroyString(right);
	return full;
}

PSTRING ReplaceSubString(PSTRING src, int first, int count, LPCSTR ins) {
	STR_CHECK(src);
	PTR_CHECK(ins);
	PSTRING tmp = DeleteSubString(src, first, count);
	PSTRING out = InsertSubString(tmp, first, ins);
	DestroyString(tmp);
	return out;
}

int FindSubString(PSTRING src, LPCSTR find) {
	STR_CHECK(src);
	PTR_CHECK(find);
	LPSTR sz = strstr(src->data, find);
	if (!sz) return -1;
	return (sz - src->data);
}

PSTRING ReverseString(PSTRING str) {
	STR_CHECK(str);
	LPSTR buf = strdup(str->data);
	strrev(buf);
	PSTRING p = alloc_str();
	p->data = buf;
	p->len = str->len;
	return p;
}

PSTRING StringToUppercase(PSTRING str) {
	STR_CHECK(str);
	LPSTR buf = strdup(str->data);
	strupr(buf);
	PSTRING p = alloc_str();
	p->data = buf;
	p->len = str->len;
	return p;
}

PSTRING StringToLowercase(PSTRING str) {
	STR_CHECK(str);
	LPSTR buf = strdup(str->data);
	strlwr(buf);
	PSTRING p = alloc_str();
	p->data = buf;
	p->len = str->len;
	return p;
}

void PrintString(PSTRING str) {
	STR_CHECK(str);
	fprintf(stderr, "%s\n", str->data);
}

char *strrev(char *str) {

      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2) {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}

void strupr(char *str) {
    if (str) {
        while (*str) {
            *str = toupper( *str );
        }
    }
}

void strlwr(char *str) {
    if (str) {
        while (*str) {
            *str = tolower( *str );
        }
    }
}

