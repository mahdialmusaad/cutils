/*
   cutils - A portable C utilities library.
   Copyright (C) 2026 Mahdi Almusaad

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "cutils.h"

#if CU_LANG_C11 || CU_COMP_MSVC
CU_DIAGNOSTICS_PUSH
CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS
CU_DIAGNOSTICS_DISABLE_RESERVED
CU_PRAGMA(clang diagnostic ignored "-Wunused-macros")
#  define __STDC_WANT_LIB_EXT1__ 1
CU_DIAGNOSTICS_POP
#endif

#if CU_COMPVER(CLANG, 19, 0)
#  define cu_strchr(S, C) \
CU_DIAGNOSTICS_PUSH \
CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS \
CU_DIAGNOSTICS_DISABLE_PREC11 \
CU_DIAGNOSTICS_DISABLE_MACRO_EXP \
CU_PRAGMA(clang diagnostic ignored "-Wc11-extensions") \
strchr(S, C) \
CU_DIAGNOSTICS_POP
#else
#  define cu_strchr(S, C) strchr(S, C)
#endif

#define CU_BITSOF(x, start, end) (CU_DWSHIFT(x, start) & (CU_UPSHIFT(1, (1 + (end - start))) - 1))

CU_ATTRIB_CONST CU_ATTRIB_NOTHROW CU_ATTRIB_WARN_UNUSED_RESULT
static uptr cu_npow2(uptr v)
{
	v--;
	v |= CU_DWSHIFT(v, 1);
	v |= CU_DWSHIFT(v, 2);
	v |= CU_DWSHIFT(v, 4);
	v |= CU_DWSHIFT(v, 8);
#if CU_PTR_BYTES >= 4
	v |= CU_DWSHIFT(v, 16);
#endif
#if CU_PTR_BYTES >= 8
	v |= CU_DWSHIFT(v, 32);
#endif
	v++;
	return v;
}

#if CU_SETTING_FUNCS

/* ========================= String ======================== */

#if CU_SETTING_STRING_FUNCS

#ifndef _ISOC99_SOURCE
#  define _ISOC99_SOURCE
#endif

#include <stdlib.h>
#include <string.h>

#if CU_LANG_C >= CU_LANG_C99 || CU_HAS_INCLUDE(<stdarg.h>) || \
    ((defined(__va_copy) || defined(va_copy)) && (CU_OS_UNIX || CU_COMPVER(MSVC, 19, 0)))
#  define CUSTR_FMT
#  include <stdio.h>
#  include <stdarg.h>
#endif

int custr_create(custr *CU_RESTRICT c, const char *CU_RESTRICT str)
{
	const uptr len = (size_t)strlen(str), allocd = cu_npow2(len + 1);
	if (!(c->str = (char *)malloc((size_t)allocd))) return 0;
	memcpy(c->str, str, (size_t)(len + 1));
	c->len = len;
	c->cap = allocd;
	return 1;
}
custr *custr_allocd(custr *CU_RESTRICT c, char *CU_RESTRICT allocdstr, uptr allocd)
{
	c->len = strlen(allocdstr);
	if (CU_UNLIKELY(allocd && allocd <= c->len)) return NULL;
	c->str = (char *)allocdstr;
	c->cap = allocd ? allocd : c->len + 1;
	return c;
}

int custr_reserve(custr *c, uptr bytes)
{
	void *ptr;

	bytes = cu_npow2(bytes);
	if (CU_LIKELY(bytes < 8)) bytes = 8;

	if (bytes == c->cap) return 1;
	else if (bytes <= c->len) return custr_shrinkto(c, bytes - 1);
	else if (bytes < c->cap) return 1;

	if (!(ptr = realloc(c->str, (size_t)bytes))) return 0;

	c->str = (char *)ptr;
	c->cap = bytes;

	return 1;
}
int custr_copy(const custr *CU_RESTRICT copy, custr *CU_RESTRICT paste)
{
	paste->len = copy->len;
	paste->cap = paste->len + 1;
	if (!(paste->str = (char *)malloc((size_t)paste->cap))) return 0;
	memcpy(paste->str, copy->str, (size_t)paste->cap);
	return 1;
}

int custr_shrinkto(custr *c, uptr shrinked_len)
{
	if (CU_UNLIKELY(shrinked_len >= c->len)) return 0;
	if (!c->str) return 1;
	c->len = shrinked_len;
	c->str[shrinked_len] = '\0';
	return 1;
}
custr *custr_clear(custr *c)
{
	c->len = c->cap = 0;
	if (!c->str) return c;
	free(c->str);
	c->str = NULL;
	return c;
}
int custr_optimize(custr *c)
{
	c->cap = 0;
	return custr_reserve(c, c->len);
}

int custr_insert(custr *CU_RESTRICT c, uptr c_offset, const char *CU_RESTRICT to_insert)
{
	uptr needed, inslen = strlen(to_insert);

	if (CU_UNLIKELY(c_offset > c->len)) return 0;
	if (CU_UNLIKELY(!inslen)) return 1;

	needed = c->len + inslen + 1;
	if (CU_UNLIKELY(!custr_reserve(c, needed))) return 0;

	memmove(c->str + c_offset + inslen, c->str + c_offset, (size_t)((c->len + (c_offset != c->len)) - c_offset));
	memmove(c->str + c_offset, to_insert, (size_t)(inslen + (c_offset == c->len)));

	c->len = needed - 1;
	return 1;
}
int custr_append(custr *CU_RESTRICT c, const char *CU_RESTRICT to_append)
{
	return custr_insert(c, c->len, to_append);
}

int custr_sub(const custr *CU_RESTRICT c, custr *CU_RESTRICT subresult, uptr start_ind, uptr end_ind)
{
	uptr charcount;

	if (CU_UNLIKELY(start_ind > end_ind || start_ind >= c->len)) return 0;
	if (CU_UNLIKELY(end_ind >= c->len)) end_ind = c->len - 1;

	charcount = (end_ind - start_ind) + 1;

	custr_reserve(subresult, charcount + 1);
	memcpy(subresult->str, c->str + start_ind, (size_t)charcount);
	subresult->str[charcount] = '\0';
	subresult->len = charcount;

	return 1;
}
int custr_tosub(custr *c, uptr start_ind, uptr end_ind)
{
	if (CU_UNLIKELY(start_ind > end_ind || start_ind > c->len)) return 0;
	if (CU_LIKELY(end_ind < c->len)) custr_shrinkto(c, end_ind + 1);

	if (start_ind) memmove(c->str, c->str + start_ind, (size_t)((c->len + 1) - start_ind));
	return 1;
}

void custr_cut(custr *c, uptr start_ind, uptr count)
{
	if (CU_UNLIKELY(start_ind >= c->len)) return;
	if (CU_UNLIKELY(start_ind + count > c->len)) count = c->len - start_ind;
	memmove(c->str + start_ind, c->str + start_ind + count, (size_t)((c->len + 1) - start_ind - count));
	c->len -= count;
}

int custr_count(const custr *c, char target)
{
	int n = 0;
	uptr offset = CU_UPTRMAX;

	while (1)  {
		if ((offset = custr_find(c, offset + 1, target, 0)) == CU_UPTRMAX) return n;
		++n;
	}
}
int custr_countsub(const custr *CU_RESTRICT c, const char *CU_RESTRICT target)
{
	uptr offset = CU_UPTRMAX;
	int n = 0;
	while (1)  {
		if ((offset = custr_findsub(c, offset + 1, target, 0)) == CU_UPTRMAX) return n;
		++n;
	}
}

int custr_fmt(custr *CU_RESTRICT c, char *CU_RESTRICT fmt, ...)
{
#ifdef CUSTR_FMT
	va_list va, copy;
	int nchars, ret = 0;

	memset(c, 0, sizeof *c);

	va_start(va, fmt);

#ifdef __va_copy
	__va_copy(copy, va);
#else
	va_copy(copy, va);
#endif

	if (CU_UNLIKELY((nchars = vsnprintf(NULL, 0, fmt, va)) < 0)) goto fail;
	if (CU_UNLIKELY(!custr_reserve(c, (uptr)(nchars + 1)))) goto fail;

	c->len = (uptr)(nchars);
	c->str[nchars] = '\0';
	vsnprintf(c->str, (size_t)(c->len + 1), fmt, copy);

	ret = 1;
fail:
	va_end(va);
	va_end(copy);
	return ret;
#else
	CU_UNUSED(c);
	CU_UNUSED(fmt);
	return 0;
#endif
}

int custr_cd(custr *CU_RESTRICT c, const char *CU_RESTRICT name)
{
	custr tmp;
	int ret = 1;

	custr_simplify(c);

	if (!name) {
		uptr ind = custr_find(c, 0, CU_FILE_SEPARATOR, -1 - (c->str[c->len - 1] == CU_FILE_SEPARATOR));
		if (ind != CU_UPTRMAX) custr_shrinkto(c, ind + 1);
		return 1;
	}

	if (c->str[c->len - 1] != CU_FILE_SEPARATOR && CU_UNLIKELY(!custr_append(c, CU_FILE_SEPARATOR_DQ))) return 0;

	if (!custr_create(&tmp, name)) goto fail;
	custr_simplify(&tmp);
	ret = custr_append(c, tmp.str + (tmp.str[0] == CU_FILE_SEPARATOR));
	custr_clear(&tmp);

fail:
	return ret;
}

void custr_simplify(custr *c)
{
	uptr ind = 0, prev;
	int dw = 1;

	for (;; ++ind, dw = 1) {
		if ((ind = custr_find(c, (prev = ind), CU_FILE_SEPARATOR, 0)) == CU_UPTRMAX) return;

		while (dw) {
			dw = 0;

			if (c->str[ind + 1] == CU_FILE_SEPARATOR) {
				uptr isep = custr_findnot(c, ind + 2, CU_FILE_SEPARATOR, 0);
				if (isep == CU_UPTRMAX) isep = c->len;
				memmove(c->str + ind + 1, c->str + isep, (size_t)(c->len - isep + 1));
				c->len -= (isep - ind) - 1;
				dw = 1;
			}
			if (!strncmp(c->str + ind, CU_FILE_SEPARATOR_DQ ".." , 3)) {
				memmove(c->str + prev, c->str + ind + 3, (size_t)(c->len - ind - 2));
				c->len -= (ind - prev) + 3;
				ind = prev - 1;
				dw = 1;
			}
			if (!strncmp(c->str + ind, CU_FILE_SEPARATOR_DQ "." , 2)) {
				memmove(c->str + ind + 1, c->str + ind + 3, (size_t)(c->len - ind - 2));
				c->len -= 2;
				dw = 1;
			}
		}
	}
}

uptr custr_find(const custr *c, uptr c_offset, char target, int n)
{
	iptr i;
	if (CU_LIKELY(n >= 0)) { for (i = (iptr)c_offset; i < (iptr)c->len; ++i) if (CU_UNLIKELY(c->str[i] == target) && !(n--)) return (uptr)i; }
	else if (CU_LIKELY(c_offset <= c->len)) { for (i = (iptr)(c->len - c_offset - 1); i >= 0; --i) if (CU_UNLIKELY(c->str[i] == target) && !(++n)) return (uptr)i; }
	return CU_UPTRMAX;
}
uptr custr_findnot(const custr *c, uptr c_offset, char target, int n)
{
	iptr i;
	if (CU_LIKELY(n >= 0)) { for (i = (iptr)c_offset; i < (iptr)c->len; ++i) if (c->str[i] != target && !(n--)) return (uptr)i; }
	else if (CU_LIKELY(c_offset <= c->len)) { for (i = (iptr)(c->len - c_offset - 1); i >= 0; --i) if (c->str[i] != target && !(++n)) return (uptr)i; }
	return CU_UPTRMAX;
}
uptr custr_findsub(const custr *CU_RESTRICT c, uptr c_offset, const char *CU_RESTRICT target_substr, int n)
{
	uptr positive = n >= 0;
	int nsearch = -1 + (int)positive;
	size_t sublen = strlen(target_substr);

	if (CU_UNLIKELY(!positive)) n = -n - 1;
	c_offset -= positive;

	while (1) {
		c_offset = custr_find(c, c_offset + positive, *target_substr, nsearch);
		if (CU_UNLIKELY(c_offset == CU_UPTRMAX || ((c->len - c_offset) < sublen))) return CU_UPTRMAX;
		if (memcmp(c->str + c_offset, target_substr, sublen) == 0 && !(n--)) return c_offset;
		if (CU_UNLIKELY(!positive)) c_offset = c->len - c_offset;
	}

	CU_UNREACHABLE();
}

