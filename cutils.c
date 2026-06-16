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
CU_DIAGNOSTICS_IGNORE_2(clang, "-Wreserved-macro-identifier", "-Wunused-macros")
#  define __STDC_WANT_LIB_EXT1__ 1
CU_DIAGNOSTICS_IGNORE_END
#endif

#if CU_COMPVER(CLANG, 19, 0)
#  define cu_strchr(S, C) \
CU_DIAGNOSTICS_IGNORE_3(clang, "-Wc11-extensions", "-Wpre-c11-compat", "-Wdisabled-macro-expansion") \
strchr(S, C) \
CU_DIAGNOSTICS_IGNORE_END
#else
#  define cu_strchr(S, C) strchr(S, C)
#endif

#define CU_API_SOURCE CU_API

#define CU_BITSOF(x, start, end) (CU_DWSHIFT(x, start) & (CU_UPSHIFT(1, (1 + (end - start))) - 1))

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

CU_ATTRIB_CONST CU_ATTRIB_NOTHROW CU_ATTRIB_WARN_UNUSED_RESULT
static uptr custr_npow2(uptr v)
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

CU_API_SOURCE int custr_create(custr *CU_RESTRICT c, const char *CU_RESTRICT str)
{
	const uptr len = (size_t)strlen(str), allocd = custr_npow2(len + 1);
	if (!(c->str = (char *)malloc((size_t)allocd))) return 0;
	memcpy(c->str, str, (size_t)(len + 1));
	c->len = len;
	c->cap = allocd;
	return 1;
}
CU_API_SOURCE custr *custr_allocd(custr *CU_RESTRICT c, char *CU_RESTRICT allocdstr, uptr allocd)
{
	c->len = strlen(allocdstr);
	if (CU_UNLIKELY(allocd && allocd <= c->len)) return NULL;
	c->str = (char *)allocdstr;
	c->cap = allocd ? allocd : c->len + 1;
	return c;
}

CU_API_SOURCE int custr_reserve(custr *c, uptr bytes)
{
	void *ptr;

	bytes = custr_npow2(bytes);
	if (CU_LIKELY(bytes < 8)) bytes = 8;

	if (bytes == c->cap) return 1;
	else if (bytes <= c->len) return custr_shrinkto(c, bytes - 1);
	else if (bytes < c->cap) return 1;

	if (!(ptr = realloc(c->str, (size_t)bytes))) return 0;

	c->str = (char *)ptr;
	c->cap = bytes;

	return 1;
}
CU_API_SOURCE int custr_copy(const custr *CU_RESTRICT copy, custr *CU_RESTRICT paste)
{
	paste->len = copy->len;
	paste->cap = paste->len + 1;
	if (!(paste->str = (char *)malloc((size_t)paste->cap))) return 0;
	memcpy(paste->str, copy->str, (size_t)paste->cap);
	return 1;
}

CU_API_SOURCE int custr_shrinkto(custr *c, uptr shrinked_len)
{
	if (CU_UNLIKELY(shrinked_len >= c->len)) return 0;
	if (!c->str) return 1;
	c->len = shrinked_len;
	c->str[shrinked_len] = '\0';
	return 1;
}
CU_API_SOURCE custr *custr_clear(custr *c)
{
	c->len = c->cap = 0;
	if (!c->str) return c;
	free(c->str);
	c->str = NULL;
	return c;
}
CU_API_SOURCE int custr_optimize(custr *c)
{
	c->cap = 0;
	return custr_reserve(c, c->len);
}

CU_API_SOURCE int custr_insert(custr *CU_RESTRICT c, uptr c_offset, const char *CU_RESTRICT to_insert)
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
CU_API_SOURCE int custr_append(custr *CU_RESTRICT c, const char *CU_RESTRICT to_append)
{
	return custr_insert(c, c->len, to_append);
}

CU_API_SOURCE int custr_sub(const custr *CU_RESTRICT c, custr *CU_RESTRICT subresult, uptr start_ind, uptr end_ind)
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
CU_API_SOURCE int custr_tosub(custr *c, uptr start_ind, uptr end_ind)
{
	if (CU_UNLIKELY(start_ind > end_ind || start_ind > c->len)) return 0;
	if (CU_LIKELY(end_ind < c->len)) custr_shrinkto(c, end_ind + 1);

	if (start_ind) memmove(c->str, c->str + start_ind, (size_t)((c->len + 1) - start_ind));
	return 1;
}

CU_API_SOURCE void custr_cut(custr *c, uptr start_ind, uptr count)
{
	if (CU_UNLIKELY(start_ind >= c->len)) return;
	if (CU_UNLIKELY(start_ind + count > c->len)) count = c->len - start_ind;
	memmove(c->str + start_ind, c->str + start_ind + count, (size_t)((c->len + 1) - start_ind - count));
	c->len -= count;
}

CU_API_SOURCE int custr_count(const custr *c, char target)
{
	int n = 0;
	uptr offset = CU_UPTRMAX;

	while (1)  {
		if ((offset = custr_find(c, offset + 1, target, 0)) == CU_UPTRMAX) return n;
		++n;
	}
}
CU_API_SOURCE int custr_countsub(const custr *CU_RESTRICT c, const char *CU_RESTRICT target)
{
	uptr offset = CU_UPTRMAX;
	int n = 0;
	while (1)  {
		if ((offset = custr_findsub(c, offset + 1, target, 0)) == CU_UPTRMAX) return n;
		++n;
	}
}

CU_API_SOURCE int custr_fmt(custr *CU_RESTRICT c, char *CU_RESTRICT fmt, ...)
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
	return 0;
#endif
}

CU_API_SOURCE int custr_cd(custr *CU_RESTRICT c, const char *CU_RESTRICT name)
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

CU_API_SOURCE void custr_simplify(custr *c)
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

