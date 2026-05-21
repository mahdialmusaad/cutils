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

#if CU_REPLACED_ALLOC
#  undef malloc
#  undef calloc
#  undef realloc
#  undef free
#endif

#if CU_SETTING_FUNCS

/* ==========================================================================
 *
 * ---------------------------- String functions ----------------------------
 *
 * ========================================================================== */

#if !defined(_ISOC99_SOURCE)
#  define _ISOC99_SOURCE
#endif

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#if CU_LANG_C >= CU_LANG_C99 || ((defined (__va_copy) || defined(va_copy)) && (CU_OS_UNIX || CU_COMPVER(MSVC, 19, 0)))
#  define CUSTR_FMT
#  if CU_COMP_MSVC
#    define __STDC_WANT_LIB_EXT1__ 1
#  endif
#  include <stdio.h>
#endif

CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_NOTHROW CU_ATTRIB_WARN_UNUSED_RESULT
static int custr_istemp(const custr *c) { return c->cap == CU_UPTRMAX; }

CU_ATTRIB_NEVERINLINE CU_ATTRIB_CONST CU_ATTRIB_NOTHROW CU_ATTRIB_WARN_UNUSED_RESULT
static uptr custr_npow2(uptr v)
{
	v--;
	v |= CU_DWSHIFT(v, 1);
	v |= CU_DWSHIFT(v, 2);
	v |= CU_DWSHIFT(v, 4);
	v |= CU_DWSHIFT(v, 8);
#if CU_UPTR_BYTES >= 4
	v |= CU_DWSHIFT(v, 16);
#endif
#if CU_UPTR_BYTES >= 8
	v |= CU_DWSHIFT(v, 32);
#endif
	v++;
	return v;
}

CU_API_SOURCE int custr_create(custr *c, const char *str)
{
	const uptr len = (size_t)strlen(str), allocd = custr_npow2(len + 1);
	if (CU_UNLIKELY(!(c->str = malloc((size_t)allocd)))) return 0;
	memcpy(c->str, str, (size_t)(len + 1));
	c->len = len;
	c->cap = allocd;
	return 1;
}
CU_DIAGNOSTICS_PUSH
CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS
CU_PRAGMA(GCC diagnostic ignored "-Wcast-qual")
CU_API_SOURCE custr *custr_allocd(custr *c, const char *allocdstr, uptr allocd)
{
	c->len = strlen(allocdstr);
	if (CU_UNLIKELY(allocd && allocd <= c->len)) return NULL;
	c->str = (char *)allocdstr;
	c->cap = allocd ? allocd : c->len + 1;
	return c;
}
CU_DIAGNOSTICS_POP

#if CU_THREAD_LOCAL_AVAILABLE

static CU_THREAD_LOCAL int custr_used = 0;
#define CUSTR_C_END(tmp_var) do { custr_used &= !custr_istemp(tmp_var); } while (0)

CU_DIAGNOSTICS_PUSH
CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS
CU_PRAGMA(GCC diagnostic ignored "-Wcast-qual")
CU_API_SOURCE custr *custr_c(const char *tmpstr)
{
	static CU_THREAD_LOCAL custr c_custr, c_custr2;
	custr *target = custr_used ? &c_custr2 : &c_custr;

	target->str = (char *)tmpstr;
	target->len = strlen(tmpstr);
	target->cap = CU_UPTRMAX;
	custr_used = 1;

	return target;
}
CU_DIAGNOSTICS_POP

#else

CU_DIAGNOSTICS_PUSH
CU_DIAGNOSTICS_DISABLE_UNKNOWN_PRAGMAS
CU_PRAGMA(GCC diagnostic ignored "-Wcast-qual")

#define CUSTR_C_END(tmp_var) do { if (custr_istemp(tmp_var)) custr_clear(tmp_var); } while (0) \

CU_API_SOURCE custr *custr_c(const char *tmpstr)
{
	custr *target = malloc(sizeof *target);
	if (!target) return NULL;

	target->len = strlen(tmpstr);
	target->str = malloc(target->len + 1);
	if (!target->str) return NULL;
	memcpy(target->str, tmpstr, target->len + 1);

	target->cap = CU_UPTRMAX;
	return target;
}

#endif

CU_API_SOURCE custr *custr_char(char chr)
{
	static CU_THREAD_LOCAL char chrstr[2] = { '\0', '\0' };
	chrstr[0] = chr;
	return custr_c(chrstr);
}

CU_API_SOURCE int custr_reserve(custr *c, uptr bytes)
{
	void *ptr;
	CU_ASSERT(!custr_istemp(c));

	if (CU_UNLIKELY(!bytes)) {
		custr_clear(c);
		return 1;
	}

	bytes = custr_npow2(bytes);
	if (CU_LIKELY(bytes < 8)) bytes = 8;

	if (bytes == c->cap) return 1;
	else if (bytes < c->len) return custr_shrinkto(c, bytes - 1);
	else if (bytes < c->cap) return 1;

	ptr = realloc(c->str, (size_t)bytes);
	if (CU_UNLIKELY(!ptr)) return 0;

	c->str = ptr;
	c->cap = bytes;

	return 1;
}
CU_API_SOURCE int custr_copy(const custr *copy, custr *paste)
{
	CU_ASSERT(!custr_istemp(paste));
	paste->len = copy->len;
	paste->cap = paste->len + 1;
	if (CU_UNLIKELY(!(paste->str = malloc((size_t)paste->cap)))) return 0;
	memcpy(paste->str, copy->str, (size_t)paste->cap);
	return 1;
}

CU_API_SOURCE int custr_shrinkto(custr *c, uptr shrinked_len)
{
	CU_ASSERT(!custr_istemp(c));
	if (CU_UNLIKELY(shrinked_len >= c->len)) return 0;
	if (!c->str) return 1;
	c->len = shrinked_len;
	c->str[shrinked_len] = '\0';
	return 1;
}
CU_API_SOURCE void custr_clear(custr *c)
{
	CU_ASSERT(!custr_istemp(c));
	if (c->str) {
		free(c->str);
		c->str = NULL;
	}
	c->len = 0;
	c->cap = 0;
}
CU_API_SOURCE int custr_optimize(custr *c)
{
	c->cap = 0;
	return custr_reserve(c, c->len);
}

CU_API_SOURCE int custr_insert(custr *c, uptr c_offset, const custr *to_insert, uptr to_insert_offset)
{
	uptr needed;
	CU_ASSERT(!custr_istemp(c));
	CUSTR_C_END(to_insert);

	if (!to_insert) return 0;
	if (c_offset > c->len || to_insert_offset >= to_insert->len) return 0;

	needed = c->len + (to_insert->len - to_insert_offset) + 1;
	if (needed > c->cap && CU_UNLIKELY(!custr_reserve(c, needed))) return 0;

	memmove(c->str + c_offset + (to_insert->len - to_insert_offset), c->str + c_offset, (size_t)((c->len + (c_offset != c->len)) - c_offset));
	memmove(c->str + c_offset, to_insert->str + to_insert_offset, (size_t)(to_insert->len - to_insert_offset + (c_offset == c->len)));

	c->len = needed - 1;
	return 1;
}
CU_API_SOURCE int custr_append(custr *c, const custr *to_append, uptr to_append_offset)
{
	return custr_insert(c, c->len, to_append, to_append_offset);
}

CU_API_SOURCE int custr_sub(const custr *c, custr *subresult, uptr start_ind, uptr end_ind)
{
	uptr charcount;
	CUSTR_C_END(c);

	if (CU_UNLIKELY(start_ind > end_ind || start_ind >= c->len)) return 0;
	if (CU_UNLIKELY(end_ind >= c->len)) end_ind = c->len - 1;

	memset(subresult, 0, sizeof *subresult);
	charcount = (end_ind - start_ind) + 1;

	custr_reserve(subresult, charcount + 1);
	memcpy(subresult->str, c->str + start_ind, (size_t)charcount);
	subresult->str[charcount] = '\0';
	subresult->len = charcount;

	return 1;
}
CU_API_SOURCE int custr_tosub(custr *c, uptr start_ind, uptr end_ind)
{
	CU_ASSERT(!custr_istemp(c));
	if (CU_UNLIKELY(start_ind > end_ind || start_ind > c->len)) return 0;
	if (CU_UNLIKELY(end_ind > c->len)) end_ind = c->len;

	custr_shrinkto(c, end_ind);
	if (start_ind) memmove(c->str, c->str + start_ind, (size_t)(c->len - start_ind));
	return 1;
}