void custr_replace(custr *c, uptr c_offset, char target, char replacement)
{
	uptr i;

	if (CU_UNLIKELY(target == '\0')) return;

	for (i = c_offset; i < c->len; ++i) {
		if (CU_LIKELY(c->str[i] != target)) continue;
		if (replacement == '\0') {
			memmove(c->str + i, c->str + i + 1, (size_t)(c->len - i - 1));
			custr_shrinkto(c, c->len - 1);
		} else c->str[i] = replacement;
	}
}
int custr_replacesub(custr *CU_RESTRICT c, uptr c_offset, const char *CU_RESTRICT target, const char *CU_RESTRICT replacement)
{
	enum { nulrep = 0x0, smallrep = 0x1, eqrep = 0x2, largerep = 0x3 } reptype;
	size_t rlen = replacement ? strlen(replacement) : 0, tlen = strlen(target);

	if (CU_UNLIKELY(!tlen)) return 1;

	if (!replacement || !rlen) reptype = nulrep;
	else if (rlen < tlen) reptype = smallrep;
	else if (rlen == tlen) reptype = eqrep;
	else reptype = largerep;

	while (1) {
		if ((c_offset = custr_findsub(c, c_offset, target, 0)) == CU_UPTRMAX) break;

		if (reptype == largerep && !custr_reserve(c, c->len + 1 + (rlen - tlen))) return 0;
		if (reptype != eqrep) memmove(c->str + c_offset + (rlen * (reptype & 0x1)), c->str + c_offset + tlen, (size_t)((c->len + 1) - c_offset - tlen));
		if (reptype) memcpy(c->str + c_offset, replacement, (size_t)(rlen));

		c->len -= tlen - rlen;
		c_offset += rlen;
	}

	return 1;
}

#endif

/* ========================= Containers ======================== */

#include <stdlib.h>
#include <string.h>

cu_list *cu_list_init(cu_list *l, uptr sizeof_element)
{
	l->elem = sizeof_element;
	l->data = NULL;
	l->cap = 0;
	l->len = 0;
	return l;
}
int cu_list_reserve(cu_list *l, uptr cap)
{
	void *rlc;
	if (cap < l->cap) return 1;
	rlc = realloc(l->data, (l->cap = cu_npow2(cap)) * l->elem);
	if (!rlc) return 0;
	l->data = rlc;
	return 1;
}
cu_list *cu_list_clear(cu_list *l)
{
	free(l->data);
	l->data = NULL;
	l->cap = 0;
	l->len = 0;
	return l;
}
void *cu_list_at(cu_list *l, uptr ind)
{
	return ((u8 *)l->data) + (ind * l->elem);
}
void *cu_list_atsf(cu_list *l, uptr ind)
{
	if (CU_UNLIKELY(ind >= l->len)) return NULL;
	else return cu_list_at(l, ind);
}
int cu_list_insert(cu_list *CU_RESTRICT l, void *CU_RESTRICT elems, uptr nelems, uptr ind)
{
	u8 *indp;
	if (CU_UNLIKELY(ind > l->len)) return 0;
	if (l->len + nelems > l->cap && !CU_UNLIKELY(cu_list_reserve(l, l->cap + nelems))) return 0;

	indp = ((u8 *)l->data) + (ind * l->elem);
	memmove(indp + (nelems * l->elem), indp, (l->len - ind) * l->elem);
	memcpy(indp, elems, nelems * l->elem);

	l->len += nelems;

	return 1;
}
int cu_list_add(cu_list *CU_RESTRICT l, void *CU_RESTRICT elem)
{
	return cu_list_insert(l, elem, 1, l->len);
}
void cu_list_cut(cu_list *l, uptr ind, uptr count)
{
	if (CU_UNLIKELY(ind >= l->len)) return;
	if (CU_UNLIKELY(ind + count > l->len)) count = l->len - ind;
	memmove(((u8 *)l->data) + (ind * l->elem), ((u8 *)l->data) + ((ind + count) * l->elem), (ind + count) * l->elem);
	l->len -= count;
}

cu_hmap *cu_hmap_init(cu_hmap *h, cu_hmap_equalfunc equalfunc, cu_hmap_hashfunc hashfunc)
{
	memset(h->buckets, 0, sizeof h->buckets);
	h->equalfunc = equalfunc;
	h->hashfunc = hashfunc;
	return h;
}
cu_hmap *cu_hmap_clear(cu_hmap *CU_RESTRICT h, int free_keys, int free_data)
{
	size_t i;
	for (i = 0; i < sizeof h->buckets / sizeof *h->buckets; ++i) {
		cu_hmap_element *e, *next;
		for (e = h->buckets[i]; e; e = next) {
			next = e->next;
			if (free_data) free(e->data);
			if (free_keys) free(e->key);
			free(e);
		}
		h->buckets[i] = NULL;
	}
	return h;
}

int cu_hmap_add(cu_hmap *CU_RESTRICT h, void *data, void *key)
{
	cu_hmap_element **e, *toadd = (cu_hmap_element *)malloc(sizeof *toadd);
	if (!toadd) return 0;

	toadd->data = data;
	toadd->key = key;
	toadd->next = NULL;

	e = h->buckets + (h->hashfunc(key) % (sizeof h->buckets / sizeof *h->buckets));

	while (*e) e = &(*e)->next;
	*e = toadd;

	return 1;
}
void *cu_hmap_find(cu_hmap *CU_RESTRICT h, const void *CU_RESTRICT key)
{
	cu_hmap_element *e;
	for (e = h->buckets[h->hashfunc(key) % (sizeof h->buckets / sizeof *h->buckets)]; e; e = e->next) {
		if (h->equalfunc(key, e->key)) return e->data;	
	}
	return NULL;
}
void *cu_hmap_remove(cu_hmap *CU_RESTRICT h, const void *CU_RESTRICT key, int free_key)
{
	cu_hmap_element **e;
	for (e = h->buckets + (h->hashfunc(key) % (sizeof h->buckets / sizeof *h->buckets)); *e; e = &(*e)->next) {
		if (h->equalfunc(key, (*e)->key)) {
			void *saved = (*e)->data;
			cu_hmap_element *next = (*e)->next;
			if (free_key) free((*e)->key);
			free(*e);
			*e = next;
			return saved;
		}
	}
	return NULL;
}

cu_hmap *cu_hmap_iterate(cu_hmap *h, void *user, void (*work_func)(const void *key, void *data, void *user))
{
	size_t i;
	for (i = 0; i < sizeof h->buckets / sizeof *h->buckets; ++i) {
		cu_hmap_element *e;
		for (e = h->buckets[i]; e; e = e->next) work_func(e->key, e->data, user);
	}
	return h;
}

/* ========================= Filesystem ======================== */

#if CU_SETTING_FILE_FUNCS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>

#if CU_OS_WINDOWS
#  include <io.h>
#  include <direct.h>
#  include <windows.h>
#  include <sys/types.h>
#  define cu_rmdir _rmdir
#  define cu_fileno _fileno
#  define cu_mkdir(path, mode) _mkdir(path)
#  define cu_fstat _fstati64
#  define cu_stat _stati64
#  define F_OK 0
#else
#  include <unistd.h>
#  include <dirent.h>
#  define cu_rmdir rmdir
#  define cu_fileno fileno
#  define cu_mkdir(path, mode) mkdir(path, mode)
#  define cu_fstat fstat
#  define cu_stat stat
#endif

#if !CU_COMP_MSVC
#  define cu_fopen(file, path, mode) (!(file = fopen(path, mode)))
#else
#  define cu_fopen(file, path, mode) (fopen_s(&file, path, mode) != 0)
#endif

int cu_file_exists(const char *path)
{
	struct cu_stat dir_stat;
	return cu_stat(path, &dir_stat) == 0 && ((dir_stat.st_mode & S_IFREG) == S_IFREG);
}

int cu_dir_exists(const char *path)
{
	struct cu_stat dir_stat;
	return cu_stat(path, &dir_stat) == 0 && ((dir_stat.st_mode & S_IFDIR) == S_IFDIR);
}

int cu_dir_create(const char *path)
{
	return cu_mkdir(path, 0x1FF) == 0;
}

char **cu_dir_list(const char *path, int fullname, int *count)
{
#if CU_OS_WINDOWS
	size_t dbufcap = 4, dbufcnt = 0, pathlen = strlen(path), nosep = path[pathlen - 1] != CU_FILE_SEPARATOR;
	char **dbuf, *findpat;
	WIN32_FIND_DATA fd;
	HANDLE dir;

	if (!(dbuf = (char **)malloc(dbufcap * sizeof *dbuf)) || !(findpat = (char *)malloc(pathlen + 4))) {
		free(dbuf);
		return NULL;
	}

	memcpy(findpat, path, pathlen);
	findpat[pathlen + 0] = CU_FILE_SEPARATOR;
	findpat[pathlen + 1] = '*';
	findpat[pathlen + 2] = '\0';
	findpat[pathlen + 3] = '\0';

	if (CU_UNLIKELY((dir = FindFirstFile(findpat, &fd)) == INVALID_HANDLE_VALUE)) goto fail;

	while ((FindNextFile(dir, &fd))) {
		size_t entrylen;
		char *cbuf;
		if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0) continue;

		if (dbufcnt >= dbufcap) {
			void *rbuf = realloc(dbuf, (dbufcap *= 2) * sizeof *dbuf);
			if (!rbuf) goto fail;
			dbuf = rbuf;
		}

		entrylen = strlen(fd.cFileName) + 1;
		if (!(cbuf = dbuf[dbufcnt++] = malloc(entrylen + (size_t)(fullname ? pathlen : 0) + nosep))) goto fail;

		if (fullname) {
			memcpy(cbuf, findpat, pathlen + 1);
			cbuf += pathlen + 1;
		}

		memcpy(cbuf, fd.cFileName, entrylen);

	}

	free(findpat);
	*count = (int)dbufcnt;
	FindClose(dir);

	return dbuf;
fail:
	free(findpat);
	if (dir != INVALID_HANDLE_VALUE) FindClose(dir);
	cu_dir_close(dbuf, (int)dbufcnt);
	return NULL;
#else
	size_t dbufcap = 4, dbufcnt = 0, pathlen = strlen(path), nosep = path[pathlen - 1] != CU_FILE_SEPARATOR;
	char **dbuf = (char **)malloc(dbufcap * sizeof *dbuf);
	struct dirent *entry;
	DIR *dir = NULL;

	if (CU_UNLIKELY(!dbuf || !(dir = opendir(path)))) goto fail;

	while ((entry = readdir(dir))) {
		size_t entrylen;
		char *cbuf;
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

		if (dbufcnt >= dbufcap) {
			void *rbuf = realloc(dbuf, (dbufcap *= 2) * sizeof *dbuf);
			if (!rbuf) goto fail;
			dbuf = (char **)rbuf;
		}

		entrylen = strlen(entry->d_name) + 1;
		if (!(cbuf = dbuf[dbufcnt++] = (char *)malloc(entrylen + (size_t)(fullname ? pathlen : 0) + nosep))) goto fail;

		if (fullname) {
			memcpy(cbuf, path, pathlen);
			cbuf += pathlen;
			if (nosep) *cbuf++ = CU_FILE_SEPARATOR;
		}

		memcpy(cbuf, entry->d_name, entrylen);
	}

	*count = (int)dbufcnt;
	closedir(dir);
	return dbuf;
fail:
	cu_dir_close(dbuf, (int)dbufcnt);
	return NULL;
#endif
}

void cu_dir_close(char **dirlist, int count)
{
	int i;
	for (i = 0; i < count; ++i) free(dirlist[i]);
	free(dirlist);
}

void *cu_file_read(const char *CU_RESTRICT path, void *CU_RESTRICT result, int binary_file, uptr *CU_RESTRICT bytes)
{
	uptr to_read, filesize;
	struct cu_stat file_stat;
	FILE *file;
	int res = 0;

	if (CU_UNLIKELY(cu_fopen(file, path, binary_file ? "rb" : "r") != 0)) return 0;
	if (CU_UNLIKELY(cu_fstat(cu_fileno(file), &file_stat) == -1)) goto fail;

	filesize = (uptr)file_stat.st_size;
	to_read = (bytes && *bytes != 0) ? (*bytes > filesize ? filesize : *bytes) : filesize;

	if (!result && !(result = malloc((size_t)(filesize) + !binary_file))) goto fail;
	res = fread(result, 1, (size_t)to_read, file) == (size_t)to_read;

	if (!binary_file) ((char *)result)[filesize] = '\0';
	if (bytes) *bytes = filesize;
fail:
	fclose(file);
	return res ? result : NULL;
}