CU_API_SOURCE uptr custr_find(const custr *c, uptr c_offset, char target, int n)
{
	iptr i;
	if (CU_LIKELY(n >= 0)) { for (i = (iptr)c_offset; i < (iptr)c->len; ++i) if (CU_UNLIKELY(c->str[i] == target) && !(n--)) return (uptr)i; }
	else if (CU_LIKELY(c_offset <= c->len)) { for (i = (iptr)(c->len - c_offset - 1); i >= 0; --i) if (CU_UNLIKELY(c->str[i] == target) && !(++n)) return (uptr)i; }
	return CU_UPTRMAX;
}
CU_API_SOURCE uptr custr_findnot(const custr *c, uptr c_offset, char target, int n)
{
	iptr i;
	if (CU_LIKELY(n >= 0)) { for (i = (iptr)c_offset; i < (iptr)c->len; ++i) if (c->str[i] != target && !(n--)) return (uptr)i; }
	else if (CU_LIKELY(c_offset <= c->len)) { for (i = (iptr)(c->len - c_offset - 1); i >= 0; --i) if (c->str[i] != target && !(++n)) return (uptr)i; }
	return CU_UPTRMAX;
}
CU_API_SOURCE uptr custr_findsub(const custr *CU_RESTRICT c, uptr c_offset, const char *CU_RESTRICT target_substr, int n)
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

CU_API_SOURCE void custr_replace(custr *c, uptr c_offset, char target, char replacement)
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
CU_API_SOURCE int custr_replacesub(custr *CU_RESTRICT c, uptr c_offset, const char *CU_RESTRICT target, const char *CU_RESTRICT replacement)
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
#  define cu_fileno(f) _fileno(f)
#  define cu_fopen(file, path, mode) (fopen_s(&file, path, mode) != 0)
#  define cu_mkdir(path, mode) _mkdir(path)
#  define cu_fstat _fstati64
#  define cu_stat _stati64
#  define F_OK 0
#else
#  include <unistd.h>
#  include <dirent.h>
#  define cu_fileno(f) fileno(f)
#  define cu_fopen(file, path, mode) (!(file = fopen(path, mode)))
#  define cu_mkdir(path, mode) mkdir(path, mode)
#  define cu_fstat fstat
#  define cu_stat stat
#endif

CU_API_SOURCE int cu_file_exists(const char *path)
{
	struct cu_stat dir_stat;
	return cu_stat(path, &dir_stat) == 0 && ((dir_stat.st_mode & S_IFREG) == S_IFREG);
}
CU_API_SOURCE int cu_dir_exists(const char *path)
{
	struct cu_stat dir_stat;
	return cu_stat(path, &dir_stat) == 0 && ((dir_stat.st_mode & S_IFDIR) == S_IFDIR);
}

CU_API_SOURCE int cu_dir_create(const char *path)
{
	return cu_mkdir(path, 0777) == 0;
}

CU_API_SOURCE void *cu_file_read(const char *CU_RESTRICT path, void *CU_RESTRICT result, int binary_file, uptr *CU_RESTRICT bytes)
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

CU_API_SOURCE int cu_file_write(const char *CU_RESTRICT path, const void *CU_RESTRICT content, unsigned int mode, uptr bytes)
{
	static const char *mode_strs[] = { "w", "wb", "a", "ab" };
	FILE *file; int res;

	if (CU_UNLIKELY(mode > 3)) return 0;
	if (CU_UNLIKELY(cu_fopen(file, path, mode_strs[mode]))) return 0;

	res = fwrite(content, (size_t)bytes, 1, file) == 1;
	fclose(file);

	return res;
}

CU_API_SOURCE int cu_file_getinfo(const char *CU_RESTRICT path, cu_file_info *CU_RESTRICT f_info)
{
	struct cu_stat dir_stat;
	if (cu_stat(path, &dir_stat) == -1) return 0;

	f_info->fsize_bytes = (u64)dir_stat.st_size;
	f_info->access_time = (i64)dir_stat.st_atime;
	f_info->mod_time = (i64)dir_stat.st_mtime;
	f_info->create_time = (i64)dir_stat.st_ctime;

	return 1;
}

CU_API_SOURCE char *cu_file_exe_path(const char *CU_RESTRICT argv, uptr *CU_RESTRICT allocd)
{
#if CU_OS_WINDOWS
	DWORD res;
	char *buf = (char *)malloc(CU_PATH_MAX);
	CU_UNUSED(argv);
	if (!buf) return NULL;
	res = GetModuleFileName(NULL, buf, CU_PATH_MAX);
	if (allocd) *allocd = CU_PATH_MAX;
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
		len = 0;
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
	} else return NULL;

	if (allocd) *allocd = (uptr)len;
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

CU_API_SOURCE int cu_file_delete(const char *path)
{
	return remove(path) == 0;
}

CU_API_SOURCE int cu_dir_delete(const char *path, int recursive)
{
#if CU_OS_WINDOWS
	WIN32_FIND_DATA fd;
	size_t full_len, dir_bytes;
	HANDLE dirhandle;
	char *dir_path;
	int res;

	if (!recursive) return RemoveDirectoryA(path) != 0;

	full_len = strlen(path);
	dir_bytes = full_len > (CU_PATH_MAX - 30) ? full_len * 2 : CU_PATH_MAX;
	dir_path = (char *)malloc(dir_bytes);

	if (!dir_path) return 0;
	res = 0;

	memcpy(dir_path, path, full_len);
	dir_path[full_len] = '\\';
	dir_path[full_len + 1] = '*';
	dir_path[full_len + 2] = '\0';
	dir_path[full_len + 3] = '\0';

	dirhandle = FindFirstFile(dir_path, &fd);
	if (dirhandle == INVALID_HANDLE_VALUE) goto fail;

	while (FindNextFile(dirhandle, &fd)) {
		uptr file_len;
		if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0) continue;
		file_len = strlen(fd.cFileName);

		if (full_len + file_len + 2 > dir_bytes) {
			void *r = realloc(dir_path, dir_bytes *= 2);
			if (!r) goto fail;
			dir_path = (char *)r;
		}

		memcpy(dir_path + full_len + 1, fd.cFileName, (size_t)(file_len + 1));

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) cu_dir_delete(dir_path, 1);
		else cu_file_delete(dir_path);
	}

	FindClose(dirhandle);
	res = RemoveDirectoryA(path) != 0;
fail:
	free(dir_path);
	return res;
#else
	struct dirent *entry;
	uptr plen, dlen, entrylen;
	char *dbuf;
	int ret = 0;
	DIR *dir;

	if (!recursive) return rmdir(path) == 0;

	plen = strlen(path);
	dlen = 0xFF;

	if (!(dbuf = (char *)malloc(plen + dlen + (path[plen - 1] != '/') + 1))) return 0;
	if (!(dir = opendir(path))) goto fail;

	memcpy(dbuf, path, plen);
	if (path[plen - 1] != '/') dbuf[plen++] = '/';

	while ((entry = readdir(dir))) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
		entrylen = strlen(entry->d_name) + 1;

		if (entrylen > dlen) {
			void *rel = realloc(dbuf, plen + entrylen);
			if (!rel) goto fail_dir;
			dbuf = (char *)rel;
		}

		memcpy(dbuf + plen, entry->d_name, entrylen);
		if (cu_dir_exists(dbuf)) { if (!cu_dir_delete(dbuf, 1)) goto fail_dir; }
		else if (!cu_file_delete(dbuf)) goto fail_dir;
	}

	ret = rmdir(path) == 0;
