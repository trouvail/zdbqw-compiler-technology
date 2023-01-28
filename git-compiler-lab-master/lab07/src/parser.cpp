/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* "%code top" blocks.  */
#line 1 "src/parser.y"

    #include <iostream>
    #include <assert.h>
    #include "parser.h"
    #include <cstring>
    #include <stack>
    extern Ast ast;

    int yylex();
    int yyerror(char const*);
    ArrayType* arrayType;
    Type* declType; // store type for variable declarations.
    Type* funcRetType; // store type for return value of funtion declarations.
    int idx;
    double* arrayValue; // store all number in float
    std::stack<InitValueListExpr*> stk;
    std::stack<StmtNode*> whileStk;
    InitValueListExpr* top;
    int leftCnt = 0;
    int whileCnt = 0;
    int paramNo = 0;
    int fpParamNo = 0;
    int stackParamNo = 0;
    int notZeroNum = 0;
    extern int yylineno;
    extern char* yytext;
    #include <iostream>

#line 96 "src/parser.cpp"




# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_INCLUDE_PARSER_H_INCLUDED
# define YY_YY_INCLUDE_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 30 "src/parser.y"

    #include "Ast.h"
    #include "SymbolTable.h"
    #include "Type.h"

#line 148 "src/parser.cpp"

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ID = 258,
    STRING = 259,
    INTEGER = 260,
    FLOATING = 261,
    IF = 262,
    ELSE = 263,
    WHILE = 264,
    INT = 265,
    VOID = 266,
    FLOAT = 267,
    LPAREN = 268,
    RPAREN = 269,
    LBRACE = 270,
    RBRACE = 271,
    SEMICOLON = 272,
    LBRACKET = 273,
    RBRACKET = 274,
    COMMA = 275,
    ADD = 276,
    SUB = 277,
    MUL = 278,
    DIV = 279,
    MOD = 280,
    OR = 281,
    AND = 282,
    LESS = 283,
    LESSEQUAL = 284,
    GREATER = 285,
    GREATEREQUAL = 286,
    ASSIGN = 287,
    EQUAL = 288,
    NOTEQUAL = 289,
    NOT = 290,
    CONST = 291,
    RETURN = 292,
    CONTINUE = 293,
    BREAK = 294,
    THEN = 295
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 38 "src/parser.y"

    double numtype; // store all number in float
    char* strtype;
    StmtNode* stmttype;
    ExprNode* exprtype;
    Type* type;
    SymbolEntry* se;

#line 209 "src/parser.cpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_INCLUDE_PARSER_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  60
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   243

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  41
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  52
/* YYNRULES -- Number of rules.  */
#define YYNRULES  110
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  183

#define YYUNDEFTOK  2
#define YYMAXUTOK   295


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    71,    71,    76,    77,    83,    83,    94,   100,   103,
     108,   108,   123,   128,   129,   130,   131,   132,   133,   134,
     139,   144,   145,   146,   149,   208,   232,   232,   269,   277,
     287,   292,   297,   303,   308,   311,   322,   326,   329,   332,
     335,   348,   352,   359,   360,   371,   384,   387,   392,   397,
     402,   409,   410,   415,   422,   423,   430,   431,   438,   441,
     442,   447,   451,   454,   460,   461,   464,   467,   470,   474,
     477,   481,   484,   485,   492,   503,   504,   514,   534,   535,
     546,   557,   572,   580,   606,   622,   622,   660,   686,   686,
     735,   758,   790,   793,   799,   854,   878,   878,   917,   920,
     925,   928,   934,   942,   934,   971,   972,   974,   978,   984,
     987
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "STRING", "INTEGER", "FLOATING",
  "IF", "ELSE", "WHILE", "INT", "VOID", "FLOAT", "LPAREN", "RPAREN",
  "LBRACE", "RBRACE", "SEMICOLON", "LBRACKET", "RBRACKET", "COMMA", "ADD",
  "SUB", "MUL", "DIV", "MOD", "OR", "AND", "LESS", "LESSEQUAL", "GREATER",
  "GREATEREQUAL", "ASSIGN", "EQUAL", "NOTEQUAL", "NOT", "CONST", "RETURN",
  "CONTINUE", "BREAK", "THEN", "$accept", "Program", "Stmts", "BlockStmt",
  "$@1", "IfStmt", "WhileStmt", "@2", "BreakStmt", "Stmt", "ConstInitVal",
  "@3", "LVal", "AssignStmt", "ExprStmt", "BlankStmt", "ContinueStmt",
  "ReturnStmt", "Exp", "Cond", "PrimaryExp", "AddExp", "RelExp", "EqExp",
  "LAndExp", "LOrExp", "ConstExp", "FuncRParams", "Type", "DeclStmt",
  "VarDeclStmt", "ConstDeclStmt", "VarDefList", "ConstDefList", "UnaryExp",
  "MulExp", "VarDef", "@4", "ConstDef", "@5", "FuncFParam", "ArrayIndices",
  "InitVal", "@6", "InitValList", "ConstInitValList", "FuncDef", "$@7",
  "@8", "MaybeFuncFParams", "FuncFParams", "FuncArrayIndices", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295
};
# endif