int cu_file_write(const char *CU_RESTRICT path, const void *CU_RESTRICT content, unsigned int mode, uptr bytes)
{
	static const char *mode_strs[] = { "w", "wb", "a", "ab" };
	FILE *file; int res;

	if (CU_UNLIKELY(mode > 3)) return 0;
	if (CU_UNLIKELY(cu_fopen(file, path, mode_strs[mode]))) return 0;

	res = fwrite(content, (size_t)bytes, 1, file) == 1;
	fclose(file);

	return res;
}

int cu_file_getinfo(const char *CU_RESTRICT path, cu_file_info *CU_RESTRICT f_info)
{
	struct cu_stat dir_stat;
	if (cu_stat(path, &dir_stat) == -1) return 0;

	f_info->fsize_bytes = (u64)dir_stat.st_size;
	f_info->access_time = (i64)dir_stat.st_atime;
	f_info->mod_time = (i64)dir_stat.st_mtime;
	f_info->create_time = (i64)dir_stat.st_ctime;

	return 1;
}

char *cu_file_exe_path(const char *CU_RESTRICT argv, uptr *CU_RESTRICT allocd)
{
#if CU_OS_WINDOWS
	DWORD res;
	char *buf = (char *)malloc(CU_PATH_MAX);
	CU_UNUSED(argv);
	if (!buf) return NULL;
	if (!(res = GetModuleFileName(NULL, buf, CU_PATH_MAX))) {
		free(buf);
		buf = NULL;
	} else if (allocd) *allocd = CU_PATH_MAX;
	return buf;
#elif CU_OS_UNIX
	struct stat s;
	char *buf;
	size_t len;

	if (argv[0] == '/') {
		len = strlen(argv) + 1;
		buf = (char *)malloc(len);
		if (!buf) return NULL;
		memcpy(buf, argv, len);
	} else if (lstat(argv, &s) != -1 && S_ISLNK(s.st_mode)) {
		size_t buflen = 0;
		buf = NULL;

		do {
			void *res = realloc(buf, buflen += CU_PATH_MAX);
			if (!res) goto fail_readlink;
			buf = (char *)res;

			len = (size_t)readlink(argv, buf, buflen);
			if (len == CU_UPTRMAX) goto fail_readlink;
			if ((size_t)len < buflen) break;
		} while (1);

		buf[len++] = '\0';
	fail_readlink:
		free(buf);
		buf = NULL;
	} else if (cu_strchr(argv, '/')) {
		uptr cwdlen = 0, argvlenterm = strlen(argv) + 1;
		len = CU_PATH_MAX * 2;
		buf = (char *)malloc(len);

		if (buf && getcwd(buf, len) && (cwdlen = strlen(buf)) < (len - argvlenterm)) {
			memcpy(buf + cwdlen, argv + (argv[0] == '.' && argv[1] == '/'), argvlenterm);
		} else {
			free(buf);
			buf = NULL;
		}
	} else if ((buf = getenv(argv))) {
		char *svenv = buf;
		len = strlen(buf) + 1;
		buf = (char *)malloc(len);
		if (!buf) return NULL;
		memcpy(buf, svenv, len);
	} else return NULL;

	if (allocd && buf) *allocd = (uptr)len;
	return buf;
#else
	size_t len = strlen(argv) + 1;
	char *buf = (char *)malloc(len);
	if (!buf) return NULL;
	memcpy(buf, argv, len);
	if (allocd) *allocd = (uptr)len;
	return buf;
#endif
}

int cu_file_delete(const char *path)
{
	return remove(path) == 0;
}

int cu_dir_delete(const char *path, int rm_contents)
{
	int dircnt, i;
	char **dirobj;

	if (!rm_contents) return cu_rmdir(path) == 0;

	dirobj = cu_dir_list(path, 1, &dircnt);
	if (!dirobj) return 0;

	for (i = 0; i < dircnt; ++i) {
		if (cu_dir_exists(dirobj[i]) && !cu_dir_delete(dirobj[i], 1)) break;
		else if (!cu_file_delete(dirobj[i])) break;
	}

	cu_dir_close(dirobj, dircnt);
	return 1;
}

#endif

/* ========================= Resources ======================== */

#if CU_SETTING_RESOURCES_FUNCS

#include <string.h>
#include <stdio.h>
#include <time.h>

#if CU_OS_MAC
#  include <mach/vm_statistics.h>
#  include <mach/mach_types.h>
#  include <mach/mach_init.h>
#  include <mach/mach_host.h>
#  include <mach/mach.h>
#  include <sys/sysctl.h>
#  include <sys/types.h>

int sysctlbyname(const char *, void *, size_t *, void *, size_t);

CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1)) static void cu_res_cpuinfo_sysctl(const char *name, void *buf, size_t bufsz)
{
	sysctlbyname(name, buf, &bufsz, NULL, 0);
}
#elif CU_OS_UNIX && CU_HAS_INCLUDE(<sys/sysinfo.h>)
#  include <sys/sysinfo.h>
#endif

#if CU_OS_UNIX
#  include <sys/utsname.h>
#  include <sys/times.h>
#  include <string.h>
#  include <unistd.h>
#  include <errno.h>
#  include <pwd.h>
#  if !CU_OS_MAC
#    define CPUDIR "/sys/devices/system/cpu/"
CU_ATTRIB_NOTHROW static u64 cu_res_cpuinfo_numfile(const char *fname)
{
	u64 res = 0;
	FILE *f = fopen(fname, "r");
	if (!f) return 0;
	fscanf(f, "%" CU_U64_FMT, &res);
	fclose(f);
	return res;
}
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1)) static void cu_res_cpuinfo_dircache(struct cu_res_cpu_cache *target, int index)
{
	#define CPUCACHE CPUDIR "cpu0/cache/index0/"
	#define CPUASSOC CPUCACHE "ways_of_associativity"
	#define CPULINE "coherency_line_size"
	#define CPUSIZE "size"

	char cachedir[sizeof CPUASSOC];
	memcpy(cachedir, CPUASSOC, sizeof CPUASSOC);
	cachedir[sizeof CPUCACHE - 3] = '0' + (char)index;

	target->assoc = (i32)cu_res_cpuinfo_numfile(cachedir);
	memcpy(cachedir + sizeof CPUCACHE - 1, CPULINE, sizeof CPULINE);
	target->line = (u32)cu_res_cpuinfo_numfile(cachedir);
	memcpy(cachedir + sizeof CPUCACHE - 1, CPUSIZE, sizeof CPUSIZE);
	target->size = (u32)cu_res_cpuinfo_numfile(cachedir) * 1024;
}
CU_ATTRIB_NOTHROW static i64 cu_res_cpuinfo_sysconf(int n)
{
	long res = sysconf(n);
	return n == -1 ? 0 : res;
}
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1)) static void cu_res_cpuinfo_syscache(struct cu_res_cpu_cache *target, int size_nproc, int index)
{
	cu_res_cpuinfo_dircache(target, index);
	if (!target->size) target->size = (u32)cu_res_cpuinfo_sysconf(size_nproc);
	errno = 0;
	if (!target->assoc) target->assoc = (i32)cu_res_cpuinfo_sysconf(size_nproc + 1);
	if (errno || !target->size) target->assoc = 0;
	if (!target->line) target->line = (u32)cu_res_cpuinfo_sysconf(size_nproc + 2);
}
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((2)) static void cu_res_cpuinfo_fsearch(FILE *f, const char *fmt, u32 *target, char *starget, int highest)
{
	char lnbuf[384], *getres;
	u32 targetex = 0;
	int scanres;
	if (!f) return;
	if (target) *target = 0;

	while (1) {
		getres = fgets(lnbuf, sizeof lnbuf, f);
		if (!getres) return;

	CU_DIAGNOSTICS_PUSH
	CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS
	CU_PRAGMA(GCC diagnostic ignored "-Wformat-nonliteral")
		scanres = sscanf(lnbuf, fmt, starget ? (void *)starget : (void *)&targetex);
	CU_DIAGNOSTICS_POP
		if (scanres == EOF) return;
		else if (scanres != 1) continue;

		if (highest) {
			if (targetex > *target) *target = targetex;
			continue;
		} else if (target) *target = targetex;

		return;
	}
}
#  endif
#elif CU_OS_WINDOWS
#  include <ntsecapi.h>
#  include <powrprof.h>
#  include <windows.h>
#  include <psapi.h>
typedef struct _PROCESSOR_POWER_INFORMATION {
	ULONG Number, MaxMhz, CurrentMhz, MhzLimit, MaxIdleState, CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;
CU_ATTRIB_NOTHROW static CU_ATTRIB_CONST u32 cu_res_winpopcnt(ULONG_PTR v)
{
	u32 n;
	for (n = 0; v; ++n, v &= v - 1);
	return n;
}
#endif

#if !CU_COMP_MSVC
#  define cu_sscanf(str, fmt, a1) sscanf(str, fmt, a1)
#  define cu_sprintf(str, max, fmt, a1) sprintf(str, fmt, a1)
#else
#  define cu_sscanf(str, fmt, a1) sscanf_s(str, fmt, a1)
#  define cu_sprintf(str, max, fmt, a1) sprintf_s(str, max, fmt, a1)
#  pragma comment(lib, "Psapi.lib")
#  pragma comment(lib, "Advapi32.lib")
#  pragma comment(lib, "PowrProf.lib")
#endif

char *cu_res_bytefmt(char *str, u64 bytes)
{
	static const char fmt_suffix[6] = { 'K', 'M', 'G', 'T', 'P', 'E' };
	int ndown = 0;
	char *start = str;
	double res = (double)bytes;
	while (res >= 1000 || ndown % 3) { res /= 10; ++ndown; }
	str += cu_sprintf(str, 6, "%.1f", res);
	if (ndown) *str++ = fmt_suffix[(ndown - 3) / 3];
	else str -= 2;
	*str++ = 'B';
	*str++ = '\0';
	return start;
}

uptr cu_res_crypto(void *data, uptr bytes)
{
#if CU_OS_UNIX
	FILE *f;
	size_t n;
	if (CU_UNLIKELY(!(f = fopen("/dev/urandom", "r")))) return 0;
	n = fread(data, 1, bytes, f);
	fclose(f);
	return n;
#elif CU_OS_WINDOWS
	return RtlGenRandom(data, (ULONG)bytes) ? bytes : 0;
#else
	CU_UNUSED(data); CU_UNUSED(bytes);
	return 0;
#endif
}

int cu_res_meminfo(cu_res_mem *info)
{
#if CU_OS_MAC
	mach_port_t mach_port = mach_host_self();
	mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
	vm_statistics64_data_t vm_stats;

	struct task_basic_info t_info;
	mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

	struct xsw_usage swap;
	size_t size = sizeof swap;

	int success = 1;
	int mib[2] = { CTL_HW, HW_MEMSIZE };
	int64_t physical_memory = 0;
	size_t length = sizeof physical_memory;

	memset(info, 0, sizeof *info);
	sysctl(mib, 2, &physical_memory, &length, NULL, 0);

	info->physical_present = (u64)physical_memory;

	if (host_statistics64(mach_port, HOST_VM_INFO64_COUNT, (host_info64_t)&vm_stats, &count) == KERN_SUCCESS) {
		vm_size_t page_size = 16384;
		host_page_size(mach_port, &page_size);
		info->physical_free = (u64)vm_stats.free_count * (u64)page_size;
	} else success = 0;

	if (sysctlbyname("vm.swapusage", &swap, &size, NULL, 0) == 0) {
		info->virtual_present = info->physical_present + (u64)swap.xsu_total;
		info->virtual_free = info->physical_free + (u64)swap.xsu_avail;
	} else success = 0;

	if (task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count) == KERN_SUCCESS) {
		info->physical_used = t_info.resident_size;
		info->virtual_used = t_info.virtual_size;
	} else success = 0;

	return success;
#elif CU_OS_UNIX && CU_HAS_INCLUDE(<sys/sysinfo.h>)
	struct sysinfo si;
	int success = 1;
	FILE *f;

	memset(info, 0, sizeof *info);

	if (sysinfo(&si) == 0) {
		info->physical_present = (u64)(si.totalram);
		info->physical_free = (u64)(si.freeram);
		info->virtual_present = (u64)(si.totalram) + (u64)(si.totalswap);
		info->virtual_free = (u64)(si.freeram) + (u64)(si.freeswap);
	} else success = 0;

	if (CU_LIKELY((f = fopen("/proc/self/statm", "r")))) {
		fscanf(f, "%" CU_U64_FMT " %" CU_U64_FMT, &info->virtual_used, &info->physical_used);
		fclose(f);
	} else success = 0;

	return success;
#elif CU_OS_WINDOWS
	int success = 1;
	MEMORYSTATUSEX mstat;
	PROCESS_MEMORY_COUNTERS_EX pmc;
	mstat.dwLength = sizeof mstat;

	memset(info, 0, sizeof *info);

	if (GlobalMemoryStatusEx(&mstat)) {
		info->physical_present = (u64)mstat.ullTotalPhys;
		info->physical_free = (u64)mstat.ullAvailPhys;
		info->virtual_present = (u64)mstat.ullTotalPageFile;
		info->virtual_free = (u64)mstat.ullAvailPageFile;
	} else success = 0;

	if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof pmc)) {
		info->physical_used = pmc.WorkingSetSize;
		info->virtual_used = pmc.PrivateUsage;
	} else success = 0;

	return success;