fail_dir:
	ret &= closedir(dir) == 0;
fail:
	free(dbuf);
	return ret;
#endif
}

#endif

/* ========================= Resources ======================== */

#if CU_SETTING_RESOURCES_FUNCS

#include <string.h>

#if CU_ARCH_X86
#  include <time.h>
#endif

#if CU_OS_MAC
#  include <mach/mach.h>
#  include <sys/types.h>
#  include <sys/sysctl.h>
#  include <mach/vm_statistics.h>
#  include <mach/mach_types.h>
#  include <mach/mach_init.h>
#  include <mach/mach_host.h>
#elif CU_OS_UNIX && CU_HAS_INCLUDE(<sys/sysinfo.h>)
#  include <sys/sysinfo.h>
#endif

#if CU_OS_UNIX
#  include <sys/utsname.h>
#  include <sys/times.h>
#  include <string.h>
#  include <unistd.h>
#  include <stdio.h>
#  include <pwd.h>
#elif CU_OS_WINDOWS
#  include <psapi.h>
#  include <stdio.h>
#  include <ntsecapi.h>
#endif

#if CU_COMP_MSVC
#  define cu_res_rdtsc() __rdtsc()
#  define cu_sscanf(str, fmt, a1) sscanf_s(str, fmt, a1)
#  pragma comment(lib, "Psapi.lib")
#  include <intrin.h>
#  pragma comment(lib, "Advapi32.lib")
#else
#  define cu_sscanf(str, fmt, a1) sscanf(str, fmt, a1)
#endif

#if CU_COMP_MSVC
#  define cu_res_rdtsc() __rdtsc()
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((2)) static u32 cu_res_cpuid(u32 id, u32 count, u32 *regs)
{
	__cpuidex((int *)regs, id, count);
	return *regs;
}
#elif CU_ARCH_X86
CU_ATTRIB_WARN_UNUSED_RESULT CU_ATTRIB_NOTHROW static u64 cu_res_rdtsc(void)
{
	u32 low, high;
	CU_ASM volatile ("rdtsc" : "=a" (low), "=d" (high));
	return CU_UPSHIFT((u64)high, 32) | low;
}
CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((3)) static u32 cu_res_cpuid(u32 id, u32 count, u32 *regs)
{
	CU_ASM(
		"  xchg{q|} {%%|}rbx,%q1\n"
		"  cpuid\n"
		"  xchg{q|} {%%|}rbx,%q1"
		: "=a"(regs[0]), "=r"(regs[1]), "=c"(regs[2]), "=d"(regs[3])
		: "0"(id), "2"(count)
	);
	return *regs;
}
#else
#  define cu_res_rdtsc() 0
#  define cu_res_cpuid(id, count, regs) 0
#endif

CU_API_SOURCE char *cu_res_bytefmt(char *str, u64 bytes)
{
	static const char bytefmt_suffix[6] = { 'K', 'M', 'G', 'T', 'P', 'E' };
	int small = bytes < 1000, suffix = -1, prev = 0;
	char *start = str;

	while (bytes >= 1000) {
		++suffix;
		prev = (int)(bytes % 1000);
		bytes /= 1000;
	}

	if (bytes >= 100) *str++ = '0' + (char)((bytes / 100) % 10);
	if (bytes >= 10) *str++ = '0' + (char)((bytes / 10) % 10);
	*str++ = '0' + (char)(bytes % 10);

	if (!small) {
		*str++ = '.';
		*str++ = '0' + (char)((prev / 100) % 10);
		*str++ = bytefmt_suffix[suffix];
	}

	*str++ = 'B';
	*str++ = '\0';
	return start;
}

CU_API_SOURCE uptr cu_res_crypto(void *data, uptr bytes)
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

CU_API_SOURCE int cu_res_meminfo(cu_res_mem *info)
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
		fscanf(f, "%lu %lu", &info->virtual_used, &info->physical_used);
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

CU_API_SOURCE real64 cu_res_cpuusage(void)
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

CU_API_SOURCE int cu_res_cpuinfo(cu_res_cpu *info)
{
	u32 regs[4] = { 0, 0, 0, 0 }, cpu_family, level, i = 1;
	struct cu_res_cpu_cache *target;

	memset(info, 0, sizeof *info);

	info->cpuid_level = cu_res_cpuid(0x0, 0, regs);
	memcpy(info->vendor + 0, regs + 1, 4);
	memcpy(info->vendor + 4, regs + 3, 4);
	memcpy(info->vendor + 8, regs + 2, 4);

	for (i = 0; i < 3; ++i) {
		cu_res_cpuid(0x80000002 + i, 0, regs);
		memcpy(info->name + i * 16, regs, 16);
	}

	cu_res_cpuid(0x1, 0, regs);
	info->stepping_id = (u32)(CU_BITSOF(regs[0], 0, 3));
	cpu_family = (u32)(CU_BITSOF(regs[0], 8, 11));
	info->model_id = (u32)(CU_BITSOF(regs[0], 4, 7) + ((cpu_family == 6 || cpu_family == 15) * CU_UPSHIFT(CU_BITSOF(regs[0], 16, 19), 4)));
	info->family_id = (u32)(cpu_family + (CU_BITSOF(regs[0], 20, 27) * (u32)(cpu_family == 15)));

	for (i = 0; ; ++i) {
		level = (cu_res_cpuid(0x04, i, regs) >> 5) & 0x7;
		if (level == 1) target = ((regs[0] & 0x1F) == 1) ? &info->l1d : &info->l1i;
		else if (level == 2) target = &info->l2;
		else if (level == 3) target = &info->l3;
		else break;

		target->assoc = (regs[1] >> 22) + 1;
		target->line = (regs[1] & 0xFFF) + 1;
		target->size = (((regs[1] >> 22) & 0x3FF) + 1) * (((regs[1] >> 12) & 0x3FF) + 1) * target->line * (regs[2] + 1);
	}

	if (info->cpuid_level >= 0x16 && cu_res_cpuid(0x16, 0, regs)) info->base_freq_hz = regs[0] * 1000 * 1000;
	else {
		float base_ghz = 0.0f;
		char *base_ghz_parser = cu_strchr(info->name, '@');
		if (base_ghz_parser) {
			CU_UNUSED(cu_sscanf(base_ghz_parser, "%*s%f", &base_ghz));
			info->base_freq_hz = (u64)(base_ghz * 1000.0f) * 1000 * 1000;
		}
	#if CU_ARCH_X86
		else {
			u64 start = cu_res_rdtsc();
			double elapsed;
			clock_t ts_start = clock();
			do elapsed = (double)(clock() - ts_start) / CLOCKS_PER_SEC; while (elapsed < 0.01);
			info->base_freq_hz = (u64)((double)(cu_res_rdtsc() - start) / elapsed);
		}
	#endif
	}

	return info->name[0] && info->vendor[0] && i >= 4 &&  info->base_freq_hz;
}