CU_API_SOURCE void custr_cut(custr *c, uptr start_ind, uptr count)
{
	CU_ASSERT(!custr_istemp(c));

	if (CU_UNLIKELY(start_ind >= c->len)) return;
	if (CU_UNLIKELY(start_ind + count >= c->len)) count = c->len - start_ind - 1;
	if (CU_UNLIKELY(count == 0)) return;

	memmove(c->str + start_ind, c->str + start_ind + count, (size_t)((c->len + 1) - start_ind - count));
	c->len -= count;
}

CU_API_SOURCE int custr_count(const custr *c, char target)
{
	int n = 0;
	uptr offset = CU_UPTRMAX;
	CUSTR_C_END(c);

	while (1)  {
		offset = custr_find(c, offset + 1, target, 0);
		if (offset == CU_UPTRMAX) break;
		++n;
	}

	return n;
}

CU_API_SOURCE int custr_fmt(custr *c, char *fmt, ...)
{
#if defined (CUSTR_FMT)
	va_list va, copy;
	int nchars, ret = 0;

	va_start(va, fmt);

#if defined (__va_copy)
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

CU_API_SOURCE int custr_cd(custr *c, const custr *name)
{
	custr tmp;
	int ret = 0;
	CU_ASSERT(!custr_istemp(c));

	if (!custr_simplify(c)) goto fail;

	if (!name) {
		uptr ind;
		ind = custr_find(c, 0, CU_FILE_SEPARATOR, -1 - (c->str[c->len - 1] == CU_FILE_SEPARATOR));
		if (CU_UNLIKELY(ind != CU_UPTRMAX)) custr_shrinkto(c, ind + 1);
		return 1;
	}

	if (c->str[c->len - 1] != CU_FILE_SEPARATOR) {
		if (!custr_append(c, custr_c(CU_FILE_SEPARATOR_DQ), 0)) goto fail;
	}

	tmp.cap = CU_UPTRMAX - 1;
	if (!custr_copy(name, &tmp)) goto fail;
	if (!custr_simplify(&tmp)) goto fail;
	ret = custr_append(c, &tmp, tmp.str[0] == CU_FILE_SEPARATOR);
	custr_clear(&tmp);

fail:
	CUSTR_C_END(name);
	return ret;
}
CU_API_SOURCE int custr_simplify(custr *c)
{
	custr *sections = calloc(1, sizeof *sections);
	uptr ind, next, i, cursection = 0, nsections = 1;
	int ret = 0;

	CU_ASSERT(!custr_istemp(c));
	if (CU_UNLIKELY(!sections)) return 0;

	ind = custr_find(c, 0, CU_FILE_SEPARATOR, 0);
	if (ind == CU_UPTRMAX) return 1;
	if (ind != 0 && !custr_sub(c, sections + cursection++, 0, --ind)) goto fail;
	else ind = CU_UPTRMAX;

	while (1) {
		ind = custr_find(c, ind + 1, CU_FILE_SEPARATOR, 0);
		if (CU_UNLIKELY(ind == CU_UPTRMAX)) break;
		if (CU_UNLIKELY(c->str[ind + 1] == CU_FILE_SEPARATOR)) continue;

		if (strncmp(c->str + ind + 1, ".." CU_FILE_SEPARATOR_DQ, 3) == 0) {
			cursection = cursection == 0 ? 0 : cursection - 1;
			continue;
		} else if (strncmp(c->str + ind + 1, "." CU_FILE_SEPARATOR_DQ, 2) == 0) continue;

		if (cursection >= nsections) {
			uptr origcnt = nsections;
			void *inc = realloc(sections, sizeof *sections * (size_t)(nsections += 2));
			if (CU_UNLIKELY(!inc)) goto fail;
			sections = inc;
			memset(sections + origcnt, 0, sizeof *sections * 2);
		}

		next = custr_find(c, ind + 1, CU_FILE_SEPARATOR, 0);
		if (!custr_sub(c, sections + cursection++, ind, next - 1)) goto fail;
	}

	custr_clear(c);
	for (i = 0; i < cursection; ++i) custr_append(c, sections + i, 0);
	ret = 1;
fail:
	for (i = 0; i < nsections; ++i) custr_clear(sections + i);
	free(sections);
	return ret;
}

CU_API_SOURCE uptr custr_find(const custr *c, uptr c_offset, char target, int n)
{
	iptr i;
	CUSTR_C_END(c);
	if (CU_LIKELY(n >= 0)) { for (i = (iptr)c_offset; i < (iptr)c->len; ++i) if (c->str[i] == target && !(n--)) return (uptr)i; }
	else if (CU_LIKELY(c_offset <= c->len)) { for (i = (iptr)(c->len - c_offset - 1); i >= 0; --i) if (c->str[i] == target && !(++n)) return (uptr)i; }
	return CU_UPTRMAX;
}
CU_API_SOURCE uptr custr_findsub(const custr *c, uptr c_offset, const custr *target_substr, int n)
{
	uptr positive = n >= 0;
	int nsearch = -1 + (int)positive;
	CUSTR_C_END(c);
	CUSTR_C_END(target_substr);

	if (CU_UNLIKELY(!positive)) n = -n - 1;
	c_offset -= positive;
	
	while (1) {
		c_offset = custr_find(c, c_offset + positive, target_substr->str[0], nsearch);
		if (CU_UNLIKELY(c_offset == CU_UPTRMAX || ((c->len - c_offset) < target_substr->len))) return CU_UPTRMAX;
		if (memcmp(c->str + c_offset, target_substr->str, (size_t)target_substr->len) == 0 && !(n--)) return c_offset;
		if (CU_UNLIKELY(!positive)) c_offset = c->len - c_offset;
	}

	CU_UNREACHABLE();
}

CU_API_SOURCE void custr_replace(custr *c, uptr c_offset, char target, char replacement)
{
	uptr i;

	CU_ASSERT(!custr_istemp(c));
	if (CU_UNLIKELY(target == '\0')) return;

	for (i = c_offset; i < c->len; ++i) {
		if (CU_LIKELY(c->str[i] != target)) continue;
		if (replacement == '\0') {
			memmove(c->str + i, c->str + i + 1, (size_t)(c->len - i - 1));
			custr_shrinkto(c, c->len - 1);
		} else c->str[i] = replacement;
	}
}
CU_API_SOURCE int custr_replacesub(custr *c, uptr c_offset, const custr *target, const custr *replacement)
{
	uptr combined;
	int reptype;
	enum { nulrep, smallrep, eqrep, largerep };

	CU_ASSERT(!custr_istemp(c));
	CUSTR_C_END(target);
	CUSTR_C_END(replacement);

	if (CU_UNLIKELY(!target || !target->len)) return 1;

	if (!replacement || !replacement->len) reptype = nulrep;
	else if (replacement->len < target->len) reptype = smallrep;
	else if (replacement->len == target->len) reptype = eqrep;
	else reptype = largerep;

	while (1) {
		c_offset = custr_findsub(c, c_offset, target, 0);
		if (c_offset == CU_UPTRMAX) break;

		switch (reptype) {
		case nulrep:
			memmove(c->str + c_offset, c->str + c_offset + target->len, (size_t)((c->len + 1) - c_offset - target->len));
			break;
		case smallrep:
			memmove(c->str + c_offset + replacement->len, c->str + c_offset + target->len, (size_t)((c->len + 1) - c_offset - target->len));
			CU_FALLTHROUGH;
		case eqrep:
			memcpy(c->str + c_offset, replacement->str, (size_t)(replacement->len));
			break;
		case largerep:
			combined = c->len + 1 + (replacement->len - target->len);
			if (!custr_reserve(c, combined)) return 0;
			memmove(c->str + c_offset + replacement->len, c->str + c_offset + target->len, (size_t)((c->len + 1) - c_offset - target->len));
			memcpy(c->str + c_offset, replacement->str, (size_t)(replacement->len));
			break;
		default:
			break;
		}
	
		c->len -= target->len - replacement->len;
		c_offset += replacement->len;
	}

	return 1;
}

#if !CU_THREAD_LOCAL_AVAILABLE
CU_DIAGNOSTICS_POP
#endif

/* ==========================================================================
 *
 * -------------------------- Filesystem functions --------------------------
 *
 * ========================================================================== */

#if CU_SETTING_FILE_FUNCS

#if CU_COMP_MSVC
#  undef __STDC_WANT_LIB_EXT1__
#  define __STDC_WANT_LIB_EXT1__ 1
#endif

#if CU_OS_WINDOWS
#  include <io.h>
#  include <direct.h>
#  define mkdir(path, mode) _mkdir(path)
#  define access _access
#  define F_OK 0
#else
#  include <unistd.h>
#endif

#include <stdio.h>
#include <sys/stat.h>

#include <string.h>
#include <stdlib.h>

#if CU_OS_WINDOWS
#include <windows.h>

CU_API_SOURCE int cu_dir_delete(const char *path, int recursive)
{
	WIN32_FIND_DATA fd;
	size_t full_len, dir_bytes;
	HANDLE dirhandle;
	char *dir_path;
	int res;

	if (!recursive) return RemoveDirectoryA(path) != 0;

	full_len = strlen(path);
	dir_bytes = full_len > (CU_PATH_MAX - 30) ? full_len * 2 : CU_PATH_MAX;
	dir_path = malloc(dir_bytes);
	
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
			dir_path = r;
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
}

#elif CU_HAS_INCLUDE(<dirent.h>) && !CU_ARCH_M68K && !defined (__MSP430__)
#include <dirent.h>

CU_API_SOURCE int cu_dir_delete(const char *path, int recursive)
{
	struct dirent *entry;
	uptr plen, dlen, entrylen;
	char *dbuf;
	int ret = 0;
	DIR *dir;

	if (!recursive) return rmdir(path) == 0;

	plen = strlen(path);
	dlen = 0xFF;

	if (!(dbuf = malloc(plen + dlen + (path[plen - 1] != CU_FILE_SEPARATOR) + 1))) return 0;
	if (!(dir = opendir(path))) goto fail;
	
	memcpy(dbuf, path, plen);
	if (path[plen - 1] != CU_FILE_SEPARATOR) dbuf[plen++] = '/';

	while ((entry = readdir(dir))) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
		entrylen = strlen(entry->d_name) + 1;

		if (entrylen > dlen) {
			void *rel = realloc(dbuf, plen + entrylen);
			if (!rel) goto fail_dir;
			dbuf = rel;
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
}

#else
#include <unistd.h>
CU_API_SOURCE int cu_dir_delete(const char *path, int recursive)
{
	if (!recursive) return rmdir(path) == 0;
	else return 0;
}

#endif

CU_API_SOURCE int cu_file_exists(const char *path)
{
	return access(path, F_OK) == 0;
}
CU_API_SOURCE int cu_dir_exists(const char *path)
{
	struct stat dir_stat;
	return stat(path, &dir_stat) == 0 && ((dir_stat.st_mode & S_IFDIR) == S_IFDIR);
}

CU_API_SOURCE int cu_dir_create(const char *path)
{
	if (cu_dir_exists(path)) return 1;
	return mkdir(path, 0777) == 0;
}

CU_API_SOURCE void *cu_file_read(const char *path, void *result, int binary_file, uptr *bytes)
{
	uptr to_read, filesize;
	struct stat file_stat;
	FILE *file;
	int res = 0;

#if CU_COMP_MSVC
	if (CU_UNLIKELY(fopen_s(&file, path, binary_file ? "rb" : "r") != 0)) return 0;
	if (CU_UNLIKELY(fstat(_fileno(file), &file_stat) == -1)) goto fail;
#else
	if (CU_UNLIKELY(!(file = fopen(path, binary_file ? "rb" : "r")))) return 0;
	if (CU_UNLIKELY(fstat(fileno(file), &file_stat) == -1)) goto fail;
#endif

	filesize = (uptr)file_stat.st_size;
	to_read = (bytes && *bytes != 0) ? (*bytes > filesize ? filesize : *bytes) : filesize;

	if (!result && !(result = malloc((size_t)filesize))) goto fail;
	res = fread(result, 1, (size_t)to_read, file) == (size_t)to_read;

	if (bytes) *bytes = filesize;
fail:
	fclose(file);
	return res ? result : NULL;
}

CU_API_SOURCE int cu_file_write(const char *path, const void *content, unsigned int mode, uptr bytes)
{
	static const char *mode_strs[] = { "w", "wb", "a", "ab" };
	FILE *file; int res;

#if CU_COMP_MSVC
	if (CU_UNLIKELY(mode > 3)) return 0;
	if (CU_UNLIKELY(fopen_s(&file, path, mode_strs[mode]) != 0)) return 0;
#else
	if (CU_UNLIKELY(mode > 3)) return 0;
	if (CU_UNLIKELY(!(file = fopen(path, mode_strs[mode])))) return 0;
#endif

	res = fwrite(content, (size_t)bytes, 1, file) == 1;
	fclose(file);

	return res;
}

CU_API_SOURCE int cu_file_getinfo(const char *path, cu_file_info *f_info)
{
	struct stat dir_stat;
	if (!stat(path, &dir_stat)) return 0;
	f_info->fsize_bytes = dir_stat.st_size;
	f_info->access_time = dir_stat.st_atime;
	f_info->mod_time = dir_stat.st_mtime;
	return 1;
}

CU_API_SOURCE int cu_file_delete(const char *path)
{
	return remove(path) == 0;
}

#if CU_OS_APPLE
#  include <mach-o/dyld.h>
#endif

CU_API_SOURCE char *cu_file_exe_path(const char *argv, uptr *allocd)
{
#if !CU_OS_APPLE && !CU_OS_WINDOWS
	char *dup;
	uptr len;
#endif
#if CU_OS_WINDOWS
	DWORD res;
	char *buf = malloc(CU_PATH_MAX);
	CU_UNUSED(argv);
	if (!buf) return NULL;
	res = GetModuleFileName(NULL, buf, CU_PATH_MAX);
	if (allocd) *allocd = CU_PATH_MAX;
	return buf;
#elif CU_OS_APPLE
	char *path = malloc(CU_PATH_MAX);
	uint32_t sz = sizeof path;
	uptr len;

	if (!path) return NULL;
	if (_NSGetExecutablePath(path, &sz) == 0) goto success;

	void *r_res = realloc(path, (uptr)sz);
	if (!r_res) {
		free(path);
		return NULL;
	}
	path = r_res;

	_NSGetExecutablePath(path, &sz);
success:
	if (allocd) *allocd = strlen(path) + 1u;
	free(path);
	return path;
#elif CU_OS_UNIX
	struct stat s;
	if (argv[0] == '/') goto skip;
	else if (lstat(argv, &s) != -1 && S_ISLNK(s.st_mode)) {
		iptr readlen = 0;
		uptr buflen = 0;
		char *buf = NULL;
	
		do {
			void *res = realloc(buf, buflen += CU_PATH_MAX);
			if (!res) goto fail_readlink;
			buf = res;

			readlen = readlink(argv, buf, buflen);
			if (readlen <= -1) goto fail_readlink;
			if ((uptr)readlen < buflen) break;
		} while (1);

		buf[readlen] = '\0';
		if (allocd) *allocd = (uptr)readlen + 1;
		return buf;
	fail_readlink:
		free(buf);
		return NULL;
	} else if (strchr(argv, '/')) {
		uptr buflen = CU_PATH_MAX * 2, cwdlen = 0, argvlenterm = strlen(argv) + 1;
		char *buf = malloc(buflen);

		if (buf && getcwd(buf, buflen) && (cwdlen = strlen(buf)) < (buflen - argvlenterm)) {
			if (strncmp(argv, "./", 2) == 0) ++argv;
			memcpy(buf + cwdlen, argv, argvlenterm);
			if (allocd) *allocd = buflen;
			return buf;
		} else {
			free(buf);
			return NULL;
		}
	} else return NULL;
skip:
#endif
#if !CU_OS_APPLE && !CU_OS_WINDOWS
	if (!argv) return NULL;
	len = strlen(argv) + 1;
	dup = malloc(len);
	if (!dup) return NULL;
	memcpy(dup, argv, len);
	if (allocd) *allocd = len;
	return dup;
#endif
}

#endif

/* ==========================================================================
 *
 * ------------------------------ RNG functions -----------------------------
 *
 * ========================================================================== */

#if CU_SETTING_RAND_FUNCS

#if CU_OS_WINDOWS && CU_HAS_INCLUDE(<bcrypt.h>)
#  include <ntsecapi.h>
#elif CU_OS_UNIX
#  include <stdio.h>
#endif

CU_API_SOURCE uptr cu_rand_cryptographic(void *data, uptr bytes)
{
#if CU_OS_WINDOWS && CU_HAS_INCLUDE(<bcrypt.h>)
	return RtlGenRandom(data, (ULONG)bytes) ? bytes : 0;
#elif CU_OS_UNIX
	FILE *f;
	if (!(f = fopen("/dev/urandom", "r"))) return 0;
	return fread(data, 1, bytes, f);
#else
	return 0;
#endif
}

static CU_THREAD_LOCAL u64 seed_state[5] = { 2196718985u, 557303564u, 464685335u, 4137498962u, 3729359040u };
static CU_THREAD_LOCAL u64 seed_counter = 0;

CU_API_SOURCE u64 cu_rand(void)
{
	u64 t = seed_state[4];
	const u64 s = seed_state[0];
	seed_state[4] = seed_state[3];
	seed_state[3] = seed_state[2];
	seed_state[2] = seed_state[1];
	seed_state[1] = s;
	
	t ^= t >> 2;
	t ^= t << 1;
	t ^= s ^ (s << 4);
	seed_state[0] = t;
	seed_counter += 362437;
	return t + seed_counter;
}
CU_API_SOURCE void cu_srand(u64 seed)
{
	seed_state[0] = seed;
	seed_state[1] = 1498368744u;
	seed_state[2] = 4148399166u;
	seed_state[3] = 2896157047u;
	seed_state[4] = 3060824860u;
	seed_counter = 0;
}

CU_API_SOURCE void cu_rand_init(cu_rand_state *state, u64 seed)
{
	u64 i;
	*state->state = seed;
	state->index = 0;
	for (i = 1; i < CU_RND_STATE_SZ; i++) {
		seed = CU_U64_C(0x5851F42D4C957F2D) * (seed ^ (seed >> 62u)) + i;
		state->state[i] = seed;
	}

}
CU_API_SOURCE u64 cu_rand_get(cu_rand_state *state)
{
	i64 ind = state->index, j;
	u64 x, y;

	j = (ind - (CU_RND_STATE_SZ - 1));
	j += CU_RND_STATE_SZ * (j < 0);

	x = (state->state[ind] & (CU_U64_C(0xFFFFFFFF) << 31u)) | (state->state[j] & (CU_U64_C(0xFFFFFFFF) >> 2u));

	j = (ind - (CU_RND_STATE_SZ - 156));
	j += CU_RND_STATE_SZ * (j < 0);

	state->state[ind++] = x = state->state[j] ^ ((x >> 1u) ^ (CU_U64_C(0xB5026F5AA96619E9) * (x & 1u)));
	state->index = ind * (ind < CU_RND_STATE_SZ);

	y = x ^ (x >> 29u);
	y ^= (y << 17u) & CU_U64_C(0x71D67FFFEDA60000);
	y ^= (y << 37u) & CU_U64_C(0xFFF7EEE000000000);

	return y ^ (y >> 43u);
}

#endif

/* ==========================================================================
 *
 * --------------------------- Resources functions --------------------------
 *
 * ========================================================================== */

#if CU_SETTING_RESOURCES_FUNCS

#include <string.h>

#if !CU_OS_WINDOWS
#  include <stdio.h>
#endif

#if CU_HAS_INCLUDE(<cpuid.h>) && CU_ARCH_X86
#  include <cpuid.h>
#  define CU_CPUID
#endif

#if CU_COMP_MSVC && !CU_ARCH_ARM
#include <intrin.h>
CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((2))
static i32 cu_res_cpuid(u32 id, u32 *regs)
{
	__cpuid((int *)regs, id);
	return (i32)*regs;
}
#elif defined(CU_CPUID)
CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((2))
static i32 cu_res_cpuid(u32 id, u32 *regs)
{
	return (i32)__get_cpuid(id, regs, regs + 1, regs + 2, regs + 3);
}
#else
CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_NOTHROW CU_ATTRIB_NONNULL((2))
static i32 cu_res_cpuid(u32 id, u32 *regs)
{
	CU_UNUSED(id);
	CU_UNUSED(regs);
	return 0;
}
#endif

#if CU_OS_APPLE
#  include <sys/types.h>
#  include <sys/sysctl.h>
#endif

#if defined(CU_CPUID)

#if !defined (__cpuid_count)
#define __cpuid_count(__leaf, __count, __eax, __ebx, __ecx, __edx) \
  __asm("  xchg{q|}  {%%|}rbx,%q1\n"                               \
        "  cpuid\n"                                                \
        "  xchg{q|}  {%%|}rbx,%q1"                                 \
        : "=a"(__eax), "=r"(__ebx), "=c"(__ecx), "=d"(__edx)       \
        : "0"(__leaf), "2"(__count))
#endif

static int cu_res_cpuinfo_cache(cu_res_cpu *info)
{
	int i;

	for (i = 0; ; ++i) {
		u32 eax, ebx, ecx, edx, c_size, c_line, c_assoc;
		struct cu_res_cpu_cache *target;
		__cpuid_count(0x04, i, eax, ebx, ecx, edx);
		CU_UNUSED(edx);

		c_assoc = (ebx >> 22) + 1;
		c_line = (ebx & 0xFFF) + 1;
		c_size = (((ebx >> 22) & 0x3FF) + 1) * (((ebx >> 12) & 0x3FF) + 1) * c_line * (ecx + 1);

		switch ((eax >> 5) & 0x7)
		{
		case 1:
			target = ((eax & 0x1F) == 1) ? &info->l1d : &info->l1i;
			break;
		case 2:
			target = &info->l2;
			break;
		case 3:
			target = &info->l3;
			break;
		default:
			return 1;
		}

		target->line = c_line;
		target->size = c_size;
		target->assoc = c_assoc;
	}
}

#elif CU_OS_WINDOWS
#include <windows.h>
#include <stdlib.h>
static int cu_res_cpuinfo_cache(cu_res_cpu *info)
{
	SYSTEM_LOGICAL_PROCESSOR_INFORMATION *buffer;
	DWORD buffer_size = 0;
	int i, count;

	GetLogicalProcessorInformation(NULL, &buffer_size);
	if (CU_UNLIKELY(!(buffer = malloc(buffer_size)))) return 0;

	GetLogicalProcessorInformation(buffer, &buffer_size);

	for (count = buffer_size / sizeof *buffer, i = 0; i < count; ++i) {
		SYSTEM_LOGICAL_PROCESSOR_INFORMATION *b = buffer + i;
		struct cu_res_cpu_cache *target;
		if (b->Relationship != RelationCache) continue;
		
		switch (b->Cache.Level) {
		case 1:
			target = b->Cache.Type == CacheData ? &info->l1d : &info->l1i;
			break;
		case 2:
			target = &info->l2;
			break;
		case 3:
			target = &info->l3;
			break;
		default:
			goto skip;
		}

		target->line = (u32)b->Cache.LineSize;
		target->size = (u32)b->Cache.Size;
		target->assoc = (u32)b->Cache.Associativity;
	skip:
		(void)(0);
	}

	free(buffer);
	return 1;
}
#else
static int cu_res_cpuinfo_cache(cu_res_cpu *info)
{
	return 0;
}
#endif

#if CU_ARCH_X86
#  include <time.h>
#  if CU_COMP_MSVC
#    include <intrin.h>
#  endif
CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_WARN_UNUSED_RESULT CU_ATTRIB_NOTHROW
static u64 cu_res_rdtsc(void)
{
#if CU_COMP_MSVC
	return __rdtsc();
#else
	u32 low, high;
	CU_ASM volatile ("rdtsc" : "=a" (low), "=d" (high));
	return CU_UPSHIFT((u64)high, 32) | low;
#endif
}

CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_WARN_UNUSED_RESULT CU_ATTRIB_NOTHROW
static u64 cu_res_baseghz(void)
{
	u64 start = cu_res_rdtsc();
	double elapsed;
	clock_t ts_start = clock();
	do elapsed = (double)(clock() - ts_start) / CLOCKS_PER_SEC; while (elapsed < 0.01);
	return (u64)((double)(cu_res_rdtsc() - start) / elapsed);
}
#endif

CU_API_SOURCE int cu_res_cpuinfo(cu_res_cpu *info)
{
	u32 chunks[4], cpu_family, i = 1;
	int success_counter = 0;

	memset(info, 0, sizeof *info);
	info->little_endian = (*(char *)&i) == 1;

	if (!cu_res_cpuid(0x0, chunks)) goto skip_vendor;
	info->cpuid_level = *chunks;
	memcpy(info->vendor, chunks + 1, 4);
	memcpy(info->vendor + 4, chunks + 3, 4);
	memcpy(info->vendor + 8, chunks + 2, 4);
	++success_counter;
skip_vendor:
	for (i = 0; i < 3; ++i) {
		if (!cu_res_cpuid(0x80000002 + i, chunks)) goto skip_name;
		memcpy(info->name + i * 16, chunks, 16);
	}
	++success_counter;
skip_name:
	if (!cu_res_cpuid(0x1, chunks)) goto skip_id;
	info->stepping_id = (u32)(CU_BITSOF(chunks[0], 0, 3));
	cpu_family = (u32)(CU_BITSOF(chunks[0], 8, 11));
	info->model_id = (u32)(CU_BITSOF(chunks[0], 4, 7) + ((cpu_family == 6 || cpu_family == 15) * CU_UPSHIFT(CU_BITSOF(chunks[0], 16, 19), 4)));
	info->family_id = (u32)(cpu_family + (CU_BITSOF(chunks[0], 20, 27) * (u32)(cpu_family == 15)));
	++success_counter;
skip_id:
	success_counter += cu_res_cpuinfo_cache(info);

	if (info->cpuid_level >= 0x16) {
		if (cu_res_cpuid(0x16, chunks)) {
			info->base_freq_hz = chunks[0] * 1000 * 1000;
			info->max_freq_hz = chunks[1] * 1000 * 1000;
		} else info->base_freq_hz = info->max_freq_hz = 0;
	} else {
		float base_ghz;
		char *base_ghz_parser = strchr(info->name, '@');
#if CU_OS_APPLE
		long long result = -1;
		uptr size = sizeof result;
		if (!(sysctlbyname("hw.cpufrequency_max", &result, &size, NULL, 0))) info->max_freq_hz = (u64)result;
#elif !CU_OS_WINDOWS
		FILE *f;
		if ((f = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq", "r"))) {
			fscanf(f, "%" CU_U64_FMT, &info->max_freq_hz);
			fclose(f);
			info->max_freq_hz *= 1000;
		}
#endif
		if (base_ghz_parser) {
		#if CU_COMP_MSVC
			sscanf_s(base_ghz_parser, "%*s%f", &base_ghz);
		#else
			sscanf(base_ghz_parser, "%*s%f", &base_ghz);
		#endif
			info->base_freq_hz = (u64)(base_ghz * 1000.0f) * 1000 * 1000;
		}
		#if CU_ARCH_X86
		else info->base_freq_hz = cu_res_baseghz();
		#endif

		if (!info->max_freq_hz) info->max_freq_hz = info->base_freq_hz;
	}

	return success_counter == 4;
}

#if CU_OS_WINDOWS
#include <psapi.h>

CU_API_SOURCE int cu_res_meminfo(cu_res_mem *info)
{
	MEMORYSTATUSEX mstat;
	PROCESS_MEMORY_COUNTERS_EX pmc;

	mstat.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&mstat);

	info->virt_present = mstat.ullTotalPageFile;
	info->virt_tot_used = mstat.ullTotalPageFile - mstat.ullAvailPageFile;

	info->phys_present = mstat.ullTotalPhys;
	info->phys_tot_used = mstat.ullTotalPhys - mstat.ullAvailPhys;

	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof pmc);

	info->phys_loc_used = pmc.WorkingSetSize;
	info->virt_loc_used = pmc.PrivateUsage;
	
	return 1;
}
CU_API_SOURCE real64 cu_res_cpuusage(void)
{
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

	res = (double)((
		sys.QuadPart - ltm_scpu.QuadPart) + (user.QuadPart - ltm_ucpu.QuadPart)
	) / (0.01 * (double)(now.QuadPart - ltm_cpu.QuadPart) * proc_count);

	ltm_cpu = now;
	ltm_ucpu = user;
	ltm_scpu = sys;

	return res;
}

#elif CU_OS_UNIX
#include <sys/sysinfo.h>
#include <sys/times.h>
#include <stdio.h>

CU_API_SOURCE int cu_res_meminfo(cu_res_mem *info)
{
	struct sysinfo si;
	FILE *f;

	sysinfo(&si);

	info->phys_present = si.totalram;
	info->phys_tot_used = si.totalswap;
	
	info->virt_present = si.totalram + si.totalswap;
	info->virt_tot_used = (si.totalram - si.freeram) + (si.totalswap - si.freeswap);

	f = fopen("/proc/self/statm", "r");
	if (f == NULL) {
		info->phys_loc_used = 0;
		info->virt_loc_used = 0;
		return 0;
	}

	fscanf(f, "%lu %lu", &info->phys_loc_used, &info->virt_loc_used);
	return 1;
}
CU_API_SOURCE real64 cu_res_cpuusage(void)
{
	static clock_t ltm_cpu = 0, ltm_scpu = 0, ltm_ucpu = 0;
	struct tms tms_buf;
	clock_t ctm_cpu;
	real64 res;

#if CU_SETTING_THREAD_FUNCS
	static int proc_count = 0;
	if (!proc_count) {
		proc_count = cu_thread_count();
	}
#else
	static int proc_count = 1;
#endif
	
	ctm_cpu = times(&tms_buf);
	if (ctm_cpu == -1) return 0.0;

	res = (double)(
		(tms_buf.tms_stime - ltm_scpu) + (tms_buf.tms_utime + ltm_ucpu)
	) / (0.01 * (double)(ctm_cpu - ltm_cpu) * proc_count);

	ltm_cpu = ctm_cpu;
	ltm_scpu = tms_buf.tms_stime;
	ltm_ucpu = tms_buf.tms_utime;

	return res;
}

#elif CU_OS_MAC
#include <sys/sysctl.h>
#include <sys/types.h>

#include <mach/vm_statistics.h>
#include <mach/mach_types.h>
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#include <mach/mach.h>

CU_API_SOURCE int cu_res_meminfo(cu_res_mem *info)
{
	info->phys_present = 0;
	info->phys_tot_used = 0;
	info->phys_loc_used = 0;
	
	info->virt_present = 0;
	info->virt_tot_used = 0;
	info->virt_loc_used = 0;

	return 1;
}
CU_API_SOURCE real64 cu_res_cpuusage(void)
{
	mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
	host_cpu_load_info_data_t cpuinfo;
	u64 ticks_cnt = 0;
	int i;

	if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpuinfo, &count) != KERN_SUCCESS) return 0.0;


	for (i = 0; i < CPU_STATE_MAX; ++i) ticks_cnt += cpuinfo.cpu_ticks[i];
	return CalculateCPULoad(cpuinfo.cpu_ticks[CPU_STATE_IDLE], ticks_cnt);
}