#else
	CU_UNUSED(info);
	return 0;
#endif
}

real64 cu_res_cpuusage(void)
{
#if CU_OS_UNIX
	static int proc_count = 0;
	static clock_t prev_rtime = 0, prev_stime = 0, prev_utime = 0;
	struct tms tbuf;
	clock_t rtime;
	real64 res;

	if ((rtime = times(&tbuf)) == (clock_t)(-1)) return -1.0;
#if CU_SETTING_THREAD_FUNCS
	if (!proc_count) proc_count = cu_thread_count();
#else
	proc_count = 1;
#endif

	res = (double)((tbuf.tms_stime - prev_stime) + (tbuf.tms_utime - prev_utime)) / (0.01 * (double)(rtime - prev_rtime) * proc_count);

	prev_rtime = rtime;
	prev_stime = tbuf.tms_stime;
	prev_utime = tbuf.tms_utime;

	return res;
#elif CU_OS_WINDOWS
	static ULARGE_INTEGER ltm_cpu, ltm_scpu, ltm_ucpu;
	static HANDLE cpu_handle;
	static int proc_count = 0;

	FILETIME ft_now, ft_sys, ft_user;
	ULARGE_INTEGER now, sys, user;
	real64 res;

	if (!proc_count) {
		proc_count = cu_thread_count();
		cpu_handle = GetCurrentProcess();
	}

	GetSystemTimeAsFileTime(&ft_now);
	memcpy(&now, &ft_now, sizeof ft_now);
	GetProcessTimes(cpu_handle, &ft_now, &ft_now, &ft_sys, &ft_user);
	memcpy(&sys, &ft_sys, sizeof ft_sys);
	memcpy(&user, &ft_user, sizeof ft_user);

	res = (double)((sys.QuadPart - ltm_scpu.QuadPart) + (user.QuadPart - ltm_ucpu.QuadPart)) / (0.01 * (double)(now.QuadPart - ltm_cpu.QuadPart) * proc_count);

	ltm_cpu = now;
	ltm_ucpu = user;
	ltm_scpu = sys;

	return res;
#else
	return (real64)0;
#endif
}

#if CU_ARCH_X86
#  define CU_RES_CPUINFO_REGS { 0, 0, 0, 0 }
#  if CU_COMP_MSVC
#    include <intrin.h>
#    define cu_rdtsc() __rdtsc()
#  else
#    include <cpuid.h>
CU_ATTRIB_WARN_UNUSED_RESULT CU_ATTRIB_NOTHROW static u64 cu_rdtsc(void)
{
	u32 low, high;
	CU_ASM volatile ("rdtsc" : "=a" (low), "=d" (high));
	return CU_UPSHIFT((u64)high, 32) | low;
}
#  endif
#  define cu_cpuid(leaf, subleaf) __cpuidex((int *)regs, (int)(leaf), (int)(subleaf))
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1)) static void cu_res_cpuinfo_cpuid0(cu_res_cpu* info, int base, int vendor)
{
	u32 regs[4] = CU_RES_CPUINFO_REGS;
	cu_cpuid(0x0, 0);

	if (base) info->cpuid_base_max = regs[0];
	if (vendor) {
		memcpy(info->vendor + 0, regs + 1, 4);
		memcpy(info->vendor + 4, regs + 3, 4);
		memcpy(info->vendor + 8, regs + 2, 4);
		info->vendor_id = !strcmp(info->vendor, "GenuineIntel") ? 1 : (!strcmp(info->vendor, "AuthenticAMD") ? 2 : 0);
	}
}
#endif

CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1)) static void cu_res_cpuinfo_idfeatures(cu_res_cpu *info, FILE *cpuf)
{
#if CU_ARCH_X86
	u32 regs[4] = CU_RES_CPUINFO_REGS, i;
#endif

#if CU_OS_MAC
	cu_res_cpuinfo_sysctl("machdep.cpu.brand_string", info->name, sizeof info->name);
	CU_UNUSED(cpuf);
#elif CU_OS_UNIX
#  if CU_ARCH_X86
	cu_res_cpuinfo_fsearch(cpuf, "model name : %51[^\n]", NULL, info->name, 0);
#  else
	cu_res_cpuinfo_fsearch(cpuf, "Model : %51[^\n]", NULL, info->name, 0);
	if (!info->name) cu_res_cpuinfo_fsearch(cpuf, "Processor : %51[^\n]", NULL, info->name, 0);
	if (!info->name) cu_res_cpuinfo_fsearch(cpuf, "model name : %51[^\n]", NULL, info->name, 0);
	if (!info->name) {
		FILE *devtree = fopen("/sys/firmware/devicetree/base/model", "r");
		if (devtree) {
			cu_res_cpuinfo_fsearch(devtree, "%51s", NULL, info->name, 0);
			fclose(devtree);
		}
	}
#  endif
#elif CU_OS_WINDOWS
	HKEY hkey;
	LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hkey);
	if (result == ERROR_SUCCESS) {
		DWORD namebytes = sizeof info->name;
		result = RegQueryValueExA(hkey, "ProcessorNameString", NULL, NULL, (LPBYTE)info->name, &namebytes);
		RegCloseKey(hkey);
	}
	CU_UNUSED(cpuf);
#else
	CU_UNUSED(cpuf);
#endif

#if CU_ARCH_X86
	cu_res_cpuinfo_cpuid0(info, 1, 1);
	if (info->cpuid_base_max >= 1) {
		cu_cpuid(0x1, 0);
		info->stepping_id = (u32)(CU_BITSOF(regs[0], 0, 3));
		info->family_id = (u32)(CU_BITSOF(regs[0], 8, 11));
		info->model_id = (u32)(CU_BITSOF(regs[0], 4, 7) + ((info->family_id == 6 || info->family_id == 15) * CU_UPSHIFT(CU_BITSOF(regs[0], 16, 19), 4)));
		info->family_id = (u32)(info->family_id + (CU_BITSOF(regs[0], 20, 27) * (u32)(info->family_id == 15)));
		info->fx86_ecx1 = regs[2];
		info->fx86_edx1 = regs[3];
	}

	if (info->cpuid_base_max >= 1) {
		cu_cpuid(0x7, 0);
		info->fx86_ebx7 = regs[1];
		info->fx86_ecx7 = regs[2];
	}

	cu_cpuid(0x80000000u, 0);
	info->cpuid_ext_max = regs[0];

	if (info->cpuid_ext_max >= 0x80000001) {
		cu_cpuid(0x80000001u, 0);
		info->fx86_ecx81 = regs[2];
		info->fx86_edx81 = regs[3];
	}

	if (info->cpuid_ext_max >= 0x80000004) {
		for (i = 0; i < 3; ++i) {
			cu_cpuid(0x80000002u + i, 0);
			memcpy(info->name + i * 16, regs, 16);
		}
	}
#else
	CU_UNUSED(info);
#endif
}

CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1)) static void cu_res_cpuinfo_corecache(cu_res_cpu *info, FILE *cpuf)
{
#if CU_ARCH_X86
	u32 regs[4] = CU_RES_CPUINFO_REGS, i;
#endif

#if CU_OS_MAC
	int64_t res;
	cu_res_cpuinfo_sysctl("hw.cachelinesize", &res, sizeof res);
	info->l1i.line = info->l1d.line = info->l2.line = info->l3.line = (u32)res;
	cu_res_cpuinfo_sysctl("hw.physicalcpu", &res, sizeof res);
	info->processor_cores = (u32)res;
	cu_res_cpuinfo_sysctl("hw.logicalcpu", &res, sizeof res);
	info->logical_processors = (u32)res;
	cu_res_cpuinfo_sysctl("hw.l1dcachesize", &res, sizeof res);
	info->l1d.size = (u32)res;
	cu_res_cpuinfo_sysctl("hw.l1icachesize", &res, sizeof res);
	info->l1i.size = (u32)res;
	cu_res_cpuinfo_sysctl("hw.l2cachesize", &res, sizeof res);
	info->l2.size = (u32)res;
	cu_res_cpuinfo_sysctl("hw.l3cachesize", &res, sizeof res);
	info->l3.size = (u32)res;
	CU_UNUSED(cpuf);
#elif CU_OS_UNIX
	int smt, firstdata, duall1, onln = (int)sysconf(_SC_NPROCESSORS_ONLN);
	FILE *typef;

	errno = 0;
	smt = (int)cu_res_cpuinfo_numfile(CPUDIR "smt/active");
	if (errno == ENOENT) smt = -1;

	if ((typef = fopen(CPUDIR "cpu0/cache/index0/type", "r"))) {
		firstdata = fgetc(typef) == 'D';
		fclose(typef);
	} else firstdata = 0;
	duall1 = cu_res_cpuinfo_numfile(CPUDIR "cpu0/cache/index1/level") == 1;

	cu_res_cpuinfo_syscache(&info->l1i, _SC_LEVEL1_ICACHE_SIZE, !firstdata);
	if (duall1) cu_res_cpuinfo_syscache(&info->l1d, _SC_LEVEL1_DCACHE_SIZE, firstdata);
	cu_res_cpuinfo_syscache(&info->l2, _SC_LEVEL2_CACHE_SIZE, 1 + duall1);
	cu_res_cpuinfo_syscache(&info->l3, _SC_LEVEL3_CACHE_SIZE, 2 + duall1);
	cu_res_cpuinfo_syscache(&info->l4, _SC_LEVEL4_CACHE_SIZE, 3 + duall1);

	if (smt != -1 && onln != -1) {
		info->processor_cores = (u32)(onln / (smt ? 2 : 1));
		info->logical_processors = (u32)onln;
	} else if (cpuf) {
		cu_res_cpuinfo_fsearch(cpuf, "cpu cores : %u", &info->processor_cores, NULL, 0);
		rewind(cpuf);
		cu_res_cpuinfo_fsearch(cpuf, "siblings : %u", &info->logical_processors, NULL, 0);

		if (!info->processor_cores && !info->logical_processors) {
			rewind(cpuf);
			cu_res_cpuinfo_fsearch(cpuf, "processor : %u", &info->logical_processors, NULL, 1);
			info->processor_cores = ++info->logical_processors / (smt == 1 ? 2 : 1);
		}
	}
#elif CU_OS_WINDOWS
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buf, ptr;
	DWORD buflen = 0, bufcnt, c;
	CU_UNUSED(cpuf);

	GetLogicalProcessorInformation(NULL, &buflen);
	if (!(buf = malloc((size_t)buflen))) goto win_fail;
	if (GetLogicalProcessorInformation(buf, &buflen) == FALSE) goto win_fail;

	for (c = 0, ptr = buf, bufcnt = buflen / sizeof *buf; c < bufcnt; ++c, ++ptr) {
		PCACHE_DESCRIPTOR cache;
		struct cu_res_cpu_cache* target;
		switch (ptr->Relationship) {
		case RelationProcessorCore:
			++info->processor_cores;
			info->logical_processors += cu_res_winpopcnt(ptr->ProcessorMask);
			break;
		case RelationCache:
			cache = &ptr->Cache;
			target = NULL;
			switch (cache->Level) {
			case 1:
				if (cache->Type == CacheData) target = &info->l1d;
				else target = &info->l1i;
				break;
			case 2:
				target = &info->l2;
				break;
			case 3:
				target = &info->l3;
				break;
			}

			if (target) {
				target->line = (u32)cache->LineSize;
				target->assoc = cache->Associativity == CACHE_FULLY_ASSOCIATIVE ? -1 : (i32)cache->Associativity;
				target->size = (u32)cache->Size;
			}

			break;
		default:
			break;
		}
	}
win_fail:
	free(buf);
#else
	CU_UNUSED(cpuf);
	CU_UNUSED(info);
#endif

#if CU_ARCH_X86
	cu_res_cpuinfo_cpuid0(info, 0, 1);

	if (info->vendor_id == 2) {
		cu_cpuid(0x80000000u, 0);
		if (regs[0] < 0x8000001Du) return;
	} else if (regs[0] < 0x4) return;

	for (i = 0; ; ++i) {
		u32 level = ((cu_cpuid(info->vendor_id == 2 ? 0x8000001Du : 0x4u, i)), ((regs[0] >> 5) & 0x7));
		struct cu_res_cpu_cache *target;
		if (level == 1) target = ((regs[0] & 0x1F) == 1) ? &info->l1d : &info->l1i;
		else if (level == 2) target = &info->l2;
		else if (level == 3) target = &info->l3;
		else if (level == 4) target = &info->l4;
		else break;

		if (!target->assoc) target->assoc = (regs[1] >> 22) + 1;
		if (!target->line) target->line = (regs[1] & 0xFFF) + 1;
		if (!target->size) target->size = (((regs[1] >> 22) & 0x3FF) + 1) * (((regs[1] >> 12) & 0x3FF) + 1) * target->line * (regs[2] + 1);
	}
#endif
}

CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1)) static void cu_res_cpuinfo_speed(cu_res_cpu *info)
{
#if CU_ARCH_X86
	u32 regs[4] = CU_RES_CPUINFO_REGS;
#endif

#if CU_OS_MAC
	i64 freq;
	cu_res_cpuinfo_sysctl("hw.cpufrequency", &freq, sizeof freq);
	info->cur_freq_hz = freq;
	cu_res_cpuinfo_sysctl("hw.cpufrequency_max", &freq, sizeof freq);
	info->max_freq_hz = freq;
	cu_res_cpuinfo_sysctl("hw.cpufrequency_min", &freq, sizeof freq);
	info->min_freq_hz = freq;
#elif CU_OS_UNIX
	int i;
	struct { const char *fname; u64 *hzp; } cpuinfo_fs[3] = {
		{ "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", NULL },
		{ "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", NULL },
		{ "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_min_freq", NULL },
	};
	cpuinfo_fs[0].hzp = &info->cur_freq_hz;
	cpuinfo_fs[1].hzp = &info->max_freq_hz;
	cpuinfo_fs[2].hzp = &info->min_freq_hz;
	for (i = 0; i < 3; ++i) if (!*cpuinfo_fs[i].hzp) *cpuinfo_fs[i].hzp = cu_res_cpuinfo_numfile(cpuinfo_fs[i].fname) * 1000;
#elif CU_OS_WINDOWS
	PROCESSOR_POWER_INFORMATION *ppi = NULL;
	SYSTEM_INFO si;
	ULONG nbytes;
	LONG keyres;
	HKEY hkey;

	GetSystemInfo(&si);
	ppi = (PROCESSOR_POWER_INFORMATION *)malloc((size_t)(nbytes = (ULONG)(sizeof *ppi * (size_t)si.dwNumberOfProcessors)));
	if (ppi && CallNtPowerInformation(ProcessorInformation, NULL, 0, ppi, nbytes) == 0) info->max_freq_hz = (u64)ppi[0].MaxMhz * 1000 * 1000;
	free(ppi);

	keyres = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hkey);
	if (keyres == ERROR_SUCCESS) {
		DWORD bootmhz = 0, mhzbytes = sizeof bootmhz;
		keyres = RegQueryValueExA(hkey, "~MHz", NULL, NULL, (LPBYTE)&bootmhz, &mhzbytes);
		if (keyres == ERROR_SUCCESS) info->base_freq_hz = bootmhz * 1000 * 1000;
		RegCloseKey(hkey);
	}
#endif

#if CU_ARCH_X86
	cu_cpuid(0x0, 0);
	info->cpuid_base_max = *regs;

	if (info->cpuid_base_max >= 0x16) {
		cu_cpuid(0x16, 0);
		info->base_freq_hz = regs[0] * 1000 * 1000;
		info->max_freq_hz = regs[1] * 1000 * 1000;
	}

	if (!info->base_freq_hz) {
		float base_ghz = 0.0f;
		char *base_ghz_parser = cu_strchr(info->name, '@');
		if (base_ghz_parser) {
			CU_UNUSED(cu_sscanf(base_ghz_parser, "%*s%f", &base_ghz));
			info->base_freq_hz = (u64)(base_ghz * 1000.0f) * 1000 * 1000;
		}
	}

	if (!info->max_freq_hz) {
		double elapsed;
		clock_t ts_start = clock();
		u64 start = cu_rdtsc();
		do elapsed = (double)(clock() - ts_start) / CLOCKS_PER_SEC; while (elapsed < 0.01);
		info->max_freq_hz = (u64)((double)(cu_rdtsc() - start) / elapsed);
	}
#elif CU_SETTING_TIME_FUNCS
	if (!info->max_freq_hz) {
		double elapsed;
		clock_t ts_start = clock();
		cu_timer start, end;
		cu_timer_fill(&start);
		do elapsed = (double)(clock() - ts_start) / CLOCKS_PER_SEC; while (elapsed < 0.01);
		cu_timer_fill(&end);
		info->max_freq_hz = (u64)((double)(cu_timer_dif(&start, &end)) / elapsed);
	}
#endif
}

void cu_res_cpuinfo(cu_res_cpu *info, int idfeatures, int corecache, int speed)
{
	FILE *cpuf = NULL;
#if CU_OS_UNIX && !CU_OS_MAC
	cpuf = fopen("/proc/cpuinfo", "r");
#endif
	memset(info, 0, sizeof *info);
#if CU_ARCH_X86
	info->arch = 1;
#elif CU_ARCH_ARM
	info->arch = 2;
#else
	info->arch = 0;
#endif

	if (idfeatures) cu_res_cpuinfo_idfeatures(info, cpuf);
	if (corecache) cu_res_cpuinfo_corecache(info, cpuf);
	if (speed) cu_res_cpuinfo_speed(info);

	if (cpuf) fclose(cpuf);
}

uptr cu_res_osname(char *namebuf)
{
#if CU_OS_UNIX
	struct utsname ubuf;
	if (uname(&ubuf) < 0) return 0;
	if (namebuf) sprintf(namebuf, "%s %s", ubuf.sysname, ubuf.release);
	return (uptr)(strlen(ubuf.sysname) + strlen(ubuf.release) + 2);
#elif CU_OS_WINDOWS
	DWORD dsize = namebuf ? CU_RES_NAME_MAXSIZE : 0, off = 0;
	LONG res = RegGetValue(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion", "ProductName", RRF_RT_REG_SZ, NULL, namebuf, &dsize);
	if (res != ERROR_SUCCESS) return 0;
	off = dsize;
	dsize = CU_RES_NAME_MAXSIZE - off;
	if (namebuf) namebuf[off - 1] = ' ';
	res = RegGetValue(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion", "DisplayVersion", RRF_RT_REG_SZ, NULL, namebuf ? namebuf + off : NULL, &dsize);
	return res != ERROR_SUCCESS ? 0 : off + dsize;
#else
	CU_UNUSED(namebuf);
	return 0;
#endif
}

uptr cu_res_hostname(char *namebuf)
{
#if CU_OS_UNIX
	char tbuf[CU_RES_NAME_MAXSIZE];
	if (!namebuf) namebuf = tbuf;
	if (gethostname(namebuf, CU_RES_NAME_MAXSIZE)) return 0;
	return strlen(namebuf) + 1;
#elif CU_OS_WINDOWS
	DWORD len = namebuf ? CU_RES_NAME_MAXSIZE : 1;
	char lbuf;
	if (!GetComputerNameEx(ComputerNamePhysicalDnsHostname, namebuf ? namebuf : &lbuf, &len)) return 0;
	return (uptr)len + 1;
#else
	CU_UNUSED(namebuf);
	return 0;
#endif
}


uptr cu_res_username(char *namebuf)
{
#if CU_OS_UNIX
	struct passwd *p = getpwuid(geteuid());
	if (p) {
		size_t tlen = strlen(p->pw_name) + 1;
		if (namebuf) memcpy(namebuf, p->pw_name, tlen);
		return tlen;
	} else return 0;
#elif CU_OS_WINDOWS
	DWORD len = namebuf ? CU_RES_NAME_MAXSIZE : 1;
	char lbuf;
	if (!GetUserNameA(namebuf ? namebuf : &lbuf, &len)) return 0;
	return (uptr)len;
#else
	CU_UNUSED(namebuf);
	return 0;
#endif
}

#endif

/* ========================= Time ======================== */

#if CU_SETTING_TIME_FUNCS

#include <time.h>

#if CU_OS_MAC
#  include <mach/clock.h>
#  include <mach/mach.h>
#elif CU_OS_UNIX
#  include <sys/time.h>
#elif CU_OS_WINDOWS
   static LARGE_INTEGER cu_timer_freq;
#  include <windows.h>
#  include <stdlib.h>
#endif

#if CU_LANG_C >= CU_LANG_C23
#  if CU_COMP_MSVC
#    define cu_tzset() _tzset()
#  else
#    define cu_tzset() tzset()
#  endif
#endif

void cu_time_now(cu_ctime *tm)
{
	cu_time_date(tm, (i64)time(NULL));
	cu_time_subsec(tm);
}

void cu_time_date(cu_ctime *tm, i64 timestamp)
{
	time_t now_val = (time_t)timestamp;
	struct tm *now;

#if CU_OS_WINDOWS
	static char _cu_windows_tzname[32];
	TIME_ZONE_INFORMATION tzinfo;
	size_t i;
#endif

#if CU_LANG_C >= CU_LANG_C23
	struct tm now_st;
	cu_tzset();
	localtime_r(&now_val, &now_st);
	now = &now_st;
#elif CU_COMP_MSVC
	struct tm now_st;
	CU_UNUSED(localtime_s(&now_st, &now_val));
	now = &now_st;
#elif CU_LANG_C11 && defined(__STDC_LIB_EXT1__)
	struct tm now_st;
	now = localtime_s(&now_val, &now_st);
	now = &now_st;
#elif CU_SETTING_THREAD_FUNCS
	struct tm now_cp;
	cu_thread_mutex mut;
	cu_thread_mutex_init(&mut);
	cu_thread_mutex_lock(&mut);
	now_cp = *localtime(&now_val);
	cu_thread_mutex_unlock(&mut);
	cu_thread_mutex_destroy(&mut);
	now = &now_cp;
#else
	now = localtime(&now_val);
#endif

	tm->second = now->tm_sec;
	tm->minute = now->tm_min;
	tm->hour = now->tm_hour;
	tm->month_day = now->tm_mday;
	tm->month = now->tm_mon;
	tm->year = now->tm_year + 1900;
	tm->week_day = now->tm_wday;
	tm->year_day = now->tm_yday;
	tm->isdst = now->tm_isdst;

#if CU_OS_UNIX && defined(__USE_MISC)
	tm->tznm = now->tm_zone;
	tm->utcdif = (int)now->tm_gmtoff;
#elif CU_OS_WINDOWS
	GetTimeZoneInformation(&tzinfo);
	wcstombs_s(&i, _cu_windows_tzname, sizeof _cu_windows_tzname, tzinfo.StandardName, sizeof _cu_windows_tzname - 1);
	tm->tznm = _cu_windows_tzname;
	tm->utcdif = (int)(-tzinfo.Bias * 60);
#else
	tm->tznm = NULL;
	tm->utcdif = 0;
#endif
}

void cu_time_subsec(cu_ctime *tm)
{
#if CU_OS_MAC
	mach_timespec_t ts;
	clock_serv_t cserv;
	host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cserv);
	clock_get_time(cserv, &ts);
	mach_port_deallocate(mach_task_self(), cserv);
	tm->nanosec = (int)(ts.tv_nsec % 1000);
	tm->microsec = (int)((ts.tv_nsec / 1000) % 1000);
	tm->millisec = (int)((ts.tv_nsec / 1000000) % 1000);
#elif CU_OS_UNIX
	struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1) ts.tv_nsec = 0;
	tm->nanosec = (int)(ts.tv_nsec % 1000);
	tm->microsec = (int)((ts.tv_nsec / 1000) % 1000);
	tm->millisec = (int)((ts.tv_nsec / 1000000) % 1000);
#elif CU_OS_WINDOWS
	ULARGE_INTEGER uli;
	FILETIME ft;
	u64 cnsec;
	GetSystemTimePreciseAsFileTime(&ft);
	uli.LowPart = ft.dwLowDateTime;
	uli.HighPart = ft.dwHighDateTime;
	cnsec = (uli.QuadPart * 100) % 1000000000;
	tm->nanosec = (int)(cnsec % 1000);
	tm->microsec = (int)((cnsec / 1000) % 1000);
	tm->millisec = (int)((cnsec / 1000000) % 1000);
#else
	tm->nanosec = 0;
	tm->microsec = 0;
	tm->millisec = 0;
#endif
}

void cu_timer_fill(cu_timer *tm)
{
#if CU_OS_MAC
	mach_timespec_t ts;
	clock_serv_t cserv;
	host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cserv);
	clock_get_time(cserv, &ts);
	mach_port_deallocate(mach_task_self(), cserv);
	tm->nsecs = (u64)ts.tv_nsec;
	tm->secs = (u64)ts.tv_sec;