CU_API_SOURCE uptr cu_res_osname(char *namebuf)
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

CU_API_SOURCE uptr cu_res_hostname(char *namebuf)
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


CU_API_SOURCE uptr cu_res_username(char *namebuf)
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

CU_API_SOURCE void cu_time_now(cu_ctime *tm)
{
	time_t now_val = time(NULL);
	struct tm *now;

#if CU_OS_MAC
	mach_timespec_t ts;
	clock_serv_t cserv;
#elif CU_OS_UNIX
	struct timespec ts;
#elif CU_OS_WINDOWS
	static char _cu_windows_tzname[32];
	TIME_ZONE_INFORMATION tzinfo;
	ULARGE_INTEGER uli;
	FILETIME ft;
	u64 cnsec;
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

#if CU_OS_MAC
	host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cserv);
	clock_get_time(cserv, &ts);
	mach_port_deallocate(mach_task_self(), cserv);
	tm->nanosec = (int)(ts.tv_nsec % 1000);
	tm->microsec = (int)((ts.tv_nsec / 1000) % 1000);
	tm->millisec = (int)((ts.tv_nsec / 1000000) % 1000);
#elif CU_OS_UNIX
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1) ts.tv_nsec = 0;
	tm->nanosec = (int)(ts.tv_nsec % 1000);
	tm->microsec = (int)((ts.tv_nsec / 1000) % 1000);
	tm->millisec = (int)((ts.tv_nsec / 1000000) % 1000);
#elif CU_OS_WINDOWS
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

CU_API_SOURCE void cu_timer_fill(cu_timer *tm)
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

CU_API_SOURCE u64 cu_timer_dif(const cu_timer *CU_RESTRICT start, const cu_timer *CU_RESTRICT end)
{
	return ((end->secs - start->secs) * CU_U64_C(1000000000)) + (end->nsecs - start->nsecs);
}

#endif

/* ========================= Networking ======================== */

#if CU_SETTING_NETWORK_FUNCS

static int cu_net_gai_err;
#define CU_NET_RECVBUF 1023

#if CU_OS_UNIX
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <sys/types.h>
#  include <ifaddrs.h>
#  include <unistd.h>
#  include <signal.h>
#  include <netdb.h>
#  include <errno.h>
#  include <poll.h>

#  define cu_close close
#  define cu_sprintf(buf, mlen, fmt, args) sprintf(buf, fmt, args)
#  define CU_UWSZ(unixsz, winsz) unixsz

#  define CU_INVALID_SOCKET -1
#  define CU_SOCKET_ERROR -1

#  define CU_ECONNRESET ECONNRESET
#  define CU_ENOTSOCK ENOTSOCK
#  define CU_EINTR EINTR
#  define CU_EBADF EBADF

#  define CU_POLLHUP POLLHUP

#  define CU_NET_GETERR() errno
#  define CU_NET_SETERR(e) errno = e
#  define CU_NETSIG_SETUP() struct sigaction sact; memset(&sact, 0, sizeof sact)
#  define CU_NETSIG_SETFUNC(f) do { \
CU_DIAGNOSTICS_IGNORE_1(clang, "-Wdisabled-macro-expansion") \
sact.sa_handler = f; \
CU_DIAGNOSTICS_IGNORE_END \
sigaction(SIGINT, &sact, NULL); \
} while (0)

typedef ssize_t cu_net_data;
typedef nfds_t cu_net_pollcnt;

#else
#  if CU_COMP_MSVC
#    pragma comment(lib, "iphlpapi.lib")
#    pragma comment(lib, "Ws2_32.lib")
#  endif
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  include <iphlpapi.h>
#  include <signal.h>

#  define poll WSAPoll
#  define cu_close closesocket
#  define cu_sprintf(buf, mlen, fmt, args) sprintf_s(buf, mlen, fmt, args)
#  define CU_UWSZ(unixsz, winsz) winsz

#  define CU_INVALID_SOCKET INVALID_SOCKET
#  define CU_SOCKET_ERROR SOCKET_ERROR
#  define MSG_NOSIGNAL 0

#  define CU_ECONNRESET WSAECONNRESET
#  define CU_ENOTSOCK WSAENOTSOCK
#  define CU_EINTR WSAEINTR
#  define CU_EBADF WSAEBADF

#  define CU_POLLHUP 0

#  define CU_NET_GETERR() WSAGetLastError()
#  define CU_NET_SETERR(e) WSASetLastError(e)
#  define CU_NETSIG_SETUP() CU_EMPTY()
#  define CU_NETSIG_SETFUNC(f) signal(SIGINT, f)

typedef int cu_net_data;
typedef ULONG cu_net_pollcnt;

static WSADATA cu_net_wsastate;
static char *cu_net_wsaerrstr;

#endif

CU_ATTRIB_NOTHROW
static int cu_net_setsockopt(cu_socket sock, int opt, int val, size_t len)
{
#if CU_OS_UNIX
	return setsockopt(sock, SOL_SOCKET, opt, &val, (socklen_t)len) != CU_SOCKET_ERROR;
#else
	BOOL res = (BOOL)val;
	return setsockopt(sock, SOL_SOCKET, opt, (char *)&res, (int)len) != CU_SOCKET_ERROR;
#endif
}

CU_ATTRIB_NOTHROW static void cu_net_sigint(int unused) { CU_UNUSED(unused); }

CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1)) static int cu_net_closesock(cu_socket *fd)
{
	int saved, res;
	if (*fd == CU_INVALID_SOCKET) return 0;
	saved = CU_NET_GETERR();
	res = cu_close(*fd) != CU_SOCKET_ERROR;
	*fd = CU_INVALID_SOCKET;
	CU_NET_SETERR(saved);
	return res;
}

CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1, 2)) static void cu_net_getaddrip(void *CU_RESTRICT ai_addr, char *CU_RESTRICT buf)
{
	inet_ntop(
		((struct sockaddr_storage *)ai_addr)->ss_family,
		((struct sockaddr_storage *)ai_addr)->ss_family == AF_INET ?
			(void *)&((struct sockaddr_in *)ai_addr)->sin_addr :
			(void *)&((struct sockaddr_in6 *)ai_addr)->sin6_addr,
		buf, (socklen_t)(INET6_ADDRSTRLEN)
	);
}

CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1)) static enum cu_net_error cu_net_getremotesock(cu_net_remote *CU_RESTRICT remote, u16 port, const char *CU_RESTRICT server_addr)
{
	struct addrinfo hints, *addr_list, *addr_it;
	int res, is_server = server_addr == NULL;
	char portbuf[6];

	if (port < 1024) return CUERR_ARGS;
	cu_sprintf(portbuf, 6, "%hu", port);

	memset(&hints, 0, sizeof hints);
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = is_server ? AI_PASSIVE : 0;
	hints.ai_family = AF_INET;

	remote->fd = CU_INVALID_SOCKET;
	remote->ext = NULL;

	if ((cu_net_gai_err = getaddrinfo(server_addr, portbuf, &hints, &addr_list)) != 0) return cu_net_gai_err == EAI_MEMORY ? CUERR_MEM : CUERR_ADDR;

	for (addr_it = addr_list; addr_it; addr_it = addr_it->ai_next) {
		if ((remote->fd = socket(addr_it->ai_family, addr_it->ai_socktype, addr_it->ai_protocol)) == CU_INVALID_SOCKET) continue;
		if (is_server) {
			cu_net_setsockopt(remote->fd, SO_REUSEADDR, 1, CU_UWSZ(sizeof(int), sizeof(BOOL)));
			if ((res = bind(remote->fd, addr_it->ai_addr, (socklen_t)addr_it->ai_addrlen)) == CU_SOCKET_ERROR) goto fail;
		} else if ((res = connect(remote->fd, addr_it->ai_addr, (socklen_t)addr_it->ai_addrlen)) == CU_SOCKET_ERROR) goto fail;
		break;
	fail:
		cu_net_closesock(&remote->fd);
	}

	if (remote->fd != CU_INVALID_SOCKET) cu_net_getaddrip(addr_it->ai_addr, remote->ip);
	freeaddrinfo(addr_list);

	return remote->fd == CU_INVALID_SOCKET ? CUERR_CONNECT : CUERR_NONE;
}


CU_API_SOURCE enum cu_net_error cu_client_start(cu_net_remote *CU_RESTRICT server_info, const char *CU_RESTRICT address, u16 port)
{
	return cu_net_getremotesock(server_info, port, address);
}

CU_API_SOURCE void cu_client_listen(cu_net_remote *CU_RESTRICT server_info, cu_client_event event_handler, int heartbeat_delay_msec)
{
	struct pollfd server_pollfd;
	void *recvd; uptr bytes;
	enum cu_net_error err;

	CU_NETSIG_SETUP();
	CU_NETSIG_SETFUNC(cu_net_sigint);

	server_pollfd.events = POLLIN | CU_POLLHUP;
	server_pollfd.fd = server_info->fd;
	server_pollfd.revents = 0;

	while (!cu_net_isclosed(server_info)) {
		int pollres = poll(&server_pollfd, 1, heartbeat_delay_msec);
		if (pollres == CU_SOCKET_ERROR) {
			if (CU_NET_GETERR() == CU_EINTR) goto client_intr;
			else goto client_msgerr;
		} else if (pollres == 0) {
			event_handler(server_info, CUEVT_HEARTBEAT, NULL, 0);
			continue;
		}

		err = cu_net_recvmsg(server_info, &recvd, &bytes);
		if (err == CUERR_NONE) event_handler(server_info, CUEVT_MESSAGE, recvd, bytes);
		else if (err == CUERR_CONNECT || CU_NET_GETERR() == CU_ENOTSOCK) {
			if (CU_NET_GETERR() != CU_ENOTSOCK) event_handler(server_info, CUEVT_DISCONNECT, NULL, CU_NET_GETERR() == CU_EBADF);
			break;
		} else if (err == CUERR_MEM) event_handler(server_info, CUEVT_ALLOCDMEMERR, NULL, bytes);
		else if (CU_NET_GETERR() == CU_EINTR) { client_intr: if (!event_handler(server_info, CUEVT_SIGNAL, NULL, 0)) break; }
		else if (!cu_net_isclosed(server_info)) { client_msgerr: event_handler(server_info, CUEVT_MSGLISTENERR, NULL, 0); }
	}

	CU_NETSIG_SETFUNC(SIG_DFL);
}

CU_API_SOURCE void cu_client_close(cu_net_remote *server_info) { cu_net_closesock(&server_info->fd); }


CU_API_SOURCE enum cu_net_error cu_server_start(cu_net_server *server, u16 port, int max_clients)
{
	enum cu_net_error sockerr;
	if (max_clients < 1) return CUERR_ARGS;
	memset(server, 0, sizeof *server);

	if (!(server->remotes = (cu_net_remote *)malloc(sizeof *server->remotes * (size_t)(server->remotes_capacity = 4)))) return CUERR_MEM;
	if (!(server->pfds = (struct pollfd *)malloc(sizeof *server->pfds * (size_t)server->remotes_capacity))) { free(server->remotes); return CUERR_MEM; }
	if ((sockerr = cu_net_getremotesock(server->remotes, port, NULL)) != CUERR_NONE) { free(server->remotes); free(server->pfds); return sockerr; }
	if (listen(server->remotes->fd, SOMAXCONN) == CU_SOCKET_ERROR) { free(server->remotes); free(server->pfds); return CUERR_LISTEN; }

	server->max_clients = max_clients;
	server->pfds->fd = server->remotes->fd;
	server->pfds->events = POLLIN;
	server->pfds->revents = 0;

	return CUERR_NONE;
}

CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1))
static void cu_server_process_connection(cu_net_server *CU_RESTRICT server, cu_server_event event_handler)
{
	struct sockaddr_storage saddr;
	socklen_t addrlen = sizeof saddr;
	cu_net_remote *client;
	struct pollfd *cfd;
	cu_socket csock = CU_INVALID_SOCKET;

	if (server->clients_count >= server->max_clients) {
		event_handler(server, NULL, CUEVT_REMOTECONERR, NULL, (uptr)server->clients_count);
		goto decline;
	} else if (server->clients_count + 1 >= server->remotes_capacity) {
		size_t rmalloc = sizeof *server->remotes * (size_t)(server->remotes_capacity * 2), pfdalloc = sizeof *server->pfds * (size_t)(server->remotes_capacity * 2);
		void *nrm = realloc(server->remotes, rmalloc), *npfd = realloc(server->pfds, pfdalloc);

		if (!nrm || !npfd) {
			event_handler(server, NULL, CUEVT_ALLOCDMEMERR, NULL, !nrm ? rmalloc : pfdalloc);
			goto decline;
		}

		server->remotes_capacity *= 2;
		server->remotes = (cu_net_remote *)nrm;
		server->pfds = (struct pollfd *)npfd;
	}

	if ((csock = accept(server->pfds->fd, (struct sockaddr *)&saddr, &addrlen)) == CU_INVALID_SOCKET) {
		event_handler(server, NULL, CUEVT_REMOTECONERR, NULL, 0);
		return;
	}

	++server->clients_count;
	client = server->remotes + server->clients_count;
	cfd = server->pfds + server->clients_count;

	cu_net_getaddrip(&saddr, client->ip);
	client->ext = NULL;
	client->fd = csock;
	cfd->events = POLLIN;
	cfd->revents = 0;
	cfd->fd = csock;

	event_handler(server, client, CUEVT_CONNECT, NULL, (uptr)server->clients_count);
	return;
decline:
	if (csock == CU_INVALID_SOCKET) csock = accept(server->pfds->fd, (struct sockaddr *)&saddr, &addrlen);
	if (csock != CU_INVALID_SOCKET) cu_net_closesock(&csock);
}

CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((1, 3))
static void cu_server_process_message(cu_net_server *CU_RESTRICT server, cu_server_event event_handler, cu_net_remote *CU_RESTRICT client)
{
	void *recvd; uptr bytes;
	enum cu_net_error err = cu_net_recvmsg(client, &recvd, &bytes);

	if (err == CUERR_NONE) event_handler(server, client, CUEVT_MESSAGE, recvd, bytes);
	else if (err == CUERR_MEM) event_handler(server, client, CUEVT_ALLOCDMEMERR, NULL, bytes);
	else if (err == CUERR_CONNECT) cu_server_disconnect_client(server, client);
	else if (CU_NET_GETERR() == CU_EINTR) event_handler(server, client, CUEVT_SIGNAL, NULL, 0);
	else event_handler(server, client, CUEVT_MSGLISTENERR, NULL, 0);
}

CU_API_SOURCE void cu_server_listen(cu_net_server *CU_RESTRICT server, cu_server_event event_handler, int heartbeat_delay_msec)
{
	CU_NETSIG_SETUP();
	CU_NETSIG_SETFUNC(cu_net_sigint);

	while (!cu_net_isclosed(server->remotes)) {
		int i, pollres = poll(server->pfds, (cu_net_pollcnt)(server->clients_count + 1), heartbeat_delay_msec);
		if (pollres == 0) event_handler(server, NULL, CUEVT_HEARTBEAT, NULL, 0);
		else if (pollres == CU_SOCKET_ERROR) {
			if (!event_handler(server, NULL, CU_NET_GETERR() == CU_EINTR ? CUEVT_SIGNAL : CUEVT_MSGLISTENERR, NULL, 0)) break;
		} else for (i = 0; i <= server->clients_count; ++i) {
			if (!(server->pfds[i].revents & (POLLIN | POLLHUP))) continue;
			if (!i) cu_server_process_connection(server, event_handler);
			else {
				cu_server_process_message(server, event_handler, server->remotes + i);
				if (cu_net_isclosed(server->remotes + i)) {
					event_handler(server, server->remotes + i, CUEVT_DISCONNECT, NULL, (uptr)(server->clients_count - 1));
					server->remotes[i] = server->remotes[server->clients_count];
					server->pfds[i--] = server->pfds[server->clients_count--];
				}
			}
		}
	}

	free(server->pfds);
	free(server->remotes);
	memset(server, 0, sizeof *server);

	CU_NETSIG_SETFUNC(SIG_DFL);
}

CU_API_SOURCE enum cu_net_error cu_server_broadcast(const cu_net_server *CU_RESTRICT server, const void *CU_RESTRICT data, uptr bytes, cu_net_remote *CU_RESTRICT *CU_RESTRICT except, int except_length)
{
	int goterr = 0;
	int i, j;

	for (i = 1; i <= server->clients_count; ++i) {
		for (j = 0; j < except_length; ++j) {
			if (except[j] != server->remotes + i) continue;
			if (except_length > 1) except[0] = except[--except_length];
			goto outer;
		}
		goterr |= (cu_net_sendmsg(server->remotes + i, data, bytes) != CUERR_NONE);
	outer:
		continue;
	}

	return goterr ? CUERR_GENERIC : CUERR_NONE;
}

CU_API_SOURCE void cu_server_disconnect_client(cu_net_server *CU_RESTRICT server, cu_net_remote *CU_RESTRICT client)
{
	CU_UNUSED(server);
	cu_net_closesock(&client->fd);
}

CU_API_SOURCE void cu_server_close(cu_net_server *server)
{
	int i;
	if (!server->remotes) return;
	for (i = 0; i <= server->clients_count; ++i) cu_net_closesock(&server->remotes[i].fd);
}


CU_API_SOURCE int cu_net_init(void)
{
#if CU_OS_UNIX
	return 1;
#else
	if (WSAStartup(MAKEWORD(2, 2), &cu_net_wsastate) != 0) return 0;
	if (!(cu_net_wsaerrstr = (char *)calloc(1, 512))) return 0;
	if (LOBYTE(cu_net_wsastate.wVersion) == 2 && HIBYTE(cu_net_wsastate.wVersion) == 2) return 1;
	WSACleanup();
	return 0;
#endif
}

CU_API_SOURCE void cu_net_terminate(void)
{
#if !CU_OS_UNIX
	free(cu_net_wsaerrstr); WSACleanup();
#endif
}