#else

CU_API_SOURCE int cu_res_meminfo(cu_res_mem *info)
{
	info->phys_present = 0;
	info->phys_tot_used = 0;
	info->phys_loc_used = 0;
	
	info->virt_present = 0;
	info->virt_tot_used = 0;
	info->virt_loc_used = 0;

	return 0;
}
CU_API_SOURCE real64 cu_res_cpuusage(void)
{
	return (real64)(0);
}

#endif

CU_API_SOURCE char *cu_res_bytefmt(char *str, u64 bytes)
{	
	static const char bytefmt_suffix[6] = { 'K', 'M', 'G', 'T', 'P', 'E' };
	uptr prev = bytes % 1000, suffix = CU_UPTRMAX;
	char *start = str;
	int lkilo = bytes < 1000;

	if (!lkilo) {
		while (1) {
			++suffix;
			bytes /= 1000;
			if (bytes < 1000) break;
			prev = bytes % 1000;
		}
	}

	if (bytes >= 100) *str++ = '0' + (char)((bytes / 100) % 10);
	if (bytes >= 10) *str++ = '0' + (char)((bytes / 10) % 10);
	*str++ = '0' + (char)(bytes % 10);

	if (lkilo) goto end;

	*str++ = '.';
	*str++ = '0' + (char)((prev / 100) % 10);
	*str++ = '0' + (char)((prev / 10) % 10);
	*str++ = '0' + (char)(prev % 10);
	*str++ = bytefmt_suffix[suffix];

	*str++ = 'B';
end:
	*str++ = '\0';
	return start;
}