#elif CU_OS_UNIX
	struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1) ts.tv_nsec = 0;
	tm->nsecs = (u64)ts.tv_nsec;
	tm->secs = (u64)ts.tv_sec;
#elif CU_OS_WINDOWS
	LARGE_INTEGER usecs;
	if (!cu_timer_freq.QuadPart) QueryPerformanceFrequency(&cu_timer_freq);
	if (!QueryPerformanceCounter(&usecs)) usecs.QuadPart = 0;
	usecs.QuadPart *= 1000000;
	usecs.QuadPart /= cu_timer_freq.QuadPart;
	tm->secs = usecs.QuadPart / 1000000;
	tm->nsecs = usecs.QuadPart * 1000;
#else
	tm->nsecs = 0;
	tm->secs = 0;
#endif
}

u64 cu_timer_dif(const cu_timer *CU_RESTRICT start, const cu_timer *CU_RESTRICT end)
{
	return ((end->secs - start->secs) * CU_U64_C(1000000000)) + (end->nsecs - start->nsecs);
}

#endif

/* ========================= Networking ======================== */

#if CU_SETTING_NETWORK_FUNCS

#define CU_NN_NT(inds) CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL(inds)
#define CU_NET_RECVBUF 1024

typedef struct cu_net_queue
{
	void *data;
	uptr bytes;
} cu_net_queue;

typedef struct cu_net_server
{
	cu_net_remote *rems;
	uptr nclients;
} cu_net_server;

enum
{
	CU_NETMODE_WRITEABLE = 4,
	CU_NETMODE_CLOSED = 8,
	CU_NETMODE_UDPSERVER = 16
};

#include <string.h>
#include <stdlib.h>

#if CU_OS_UNIX
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <sys/types.h>
#  include <ifaddrs.h>
#  include <unistd.h>
#  include <netdb.h>
#  include <fcntl.h>
#  include <errno.h>
#  include <poll.h>
#  define cu_close close
#  define CU_EAGAIN EAGAIN
#  define CU_EWOULDBLOCK EWOULDBLOCK
#  define CU_EPIPE EPIPE
#  define CU_ECONNRESET ECONNRESET
#  define CU_EINTR EINTR
#  define CU_EBADF EBADF
#  define CU_ERRNO_SET(e) errno = e;
#  define CU_ERRNO errno
#  define CU_NETERR -1
#  define CU_NETERRSOCK -1
typedef ssize_t cu_transfer;
typedef size_t cu_tbytes;
#else
#  if CU_COMP_MSVC
#    pragma comment(lib, "iphlpapi.lib")
#    pragma comment(lib, "Ws2_32.lib")
#  endif
#  include <Winsock2.h>
#  include <ws2tcpip.h>
#  include <iphlpapi.h>
#  define cu_close closesocket
#  define CU_EAGAIN WSAEWOULDBLOCK
#  define CU_EWOULDBLOCK WSAEWOULDBLOCK
#  define CU_EPIPE WSAEHOSTUNREACH
#  define CU_ECONNRESET WSAECONNRESET
#  define CU_EINTR WSAEINTR
#  define CU_EBADF WSAENOTSOCK
#  define CU_ERRNO_SET(e)
#  define CU_ERRNO WSAGetLastError()
#  define CU_NETERR SOCKET_ERROR
#  define CU_NETERRSOCK INVALID_SOCKET
#  define MSG_NOSIGNAL 0
typedef int cu_transfer;
typedef int cu_tbytes;
#endif

#if CU_HAS_INCLUDE(<sys/epoll.h>)
#  define CU_EPOLL
#  include <sys/epoll.h>
typedef struct epoll_event cu_event;
#  define CU_NET_EVOF(ev) ((unsigned int)ev->events)
#  define CU_POLLIN EPOLLIN
#  define CU_POLLOUT EPOLLOUT
#  define CU_POLLHUP EPOLLHUP
#elif CU_OS_UNIX
typedef struct pollfd cu_event;
#  define CU_NET_EVOF(ev) ((unsigned int)ev->revents)
#  define CU_POLLIN POLLIN
#  define CU_POLLOUT POLLOUT
#  define CU_POLLHUP POLLHUP
#else
typedef struct pollfd cu_event;
#  define CU_NET_EVOF(ev) ((unsigned int)ev->revents)
#  define CU_POLLIN POLLIN
#  define CU_POLLOUT POLLOUT
#  define CU_POLLHUP POLLHUP
#  define poll WSAPoll
#endif

#define CU_NET_DEFEVOF(ev) ((unsigned int)ev->revents)
static CU_THREAD_LOCAL int cu_net_getaddrinfo_errno;

int cu_net_init(void)
{
#if CU_OS_UNIX
	return 1;
#else
	WSADATA wsa_init;
	if (WSAStartup(MAKEWORD(2, 2), &wsa_init) != 0) return 0;
	if (LOBYTE(wsa_init.wVersion) == 2 && HIBYTE(wsa_init.wVersion) == 2) return 1;
	WSACleanup();
	return 0;
#endif
}

void cu_net_terminate(void)
{
#if !CU_OS_UNIX
	WSACleanup();
#endif
}

CU_NN_NT((1, 2)) static int cu_net_getaddrip(const void *CU_RESTRICT a, char *CU_RESTRICT buf)
{
	const void *in = ((const struct sockaddr_storage *)a)->ss_family == AF_INET ? (const void *)&((const struct sockaddr_in *)a)->sin_addr : (const void *)&((const struct sockaddr_in6 *)a)->sin6_addr;
	return inet_ntop(((const struct sockaddr_storage *)a)->ss_family, in, buf, CU_NET_IPADDR_LEN) != NULL;
}

CU_ATTRIB_NOTHROW static int cu_net_nonblock(cu_socket sock)
{
#if CU_OS_UNIX
	return fcntl(sock, F_SETFL, O_NONBLOCK) != CU_NETERR;
#else
	u_long mode = 1;
	return ioctlsocket(sock, FIONBIO, &mode) == NO_ERROR;
#endif
}

char *cu_net_ipinfo(const cu_net_remote *CU_RESTRICT remote, char *CU_RESTRICT ipbuf)
{
	return cu_net_getaddrip(remote->ip_info, ipbuf) ? ipbuf : NULL;
}


char *cu_net_interfaces(char *ipbuf, int if_fmt, int id)
{
#if CU_OS_UNIX
	struct ifaddrs *ifd, *ifd_it;
	if (id < 0 || getifaddrs(&ifd) == CU_NETERR) return NULL;

	for (ifd_it = ifd; ifd_it; ifd_it = ifd_it->ifa_next) {
		if (if_fmt == CU_NET_INTERFACE_IPV4 && ifd_it->ifa_addr->sa_family != AF_INET) continue;
		else if (if_fmt == CU_NET_INTERFACE_IPV6 && ifd_it->ifa_addr->sa_family != AF_INET6) continue;
		else if (ifd_it->ifa_addr->sa_family != AF_INET && ifd_it->ifa_addr->sa_family != AF_INET6) continue;
		else if (id--) continue;
		cu_net_getaddrip(ifd_it->ifa_addr, ipbuf);
		break;
	}

	freeifaddrs(ifd);
	return ifd_it ? ipbuf : NULL;
#else
	PIP_ADAPTER_UNICAST_ADDRESS unicast;
	PIP_ADAPTER_ADDRESSES paddr, cur_it = NULL;
	ULONG buflen = 16384;

	if (id < 0 || !(paddr = (PIP_ADAPTER_ADDRESSES)malloc((size_t)buflen))) return NULL;

	if (GetAdaptersAddresses(
		if_fmt == CU_NET_INTERFACE_IPV6 ? AF_INET6 : (if_fmt == CU_NET_INTERFACE_IPV4 ? AF_INET : AF_UNSPEC),
		GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_FRIENDLY_NAME,
		NULL, paddr, &buflen
	) != NO_ERROR) goto end;

	for (cur_it = paddr; cur_it; cur_it = cur_it->Next) {
		for (unicast = cur_it->FirstUnicastAddress; unicast; unicast = unicast->Next) {
			if (if_fmt == CU_NET_INTERFACE_IPV4 && unicast->Address.lpSockaddr->sa_family != AF_INET) continue;
			else if (if_fmt == CU_NET_INTERFACE_IPV6 && unicast->Address.lpSockaddr->sa_family != AF_INET6) continue;
			else if (unicast->Address.lpSockaddr->sa_family != AF_INET && unicast->Address.lpSockaddr->sa_family != AF_INET6) continue;
			else if (id--) continue;
			cu_net_getaddrip(unicast->Address.lpSockaddr, ipbuf);
			goto end;
		}
	}
end:
	free(paddr);
	return cur_it ? ipbuf : NULL;
#endif
}

const char *cu_net_lasterr(void)
{
#if CU_OS_UNIX
	if (cu_net_getaddrinfo_errno && !CU_ERRNO) return gai_strerror(cu_net_getaddrinfo_errno);
	else return strerror(CU_ERRNO);
#else
	static char cu_net_wsaerrstr[512];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), 0, cu_net_wsaerrstr, 512, NULL);
	return cu_net_wsaerrstr;
#endif
}

int cu_net_sendmsg(cu_net_remote *CU_RESTRICT target, const void *CU_RESTRICT data, uptr n)
{
	if (target->mode & CU_NETMODE_CLOSED) return 0;

	if (target->mode & CU_NETMODE_UDPSERVER) {
		cu_transfer off = 0;
		while (1) {
			cu_transfer now = sendto(target->fd, (const char *)data + off, (cu_tbytes)(n - (uptr)off), MSG_NOSIGNAL, (struct sockaddr *)target->ip_info, sizeof target->ip_info);
			if (now == CU_NETERR) return 0;
			else if (n == (uptr)(off += now)) return 1;
		}
	} else if (!(target->mode & CU_NETMODE_WRITEABLE)) {
		cu_net_queue *q = (cu_net_queue *)realloc(target->queued, sizeof *q * (size_t)(target->nqueue + 1));
		void *copy = n ? malloc(n) : NULL;
		if (!q || (!copy && n)) return 0;
		target->queued = q;
		q = (cu_net_queue *)target->queued + target->nqueue++;
		if (n) memcpy(copy, data, n);
		q->data = copy;
		q->bytes = n;
		return 1;
	} else if (n != 0) {
		cu_transfer off = 0;
		while (1) {
			cu_transfer now = send(target->fd, (const char *)data + off, (cu_tbytes)(n - (uptr)off), MSG_NOSIGNAL);
			if (now == CU_NETERR) return 0;
			else if (n == (uptr)(off += now)) return 1;
		}
	} else return 0;
}

CU_NN_NT((1, 2, 3)) static int cu_net_recvmsg(cu_net_remote *CU_RESTRICT target, void **CU_RESTRICT data, uptr *CU_RESTRICT n)
{
	void *rlc;

	if (target->mode & CU_NETMODE_UDPSERVER) {
		socklen_t sa_len = sizeof target->ip_info;
		cu_transfer nres;

		if (!(*data = malloc(CU_NET_RECVBUF))) goto fail;
		nres = recvfrom(target->fd, (char *)*data, CU_NET_RECVBUF, 0, (struct sockaddr *)&target->ip_info, &sa_len);
		*n = 0;

		while (1) {
			*n += (uptr)nres;

			if (nres == CU_NETERR) goto fail;
			else if (nres < CU_NET_RECVBUF) return 1;
			else if (!(rlc = realloc(*data, *n + CU_NET_RECVBUF))) goto fail;
			else *data = rlc;

			nres = recvfrom(target->fd, (char *)*data + *n, CU_NET_RECVBUF, 0, NULL, NULL);
		}
	} else {
		char c = '\0';
		int is_udp = target->mode & CU_NETMODE_UDP;
		cu_transfer inres = is_udp ? 0 : recv(target->fd, &c, 1, 0);

		if (!is_udp && inres == 0) return CU_NETERR;
		else if (inres == CU_NETERR || !(*data = malloc(CU_NET_RECVBUF + 1))) goto fail;

		*(char *)*data = c;
		*n = (uptr)inres;

		while (1) {
			cu_transfer nres = recv(target->fd, (char *)*data + *n, CU_NET_RECVBUF, 0);
			*n += (uptr)nres;

			if (nres < CU_NET_RECVBUF) return 1;
			else if (nres == CU_NETERR) goto fail;
			else if (!(rlc = realloc(*data, *n + CU_NET_RECVBUF))) goto fail;
			else *data = rlc;
		}
	}
fail:
	free(*data);
	return 0;
}

