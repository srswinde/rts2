/* 
 * Various utility functions.
 * Copyright (C) 2003-2009 Petr Kubanek <petr@kubanek.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __RTS_UTILSFUNC__
#define __RTS_UTILSFUNC__

#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/time.h>
#include <sstream>
#include <libnova/libnova.h>

#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "nan.h"
#include <math.h>

#ifndef HAVE_ISINF
#include <ieeefp.h>
#endif

#ifndef JD_TO_MJD_OFFSET
#define JD_TO_MJD_OFFSET  2400000.5
#endif

/**
 * Return random number in 0-1 range.
 */
double random_num ();

/**
 * Create directory recursively.
 *
 * @param path Path that will be created.
 * @param mode Create mask for path creation.
 *
 * @return 0 on success, otherwise error code.
 */
int mkpath (const char *path, mode_t mode);

/**
 * Parses and initialize tm structure from char.
 *
 * String can contain either date, in that case it will be converted to night
 * starting on that date, or full date with time (hour, hour:min, or hour:min:sec).
 *
 * @return -1 on error, 0 on succes
 */
int parseDate (const char *in_date, struct ln_date *out_time);

int parseDate (const char *in_date, double &JD);

int parseDate (const char *in_date, time_t *out_time);

/**
 * Split std::string to vector of strings.
 *
 * @param text        String which will be splitted.
 * @param delimeter   String shich separates entries in vector.
 *
 * @return Vector of std::string.
 */
std::vector<std::string> SplitStr (const std::string& text, const std::string& delimeter);

/**
 * Splits string to vector of chars
 *
 * @param text   Text which will be splited.
 */
std::vector<char> Str2CharVector (std::string text);

/**
 * Parse range range string which uses : and , to specify range. Syntax is similar
 * to Python slices. Example of valid ranges:
 *
 *   - 1:10
 *   - 1:
 *   - 1:2,4:16
 *
 * Index of first member is assumed to be 1. The function returns C-style
 * indexes, where index of the first member is 0.
 *
 * @param range_str   string to parse
 * @param array_size  size of resulting array
 * @return vector of integer values with array indices
 */
std::vector <int> parseRange (const char *range_str, int array_size, const char *& endp);

int charToBool (const char *in_value, bool &ret);

/**
 * Fill value to const char**.
 *
 * @param p    Pointer to char which will be filled,
 * @param val  Value which will be copied to character.
 */
template < typename T > void fillIn (char **p, T val)
{
	std::ostringstream _os;
	_os << val;
	*p = new char[_os.str ().length () + 1];
	strcpy (*p, _os.str (). c_str ());
}

/**
 * Replacement for isinf - on Solaris platform
 */
#ifndef HAVE_ISINF
int isinf(double x);
#endif

#ifndef HUGE_VALF
	#ifdef sun
		#define HUGE_VALF	(__extension__ 0x1.0p255f)
	#elif __GNUC_PREREQ(3,3)
		#define HUGE_VALF	(__builtin_huge_valf())
	#elif __GNUC_PREREQ(2,96)
		#define HUGE_VALF	(__extension__ 0x1.0p255f)
	#elif defined __GNUC__

		#define HUGE_VALF \
  (__extension__							      \
   ((union { unsigned __l __attribute__((__mode__(__SI__))); float __d; })    \
    { __l: 0x7f800000UL }).__d)
	#else /* not GCC */
		typedef union { unsigned char __c[4]; float __f; } __huge_valf_t;

		#if __BYTE_ORDER == __BIG_ENDIAN
			#define __HUGE_VALF_bytes	{ 0x7f, 0x80, 0, 0 }
		#endif
		#if __BYTE_ORDER == __LITTLE_ENDIAN
			#define __HUGE_VALF_bytes	{ 0, 0, 0x80, 0x7f }
		#endif

		static __huge_valf_t __huge_valf = { __HUGE_VALF_bytes };
		#define HUGE_VALF	(__huge_valf.__f)

	#endif	/* GCC.  */
#endif

/**
 * Define INIFINITY
 */
#ifndef INFINITY
 	#ifdef sun
		#define INFINITY	HUGE_VALF
	#elif __GNUC_PREREQ(3,3)
		#define INFINITY	(__builtin_inff())
	#else
		#define INFINITY	HUGE_VALF
	#endif
#endif

/**
 * Replacement for isfinite - on Solaris platform
 */
#ifndef isfinite
int isfinite(double x);
#endif

#ifndef timerisset
#define timerisset(tvp)	((tvp)->tv_sec || (tvp)->tv_usec)
#endif

#ifndef timerclear
#define timerclear(tvp)	((tvp)->tv_sec = (tvp)->tv_usec = 0)
#endif

#ifndef timercmp
#define timercmp(a, b, CMP) 						      \
  (((a)->tv_sec == (b)->tv_sec) ? 					      \
   ((a)->tv_usec CMP (b)->tv_usec) : 					      \
   ((a)->tv_sec CMP (b)->tv_sec))
#endif

#ifndef timeradd
#define timeradd(a, b, result)						      \
  do {									      \
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;			      \
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;			      \
    if ((result)->tv_usec >= 1000000)					      \
      {									      \
	++(result)->tv_sec;						      \
	(result)->tv_usec -= 1000000;					      \
      }									      \
  } while (0)
#endif

#ifndef timersub
#define timersub(a, b, result)						      \
  do {									      \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;			      \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;			      \
    if ((result)->tv_usec < 0) {					      \
      --(result)->tv_sec;						      \
      (result)->tv_usec += 1000000;					      \
    }									      \
  } while (0)
#endif

#ifndef HAVE_ISBLANK
#define isblank(x)   (isspace(x) || x == '\t')
#endif

#ifndef HAVE_STRCASESTR
char * strcasestr(const char * haystack, const char * needle);
#endif

/**
 * Case ingore string traits operations.
 */
struct ci_char_traits : public std::char_traits<char>
{
	static bool eq( char c1, char c2 ) { return toupper(c1) == toupper(c2); }

	static bool ne( char c1, char c2 ) { return toupper(c1) != toupper(c2); }

	static bool lt( char c1, char c2 ) { return toupper(c1) <  toupper(c2); }

	static int compare( const char* s1, const char* s2, size_t n )
	{
		return strncasecmp ( s1, s2, n );
		// if available on your compiler,
		//  otherwise you can roll your own
	}

	static const char* find( const char* s, int n, char a )
	{
		while( n-- > 0 && toupper(*s) != toupper(a) ) {
			++s;
		}
		return s;
	}
};

typedef std::basic_string<char, ci_char_traits> ci_string;

#ifndef HAVE_GETLINE
ssize_t getline(char **lineptr, size_t *n, FILE *stream);
#endif

#endif							 /* !__RTS_UTILSFUNC__ */