#endif

/* ==========================================================================
 *
 * ----------------------------- Time functions -----------------------------
 *
 * ========================================================================== */

#if CU_SETTING_TIME_FUNCS

#include <time.h>

CU_API_SOURCE void cu_time_now(cu_time *tm)
{
	time_t now_val = time(NULL);
	struct tm now = *localtime(&now_val);

#if CU_OS_UNIX
	struct timespec spec;
	if (clock_gettime(CLOCK_REALTIME, &spec) == -1) spec.tv_nsec = 0;
	tm->nanosec =  (int)(spec.tv_nsec % 1000);
	tm->microsec = (int)((spec.tv_nsec / 1000) % 1000);
	tm->millisec = (int)((spec.tv_nsec / 1000000) % 1000);
#else
	tm->nanosec = 0;
	tm->microsec = 0;
	tm->millisec = 0;
#endif

	memcpy(&tm->second, &now.tm_sec, 9 * sizeof(int));
	tm->year = now.tm_year + 1900;

#if defined(__USE_MISC)
	tm->tznm = now.tm_zone;
	tm->utc_dif = now.tm_gmtoff;
#else
	tm->tznm = NULL;
	tm->utc_dif = 0;
#endif
}

CU_API_SOURCE void cu_timer_begin(cu_timer *tm)
{
	*tm = clock();
}
CU_API_SOURCE cu_timer cu_timer_end(const cu_timer *tm)
{
	return (cu_timer)((((real64)(clock() - *tm)) / CLOCKS_PER_SEC) * 1000.0 * 1000.0);
}