#define YYPACT_NINF (-137)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-103)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      92,    49,  -137,  -137,  -137,    -4,     9,  -137,  -137,  -137,
     112,    12,  -137,   112,   112,   112,     6,    21,    38,    53,
      61,    92,  -137,  -137,  -137,  -137,  -137,    60,  -137,  -137,
    -137,  -137,  -137,    83,  -137,    42,   103,  -137,  -137,  -137,
    -137,   126,  -137,    44,   112,    26,   112,   112,  -137,    96,
    -137,    92,  -137,  -137,  -137,   109,  -137,   118,  -137,  -137,
    -137,  -137,   112,  -137,   112,   112,    19,    63,  -137,   112,
     112,   112,  -137,  -137,    -8,    42,   105,   112,   140,    42,
     115,   107,    99,   106,   142,  -137,   160,    22,   102,  -137,
    -137,   143,   126,   126,   187,    27,   148,  -137,   159,  -137,
    -137,  -137,  -137,   112,  -137,   155,    92,   112,   112,   112,
     112,   112,   112,   112,   112,  -137,  -137,   208,    28,  -137,
     109,  -137,   162,  -137,  -137,  -137,     6,    50,  -137,  -137,
    -137,   171,    42,    42,    42,    42,   115,   115,   107,    99,
      92,   164,  -137,  -137,  -137,  -137,  -137,   187,   187,   180,
    -137,   170,   165,    92,  -137,  -137,   208,   208,  -137,    -1,
    -137,   168,  -137,     6,  -137,  -137,    74,  -137,  -137,   187,
     169,   176,   172,  -137,  -137,   208,  -137,  -137,   112,  -137,
    -137,   182,  -137
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    28,    40,    41,    42,     0,     0,    61,    62,    63,
       0,     5,    32,     0,     0,     0,     0,     0,     0,     0,
       0,     2,    15,    17,    18,    19,     3,    39,    13,    14,
      16,    20,    21,     0,    72,    36,     0,    22,    64,    65,
      78,    43,    23,     0,     0,    29,     0,     0,    39,     0,
       7,     0,    75,    76,    77,     0,    34,     0,    33,    12,
       1,     4,     0,    31,     0,     0,    82,     0,    69,     0,
       0,     0,    74,    59,     0,    58,     0,     0,     0,    46,
      51,    54,    56,    37,     0,    38,     0,     0,     0,    71,
      35,     0,    44,    45,     0,    83,     0,    66,     0,    79,
      80,    81,    73,     0,    92,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    10,     6,     0,     0,    67,
       0,    30,    96,    94,    84,    85,   106,    82,    68,    60,
      93,     8,    47,    48,    49,    50,    52,    53,    55,    57,
       0,    26,    87,    24,    88,    70,    95,     0,     0,     0,
     108,     0,   105,     0,    11,    25,     0,     0,    98,     0,
      86,    90,   103,     0,     9,   100,     0,    89,    97,     0,
       0,    91,     0,   107,    27,     0,    99,   109,     0,   104,
     101,     0,   110
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -137,  -137,   138,    31,  -137,  -137,  -137,  -137,  -137,   -17,
    -136,  -137,     2,  -137,  -137,  -137,  -137,  -137,   -10,   157,
    -137,   -36,    41,    94,    91,  -137,   -41,  -137,   -15,  -137,
    -137,  -137,  -137,  -137,    16,    93,   108,  -137,    90,  -137,
      52,     4,  -134,  -137,  -137,  -137,  -137,  -137,  -137,  -137,
    -137,  -137
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    20,    21,    22,    51,    23,    24,   140,    25,    26,
     142,   156,    48,    28,    29,    30,    31,    32,    33,    78,
      34,    35,    80,    81,    82,    83,   143,    74,    36,    37,
      38,    39,    67,    88,    40,    41,    68,   148,    89,   157,
     150,    95,   124,   147,   159,   166,    42,    96,   172,   151,
     152,   171
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      49,    55,    27,    76,    61,    45,   102,    57,    75,    46,
      79,    79,   103,   158,   160,   168,     7,     8,     9,   169,
     165,   167,    47,    27,     1,     2,     3,     4,    50,    52,
      53,    54,  -102,    73,    10,   176,   105,    44,    56,   180,
      44,    75,    13,    14,    77,    77,    77,     1,     2,     3,
       4,    94,    91,    27,   117,    58,    15,    10,    72,   125,
     144,    60,    43,    64,    65,    13,    14,    44,    44,    61,
      59,   132,   133,   134,   135,    79,    79,    79,    79,    15,
      97,    75,    94,    98,   123,    99,   100,   101,    27,   131,
     174,   118,    62,   129,   175,     1,     2,     3,     4,     5,
      63,     6,     7,     8,     9,    10,    66,    11,    27,    12,
      85,   149,    87,    13,    14,     1,     2,     3,     4,   119,
      75,    75,   120,   154,   104,    10,   113,    15,    16,    17,
      18,    19,   114,    13,    14,    90,   164,   123,   123,    75,
     111,   112,    27,   107,   108,   109,   110,    15,   149,    69,
      70,    71,   136,   137,   106,    27,   115,    92,    93,   123,
     121,   126,   127,     1,     2,     3,     4,     5,   181,     6,
       7,     8,     9,    10,   130,    11,   116,    12,   146,   153,
     155,    13,    14,   161,   162,   163,   170,    11,   177,    86,
       1,     2,     3,     4,   178,    15,    16,    17,    18,    19,
      10,   182,   122,   179,    84,   139,   128,   138,    13,    14,
     145,     1,     2,     3,     4,   173,     0,     0,     0,     0,
       0,    10,    15,   141,     0,     0,     0,     0,     0,    13,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    15
};