CU_API_SOURCE enum cu_net_error cu_net_sendmsg(const cu_net_remote *CU_RESTRICT target, const void *CU_RESTRICT data, uptr bytes)
{
	uptr offset = 0;
	while (1) {
		cu_net_data res = send(target->fd, (const char *)(data) + offset, (CU_UWSZ(size_t, int))(bytes - offset), MSG_NOSIGNAL);
		if (res < 1) return CUERR_GENERIC;
		else if ((offset += (uptr)res) >= bytes) return CUERR_NONE;
	}
}

CU_API_SOURCE enum cu_net_error cu_net_recvmsg(const cu_net_remote *CU_RESTRICT target, void **CU_RESTRICT data, uptr *CU_RESTRICT bytes)
{
	cu_net_data res;
	void *rbuf_r;
	char tmpchr;
	CU_NET_SETERR(0);

	*bytes = 0;
	*data = NULL;
	res = recv(target->fd, &tmpchr, 1, 0);

	#define CU_NET_DISCONN_ERRCHK() (CU_NET_GETERR() == CU_ECONNRESET || CU_NET_GETERR() == CU_EBADF)

	if (res == 0 || CU_NET_DISCONN_ERRCHK()) return CUERR_CONNECT;
	else if (res == CU_SOCKET_ERROR) return CUERR_GENERIC;
	if (!(*data = calloc(1, CU_NET_RECVBUF + 1))) goto fail_mem;

	*((char *)*data) = tmpchr;
	*bytes = 1;

	while (1) {
		res = recv(target->fd, (char *)(*data) + *bytes, CU_NET_RECVBUF, 0);

		if (CU_NET_DISCONN_ERRCHK()) {
			if (*data) { free(*data); *data = NULL; }
			return CUERR_CONNECT;
		} else if (res == CU_SOCKET_ERROR) {
			if (*data) { free(*data); *data = NULL; }
			return CUERR_GENERIC;
		}

		*bytes += (size_t)res;

		if ((size_t)res < CU_NET_RECVBUF) return CUERR_NONE;
		else {
			rbuf_r = realloc(*data, (size_t)*bytes + CU_NET_RECVBUF + 1);
			if (!rbuf_r) {
				free(*data);
			fail_mem:
				*bytes += CU_NET_RECVBUF + 1;
				return CUERR_MEM;
			}
			*data = rbuf_r;
			memset((char *)(*data) + *bytes, 0, CU_NET_RECVBUF + 1);
		}
	}
}

CU_API_SOURCE char *cu_net_interfaces(char *ipbuf, int if_fmt, int id)
{
#if CU_OS_UNIX
	struct ifaddrs *ifd, *ifd_it;
	if (id < 0 || getifaddrs(&ifd) == -1) return NULL;

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

CU_API_SOURCE const char *cu_net_lasterr(void)
{
#if CU_OS_UNIX
	int gai = cu_net_gai_err;
	cu_net_gai_err = 0;
	return (gai && gai != EAI_SYSTEM) ? gai_strerror(gai) : strerror(errno);
#else
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), 0, cu_net_wsaerrstr, 512, NULL);
	return cu_net_wsaerrstr;
#endif
}

CU_API_SOURCE int cu_net_isclosed(const cu_net_remote *remote) { return remote->fd == CU_INVALID_SOCKET; }

#endif

/* ========================= Threading ======================== */

#if CU_SETTING_THREAD_FUNCS

#if CU_THREAD_POSIX_USED
#  include <pthread.h>
#  include <unistd.h>

CU_API_SOURCE cu_thread cu_thread_create(cu_thread_func function, void *arg)
{
	cu_thread thr;
	if (pthread_create(&thr, NULL, function, arg) != 0) return 0;
	return thr;
}
CU_API_SOURCE int cu_thread_join(cu_thread thread) { return pthread_join(thread, NULL) == 0; }
CU_API_SOURCE int cu_thread_detach(cu_thread thread) { return pthread_detach(thread) == 0; }

CU_API_SOURCE void cu_thread_sleep(u64 nsecs, u64 secs)
{
	struct timespec abstime;
	pthread_cond_t fcond = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t fmut = PTHREAD_MUTEX_INITIALIZER;
	abstime.tv_nsec = nsecs % 1000000000;
	abstime.tv_sec = (long)(secs + (nsecs / 1000000000));
	pthread_cond_timedwait(&fcond, &fmut, &abstime);
}
CU_API_SOURCE int cu_thread_count(void) { return (int)sysconf(_SC_NPROCESSORS_ONLN); }

CU_API_SOURCE int cu_thread_mutex_init(cu_thread_mutex *mutex) { return pthread_mutex_init(mutex, NULL) == 0; }
CU_API_SOURCE int cu_thread_mutex_lock(cu_thread_mutex *mutex) { return pthread_mutex_lock(mutex) == 0; }
CU_API_SOURCE int cu_thread_mutex_unlock(cu_thread_mutex *mutex) { return pthread_mutex_unlock(mutex) == 0; }
CU_API_SOURCE int cu_thread_mutex_trylock(cu_thread_mutex *mutex) { return pthread_mutex_trylock(mutex) == 0; }
CU_API_SOURCE int cu_thread_mutex_destroy(cu_thread_mutex *mutex) { return pthread_mutex_destroy(mutex) == 0; }

CU_API_SOURCE int cu_thread_cond_init(cu_thread_cond *cond) { return pthread_cond_init(cond, NULL) == 0; }
CU_API_SOURCE int cu_thread_cond_wait(cu_thread_cond *cond, cu_thread_mutex *mutex) { return pthread_cond_wait(cond, mutex) == 0; }
CU_API_SOURCE int cu_thread_cond_signal(cu_thread_cond *cond) { return pthread_cond_signal(cond) == 0; }
CU_API_SOURCE int cu_thread_cond_broadcast(cu_thread_cond *cond) { return pthread_cond_broadcast(cond) == 0; }
CU_API_SOURCE int cu_thread_cond_destroy(cu_thread_cond *cond) { return pthread_cond_destroy(cond) == 0; }
CU_API_SOURCE cu_thread cu_thread_self(void) { return pthread_self(); }
CU_API_SOURCE u64 cu_thread_pid(void) { return (u64)getpid(); }
#if CU_OS_MAC
CU_API_SOURCE u64 cu_thread_tid(void)
{
	uint64_t res;
	pthread_threadid_np(NULL, &res);
	return (u64)res;
}
#else
CU_API_SOURCE u64 cu_thread_tid(void) { return (u64)gettid(); }
#endif