CU_API_SOURCE real64 cu_timer_endf(const cu_timer *tm)
{
	return ((real64)(clock() - *tm)) / CLOCKS_PER_SEC;
}

#endif

/* ==========================================================================
 *
 * --------------------------- Threading functions --------------------------
 *
 * ========================================================================== */

#if CU_SETTING_THREAD_FUNCS

#if CU_THREAD_POSIX_USED
#include <pthread.h>
#include <sched.h>

CU_API_SOURCE cu_thread cu_thread_create(cu_thread_func function, cu_thread_arg arg)
{
	cu_thread thr;
	if (pthread_create(&thr, NULL, function, arg) != 0) return 0;
	return thr;
}

CU_API_SOURCE int cu_thread_join(cu_thread *thread) { return pthread_join(*thread, NULL) == 0; }
CU_API_SOURCE int cu_thread_detach(cu_thread *thread) { return pthread_detach(*thread) == 0; }

CU_API_SOURCE int cu_thread_mutex_init(cu_thread_mutex *mutex) { return pthread_mutex_init(mutex, NULL) == 0; }
CU_API_SOURCE int cu_thread_mutex_lock(cu_thread_mutex *mutex) { return pthread_mutex_lock(mutex) == 0; }
CU_API_SOURCE int cu_thread_mutex_unlock(cu_thread_mutex *mutex) { return pthread_mutex_unlock(mutex) == 0; }
CU_API_SOURCE int cu_thread_mutex_trylock(cu_thread_mutex *mutex) { return pthread_mutex_trylock(mutex) == 0; }
CU_API_SOURCE int cu_thread_mutex_destroy(cu_thread_mutex *mutex) { return pthread_mutex_destroy(mutex) == 0; }

