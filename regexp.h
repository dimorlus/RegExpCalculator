/************************************************\
*                                                *
*   REGEXP.DDL Internal data definitions         *
*   Copyright (c) 1992 by Borland International  *
*   Copyright (c) 1986 by Univerisity of Toronto *
*                                                *
\************************************************/


#define RE_OK                   0
#define RE_NOTFOUND             1
#define RE_INVALIDPARAMETER     2
#define RE_EXPRESSIONTOOBIG     3
#define RE_OUTOFMEMORY          4
#define RE_TOOMANYSUBEXPS       5
#define RE_UNMATCHEDPARENS      6
#define RE_INVALIDREPEAT        7
#define RE_NESTEDREPEAT         8
#define RE_INVALIDRANGE         9
#define RE_UNMATCHEDBRACKET     10
#define RE_TRAILINGBACKSLASH    11
#define RE_INTERNAL             20

#define NSUBEXP  10

/*
 * The first byte of the regexp internal "program" is actually this magic
 * number; the start node begins in the second byte.
 */
#define MAGIC   0234

typedef struct regexp {
        const char *startp[NSUBEXP+1];
        const char *endp[NSUBEXP+1];
        char regstart;          /* Internal use only. */
        char reganch;           /* Internal use only. */
        char *regmust;          /* Internal use only. */
        int regmlen;            /* Internal use only. */
        char program[1];        /* Internal use only. */
} regexp;

extern int regerror;

#ifdef __cplusplus
/*
 - regcomp - compile a regular expression into internal code
 *
 * We can't allocate space until we know how big the compiled form will be,
 * but we can't compile it (and thus know how big it is) until we've got a
 * place to put the code.  So we cheat:  we compile it twice, once with code
 * generation turned off and size counting turned on, and once "for real".
 * This also means that we don't allocate space until we are sure that the
 * thing really will compile successfully, and we never have to move the
 * code and thus invalidate pointers into it.  (Note that it has to be in
 * one piece because free() must be able to free it all.)
 *
 * Beware that the optimization-preparation code in here knows about some
 * of the structure of the compiled regexp.
 */
extern "C" regexp * regcomp(const char *exp);
/*
 - regexec - match a regexp against a string
 */
extern "C" int regexec(regexp *prog, const char *string);
/*
 - regsub - perform substitutions after a regexp match
 */
extern "C" int regsub(regexp *prog, const char *replace, char *dest);
extern "C" void regfree(regexp *prog);
extern "C" char const *regerr(void);
#else
extern regexp * regcomp(const char *exp);
extern int regexec(regexp *prog, const char *string);
extern int regsub(regexp *prog, const char *replace, char *dest);
extern void regfree(regexp *prog);
#endif