CU_NN_NT((1)) static int cu_net_applyqueue(cu_net_remote *target)
{
	int i, successful = 0;

	if (target->mode & CU_NETMODE_CLOSED) return CU_NETERR;
	target->mode |= CU_NETMODE_WRITEABLE;
	if (!target->nqueue) return CU_NETERR;

	for (i = 0; i < target->nqueue; ++i) {
		cu_net_queue *cq = (cu_net_queue *)target->queued + i;
		if (cu_net_sendmsg(target, cq->data, cq->bytes) == 1) {
			free(cq->data);
			++successful;
		} else if (CU_ERRNO == CU_EAGAIN || CU_ERRNO == CU_EWOULDBLOCK) {
			target->mode &= ~((uptr)CU_NETMODE_WRITEABLE);
			goto outer;
		} else if (CU_ERRNO == CU_EBADF || CU_ERRNO == CU_EPIPE || CU_ERRNO == CU_ECONNRESET) goto outer;
	}
outer:
	if (!(target->nqueue -= successful)) {
		free(target->queued);
		target->queued = NULL;
		return 1;
	} else {
		memmove(target->queued, (cu_net_queue *)target->queued + successful, sizeof(cu_net_queue) * (size_t)target->nqueue);
		return 0;
	}
}

CU_NN_NT((1, 3)) static int cu_net_generic_socket(cu_net_remote *rem, const char *CU_RESTRICT address, const char *CU_RESTRICT port, void *CU_RESTRICT user, u32 retryval, uptr mode)
{
	struct addrinfo hints, *addr_list, *addr_it;
	int is_server = address == NULL;
	int udp = mode & CU_NETMODE_UDP;

	memset(rem, 0, sizeof *rem);
	memset(&hints, 0, sizeof hints);
	rem->user = user;

	hints.ai_socktype = (mode & CU_NETMODE_UDP) ? SOCK_DGRAM : SOCK_STREAM;
	hints.ai_family = (mode & CU_NETMODE_IPV6) ? AF_INET6 : AF_INET;
	hints.ai_flags = is_server ? AI_PASSIVE : 0;

	if ((cu_net_getaddrinfo_errno = getaddrinfo(address, port, &hints, &addr_list)) != 0) {
		CU_ERRNO_SET(0)
		return 0;
	}

	for (addr_it = addr_list; addr_it; addr_it = addr_it->ai_next) {
		if ((rem->fd = socket(addr_it->ai_family, addr_it->ai_socktype, addr_it->ai_protocol)) == CU_NETERRSOCK) continue;
		if (is_server) {
			int optval = 1;
			setsockopt(rem->fd, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof optval);
			if (bind(rem->fd, addr_it->ai_addr, (socklen_t)addr_it->ai_addrlen) == CU_NETERR) goto fail;
			if ((udp && cu_net_nonblock(rem->fd)) || (!udp && listen(rem->fd, SOMAXCONN) != CU_NETERR)) break;
		} else {
			while (1) {
				if (connect(rem->fd, addr_it->ai_addr, (socklen_t)addr_it->ai_addrlen) != CU_NETERR) break;
				if (CU_DWSHIFT(retryval, 16) == 0) goto fail;
				retryval -= 0x10000;
				poll(NULL, 0, retryval & 0xFFFF);
			}
			if (cu_net_nonblock(rem->fd)) break;
		}
	fail:
		cu_close(rem->fd);
		rem->fd = CU_NETERRSOCK;
	}

	if (rem->fd != CU_NETERR) memcpy(rem->ip_info, addr_it->ai_addr, (mode & CU_NETMODE_IPV6) ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in));
	freeaddrinfo(addr_list);
	if (rem->fd == CU_NETERR) return 0;

	rem->mode = (mode & (CU_NETMODE_UDP | CU_NETMODE_IPV6)) | (udp && is_server ? CU_NETMODE_UDPSERVER : 0);
	return 1;
}

CU_NN_NT((1, 2)) static int cu_net_generic_listen(cu_net_remote *CU_RESTRICT target, cu_event_handler ehandler, int heartbeat_delay_msec)
{
	struct pollfd pfd;

	pfd.events = POLLIN | POLLOUT;
	pfd.fd = target->fd;

	while (!(target->mode & CU_NETMODE_CLOSED)) {
		int pollres = poll(&pfd, 1, heartbeat_delay_msec);
		if (pollres == 0) {
			ehandler(target, NULL, CUEVT_HEARTBEAT, NULL, 0);
			continue;
		} else if (pollres != CU_NETERR) {
			if ((CU_NET_DEFEVOF((&pfd)) & CU_POLLOUT) && cu_net_applyqueue(target) != 0) pfd.events &= ~CU_POLLOUT;
			else if (target->nqueue) pfd.events |= CU_POLLOUT;
			if (CU_NET_DEFEVOF((&pfd)) & (CU_POLLIN | CU_POLLHUP)) {
				void *recvd; uptr bytes;
				int res = cu_net_recvmsg(target, &recvd, &bytes);
				if (res == 1) ehandler(target, NULL, CUEVT_MESSAGE, recvd, bytes);
				else if (res == CU_NETERR) target->mode |= CU_NETMODE_CLOSED;
			}
		}
	}

	free(target->queued);
	cu_close(target->fd);
	return CU_ERRNO != CU_EINTR;
}

int cu_client_listen(cu_net_remote *CU_RESTRICT srem, const char *CU_RESTRICT address, const char *CU_RESTRICT port, void *CU_RESTRICT user, u32 retryval, uptr mode, cu_event_handler ehandler, int heartbeat_delay_msec)
{
	if (!cu_net_generic_socket(srem, address, port, user, retryval, mode)) return 0;
	if (cu_net_generic_listen(srem, ehandler, heartbeat_delay_msec) && !(mode & CU_NETMODE_UDP)) ehandler(srem, NULL, CUEVT_DISCONNECT, NULL, 0);
	return 1;
}

CU_NN_NT((1, 3)) static int cu_net_modev(cu_event *event, int master, cu_net_remote *rem, int is_add, int out)
{
#ifdef CU_EPOLL
	cu_event e;
	CU_UNUSED(event);
	e.data.ptr = rem;
	e.events = EPOLLIN | (out ? EPOLLOUT | EPOLLET : 0);
	return epoll_ctl(master, is_add ? EPOLL_CTL_ADD : EPOLL_CTL_MOD, rem->fd, &e) != CU_NETERR;
#else
	CU_UNUSED(master);
	CU_UNUSED(is_add);
	event->fd = rem->fd;
	event->events = CU_POLLIN | (out ? CU_POLLOUT : 0);
	event->revents = 0;
	return 1;
#endif
}

int cu_server_listen(cu_net_remote *CU_RESTRICT srem, const char *CU_RESTRICT port, void *CU_RESTRICT user, uptr mode, uptr tcp_maxclients, cu_event_handler ehandler, int heartbeat_delay_msec)
{
	int master = CU_NETERRSOCK, nfree = 0, ret = 0, *freelist = NULL;
	cu_net_server s = { NULL, 0 };
	cu_event *evs_list = NULL;
	uptr i, j;

	if ((!(mode & CU_NETMODE_UDP) && tcp_maxclients <= 0) || !cu_net_generic_socket(srem, NULL, port, user, 0, mode)) return 0;
	if (mode & CU_NETMODE_UDP) {
		struct pollfd pfd;
		cu_net_remote udp_tmp;
		void *recvd; uptr bytes;

		udp_tmp.mode = CU_NETMODE_UDPSERVER;
		udp_tmp.fd = srem->fd;
		pfd.events = POLLIN;
		pfd.fd = srem->fd;

		while (!(srem->mode & CU_NETMODE_CLOSED)) {
			int pollres = poll(&pfd, 1, heartbeat_delay_msec);
			if (pollres == 0) ehandler(srem, NULL, CUEVT_HEARTBEAT, NULL, 0);
			else if (pollres != CU_NETERR && cu_net_recvmsg(&udp_tmp, &recvd, &bytes) == 1) ehandler(srem, &udp_tmp, CUEVT_MESSAGE, recvd, bytes);
		}

		cu_close(srem->fd);
		return 1;
	}

	if (!(evs_list = (cu_event *)calloc((size_t)tcp_maxclients + 1, sizeof *evs_list))) goto fail;
	if (!(s.rems = (cu_net_remote *)malloc(sizeof *s.rems * (size_t)tcp_maxclients))) goto fail;
	if (!(freelist = (int *)malloc(sizeof *freelist * (size_t)tcp_maxclients))) goto fail;
#ifdef CU_EPOLL
	if ((master = epoll_create1(0)) == CU_NETERRSOCK) goto fail;
#else
	for (i = 0; i < tcp_maxclients; ++i) {
		s.rems->fd = CU_NETERRSOCK;
		evs_list[i + 1].fd = CU_NETERRSOCK;
	}
#endif
	if (!cu_net_modev(evs_list, master, srem, 1, 0)) goto fail;

	srem->internal = &s;

	while (!(srem->mode & CU_NETMODE_CLOSED)) {
	#ifdef CU_EPOLL
		int nevs = epoll_wait(master, evs_list, (int)s.nclients + 1, heartbeat_delay_msec);
	#else
		int nevs = poll(evs_list, (unsigned long)(tcp_maxclients + 1), heartbeat_delay_msec);
	#endif
		if (nevs == 0) {
			ehandler(srem, NULL, CUEVT_HEARTBEAT, NULL, 0);
			continue;
		} else if (nevs > 0) for (i = 0; nevs; ++i) {
			cu_event *e = evs_list + i;
		#ifdef CU_EPOLL
			cu_net_remote *r = (cu_net_remote *)e->data.ptr;
			--nevs;
		#else
			cu_net_remote *r = NULL;
			if (e->fd == CU_NETERRSOCK) continue;
			if (CU_NET_EVOF(e) & (CU_POLLIN | CU_POLLOUT | CU_POLLHUP)) --nevs;
			else if (e->revents) { CU_ASSERT(0); --nevs; continue; }
			else continue;

			if (e->fd == srem->fd) r = srem;
			else for (j = 0; j < tcp_maxclients; ++j) {
				if (s.rems[j].fd != e->fd) continue;
				r = s.rems + j;
				break;
			}
		#endif

			if (r == srem) {
				socklen_t sa_len = sizeof r->ip_info;

				if (nfree) {
					r = s.rems + freelist[0];
					freelist[0] = freelist[--nfree];
				} else r = s.rems + s.nclients;
				
				if ((r->fd = accept(srem->fd, (struct sockaddr *)&r->ip_info, &sa_len)) == CU_NETERRSOCK) continue;
				if (s.nclients == tcp_maxclients) {
				decline:
					cu_close(r->fd);
					continue;
				}

				if (!cu_net_nonblock(srem->fd)) goto decline;
				if (!cu_net_modev(evs_list + (r - s.rems) + 1, master, r, 1, 1)) goto decline;

				r->mode = (srem->mode & (CU_NETMODE_UDP | CU_NETMODE_IPV6));
				r->queued = r->user = r->internal = NULL;
				r->nqueue = 0;

				++s.nclients;
				ehandler(srem, r, CUEVT_CONNECT, NULL, (uptr)s.nclients);
				continue;
			} else if (!(r->mode & CU_NETMODE_CLOSED)) {
				if ((CU_NET_EVOF(e) & CU_POLLOUT) && cu_net_applyqueue(r) != 0) cu_net_modev(e, master, r, 0, 0);
				else if (r->nqueue) cu_net_modev(e, master, r, 0, 1);
				if (CU_NET_EVOF(e) & (CU_POLLIN | CU_POLLHUP)) {
					void *recvd; uptr bytes;
					int res = cu_net_recvmsg(r, &recvd, &bytes);
					if (res == 1) ehandler(srem, r, CUEVT_MESSAGE, recvd, bytes);
					else if (res == CU_NETERR) cu_net_close(r);
				}
			}

			if (r->mode & CU_NETMODE_CLOSED) {
				cu_close(r->fd);
			#ifdef CU_EPOLL
				epoll_ctl(master, EPOLL_CTL_DEL, r->fd, NULL);
			#else
				r->fd = e->fd = CU_NETERRSOCK;
			#endif
				ehandler(srem, r, CUEVT_DISCONNECT, NULL, (uptr)--s.nclients);
				free(r->queued);
				freelist[nfree++] = (int)(r - s.rems);
			}
		}
	}

	for (j = 0; j < s.nclients; ++j) cu_close(s.rems[j].fd);
	ret = 1;
fail:
	cu_close(srem->fd);
	free(s.rems);
	free(freelist);
	free(evs_list);
	return ret;
}