#elif CU_THREAD_C_USED
#include <threads.h>

CU_API_SOURCE cu_thread cu_thread_create(cu_thread_func function, cu_thread_arg arg)
{
	thrd_t thr;
	if (thrd_create(&thr, function, arg) != thrd_success) return 0;
	return thr;
}
CU_API_SOURCE int cu_thread_join(cu_thread *thread) { return thrd_join(*thread, NULL) == thrd_success; }
CU_API_SOURCE int cu_thread_detach(cu_thread *thread) { return thrd_detach(*thread) == thrd_success; }

CU_API_SOURCE int cu_thread_mutex_init(cu_thread_mutex *mutex) { return mtx_init(mutex, mtx_plain) == thrd_success; }
CU_API_SOURCE int cu_thread_mutex_lock(cu_thread_mutex *mutex) { return mtx_lock(mutex) == thrd_success; }
CU_API_SOURCE int cu_thread_mutex_unlock(cu_thread_mutex *mutex) { return mtx_unlock(mutex) == thrd_success; }
CU_API_SOURCE int cu_thread_mutex_trylock(cu_thread_mutex *mutex) { return mtx_trylock(mutex) == thrd_success; }
CU_API_SOURCE int cu_thread_mutex_destroy(cu_thread_mutex *mutex) { mtx_destroy(mutex); return 1; }

#elif CU_THREAD_WIN_USED
#include <windows.h>

CU_API_SOURCE cu_thread cu_thread_create(cu_thread_func function, cu_thread_arg arg) { return CreateThread(NULL, 0, function, arg, 0, 0); }

CU_API_SOURCE int cu_thread_join(cu_thread *thread) { return WaitForSingleObject(*thread, INFINITE) == WAIT_OBJECT_0; }
CU_API_SOURCE int cu_thread_detach(cu_thread *thread) { return CloseHandle(*thread) != 0; }

CU_API_SOURCE int cu_thread_mutex_init(cu_thread_mutex *mutex) { return (*mutex = CreateMutex(NULL, FALSE, NULL)) != NULL; }
CU_API_SOURCE int cu_thread_mutex_lock(cu_thread_mutex *mutex) { return WaitForSingleObject(*mutex, INFINITE) == WAIT_OBJECT_0; }
CU_API_SOURCE int cu_thread_mutex_unlock(cu_thread_mutex *mutex) { return ReleaseMutex(*mutex) != 0; }
CU_API_SOURCE int cu_thread_mutex_trylock(cu_thread_mutex *mutex) { return WaitForSingleObject(*mutex, 0u) == WAIT_OBJECT_0; }
CU_API_SOURCE int cu_thread_mutex_destroy(cu_thread_mutex *mutex) { return CloseHandle(*mutex) != 0; }

#else

CU_API_SOURCE cu_thread cu_thread_create(cu_thread_func function, cu_thread_arg arg) { return 0; }

CU_API_SOURCE int cu_thread_join(cu_thread *thread) { return 1; }
CU_API_SOURCE int cu_thread_detach(cu_thread *thread) { return 1; }

CU_API_SOURCE int cu_thread_mutex_init(cu_thread_mutex *mutex) { return 1; }
CU_API_SOURCE int cu_thread_mutex_lock(cu_thread_mutex *mutex) { return 1; }
CU_API_SOURCE int cu_thread_mutex_unlock(cu_thread_mutex *mutex) { return 1; }
CU_API_SOURCE int cu_thread_mutex_trylock(cu_thread_mutex *mutex) { return 1; }
CU_API_SOURCE int cu_thread_mutex_destroy(cu_thread_mutex *mutex) { return 1; }

#endif


#if CU_OS_WINDOWS
#include <windows.h>
CU_API_SOURCE int cu_thread_count(void)
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return (int)info.dwNumberOfProcessors;
}
#elif CU_OS_LINUX || CU_OS_SOLARIS || CU_OS_MAC
#include <unistd.h>
CU_API_SOURCE int cu_thread_count(void)
{
	return (int)sysconf(_SC_NPROCESSORS_ONLN);
}
#elif CU_OS_BSD
#include <sys/sysctl.h>
CU_API_SOURCE int cu_thread_count(void)
{
	int mib[4] = { CTL_HW, HW_AVAILCPU, 0, 0 }, res;
	uptr len = sizeof res; 

	sysctl(mib, 2, &res, &len, NULL, 0);
	if (res > 0) return res;

	mib[1] = HW_NCPU;
	sysctl(mib, 2, &res, &len, NULL, 0);
	if (res > 0) return res;

	return 1;
}
#elif CU_OS_HPUX
#include <sys/mpctl.h>
CU_API_SOURCE int cu_thread_count(void) { return (int)mpctl(MPC_GETNUMSPUS, NULL, NULL); }
#elif CU_OS_IRIX
#include <unistd.h>
CU_API_SOURCE int cu_thread_count(void) { return (int)sysconf(_SC_NPROC_ONLN); }
#else
CU_API_SOURCE int cu_thread_count(void) { return 1; }
#endif