#elif CU_THREAD_WIN_USED
#  include <windows.h>

CU_API_SOURCE cu_thread cu_thread_create(cu_thread_func function, void *arg) { return CreateThread(NULL, 0, function, arg, 0, 0); }
CU_API_SOURCE int cu_thread_join(cu_thread thread) { return WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0; }
CU_API_SOURCE int cu_thread_detach(cu_thread thread) { return CloseHandle(thread) != 0; }

CU_API_SOURCE void cu_thread_sleep(u64 nsecs, u64 secs)
{
	HANDLE timer;
	LARGE_INTEGER ft;
	ft.QuadPart = (secs * 10000000) + (nsecs / -100);

	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
	WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);
}
CU_API_SOURCE int cu_thread_count(void)
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return (int)info.dwNumberOfProcessors;
}

CU_API_SOURCE int cu_thread_mutex_init(cu_thread_mutex *mutex) { InitializeCriticalSection(mutex); return 1; }
CU_API_SOURCE int cu_thread_mutex_lock(cu_thread_mutex *mutex) { EnterCriticalSection(mutex); return 1; }
CU_API_SOURCE int cu_thread_mutex_unlock(cu_thread_mutex *mutex) { LeaveCriticalSection(mutex); return 1; }
CU_API_SOURCE int cu_thread_mutex_trylock(cu_thread_mutex *mutex) { return TryEnterCriticalSection(mutex) != 0; }
CU_API_SOURCE int cu_thread_mutex_destroy(cu_thread_mutex *mutex) { DeleteCriticalSection(mutex); return 1; }

CU_API_SOURCE int cu_thread_cond_init(cu_thread_cond *cond) { InitializeConditionVariable(cond); return 1; }
CU_API_SOURCE int cu_thread_cond_wait(cu_thread_cond *cond, cu_thread_mutex *mutex) { return SleepConditionVariableCS(cond, mutex, INFINITE) != 0; }
CU_API_SOURCE int cu_thread_cond_signal(cu_thread_cond *cond) { WakeConditionVariable(cond); return 1; }
CU_API_SOURCE int cu_thread_cond_broadcast(cu_thread_cond *cond) { WakeAllConditionVariable(cond); return 1; }
CU_API_SOURCE int cu_thread_cond_destroy(cu_thread_cond *cond) { return 1; }

CU_API_SOURCE cu_thread cu_thread_self(void) { return GetCurrentThread(); }
CU_API_SOURCE u64 cu_thread_pid(void) { return (u64)GetCurrentProcessId(); }
CU_API_SOURCE u64 cu_thread_tid(void) { return (u64)GetCurrentThreadId(); }

#elif CU_THREAD_C_USED
#  include <threads.h>
#  include <time.h>

CU_API_SOURCE cu_thread cu_thread_create(cu_thread_func function, void *arg)
{
	thrd_t thr;
	if (thrd_create(&thr, function, arg) != thrd_success) return 0;
	return thr;
}
CU_API_SOURCE int cu_thread_join(cu_thread thread) { return thrd_join(thread, NULL) == thrd_success; }
CU_API_SOURCE int cu_thread_detach(cu_thread thread) { return thrd_detach(thread) == thrd_success; }

CU_API_SOURCE void cu_thread_sleep(u64 nsecs, u64 secs)
{
	struct timespec ts;
	ts.tv_nsec = nsecs % 1000000000;
	ts.tv_sec = (long)(secs + (nsecs / 1000000000));
	while (thrd_sleep(&ts, &ts) == -1);
}
CU_API_SOURCE int cu_thread_count(void) { return 1; }

CU_API_SOURCE int cu_thread_mutex_init(cu_thread_mutex *mutex) { return mtx_init(mutex, mtx_plain) == thrd_success; }
CU_API_SOURCE int cu_thread_mutex_lock(cu_thread_mutex *mutex) { return mtx_lock(mutex) == thrd_success; }
CU_API_SOURCE int cu_thread_mutex_unlock(cu_thread_mutex *mutex) { return mtx_unlock(mutex) == thrd_success; }
CU_API_SOURCE int cu_thread_mutex_trylock(cu_thread_mutex *mutex) { return mtx_trylock(mutex) == thrd_success; }
CU_API_SOURCE int cu_thread_mutex_destroy(cu_thread_mutex *mutex) { mtx_destroy(mutex); return 1; }

CU_API_SOURCE int cu_thread_cond_init(cu_thread_cond *cond) { return cnd_init(cond) == thrd_success; }
CU_API_SOURCE int cu_thread_cond_wait(cu_thread_cond *cond, cu_thread_mutex *mutex) { return cnd_wait(cond, mutex) == thrd_success; }
CU_API_SOURCE int cu_thread_cond_signal(cu_thread_cond *cond) { return cnd_signal(cond) == thrd_success; }
CU_API_SOURCE int cu_thread_cond_broadcast(cu_thread_cond *cond) { return cnd_broadcast(cond) == thrd_success; }
CU_API_SOURCE int cu_thread_cond_destroy(cu_thread_cond *cond) { return cnd_destroy(cond) == thrd_success; }

CU_API_SOURCE cu_thread cu_thread_self(void) { return thrd_current(); }
CU_API_SOURCE u64 cu_thread_pid(void) { return (u64)0; }
CU_API_SOURCE u64 cu_thread_tid(void) { return (u64)0; }

#endif

CU_API_SOURCE int cu_thread_split(cu_thread_func func, u64 work_count, void **each_thread_arg, int thread_count)
{
	struct cu_split { cu_thread thread; cu_thread_split_arg arg; } *thrs;
	u64 effective_threads, div, remaining, c, i;

	if (!work_count) return 0;
	else if (thread_count <= 0) return 0;

	effective_threads = (u64)(thread_count - 1);
	if (effective_threads && !(thrs = (struct cu_split *)malloc(sizeof *thrs * (size_t)thread_count))) return 0;
	div = work_count / (u64)thread_count;
	remaining = work_count - (div * (u64)thread_count);

	for (i = c = 0; i <= effective_threads; ++i) {
		cu_thread_split_arg *a = &thrs[i].arg;
		a->thread_arg = each_thread_arg ? each_thread_arg[i] : NULL;
		a->start_index = c;
		a->end_index = (u64)(c += div + (remaining ? (--remaining, 1) : 0));
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

CU_THREAD_FUNCTION(cu_thread_pool_inner, arg)
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