static const yytype_int16 yycheck[] =
{
      10,    16,     0,    44,    21,     1,    14,    17,    44,    13,
      46,    47,    20,   147,   148,    16,    10,    11,    12,    20,
     156,   157,    13,    21,     3,     4,     5,     6,    16,    13,
      14,    15,    13,    43,    13,   169,    77,    18,    17,   175,
      18,    77,    21,    22,    18,    18,    18,     3,     4,     5,
       6,    32,    62,    51,    32,    17,    35,    13,    14,    32,
      32,     0,    13,    21,    22,    21,    22,    18,    18,    86,
      17,   107,   108,   109,   110,   111,   112,   113,   114,    35,
      17,   117,    32,    20,    94,    69,    70,    71,    86,   106,
      16,    87,    32,   103,    20,     3,     4,     5,     6,     7,
      17,     9,    10,    11,    12,    13,     3,    15,   106,    17,
      14,   126,     3,    21,    22,     3,     4,     5,     6,    17,
     156,   157,    20,   140,    19,    13,    27,    35,    36,    37,
      38,    39,    26,    21,    22,    17,   153,   147,   148,   175,
      33,    34,   140,    28,    29,    30,    31,    35,   163,    23,
      24,    25,   111,   112,    14,   153,    14,    64,    65,   169,
      17,    13,     3,     3,     4,     5,     6,     7,   178,     9,
      10,    11,    12,    13,    19,    15,    16,    17,    16,     8,
      16,    21,    22,     3,    14,    20,    18,    15,    19,    51,
       3,     4,     5,     6,    18,    35,    36,    37,    38,    39,
      13,    19,    15,   172,    47,   114,    98,   113,    21,    22,
     120,     3,     4,     5,     6,   163,    -1,    -1,    -1,    -1,
      -1,    13,    35,    15,    -1,    -1,    -1,    -1,    -1,    21,
      22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    35
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     9,    10,    11,    12,
      13,    15,    17,    21,    22,    35,    36,    37,    38,    39,
      42,    43,    44,    46,    47,    49,    50,    53,    54,    55,
      56,    57,    58,    59,    61,    62,    69,    70,    71,    72,
      75,    76,    87,    13,    18,    82,    13,    13,    53,    59,
      16,    45,    75,    75,    75,    69,    17,    59,    17,    17,
       0,    50,    32,    17,    21,    22,     3,    73,    77,    23,
      24,    25,    14,    59,    68,    62,    67,    18,    60,    62,
      63,    64,    65,    66,    60,    14,    43,     3,    74,    79,
      17,    59,    76,    76,    32,    82,    88,    17,    20,    75,
      75,    75,    14,    20,    19,    67,    14,    28,    29,    30,
      31,    33,    34,    27,    26,    14,    16,    32,    82,    17,
      20,    17,    15,    59,    83,    32,    13,     3,    77,    59,
      19,    50,    62,    62,    62,    62,    63,    63,    64,    65,
      48,    15,    51,    67,    32,    79,    16,    84,    78,    69,
      81,    90,    91,     8,    50,    16,    52,    80,    83,    85,
      83,     3,    14,    20,    50,    51,    86,    51,    16,    20,
      18,    92,    89,    81,    16,    20,    83,    19,    18,    44,
      51,    59,    19
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    41,    42,    43,    43,    45,    44,    44,    46,    46,
      48,    47,    49,    50,    50,    50,    50,    50,    50,    50,
      50,    50,    50,    50,    51,    51,    52,    51,    53,    53,
      54,    55,    56,    57,    58,    58,    59,    60,    61,    61,
      61,    61,    61,    62,    62,    62,    63,    63,    63,    63,
      63,    64,    64,    64,    65,    65,    66,    66,    67,    68,
      68,    69,    69,    69,    70,    70,    71,    72,    73,    73,
      74,    74,    75,    75,    75,    75,    75,    75,    76,    76,
      76,    76,    77,    77,    77,    78,    77,    79,    80,    79,
      81,    81,    82,    82,    83,    83,    84,    83,    85,    85,
      86,    86,    88,    89,    87,    90,    90,    91,    91,    92,
      92
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     0,     4,     2,     5,     7,
       0,     6,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     0,     4,     1,     2,
       4,     2,     1,     2,     2,     3,     1,     1,     3,     1,
       1,     1,     1,     1,     3,     3,     1,     3,     3,     3,
       3,     1,     3,     3,     1,     3,     1,     3,     1,     1,
       3,     1,     1,     1,     1,     1,     3,     4,     3,     1,
       3,     1,     1,     4,     3,     2,     2,     2,     1,     3,
       3,     3,     1,     2,     3,     0,     5,     3,     0,     5,
       2,     3,     3,     4,     1,     2,     0,     4,     1,     3,
       1,     3,     0,     0,     8,     1,     0,     3,     1,     2,
       4
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 71 "src/parser.y"
            {
        ast.setRoot((yyvsp[0].stmttype));
    }
#line 1538 "src/parser.cpp"
    break;

  case 3:
#line 76 "src/parser.y"
           {(yyval.stmttype)=(yyvsp[0].stmttype);}
#line 1544 "src/parser.cpp"
    break;

  case 4:
#line 77 "src/parser.y"
                {
        (yyval.stmttype) = new SeqNode((yyvsp[-1].stmttype), (yyvsp[0].stmttype));
    }
#line 1552 "src/parser.cpp"
    break;

  case 5:
#line 83 "src/parser.y"
             {
        identifiers = new SymbolTable(identifiers);
    }
#line 1560 "src/parser.cpp"
    break;

  case 6:
#line 86 "src/parser.y"
                   {
        // midrule actions https://www.gnu.org/software/bison/manual/html_node/Using-Midrule-Actions.html
        (yyval.stmttype) = new CompoundStmt((yyvsp[-1].stmttype));

        SymbolTable* top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
#line 1573 "src/parser.cpp"
    break;

  case 7:
#line 94 "src/parser.y"
                    {
        // 这里这个用加嘛 不确定
        (yyval.stmttype) = new CompoundStmt();
    }
#line 1582 "src/parser.cpp"
    break;

  case 8:
#line 100 "src/parser.y"
                                            {
        (yyval.stmttype) = new IfStmt((yyvsp[-2].exprtype), (yyvsp[0].stmttype));
    }
#line 1590 "src/parser.cpp"
    break;

  case 9:
#line 103 "src/parser.y"
                                           {
        (yyval.stmttype) = new IfElseStmt((yyvsp[-4].exprtype), (yyvsp[-2].stmttype), (yyvsp[0].stmttype));
    }
#line 1598 "src/parser.cpp"
    break;

  case 10:
#line 108 "src/parser.y"
                               {
        whileCnt++;
        WhileStmt *whileNode = new WhileStmt((yyvsp[-1].exprtype));
        (yyval.stmttype) = whileNode;
        whileStk.push(whileNode);
    }
#line 1609 "src/parser.cpp"
    break;

  case 11:
#line 114 "src/parser.y"
         {
        StmtNode *whileNode = (yyvsp[-1].stmttype); 
        ((WhileStmt*)whileNode)->setStmt((yyvsp[0].stmttype));
        (yyval.stmttype) = whileNode;
        whileStk.pop();
        whileCnt--;
    }
#line 1621 "src/parser.cpp"
    break;

  case 12:
#line 123 "src/parser.y"
                      {
        (yyval.stmttype) = new BreakStmt(whileStk.top());
    }
#line 1629 "src/parser.cpp"
    break;

  case 13:
#line 128 "src/parser.y"
                 { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1635 "src/parser.cpp"
    break;

  case 14:
#line 129 "src/parser.y"
               { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1641 "src/parser.cpp"
    break;

  case 15:
#line 130 "src/parser.y"
                {(yyval.stmttype) = (yyvsp[0].stmttype);}
#line 1647 "src/parser.cpp"
    break;

  case 16:
#line 131 "src/parser.y"
                { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1653 "src/parser.cpp"
    break;

  case 17:
#line 132 "src/parser.y"
             { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1659 "src/parser.cpp"
    break;

  case 18:
#line 133 "src/parser.y"
                { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1665 "src/parser.cpp"
    break;

  case 19:
#line 134 "src/parser.y"
                {
        if (!whileCnt)
            fprintf(stderr, "\'break\' statement not in while statement\n");
        (yyval.stmttype) = (yyvsp[0].stmttype);
    }
#line 1675 "src/parser.cpp"
    break;

  case 20:
#line 139 "src/parser.y"
                   {
        if (!whileCnt)
            fprintf(stderr, "\'continue\' statement not in while statement\n");
        (yyval.stmttype) = (yyvsp[0].stmttype);
    }
#line 1685 "src/parser.cpp"
    break;

  case 21:
#line 144 "src/parser.y"
                 { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1691 "src/parser.cpp"
    break;

  case 22:
#line 145 "src/parser.y"
               { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1697 "src/parser.cpp"
    break;

  case 23:
#line 146 "src/parser.y"
              { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1703 "src/parser.cpp"
    break;

  case 24:
#line 149 "src/parser.y"
               {
        (yyval.exprtype) = (yyvsp[0].exprtype);
        if (!stk.empty()) {

            double val = (yyvsp[0].exprtype)->getValue();
            if (val)
                notZeroNum++;
            if (declType->isInt() && (yyvsp[0].exprtype)->getType()->isFloat()) {
                float temp = (float)val;
                int temp1 = (int)temp;
                arrayValue[idx++] = (double)temp1;
            } else {
                arrayValue[idx++] = val;
            }

            Type* arrTy = stk.top()->getSymbolEntry()->getType();
            if (arrTy == TypeSystem::constIntType || arrTy == TypeSystem::constFloatType) {
                if ((arrTy->isFloat() && (yyvsp[0].exprtype)->getType()->isInt()) || 
                    (arrTy->isInt() && (yyvsp[0].exprtype)->getType()->isFloat())) {

                    ImplicitCastExpr* temp = new ImplicitCastExpr((yyvsp[0].exprtype), arrTy); 
                    // arrTy is const here
                    stk.top()->addExpr(temp);

                } else {
                    stk.top()->addExpr((yyvsp[0].exprtype));
                }
            } else {
                while (arrTy) {
                    if (((ArrayType*)arrTy)->getElementType() != TypeSystem::constIntType &&
                        ((ArrayType*)arrTy)->getElementType() != TypeSystem::constFloatType) {
                        arrTy = ((ArrayType*)arrTy)->getElementType();
                        SymbolEntry* se = new ConstantSymbolEntry(arrTy);
                        InitValueListExpr* list = new InitValueListExpr(se);
                        stk.top()->addExpr(list);
                        stk.push(list);
                    } else {
                        Type* elemType = ((ArrayType*)arrTy)->getElementType();
                        if ((elemType->isFloat() && (yyvsp[0].exprtype)->getType()->isInt()) || 
                            (elemType->isInt() && (yyvsp[0].exprtype)->getType()->isFloat())) {

                            ImplicitCastExpr* temp = new ImplicitCastExpr((yyvsp[0].exprtype), elemType); 
                            // `elemType` is const here
                            stk.top()->addExpr(temp);

                        } else {
                            stk.top()->addExpr((yyvsp[0].exprtype));
                        }

                        while (stk.top()->isFull() && stk.size() != (long unsigned int)leftCnt) {
                            arrTy = ((ArrayType*)arrTy)->getArrayType();
                            stk.pop();
                        }
                        break;
                    }
                }
            }
        }
    }
#line 1767 "src/parser.cpp"
    break;

  case 25:
#line 208 "src/parser.y"
                    {
        SymbolEntry* se;
        ExprNode* list;
        if (stk.empty()) {
            // 栈为空 则不用加入栈
            memset(arrayValue, 0, arrayType->getSize());
            idx += arrayType->getSize() / declType->getSize();
            se = new ConstantSymbolEntry(arrayType);
            list = new InitValueListExpr(se);
        } else {
            // 栈不空 则需要确定元素个数
            Type* type = ((ArrayType*)(stk.top()->getSymbolEntry()->getType()))->getElementType();
            int len = type->getSize() / declType->getSize();
            memset(arrayValue + idx, 0, type->getSize());
            idx += len;
            se = new ConstantSymbolEntry(type);
            list = new InitValueListExpr(se);
            stk.top()->addExpr(list);
            while (stk.top()->isFull() && stk.size() != (long unsigned int)leftCnt) {
                stk.pop();
            }
        }
        (yyval.exprtype) = list;
    }
#line 1796 "src/parser.cpp"
    break;

  case 26:
#line 232 "src/parser.y"
             {
        SymbolEntry* se;
        if (!stk.empty())
            arrayType = (ArrayType*)(
                ((ArrayType*)(stk.top()->getSymbolEntry()->getType()))->getElementType());
        se = new ConstantSymbolEntry(arrayType);
        if (arrayType->getElementType() != TypeSystem::intType &&
            arrayType->getElementType() != TypeSystem::floatType) {
            arrayType = (ArrayType*)(arrayType->getElementType());
        }
        InitValueListExpr* expr = new InitValueListExpr(se);
        if (!stk.empty())
            stk.top()->addExpr(expr);
        stk.push(expr);
        (yyval.exprtype) = expr;
        leftCnt++;
    }
#line 1818 "src/parser.cpp"
    break;

  case 27:
#line 249 "src/parser.y"
                              {
        leftCnt--;
        while (stk.top() != (yyvsp[-2].exprtype) && stk.size() > (long unsigned int)(leftCnt + 1))
            stk.pop();
        if (stk.top() == (yyvsp[-2].exprtype))
            stk.pop();
        (yyval.exprtype) = (yyvsp[-2].exprtype);
        if (!stk.empty())
            while (stk.top()->isFull() && stk.size() != (long unsigned int)leftCnt) {
                stk.pop();
            }
        while (
            idx % (((ArrayType*)((yyval.exprtype)->getSymbolEntry()->getType()))->getSize() / sizeof(int)) != 0)
            arrayValue[idx++] = 0;
        if (!stk.empty())
            arrayType = (ArrayType*)(
                ((ArrayType*)(stk.top()->getSymbolEntry()->getType()))->getElementType());
    }
#line 1841 "src/parser.cpp"
    break;

  case 28:
#line 269 "src/parser.y"
         {
        SymbolEntry* se;
        se = identifiers->lookup((yyvsp[0].strtype));
        if (se == nullptr)
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)(yyvsp[0].strtype));
        (yyval.exprtype) = new Id(se);
        delete [](yyvsp[0].strtype);
    }
#line 1854 "src/parser.cpp"
    break;

  case 29:
#line 277 "src/parser.y"
                     {
        SymbolEntry* se;
        se = identifiers->lookup((yyvsp[-1].strtype));
        if (se == nullptr)
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)(yyvsp[-1].strtype));
        (yyval.exprtype) = new Id(se, (yyvsp[0].exprtype));
        delete [](yyvsp[-1].strtype);
    }
#line 1867 "src/parser.cpp"
    break;

  case 30:
#line 287 "src/parser.y"
                                {
        (yyval.stmttype) = new AssignStmt((yyvsp[-3].exprtype), (yyvsp[-1].exprtype));
    }
#line 1875 "src/parser.cpp"
    break;

  case 31:
#line 292 "src/parser.y"
                    {
        (yyval.stmttype) = new ExprStmt((yyvsp[-1].exprtype));
    }
#line 1883 "src/parser.cpp"
    break;

  case 32:
#line 297 "src/parser.y"
                {
        (yyval.stmttype) = new BlankStmt();
    }
#line 1891 "src/parser.cpp"
    break;

  case 33:
#line 303 "src/parser.y"
                         {
        (yyval.stmttype) = new ContinueStmt(whileStk.top());
    }
#line 1899 "src/parser.cpp"
    break;

  case 34:
#line 308 "src/parser.y"
                       {
        (yyval.stmttype) = new ReturnStmt();
    }
#line 1907 "src/parser.cpp"
    break;

  case 35:
#line 311 "src/parser.y"
                           {
        if (((yyvsp[-1].exprtype)->getType()->isFloat() && funcRetType->isInt()) ||
            ((yyvsp[-1].exprtype)->getType()->isInt() && funcRetType->isFloat())) {
            (yyval.stmttype) = new ReturnStmt(new ImplicitCastExpr((yyvsp[-1].exprtype), funcRetType));
        } else {
            (yyval.stmttype) = new ReturnStmt((yyvsp[-1].exprtype));            
        }
    }
#line 1920 "src/parser.cpp"
    break;

  case 36:
#line 322 "src/parser.y"
           { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1926 "src/parser.cpp"
    break;

  case 37:
#line 326 "src/parser.y"
           { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1932 "src/parser.cpp"
    break;

  case 38:
#line 329 "src/parser.y"
                        {
        (yyval.exprtype) = (yyvsp[-1].exprtype);
    }
#line 1940 "src/parser.cpp"
    break;

  case 39:
#line 332 "src/parser.y"
           {
        (yyval.exprtype) = (yyvsp[0].exprtype);
    }
#line 1948 "src/parser.cpp"
    break;

  case 40:
#line 335 "src/parser.y"
             {
        SymbolEntry* se;
        se = globals->lookup(std::string((yyvsp[0].strtype)));
        // 这里如果str内容和变量名相同 怎么处理
        if (se == nullptr) {
            Type* type = new StringType(strlen((yyvsp[0].strtype)));
            se = new ConstantSymbolEntry(type, std::string((yyvsp[0].strtype)));
            globals->install(std::string((yyvsp[0].strtype)), se);
        }
        ExprNode* expr = new ExprNode(se);

        (yyval.exprtype) = expr;
    }
#line 1966 "src/parser.cpp"
    break;

  case 41:
#line 348 "src/parser.y"
              {
        SymbolEntry* se = new ConstantSymbolEntry(TypeSystem::intType, (yyvsp[0].numtype));
        (yyval.exprtype) = new Constant(se);
    }
#line 1975 "src/parser.cpp"
    break;

  case 42:
#line 352 "src/parser.y"
               {
        SymbolEntry* se = new ConstantSymbolEntry(TypeSystem::floatType, (yyvsp[0].numtype));
        (yyval.exprtype) = new Constant(se);
    }
#line 1984 "src/parser.cpp"
    break;

  case 43:
#line 359 "src/parser.y"
             { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1990 "src/parser.cpp"
    break;

  case 44:
#line 360 "src/parser.y"
                        {
        SymbolEntry* se;
        if ((yyvsp[-2].exprtype)->getType()->isFloat() || (yyvsp[0].exprtype)->getType()->isFloat()) {
            se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::ADD, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        } else {
            se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::ADD, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
            (yyval.exprtype) = tmpExpr->const_fold();
        }
    }
#line 2006 "src/parser.cpp"
    break;

  case 45:
#line 371 "src/parser.y"
                        {
        SymbolEntry* se;
        if ((yyvsp[-2].exprtype)->getType()->isFloat() || (yyvsp[0].exprtype)->getType()->isFloat()) {
            se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::SUB, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        } else {
            se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::SUB, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
            (yyval.exprtype) = tmpExpr->const_fold();
        }
    }
#line 2022 "src/parser.cpp"
    break;

  case 46:
#line 384 "src/parser.y"
             {
        (yyval.exprtype) = (yyvsp[0].exprtype);
    }
#line 2030 "src/parser.cpp"
    break;

  case 47:
#line 387 "src/parser.y"
                         {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::LESS, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 2040 "src/parser.cpp"
    break;

  case 48:
#line 392 "src/parser.y"
                              {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::LESSEQUAL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 2050 "src/parser.cpp"
    break;

  case 49:
#line 397 "src/parser.y"
                            {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::GREATER, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 2060 "src/parser.cpp"
    break;

  case 50:
#line 402 "src/parser.y"
                                 {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::GREATEREQUAL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 2070 "src/parser.cpp"
    break;

  case 51:
#line 409 "src/parser.y"
             { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 2076 "src/parser.cpp"
    break;

  case 52:
#line 410 "src/parser.y"
                         {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::EQUAL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 2086 "src/parser.cpp"
    break;

  case 53:
#line 415 "src/parser.y"
                            {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::NOTEQUAL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 2096 "src/parser.cpp"
    break;

  case 54:
#line 422 "src/parser.y"
            { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 2102 "src/parser.cpp"
    break;

  case 55:
#line 423 "src/parser.y"
                        {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::AND, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 2112 "src/parser.cpp"
    break;

  case 56:
#line 430 "src/parser.y"
              { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 2118 "src/parser.cpp"
    break;

  case 57:
#line 431 "src/parser.y"
                        {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::OR, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 2128 "src/parser.cpp"
    break;

  case 58:
#line 438 "src/parser.y"
             { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 2134 "src/parser.cpp"
    break;

  case 59:
#line 441 "src/parser.y"
          { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 2140 "src/parser.cpp"
    break;

  case 60:
#line 442 "src/parser.y"
                            {
        (yyval.exprtype) = (yyvsp[-2].exprtype);
        (yyval.exprtype)->setNext((yyvsp[0].exprtype));
    }
#line 2149 "src/parser.cpp"
    break;

  case 61:
#line 447 "src/parser.y"
          {
        (yyval.type) = TypeSystem::intType;
        declType = TypeSystem::intType;
    }
#line 2158 "src/parser.cpp"
    break;

  case 62:
#line 451 "src/parser.y"
           {
        (yyval.type) = TypeSystem::voidType;
    }
#line 2166 "src/parser.cpp"
    break;

  case 63:
#line 454 "src/parser.y"
            {
        (yyval.type) = TypeSystem::floatType;
        declType = TypeSystem::floatType;
    }
#line 2175 "src/parser.cpp"
    break;

  case 64:
#line 460 "src/parser.y"
                  { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 2181 "src/parser.cpp"
    break;

  case 65:
#line 461 "src/parser.y"
                    { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 2187 "src/parser.cpp"
    break;

  case 66:
#line 464 "src/parser.y"
                                { (yyval.stmttype) = (yyvsp[-1].stmttype); }
#line 2193 "src/parser.cpp"
    break;

  case 67:
#line 467 "src/parser.y"
                                        { (yyval.stmttype) = (yyvsp[-1].stmttype); }
#line 2199 "src/parser.cpp"
    break;

  case 68:
#line 470 "src/parser.y"
                              {
        (yyval.stmttype) = (yyvsp[-2].stmttype);
        (yyvsp[-2].stmttype)->setNext((yyvsp[0].stmttype));
    }
#line 2208 "src/parser.cpp"
    break;

  case 69:
#line 474 "src/parser.y"
             { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 2214 "src/parser.cpp"
    break;

  case 70:
#line 477 "src/parser.y"
                                  {
        (yyval.stmttype) = (yyvsp[-2].stmttype);
        (yyvsp[-2].stmttype)->setNext((yyvsp[0].stmttype));
    }
#line 2223 "src/parser.cpp"
    break;

  case 71:
#line 481 "src/parser.y"
               { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 2229 "src/parser.cpp"
    break;

  case 72:
#line 484 "src/parser.y"
                 { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 2235 "src/parser.cpp"
    break;

  case 73:
#line 485 "src/parser.y"
                                   {
        SymbolEntry* se;
        se = identifiers->lookup((yyvsp[-3].strtype));
        if (se == nullptr)
            fprintf(stderr, "function \"%s\" is undefined\n", (char*)(yyvsp[-3].strtype));
        (yyval.exprtype) = new CallExpr(se, (yyvsp[-1].exprtype));
    }
#line 2247 "src/parser.cpp"
    break;

  case 74:
#line 492 "src/parser.y"
                       {
        SymbolEntry* se;
        se = identifiers->lookup((yyvsp[-2].strtype));
        if (se == nullptr)
            fprintf(stderr, "function \"%s\" is undefined\n", (char*)(yyvsp[-2].strtype));
        if (strcmp((yyvsp[-2].strtype), "_sysy_starttime") == 0 || strcmp((yyvsp[-2].strtype), "_sysy_stoptime") == 0) {
            ExprNode* param = new Constant(new ConstantSymbolEntry(TypeSystem::intType, yylineno));
            (yyval.exprtype) = new CallExpr(se, param);
        } else
            (yyval.exprtype) = new CallExpr(se);
    }
#line 2263 "src/parser.cpp"
    break;

  case 75:
#line 503 "src/parser.y"
                   { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 2269 "src/parser.cpp"
    break;

  case 76:
#line 504 "src/parser.y"
                   {
        Type* exprType = (yyvsp[0].exprtype)->getType();
        SymbolEntry* se = new TemporarySymbolEntry(exprType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new UnaryExpr(se, UnaryExpr::SUB, (yyvsp[0].exprtype));
        if (exprType->isFloat()) {
            (yyval.exprtype) = tmpExpr;
        } else {
            (yyval.exprtype) = tmpExpr->const_fold();
        }
    }
#line 2284 "src/parser.cpp"
    break;

  case 77:
#line 514 "src/parser.y"
                   {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr;
        if ((yyvsp[0].exprtype)->getType()->isFloat()) {

            SymbolEntry* zero =
                new ConstantSymbolEntry(TypeSystem::constFloatType, 0);
            SymbolEntry* temp = new TemporarySymbolEntry(
                TypeSystem::boolType, SymbolTable::getLabel());
            BinaryExpr* cmpZero = new BinaryExpr(temp, BinaryExpr::NOTEQUAL,
                                                    (yyvsp[0].exprtype), new Constant(zero));

            (yyval.exprtype) = new UnaryExpr(se, UnaryExpr::NOT, cmpZero);
        } else {
            tmpExpr = new UnaryExpr(se, UnaryExpr::NOT, (yyvsp[0].exprtype));
            (yyval.exprtype) = tmpExpr->const_fold();
        }
    }
#line 2307 "src/parser.cpp"
    break;

  case 78:
#line 534 "src/parser.y"
               { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 2313 "src/parser.cpp"
    break;

  case 79:
#line 535 "src/parser.y"
                          {
        SymbolEntry* se;
        if ((yyvsp[-2].exprtype)->getType()->isFloat() || (yyvsp[0].exprtype)->getType()->isFloat()) {
            se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::MUL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        } else {
            se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::MUL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
            (yyval.exprtype) = tmpExpr->const_fold();
        }
    }
#line 2329 "src/parser.cpp"
    break;

  case 80:
#line 546 "src/parser.y"
                          {
        SymbolEntry* se;
        if ((yyvsp[-2].exprtype)->getType()->isFloat() || (yyvsp[0].exprtype)->getType()->isFloat()) {
            se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::DIV, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        } else {
            se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::DIV, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
            (yyval.exprtype) = tmpExpr->const_fold();
        }
    }
#line 2345 "src/parser.cpp"
    break;

  case 81:
#line 557 "src/parser.y"
                          {

        if ((yyvsp[-2].exprtype)->getType()->isFloat() || (yyvsp[0].exprtype)->getType()->isFloat()) {
            // error
            // already handled in `ast.cpp`
            fprintf(stderr, "Operands of `mod` must be both integers");
        }

        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::MOD, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 2362 "src/parser.cpp"
    break;

  case 82:
#line 572 "src/parser.y"
         {
        SymbolEntry* se;
        se = new IdentifierSymbolEntry(declType, (yyvsp[0].strtype), identifiers->getLevel());
        if (!identifiers->install((yyvsp[0].strtype), se))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)(yyvsp[0].strtype));
        (yyval.stmttype) = new DeclStmt(new Id(se));
        delete [](yyvsp[0].strtype);
    }
#line 2375 "src/parser.cpp"
    break;

  case 83:
#line 580 "src/parser.y"
                      {
        SymbolEntry* se;
        std::vector<int> vec;
        ExprNode* temp = (yyvsp[0].exprtype);
        while (temp) {
            vec.push_back(temp->getValue());
            temp = (ExprNode*)(temp->getNext());
        }
        Type* type = declType;
        Type* temp1;
        while (!vec.empty()) {
            temp1 = new ArrayType(type, vec.back());
            if (type->isArray())
                ((ArrayType*)type)->setArrayType(temp1);
            type = temp1;
            vec.pop_back();
        }
        arrayType = (ArrayType*)type;
        se = new IdentifierSymbolEntry(type, (yyvsp[-1].strtype), identifiers->getLevel());
        ((IdentifierSymbolEntry*)se)->setAllZero();

        if (!identifiers->install((yyvsp[-1].strtype), se))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)(yyvsp[-1].strtype));
        (yyval.stmttype) = new DeclStmt(new Id(se));
        delete [](yyvsp[-1].strtype);
    }
#line 2406 "src/parser.cpp"
    break;

  case 84:
#line 606 "src/parser.y"
                        {
        SymbolEntry* se;
        se = new IdentifierSymbolEntry(declType, (yyvsp[-2].strtype), identifiers->getLevel());
        if (!identifiers->install((yyvsp[-2].strtype), se))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)(yyvsp[-2].strtype));

        double val = (yyvsp[0].exprtype)->getValue();
        if (declType->isInt() && (yyvsp[0].exprtype)->getType()->isFloat()) {
            float temp = (float)val;
            int temp1 = (int)temp;
            val = (double)temp1;
        }
        ((IdentifierSymbolEntry*)se)->setValue(val);
        (yyval.stmttype) = new DeclStmt(new Id(se), (yyvsp[0].exprtype));
        delete [](yyvsp[-2].strtype);
    }
#line 2427 "src/parser.cpp"
    break;

  case 85:
#line 622 "src/parser.y"
                             {
        SymbolEntry* se;
        std::vector<int> vec;
        ExprNode* temp = (yyvsp[-1].exprtype);
        while (temp) {
            vec.push_back(temp->getValue());
            temp = (ExprNode*)(temp->getNext());
        }
        Type* type = declType;
        Type* temp1;
        for(auto it = vec.rbegin(); it != vec.rend(); it++) {
            temp1 = new ArrayType(type, *it);
            if (type->isArray())
                ((ArrayType*)type)->setArrayType(temp1);
            type = temp1;
        }
        arrayType = (ArrayType*)type;
        idx = 0;
        std::stack<InitValueListExpr*>().swap(stk);
        se = new IdentifierSymbolEntry(type, (yyvsp[-2].strtype), identifiers->getLevel());
        (yyval.se) = se;
        arrayValue = new double[arrayType->getSize()];
        notZeroNum = 0;
    }
#line 2456 "src/parser.cpp"
    break;

  case 86:
#line 646 "src/parser.y"
              {
        ((IdentifierSymbolEntry*)(yyvsp[-1].se))->setArrayValue(arrayValue);
        ((IdentifierSymbolEntry*)(yyvsp[-1].se))->setNotZeroNum(notZeroNum);
        if ((notZeroNum == 0) || ((InitValueListExpr*)(yyvsp[0].exprtype))->isEmpty()){
            ((IdentifierSymbolEntry*)(yyvsp[-1].se))->setAllZero();
            ((InitValueListExpr*)(yyvsp[0].exprtype))->setAllZero();
        }
        if (!identifiers->install((yyvsp[-4].strtype), (yyvsp[-1].se)))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)(yyvsp[-4].strtype));
        (yyval.stmttype) = new DeclStmt(new Id((yyvsp[-1].se)), (yyvsp[0].exprtype));
        delete [](yyvsp[-4].strtype);
    }
#line 2473 "src/parser.cpp"
    break;

  case 87:
#line 660 "src/parser.y"
                             {

        if (declType->isFloat()) {
            declType = TypeSystem::constFloatType;
        } else if (declType->isInt()) {
            declType = TypeSystem::constIntType;
        } else {
            // error
        }

        SymbolEntry* se;
        se = new IdentifierSymbolEntry(declType, (yyvsp[-2].strtype), identifiers->getLevel());
        ((IdentifierSymbolEntry*)se)->setConst();
        if (!identifiers->install((yyvsp[-2].strtype), se))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)(yyvsp[-2].strtype));
        identifiers->install((yyvsp[-2].strtype), se);
        double val = (yyvsp[0].exprtype)->getValue();
        if (declType->isInt() && (yyvsp[0].exprtype)->getType()->isFloat()) {
            float temp = (float)val;
            int temp1 = (int)temp;
            val = (double)temp1;
        }
        ((IdentifierSymbolEntry*)se)->setValue(val);
        (yyval.stmttype) = new DeclStmt(new Id(se), (yyvsp[0].exprtype));
        delete [](yyvsp[-2].strtype);
    }
#line 2504 "src/parser.cpp"
    break;

  case 88:
#line 686 "src/parser.y"
                             {

        if (declType->isFloat()) {
            declType = TypeSystem::constFloatType;
        } else if (declType->isInt()) {
            declType = TypeSystem::constIntType;
        } else {
            // error
        }

        SymbolEntry* se;
        std::vector<int> vec;
        ExprNode* temp = (yyvsp[-1].exprtype);
        while (temp) {
            vec.push_back(temp->getValue());
            temp = (ExprNode*)(temp->getNext());
        }
        Type* type = declType;
        Type* temp1;
        for(auto it = vec.rbegin(); it != vec.rend(); it++) {
            temp1 = new ArrayType(type, *it, true);
            if (type->isArray())
                ((ArrayType*)type)->setArrayType(temp1);
            type = temp1;
        }
        arrayType = (ArrayType*)type;
        idx = 0;
        std::stack<InitValueListExpr*>().swap(stk);
        se = new IdentifierSymbolEntry(type, (yyvsp[-2].strtype), identifiers->getLevel());
        ((IdentifierSymbolEntry*)se)->setConst();
        (yyval.se) = se;
        arrayValue = new double[arrayType->getSize()];
        notZeroNum = 0;
    }
#line 2543 "src/parser.cpp"
    break;

  case 89:
#line 720 "src/parser.y"
                   {
        ((IdentifierSymbolEntry*)(yyvsp[-1].se))->setArrayValue(arrayValue);
        ((IdentifierSymbolEntry*)(yyvsp[-1].se))->setNotZeroNum(notZeroNum);
        if ((notZeroNum == 0) || ((InitValueListExpr*)(yyvsp[0].exprtype))->isEmpty()){
            ((IdentifierSymbolEntry*)(yyvsp[-1].se))->setAllZero();
            ((InitValueListExpr*)(yyvsp[0].exprtype))->setAllZero();
        }
        if (!identifiers->install((yyvsp[-4].strtype), (yyvsp[-1].se)))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)(yyvsp[-4].strtype));
        identifiers->install((yyvsp[-4].strtype), (yyvsp[-1].se));
        (yyval.stmttype) = new DeclStmt(new Id((yyvsp[-1].se)), (yyvsp[0].exprtype));
        delete [](yyvsp[-4].strtype);
    }
#line 2561 "src/parser.cpp"
    break;

  case 90:
#line 735 "src/parser.y"
              {
        SymbolEntry* se;
        if ((yyvsp[-1].type)->isFloat()) {
            se = new IdentifierSymbolEntry((yyvsp[-1].type), (yyvsp[0].strtype), identifiers->getLevel(), fpParamNo++);
            ((IdentifierSymbolEntry*)se)->setAllParamNo(fpParamNo + paramNo - 1);
            if (fpParamNo > 4){
                ((IdentifierSymbolEntry*)se)->setStackParamNo(stackParamNo);
                stackParamNo++;
            }
        } else {
            se = new IdentifierSymbolEntry((yyvsp[-1].type), (yyvsp[0].strtype), identifiers->getLevel(), paramNo++);
            ((IdentifierSymbolEntry*)se)->setAllParamNo(fpParamNo + paramNo - 1);
            if (paramNo > 4){
                ((IdentifierSymbolEntry*)se)->setStackParamNo(stackParamNo);
                stackParamNo++;
            }
        }
        identifiers->install((yyvsp[0].strtype), se);
        ((IdentifierSymbolEntry*)se)->setLabel();
        ((IdentifierSymbolEntry*)se)->setAddr(new Operand(se));
        (yyval.stmttype) = new DeclStmt(new Id(se));
        delete [](yyvsp[0].strtype);
    }
#line 2589 "src/parser.cpp"
    break;

  case 91:
#line 758 "src/parser.y"
                               {
        // 这里也需要求值
        SymbolEntry* se;
        ExprNode* temp = (yyvsp[0].exprtype);
        Type* arr = (yyvsp[-2].type);
        Type* arr1;
        std::stack<ExprNode*> stk;
        while (temp) {
            stk.push(temp);
            temp = (ExprNode*)(temp->getNext());
        }
        while (!stk.empty()) {
            arr1 = new ArrayType(arr, stk.top()->getValue());
            if (arr->isArray())
                ((ArrayType*)arr)->setArrayType(arr1);
            arr = arr1;
            stk.pop();
        }
        se = new IdentifierSymbolEntry(arr, (yyvsp[-1].strtype), identifiers->getLevel(), paramNo++);
        ((IdentifierSymbolEntry*)se)->setAllParamNo(fpParamNo + paramNo - 1);
        if (paramNo > 4){
            ((IdentifierSymbolEntry*)se)->setStackParamNo(stackParamNo);
            stackParamNo++;
        }
        identifiers->install((yyvsp[-1].strtype), se);
        ((IdentifierSymbolEntry*)se)->setLabel();
        ((IdentifierSymbolEntry*)se)->setAddr(new Operand(se));
        (yyval.stmttype) = new DeclStmt(new Id(se));
        delete [](yyvsp[-1].strtype);
    }
#line 2624 "src/parser.cpp"
    break;

  case 92:
#line 790 "src/parser.y"
                                 {
        (yyval.exprtype) = (yyvsp[-1].exprtype);
    }
#line 2632 "src/parser.cpp"
    break;

  case 93:
#line 793 "src/parser.y"
                                              {
        (yyval.exprtype) = (yyvsp[-3].exprtype);
        (yyvsp[-3].exprtype)->setNext((yyvsp[-1].exprtype));
    }
#line 2641 "src/parser.cpp"
    break;

  case 94:
#line 799 "src/parser.y"
          {
        if (!(yyvsp[0].exprtype)->getType()->isInt() && !(yyvsp[0].exprtype)->getType()->isFloat()) {
            // error
            fprintf(stderr, "rval is invalid.\n");
        }
        (yyval.exprtype) = (yyvsp[0].exprtype);
        if (!stk.empty()) {
            double val = (yyvsp[0].exprtype)->getValue();
            if (val)
                notZeroNum++;
            if (declType->isInt() && (yyvsp[0].exprtype)->getType()->isFloat()) {
                float temp = (float)val;
                int temp1 = (int)temp;
                arrayValue[idx++] = (double)temp1;
            } else {
                arrayValue[idx++] = val;
            }

            Type* arrTy = stk.top()->getSymbolEntry()->getType();
            if (arrTy == TypeSystem::intType || arrTy == TypeSystem::floatType) {
                if ((arrTy->isInt() && (yyvsp[0].exprtype)->getType()->isFloat()) ||
                    (arrTy->isFloat() && (yyvsp[0].exprtype)->getType()->isInt())) {
                    ImplicitCastExpr* temp = new ImplicitCastExpr((yyvsp[0].exprtype), declType);
                    stk.top()->addExpr(temp);
                } else {
                    stk.top()->addExpr((yyvsp[0].exprtype));
                }
            } else {
                while (arrTy) {
                    if (((ArrayType*)arrTy)->getElementType() != TypeSystem::intType &&
                        ((ArrayType*)arrTy)->getElementType() != TypeSystem::floatType) {
                        arrTy = ((ArrayType*)arrTy)->getElementType();
                        SymbolEntry* se = new ConstantSymbolEntry(arrTy);
                        InitValueListExpr* list = new InitValueListExpr(se);
                        stk.top()->addExpr(list);
                        stk.push(list);
                    } else {
                        Type* elemType = ((ArrayType*)arrTy)->getElementType();
                        if ((elemType->isInt() && (yyvsp[0].exprtype)->getType()->isFloat()) ||
                            (elemType->isFloat() && (yyvsp[0].exprtype)->getType()->isInt())) {
                            ImplicitCastExpr* temp = new ImplicitCastExpr((yyvsp[0].exprtype), elemType);
                            stk.top()->addExpr(temp);
                        } else {
                            stk.top()->addExpr((yyvsp[0].exprtype));
                        }
                        while (stk.top()->isFull() && stk.size() != (long unsigned int)leftCnt) {
                            arrTy = ((ArrayType*)arrTy)->getArrayType();
                            stk.pop();
                        }
                        break;
                    }
                }
            }
        }         
    }
#line 2701 "src/parser.cpp"
    break;

  case 95:
#line 854 "src/parser.y"
                    {
        SymbolEntry* se;
        ExprNode* list;
        if (stk.empty()) {
            // 栈为空 则不用加入栈
            memset(arrayValue, 0, arrayType->getSize());
            idx += arrayType->getSize() / declType->getSize();
            se = new ConstantSymbolEntry(arrayType);
            list = new InitValueListExpr(se);
        } else {
            // 栈不空 则需要确定元素个数
            Type* type = ((ArrayType*)(stk.top()->getSymbolEntry()->getType()))->getElementType();
            int len = type->getSize() / declType->getSize();
            memset(arrayValue + idx, 0, type->getSize());
            idx += len;
            se = new ConstantSymbolEntry(type);
            list = new InitValueListExpr(se);
            stk.top()->addExpr(list);
            while (stk.top()->isFull() && stk.size() != (long unsigned int)leftCnt) {
                stk.pop();
            }
        }
        (yyval.exprtype) = list;
    }
#line 2730 "src/parser.cpp"
    break;

  case 96:
#line 878 "src/parser.y"
             {
        SymbolEntry* se;
        if (!stk.empty())
            arrayType = (ArrayType*)(
                ((ArrayType*)(stk.top()->getSymbolEntry()->getType()))->getElementType());
        se = new ConstantSymbolEntry(arrayType);
        if (arrayType->getElementType() != TypeSystem::intType &&
            arrayType->getElementType() != TypeSystem::floatType) {
            arrayType = (ArrayType*)(arrayType->getElementType());
        }
        InitValueListExpr* expr = new InitValueListExpr(se);
        if (!stk.empty())
            stk.top()->addExpr(expr);
        stk.push(expr);
        (yyval.exprtype) = expr;
        leftCnt++;
    }
#line 2752 "src/parser.cpp"
    break;

  case 97:
#line 895 "src/parser.y"
                         {
        leftCnt--;
        while (stk.top() != (yyvsp[-2].exprtype) && stk.size() > (long unsigned int)(leftCnt + 1))
            stk.pop();
        if (stk.top() == (yyvsp[-2].exprtype))
            stk.pop();
        (yyval.exprtype) = (yyvsp[-2].exprtype);
        if (!stk.empty())
            while (stk.top()->isFull() && stk.size() != (long unsigned int)leftCnt) {
                stk.pop();
            }
        int size = ((ArrayType*)((yyval.exprtype)->getSymbolEntry()->getType()))->getSize() / declType->getSize();
        while (idx % size != 0)
            arrayValue[idx++] = 0;
        if (!stk.empty())
            arrayType = (ArrayType*)(
                ((ArrayType*)(stk.top()->getSymbolEntry()->getType()))->getElementType());
    }
#line 2775 "src/parser.cpp"
    break;

  case 98:
#line 917 "src/parser.y"
              {
        (yyval.exprtype) = (yyvsp[0].exprtype);
    }
#line 2783 "src/parser.cpp"
    break;

  case 99:
#line 920 "src/parser.y"
                                {
        (yyval.exprtype) = (yyvsp[-2].exprtype);
    }
#line 2791 "src/parser.cpp"
    break;

  case 100:
#line 925 "src/parser.y"
                   {
        (yyval.exprtype) = (yyvsp[0].exprtype);
    }
#line 2799 "src/parser.cpp"
    break;

  case 101:
#line 928 "src/parser.y"
                                          {
        (yyval.exprtype) = (yyvsp[-2].exprtype);
    }
#line 2807 "src/parser.cpp"
    break;

  case 102:
#line 934 "src/parser.y"
            {
        // SymbolTable::resetLabel();
        identifiers = new SymbolTable(identifiers);
        paramNo = 0;
        fpParamNo = 0;
        stackParamNo = 0;
        funcRetType = (yyvsp[-1].type);
    }
#line 2820 "src/parser.cpp"
    break;

  case 103:
#line 942 "src/parser.y"
                                     {
        Type* funcType;
        std::vector<Type*> vec;
        std::vector<SymbolEntry*> vec1;
        DeclStmt* temp = (DeclStmt*)(yyvsp[-1].stmttype);
        while (temp) {
            vec.push_back(temp->getId()->getSymbolEntry()->getType());
            vec1.push_back(temp->getId()->getSymbolEntry());
            temp = (DeclStmt*)(temp->getNext());
        }
        funcType = new FunctionType((yyvsp[-5].type), vec, vec1);
        SymbolEntry* se = new IdentifierSymbolEntry(
            funcType, (yyvsp[-4].strtype), identifiers->getPrev()->getLevel());
        ((IdentifierSymbolEntry*)se)->setIntParamNo(paramNo);
        ((IdentifierSymbolEntry*)se)->setFloatParamNo(fpParamNo);
        if (!identifiers->getPrev()->install((yyvsp[-4].strtype), se)) {
            fprintf(stderr, "redefinition of \'%s %s\'\n", (yyvsp[-4].strtype), se->getType()->toStr().c_str());
        }
        (yyval.se) = se; 
    }
#line 2845 "src/parser.cpp"
    break;

  case 104:
#line 962 "src/parser.y"
                {
        (yyval.stmttype) = new FunctionDef((yyvsp[-1].se), (DeclStmt*)(yyvsp[-3].stmttype), (yyvsp[0].stmttype));
        SymbolTable* top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete [](yyvsp[-6].strtype);
    }
#line 2857 "src/parser.cpp"
    break;

  case 105:
#line 971 "src/parser.y"
                  { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 2863 "src/parser.cpp"
    break;

  case 106:
#line 972 "src/parser.y"
             { (yyval.stmttype) = nullptr; }
#line 2869 "src/parser.cpp"
    break;

  case 107:
#line 974 "src/parser.y"
                                   {
        (yyval.stmttype) = (yyvsp[-2].stmttype);
        (yyval.stmttype)->setNext((yyvsp[0].stmttype));
    }
#line 2878 "src/parser.cpp"
    break;

  case 108:
#line 978 "src/parser.y"
                 {
        (yyval.stmttype) = (yyvsp[0].stmttype);
    }
#line 2886 "src/parser.cpp"
    break;

  case 109:
#line 984 "src/parser.y"
                        {
        (yyval.exprtype) = new ExprNode(nullptr);
    }
#line 2894 "src/parser.cpp"
    break;

  case 110:
#line 987 "src/parser.y"
                                             {
        (yyval.exprtype) = (yyvsp[-3].exprtype);
        (yyval.exprtype)->setNext((yyvsp[-1].exprtype));
    }
#line 2903 "src/parser.cpp"
    break;


#line 2907 "src/parser.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 991 "src/parser.y"


int yyerror(char const* message)
{
    std::cerr << message << std::endl;
    std::cerr << yytext << std::endl;
    return -1;
}