#if CU_OS_WINDOWS
#include <windows.h>
CU_API_SOURCE int cu_thread_sleep(u64 secs, u64 microsecs)
{
	Sleep((DWORD)(secs + (microsecs / 1000000)));
	return 1;
}
#elif CU_OS_UNIX
#include <time.h>
CU_API_SOURCE int cu_thread_sleep(u64 secs, u64 microsecs)
{
	while ((secs = (u64)sleep((unsigned int)secs)));
	usleep((unsigned int)microsecs);
	return 1;
}
#else
#include <time.h>
CU_API_SOURCE int cu_thread_sleep(u64 secs, u64 microsecs)
{
	const clock_t end = clock() + CLOCKS_PER_SEC * (clock_t)(secs + (microsecs / 1000000));
	while (clock() > end);
	return 1;
}
#endif


#if CU_OS_UNIX
#  include <unistd.h>
#  if CU_HAS_INCLUDE(<sys/syscall.h>)
#    include <sys/syscall.h>
#  endif
CU_API_SOURCE u32 cu_thread_pid(void) { return (u32)getpid(); }
#  if defined (SYS_gettid)
CU_API_SOURCE u32 cu_thread_tid(void) { return (u32)syscall(SYS_gettid); }
#  else
CU_API_SOURCE u32 cu_thread_tid(void) { return 0u; }
#  endif
#elif CU_OS_WINDOWS
CU_API_SOURCE u32 cu_thread_pid(void) { return (u32)GetCurrentProcessId(); }
CU_API_SOURCE u32 cu_thread_tid(void) { return (u32)GetCurrentThreadId(); }
#else
CU_API_SOURCE u32 cu_thread_pid(void) { return 0; }
CU_API_SOURCE u32 cu_thread_tid(void) { return 0; }
#endif

#endif

/* ==========================================================================
 *
 * ----------------------- Allocation debug functions -----------------------
 *
 * ========================================================================== */

#if CU_SETTING_ALLOC_FUNCS

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#define CU_DEBALLOC_FWD CU_U64_C(0xAABBCCDD11223344)
#define CU_DEBALLOC_BACK CU_U64_C(0xCDCDCDCDCDCDCDCD)

typedef struct cu_deballoc
{
	void *address;
	u64 *back_mnum;
	u64 *fwd_mnum;
	uptr bytes;
	uptr is_freed;
} cu_deballoc;

static cu_deballoc_stats cu_alloc_stats;
static FILE *cu_allocation_file;
static int cu_alloc_verbose;

static cu_deballoc *cu_allocs;
static uptr cu_allocs_cnt, cu_allocs_cap;

#if CU_SETTING_THREAD_FUNCS
static cu_thread_mutex mutex;
#  define CU_DEBALLOC_INITMUTEX cu_thread_mutex_init(&mutex);
#  define CU_DEBALLOC_LOCKMUTEX cu_thread_mutex_lock(&mutex);
#  define CU_DEBALLOC_UNLOCKMUTEX cu_thread_mutex_unlock(&mutex);
#  define CU_DEBALLOC_DESTROYMUTEX cu_thread_mutex_destroy(&mutex);
#else
#  define CU_DEBALLOC_INITMUTEX
#  define CU_DEBALLOC_LOCKMUTEX
#  define CU_DEBALLOC_UNLOCKMUTEX
#  define CU_DEBALLOC_DESTROYMUTEX
#endif

#if CU_COMPVER(GNU, 2, 0)
__attribute__((__format__(printf, 3, 4)))
#endif
static void cu_deballoc_log(int is_err, const char *call_location, const char *msg, ...)
{
	va_list args;
	if (!cu_allocation_file || (!is_err && !cu_alloc_verbose)) return;
	fprintf(cu_allocation_file, "\n%s: ", call_location);
	va_start(args, msg);
	vfprintf(cu_allocation_file, msg, args);
	va_end(args);
	fflush(cu_allocation_file);
}

CU_ATTRIB_NOTHROW CU_ATTRIB_ALWAYSINLINE CU_ATTRIB_CONST
static uptr cu_deballoc_rnd(uptr orig)
{
	return ((orig + 7) & (uptr)(-8));
}

static cu_deballoc *cu_deballoc_fromaddr(void *user_addr)
{
	uptr i;

	if (!user_addr) return NULL;
	for (i = 0; i < cu_allocs_cnt; ++i) {
		cu_deballoc *d = cu_allocs + i;
		if (d->address == user_addr) return d;
	}
	return NULL;
}

static void cu_deballoc_verify(void)
{
	uptr i;
	CU_DEBALLOC_LOCKMUTEX

	for (i = 0; i < cu_allocs_cnt; ++i) {
		cu_deballoc *d = cu_allocs + i;
		if (d->is_freed) continue;

		if (*d->fwd_mnum != CU_DEBALLOC_FWD) {
			++cu_alloc_stats.overruns;
			cu_deballoc_log(1, "(Verification)", "Buffer overrun in %p", d->address);
		}
		if (*d->back_mnum != CU_DEBALLOC_BACK) {
			++cu_alloc_stats.underruns;
			cu_deballoc_log(1, "(Verification)", "Buffer underrun in %p", d->address);
		}
		*d->fwd_mnum = CU_DEBALLOC_FWD;
		*d->back_mnum = CU_DEBALLOC_BACK;
	}

	CU_DEBALLOC_UNLOCKMUTEX
}

static cu_deballoc *cu_deballoc_add(void *user_p, uptr b)
{
	cu_deballoc *d = NULL;

	if (cu_allocs_cnt + 1 > cu_allocs_cap) {
		void *r;
		cu_allocs_cap = (cu_allocs_cnt + 1) * 2;
		r = realloc(cu_allocs, (size_t)cu_allocs_cap * sizeof(cu_deballoc));
		if (!r) goto fail;
		cu_allocs = r;
	}

	d = cu_allocs + cu_allocs_cnt++;
	d->address = user_p;
	d->bytes = b;
	d->is_freed = 0;
	d->back_mnum = (u64 *)(user_p) - 1;
	*d->back_mnum = CU_DEBALLOC_BACK;
	b = cu_deballoc_rnd(b);
	d->fwd_mnum = (u64 *)(user_p) + (b / 8);
	*d->fwd_mnum = CU_DEBALLOC_FWD;
fail:
	return d;
}

static void cu_deballoc_summary_internal(FILE *f)
{
	cu_deballoc_verify();

	fprintf(f, "\n\n=============== DEBUG SUMMARY ===============\n\n");

	fprintf(f, "  Current memory usage: %" CU_UPTR_FMT "B\n", cu_alloc_stats.memusage);
	fprintf(f, "  Highest memory usage: %" CU_UPTR_FMT "B\n\n", cu_alloc_stats.highest_memusage);

	fprintf(f, "  No. of active allocated pointers: %" CU_UPTR_FMT "\n\n", cu_alloc_stats.allocd_ptrs);

	fprintf(f, "  Malloc calls:  %" CU_UPTR_FMT "\n", cu_alloc_stats.malloc_count);
	fprintf(f, "  Calloc calls:  %" CU_UPTR_FMT "\n", cu_alloc_stats.calloc_count);
	fprintf(f, "  Realloc calls: %" CU_UPTR_FMT "\n", cu_alloc_stats.realloc_count);
	fprintf(f, "  Free calls:    %" CU_UPTR_FMT "\n\n", cu_alloc_stats.free_count);

	fprintf(
		f, "  Total allocations: %" CU_UPTR_FMT "\n",
		cu_alloc_stats.malloc_count + cu_alloc_stats.calloc_count + cu_alloc_stats.realloc_count
	);
	fprintf(f, "  No. of alloc fails: %" CU_UPTR_FMT "\n\n", cu_alloc_stats.alloc_fails);

	fprintf(f, "  Errors: \n");
	fprintf(f, "    Detected underruns: %" CU_UPTR_FMT "\n", cu_alloc_stats.underruns);
	fprintf(f, "    Detected overruns: %" CU_UPTR_FMT "\n", cu_alloc_stats.overruns);
	fprintf(f, "    Invalid frees: %" CU_UPTR_FMT "\n", cu_alloc_stats.invalid_free);
	fprintf(f, "    Invalid reallocs: %" CU_UPTR_FMT "\n", cu_alloc_stats.invalid_realloc);
	fprintf(f, "    0-byte reallocs: %" CU_UPTR_FMT "\n\n", cu_alloc_stats.empty_realloc);

	fprintf(f, "=============================================\n\n");
	fflush(f);
}