void cu_server_broadcast(const cu_net_remote *CU_RESTRICT server, const void *CU_RESTRICT data, uptr bytes, cu_net_remote *CU_RESTRICT *CU_RESTRICT except, uptr except_len)
{
	const cu_net_server *s = (cu_net_server *)server->internal;
	uptr i, j;
	for (i = 0; i < s->nclients; ++i) {
		for (j = 0; j < except_len; ++j) {
			if (except[i]->fd != s->rems[i].fd) continue;
			except[i] = except[--except_len];
			goto skip;
		}

		cu_net_sendmsg(s->rems + i, data, bytes);
	skip:
		continue;
	}
}

void cu_net_close(cu_net_remote *remote)
{
	remote->mode |= CU_NETMODE_CLOSED;
}

#endif

/* ========================= Threading ======================== */

#if CU_SETTING_THREAD_FUNCS

#if CU_THREAD_POSIX_USED
#  include <pthread.h>
#  include <unistd.h>

cu_thread cu_thread_create(cu_thread_func function, void *arg)
{
	cu_thread thr;
	if (pthread_create(&thr, NULL, function, arg) != 0) return 0;
	return thr;
}
int cu_thread_join(cu_thread thread) { return pthread_join(thread, NULL) == 0; }
int cu_thread_detach(cu_thread thread) { return pthread_detach(thread) == 0; }

void cu_thread_sleep(u64 nsecs, u64 secs)
{
	struct timespec rtime;
	rtime.tv_nsec = (long)(nsecs % 1000000000);
	rtime.tv_sec = (long)(secs + (nsecs / 1000000000));
	while (nanosleep(&rtime, &rtime));
}
int cu_thread_count(void) { return (int)sysconf(_SC_NPROCESSORS_ONLN); }

int cu_thread_mutex_init(cu_thread_mutex *mutex) { return pthread_mutex_init(mutex, NULL) == 0; }
int cu_thread_mutex_lock(cu_thread_mutex *mutex) { return pthread_mutex_lock(mutex) == 0; }
int cu_thread_mutex_unlock(cu_thread_mutex *mutex) { return pthread_mutex_unlock(mutex) == 0; }
int cu_thread_mutex_trylock(cu_thread_mutex *mutex) { return pthread_mutex_trylock(mutex) == 0; }
int cu_thread_mutex_destroy(cu_thread_mutex *mutex) { return pthread_mutex_destroy(mutex) == 0; }

int cu_thread_cond_init(cu_thread_cond *cond) { return pthread_cond_init(cond, NULL) == 0; }
int cu_thread_cond_wait(cu_thread_cond *cond, cu_thread_mutex *mutex) { return pthread_cond_wait(cond, mutex) == 0; }
int cu_thread_cond_signal(cu_thread_cond *cond) { return pthread_cond_signal(cond) == 0; }
int cu_thread_cond_broadcast(cu_thread_cond *cond) { return pthread_cond_broadcast(cond) == 0; }
int cu_thread_cond_destroy(cu_thread_cond *cond) { return pthread_cond_destroy(cond) == 0; }
cu_thread cu_thread_self(void) { return pthread_self(); }
u64 cu_thread_pid(void) { return (u64)getpid(); }
#if CU_OS_MAC
u64 cu_thread_tid(void)
{
	uint64_t res;
	pthread_threadid_np(NULL, &res);
	return (u64)res;
}
#else
u64 cu_thread_tid(void) { return (u64)gettid(); }
#endif

#elif CU_THREAD_WIN_USED
#  include <windows.h>

cu_thread cu_thread_create(cu_thread_func function, void *arg) { return CreateThread(NULL, 0, function, arg, 0, 0); }
int cu_thread_join(cu_thread thread) { return WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0; }
int cu_thread_detach(cu_thread thread) { return CloseHandle(thread) != 0; }

void cu_thread_sleep(u64 nsecs, u64 secs)
{
	HANDLE timer;
	LARGE_INTEGER ft;
	ft.QuadPart = (secs * 10000000) + (nsecs / -100);

	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
	WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);
}
int cu_thread_count(void)
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return (int)info.dwNumberOfProcessors;
}

int cu_thread_mutex_init(cu_thread_mutex *mutex) { InitializeCriticalSection(mutex); return 1; }
int cu_thread_mutex_lock(cu_thread_mutex *mutex) { EnterCriticalSection(mutex); return 1; }
int cu_thread_mutex_unlock(cu_thread_mutex *mutex) { LeaveCriticalSection(mutex); return 1; }
int cu_thread_mutex_trylock(cu_thread_mutex *mutex) { return TryEnterCriticalSection(mutex) != 0; }
int cu_thread_mutex_destroy(cu_thread_mutex *mutex) { DeleteCriticalSection(mutex); return 1; }

int cu_thread_cond_init(cu_thread_cond *cond) { InitializeConditionVariable(cond); return 1; }
int cu_thread_cond_wait(cu_thread_cond *cond, cu_thread_mutex *mutex) { return SleepConditionVariableCS(cond, mutex, INFINITE) != 0; }
int cu_thread_cond_signal(cu_thread_cond *cond) { WakeConditionVariable(cond); return 1; }
int cu_thread_cond_broadcast(cu_thread_cond *cond) { WakeAllConditionVariable(cond); return 1; }
int cu_thread_cond_destroy(cu_thread_cond *cond) { CU_UNUSED(cond); return 1; }

cu_thread cu_thread_self(void) { return GetCurrentThread(); }
u64 cu_thread_pid(void) { return (u64)GetCurrentProcessId(); }
u64 cu_thread_tid(void) { return (u64)GetCurrentThreadId(); }

#elif CU_THREAD_C_USED
#  include <threads.h>
#  include <time.h>

cu_thread cu_thread_create(cu_thread_func function, void *arg)
{
	thrd_t thr;
	if (thrd_create(&thr, function, arg) != thrd_success) return 0;
	return thr;
}
int cu_thread_join(cu_thread thread) { return thrd_join(thread, NULL) == thrd_success; }
int cu_thread_detach(cu_thread thread) { return thrd_detach(thread) == thrd_success; }

void cu_thread_sleep(u64 nsecs, u64 secs)
{
	struct timespec ts;
	ts.tv_nsec = nsecs % 1000000000;
	ts.tv_sec = (long)(secs + (nsecs / 1000000000));
	while (thrd_sleep(&ts, &ts) == -1);
}
int cu_thread_count(void) { return 1; }

int cu_thread_mutex_init(cu_thread_mutex *mutex) { return mtx_init(mutex, mtx_plain) == thrd_success; }
int cu_thread_mutex_lock(cu_thread_mutex *mutex) { return mtx_lock(mutex) == thrd_success; }
int cu_thread_mutex_unlock(cu_thread_mutex *mutex) { return mtx_unlock(mutex) == thrd_success; }
int cu_thread_mutex_trylock(cu_thread_mutex *mutex) { return mtx_trylock(mutex) == thrd_success; }
int cu_thread_mutex_destroy(cu_thread_mutex *mutex) { mtx_destroy(mutex); return 1; }

int cu_thread_cond_init(cu_thread_cond *cond) { return cnd_init(cond) == thrd_success; }
int cu_thread_cond_wait(cu_thread_cond *cond, cu_thread_mutex *mutex) { return cnd_wait(cond, mutex) == thrd_success; }
int cu_thread_cond_signal(cu_thread_cond *cond) { return cnd_signal(cond) == thrd_success; }
int cu_thread_cond_broadcast(cu_thread_cond *cond) { return cnd_broadcast(cond) == thrd_success; }
int cu_thread_cond_destroy(cu_thread_cond *cond) { return cnd_destroy(cond) == thrd_success; }

cu_thread cu_thread_self(void) { return thrd_current(); }
u64 cu_thread_pid(void) { return (u64)0; }
u64 cu_thread_tid(void) { return (u64)0; }

#endif

#include <stdlib.h>
#include <string.h>

int cu_thread_split(cu_thread_func func, u64 work_count, void **each_thread_arg, int thread_count)
{
	struct cu_split { cu_thread thread; cu_thread_split_arg arg; } *thrs;
	u64 effective_threads, div, remaining, c, i;

	if (!work_count) return 0;
	else if (thread_count <= 0) return 0;

	effective_threads = (u64)(thread_count - 1);
	if (!(thrs = (struct cu_split *)malloc(sizeof *thrs * (size_t)thread_count))) return 0;
	div = work_count / (u64)thread_count;
	remaining = work_count - (div * (u64)thread_count);

	for (i = c = 0; i <= effective_threads; ++i) {
		cu_thread_split_arg *a = &thrs[i].arg;
		a->thread_arg = each_thread_arg ? each_thread_arg[i] : NULL;
		a->start_index = c;
		c += div + remaining ? 1 : 0;
		if (remaining) --remaining;
		a->end_index = c;
		if (i == effective_threads || CU_UNLIKELY(!(thrs[i].thread = cu_thread_create(func, a)))) {
			thrs[i].thread = 0;
			func(a);
		}
	}

	for (i = 0; i < effective_threads; ++i) if (thrs[i].thread) cu_thread_join(thrs[i].thread);
	free(thrs);

	return 1;
}

struct cu_thread_pool_job
{
	struct cu_thread_pool_job *next;
	cu_thread_func func;
	void *arg;
};

#define CU_THREAD_POOL_CLOSING -2
#define CU_THREAD_POOL_WAITING -1
#define CU_THREAD_POOL_WORKING 0

static CU_THREAD_FUNCTION(cu_thread_pool_inner, arg)
{
	cu_thread_pool *pool = (cu_thread_pool *)arg;
	struct cu_thread_pool_job *cjob;

	while (pool->jobsig != CU_THREAD_POOL_CLOSING) {
		cu_thread_mutex_lock(&pool->mutex);

		while (!pool->jobs && pool->jobsig == CU_THREAD_POOL_WAITING) cu_thread_cond_wait(&pool->cond, &pool->mutex);
		if (pool->jobsig == CU_THREAD_POOL_CLOSING) {
			cu_thread_mutex_unlock(&pool->mutex);
			break;
		}

		pool->jobsig = CU_THREAD_POOL_WAITING;
		cjob = (struct cu_thread_pool_job *)pool->jobs;
		pool->jobs = cjob->next;
		cu_thread_mutex_unlock(&pool->mutex);

		cjob->func(cjob->arg);
		free(cjob);
	}

	return CU_THREAD_RETURN_VAL;
}

int cu_thread_pool_init(cu_thread_pool *pool, int nthreads)
{
	int i;
	memset(pool, 0, sizeof *pool);

	if (nthreads <= 0) return 0;

	if (CU_UNLIKELY(!cu_thread_cond_init(&pool->cond))) return 0;
	if (CU_UNLIKELY(!cu_thread_mutex_init(&pool->mutex))) {
		cu_thread_cond_destroy(&pool->cond);
		return 0;
	}

	if (!(pool->thrs = (cu_thread *)calloc((size_t)nthreads, sizeof *pool->thrs))) goto fail;
	pool->jobsig = CU_THREAD_POOL_WAITING;

	for (i = 0; i < nthreads; ++i) {
		++pool->nthreads;
		if ((pool->thrs[i] = cu_thread_create(cu_thread_pool_inner, pool))) continue;
		cu_thread_pool_destroy(pool);
		return 0;
	}

	return 1;
fail:
	cu_thread_pool_destroy(pool);
	return 0;
}

int cu_thread_pool_add(cu_thread_pool *pool, cu_thread_func job, void *arg)
{
	struct cu_thread_pool_job *ljob = (struct cu_thread_pool_job *)malloc(sizeof *ljob);
	if (!ljob) return 0;

	ljob->arg = arg;
	ljob->func = job;

	cu_thread_mutex_lock(&pool->mutex);
	ljob->next = pool->jobs ? (struct cu_thread_pool_job *)pool->jobs : NULL;
	pool->jobs = ljob;
	pool->jobsig = CU_THREAD_POOL_WORKING;
	cu_thread_cond_signal(&pool->cond);
	cu_thread_mutex_unlock(&pool->mutex);

	return 1;
}

void cu_thread_pool_destroy(cu_thread_pool *pool)
{
	int i;

	cu_thread_mutex_lock(&pool->mutex);
	pool->jobsig = CU_THREAD_POOL_CLOSING;
	cu_thread_cond_broadcast(&pool->cond);
	cu_thread_mutex_unlock(&pool->mutex);

	for (i = 0; i < pool->nthreads; ++i) cu_thread_join(pool->thrs[i]);

	free(pool->thrs);
	free(pool->jobs);

	cu_thread_mutex_destroy(&pool->mutex);
	cu_thread_cond_destroy(&pool->cond);
}

#endif /* CU_SETTING_THREAD_FUNCS */

#endif /* CU_SETTING_FUNCS */