CU_API_SOURCE void cu_deballoc_assert_fail(const char *cond_str)
{
	fputs(cond_str, stderr);
	fflush(stderr);
	CU_BREAKPOINT();
	while (1);
}

CU_API_SOURCE void cu_deballoc_free(const char *call_location, void *user_ptr)
{
	cu_deballoc *d;
	void *full_ptr = (u64 *)(user_ptr) - 1;

	CU_DEBALLOC_LOCKMUTEX
	
	d = cu_deballoc_fromaddr(user_ptr);
	++cu_alloc_stats.free_count;

	if (!d || d->is_freed) {
		++cu_alloc_stats.invalid_free;
		cu_deballoc_log(
			1, call_location,
			d ? "Free on previously free'd pointer %p." : "Free on non-allocated pointer %p.",
			user_ptr
		);
	} else {
		d->is_freed = 1;
		cu_alloc_stats.memusage -= d->bytes;
		--cu_alloc_stats.allocd_ptrs;
		cu_deballoc_log(0, call_location, "Free'd %p (%" CU_UPTR_FMT "B)", user_ptr, d->bytes);
	}

	free(full_ptr);
	CU_DEBALLOC_UNLOCKMUTEX
}
static void *cu_deballoc_alloc_internal(const char *call_location, uptr n, uptr size_each, int is_clear)
{
	uptr orig = n * size_each, bytes = orig;
	void *full_p, *user_p;
	
	CU_DEBALLOC_LOCKMUTEX

	if (is_clear) ++cu_alloc_stats.calloc_count;
	else ++cu_alloc_stats.malloc_count;

	bytes = cu_deballoc_rnd(bytes) + 16;
	full_p = is_clear ? calloc(1, (size_t)bytes) : malloc((size_t)bytes);
	user_p = (uptr *)(full_p) + 1;

	if (!full_p) {
		++cu_alloc_stats.alloc_fails;
		if (is_clear) cu_deballoc_log(1, call_location, "Allocation of %" CU_UPTR_FMT " * %" CU_UPTR_FMT "B = %" CU_UPTR_FMT " returned NULL", n, size_each, orig);
		else cu_deballoc_log(1, call_location, "Allocation of %" CU_UPTR_FMT "B returned NULL", orig);
		goto fail;
	}

	if (is_clear) {
		cu_deballoc_log(
			0, call_location,
			"Calloc'd %" CU_UPTR_FMT " * %" CU_UPTR_FMT " = %" CU_UPTR_FMT "B -> %p",
			n, size_each, orig, user_p
		);
	} else {
		cu_deballoc_log(
			0, call_location,
			"Malloc'd %" CU_UPTR_FMT "B -> %p",
			orig, user_p
		);
	}

	++cu_alloc_stats.allocd_ptrs;
	cu_alloc_stats.memusage += orig;
	if (cu_alloc_stats.memusage > cu_alloc_stats.highest_memusage) {
		cu_alloc_stats.highest_memusage = cu_alloc_stats.memusage;
	}

	cu_deballoc_add(user_p, orig);
fail:
	CU_DEBALLOC_UNLOCKMUTEX
	return user_p;
}
CU_API_SOURCE void *cu_deballoc_malloc(const char *call_location, uptr bytes)
{
	return cu_deballoc_alloc_internal(call_location, 1, bytes, 0);
}
CU_API_SOURCE void *cu_deballoc_calloc(const char *call_location, uptr n, uptr size_each)
{
	return cu_deballoc_alloc_internal(call_location, n, size_each, 1);
}
CU_API_SOURCE void *cu_deballoc_realloc(const char *call_location, void *orig_user_ptr, uptr bytes)
{
	cu_deballoc *d;
	uptr original = 0, bytes_given = bytes;
	int msg = 1;
	void *full_r, *user_r;

	CU_DEBALLOC_LOCKMUTEX

	d = cu_deballoc_fromaddr(orig_user_ptr);
	++cu_alloc_stats.realloc_count;

	if (!bytes) {
		++cu_alloc_stats.empty_realloc;
		cu_deballoc_log(1, call_location, "0-byte realloc");
		msg = 0;
	} else if (orig_user_ptr && (!d || d->is_freed)) {
		++cu_alloc_stats.invalid_realloc;
		cu_deballoc_log(
			1, call_location,
			d ? "Realloc on previously free'd ptr %p." : "Realloc on non-allocated ptr %p.",
			orig_user_ptr
		);
		msg = 0;
	} else original = d->bytes;

	bytes = cu_deballoc_rnd(bytes) + 16;
	full_r = realloc((u64 *)(orig_user_ptr) - 1, (size_t)bytes);

	if (!full_r && msg) {
		++cu_alloc_stats.alloc_fails;
		cu_deballoc_log(
			0, call_location,
			"Realloc of %p from %" CU_UPTR_FMT "B to %" CU_UPTR_FMT "B returned NULL",
			orig_user_ptr, original, bytes_given
		);
		user_r = NULL;
		goto fail;
	}

	user_r = (u64 *)(full_r) + 1;

	cu_alloc_stats.memusage += bytes_given - original;
	if (cu_alloc_stats.memusage > cu_alloc_stats.highest_memusage) {
		cu_alloc_stats.highest_memusage = cu_alloc_stats.memusage;
	}

	if (d) {
		d->address = (u64 *)(user_r);
		d->back_mnum = (u64 *)(full_r);
		d->fwd_mnum = ((u64 *)(full_r) + (bytes / 8)) - 1;
		*d->fwd_mnum = CU_DEBALLOC_FWD;
		d->bytes = bytes_given;
	} else d = cu_deballoc_add(user_r, bytes_given);
	
	if (msg) {
		cu_deballoc_log(
			0, call_location,
			"Realloc'd %p from %" CU_UPTR_FMT "B to %" CU_UPTR_FMT "B -> %p",
			orig_user_ptr, original, bytes_given, user_r
		);
	}

fail:
	CU_DEBALLOC_UNLOCKMUTEX
	return user_r;
}

CU_API_SOURCE const cu_deballoc_stats *cu_deballoc_getstats(void)
{
	cu_deballoc_verify();
	return &cu_alloc_stats;
}
CU_API_SOURCE void cu_deballoc_summary(void) { cu_deballoc_summary_internal(stdout); }

CU_API_SOURCE int cu_deballoc_start(int log_file, int verbose)
{
	struct tm now, *np;
	time_t now_val = time(NULL);

	CU_DEBALLOC_INITMUTEX

	if (!log_file) return 1;
	cu_alloc_verbose = verbose;

#if CU_COMP_MSVC
	CU_UNUSED(np);
	if (CU_UNLIKELY(fopen_s(&cu_allocation_file, "ALLOC_DEBUG.txt", "a") != 0)) return 0;
	if (CU_UNLIKELY(localtime_s(&now, &now_val) != 0)) return 0;
#else
	if (CU_UNLIKELY(!(cu_allocation_file = fopen("ALLOC_DEBUG.txt", "a")))) return 0;
	if (CU_UNLIKELY(!(np = localtime(&now_val)))) return 0;
	now = *np;
#endif

	fprintf(
		cu_allocation_file,
		"%d/%d/%d %d:%d:%d",
		now.tm_mday, now.tm_mon + 1, now.tm_year + 1900, now.tm_hour, now.tm_min, now.tm_sec
	);
	return 1;
}
CU_API_SOURCE void cu_deballoc_end(void)
{
	uptr i;
	cu_allocs = NULL;
	cu_allocs_cnt = 0;

	for (i = 0; i < cu_allocs_cnt; ++i) free(cu_allocs + i);

	CU_DEBALLOC_DESTROYMUTEX

	if (!cu_allocation_file) return;
	cu_deballoc_summary_internal(cu_allocation_file);
	fclose(cu_allocation_file);
}

#endif

#endif /* CU_SETTING_FUNCS */
