#ifndef COMMON
#define COMMON


#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <random>
#include <math.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <complex>
#include <vector>
using namespace std;  
//#define DIFFTEST 

//是否发送全0数据
#define Send_All_Zero 0
//定义数据类型
//double FixedPoint
#define DataType double
#define PI 3.14159265358979323846
//定义系统子载波数
const int Num_subcarriers = 52;
//定义数据子载波数
const int Num_data_subcarriers = 48;
//定义导频子载波数
const int Num_pilot_subcarriers = Num_subcarriers - Num_data_subcarriers;
//定义导频位置 根据802.11a标准 当然也可以自己定义
// 具体对性能的影响现在还不明白
/*
52个子载波标号为-26,-25,...,-1,1,...,25,26
4个导频位置分别为-21,-7,7,21

对于给ifft的映射
然后1到26映射到1到26
-26到-1映射到38到63
所以导频位置为-21 映射到43
-7 映射到57
7 映射到7
21 映射到21
*/
const int Pilot_Position[Num_pilot_subcarriers] = {43, 57, 7, 21 };
//定义导频间隔 该参数unused
const int Pilot_Interval = 4;
//定义导频频率 即导频的实部
const int Pilot_Frequency = 1;
//定义FFT点数
const int Num_FFT = 64;
//定义循环前缀CP长度
const int Num_CP = 16;
//定义OFDM符号长度
const int Num_OFDM = Num_FFT + Num_CP;
//定义调制类型
/*
0 bpsk 
1 qpsk
2 16qam
3 64qam
*/
#define Modulation_Type 2
//定义一个OFDM符号中一个数据对应的的数据数
#if Modulation_Type == 0
const int Num_data_in_OFDM_Symbol = 1;
const DataType KMod = 1/sqrt(2);
#elif Modulation_Type == 1
const int Num_data_in_OFDM_Symbol = 2;
const DataType KMod = 1/sqrt(2);
#elif Modulation_Type == 2
const int Num_data_in_OFDM_Symbol = 4;
const DataType KMod = 1/sqrt(10);
#elif Modulation_Type == 3
const int Num_data_in_OFDM_Symbol = 6;
const DataType KMod = 1/sqrt(42);
#endif
//定义一个OFDM符号对应的数据个数
const int Num_data_in_OFDM = Num_data_in_OFDM_Symbol * Num_data_subcarriers;

//每帧包含的OFDM符号数
const int Num_OFDM_Symbols = 14;


//训练序列长度
const int Num_Train_Symbols = 320;
//短训练序列周期
const int Short_Train_Symbols = 16;
//长训练序列长度
const int Num_Long_Train_Symbols = 160;
//窗口能量检测阈值
const DataType Energy_Detect_Threshold = 0.1;
//窗口能量比值检测阈值
const DataType Energy_Ratio_Threshold = 0.5;
//符号同步时的幅度阈值门限
const DataType Symbol_Sync_Threshold = 1.8;


//长训练序列

const vector<complex<DataType>> LongTrainSeq = {
	0,0,0,0,
	0,0,1,1,
	-1,-1,1,1,
	-1,1,-1,1,
	1,1,1,1,
	1,-1,-1,1,
	1,-1,1,-1,
	1,1,1,1,
	0,1,-1,-1,
	1,1,-1,1,
	-1,1,-1,-1,
	-1,-1,-1,1,
	1,-1,-1,1,
	-1,1,-1,1,
	1,1,1,0,
	0,0,0,0};







# define __PRI64_PREFIX	"l"
# define PRIx8		"x"
# define PRIx16		"x"
# define PRIx32		"x"
# define PRIx64		__PRI64_PREFIX "x"
#define FMT_WORD    "0x%016lx"

#define ANSI_FG_BLACK   "\33[1;30m"
#define ANSI_FG_RED     "\33[1;31m"
#define ANSI_FG_GREEN   "\33[1;32m"
#define ANSI_FG_YELLOW  "\33[1;33m"
#define ANSI_FG_BLUE    "\33[1;34m"
#define ANSI_FG_MAGENTA "\33[1;35m"
#define ANSI_FG_CYAN    "\33[1;36m"
#define ANSI_FG_WHITE   "\33[1;37m"
#define ANSI_BG_BLACK   "\33[1;40m"
#define ANSI_BG_RED     "\33[1;41m"
#define ANSI_BG_GREEN   "\33[1;42m"
#define ANSI_BG_YELLOW  "\33[1;43m"
#define ANSI_BG_BLUE    "\33[1;44m"
#define ANSI_BG_MAGENTA "\33[1;35m"
#define ANSI_BG_CYAN    "\33[1;46m"
#define ANSI_BG_WHITE   "\33[1;47m"
#define ANSI_NONE       "\33[0m"

#define ANSI_FMT(str, fmt) fmt str ANSI_NONE

#include <string.h>

//// macro stringizing
//#define str_temp(x) #x
//#define str(x) str_temp(x)

// strlen() for string constant
#define STRLEN(CONST_STR) (sizeof(CONST_STR) - 1)

// calculate the length of an array
#define ARRLEN(arr) (int)(sizeof(arr) / sizeof(arr[0]))

// macro concatenation
#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)
#define concat3(x, y, z) concat(concat(x, y), z)
#define concat4(x, y, z, w) concat3(concat(x, y), z, w)
#define concat5(x, y, z, v, w) concat4(concat(x, y), z, v, w)

// macro testing
// See https://stackoverflow.com/questions/26099745/test-if-preprocessor-symbol-is-defined-inside-macro
#define CHOOSE2nd(a, b, ...) b
#define MUX_WITH_COMMA(contain_comma, a, b) CHOOSE2nd(contain_comma a, b)
#define MUX_MACRO_PROPERTY(p, macro, a, b) MUX_WITH_COMMA(concat(p, macro), a, b)
// define placeholders for some property
#define __P_DEF_0  X,
#define __P_DEF_1  X,
#define __P_ONE_1  X,
#define __P_ZERO_0 X,
// define some selection functions based on the properties of BOOLEAN macro
#define MUXDEF(macro, X, Y)  MUX_MACRO_PROPERTY(__P_DEF_, macro, X, Y)
#define MUXNDEF(macro, X, Y) MUX_MACRO_PROPERTY(__P_DEF_, macro, Y, X)
#define MUXONE(macro, X, Y)  MUX_MACRO_PROPERTY(__P_ONE_, macro, X, Y)
#define MUXZERO(macro, X, Y) MUX_MACRO_PROPERTY(__P_ZERO_,macro, X, Y)

// test if a boolean macro is defined
#define ISDEF(macro) MUXDEF(macro, 1, 0)
// test if a boolean macro is undefined
#define ISNDEF(macro) MUXNDEF(macro, 1, 0)
// test if a boolean macro is defined to 1
#define ISONE(macro) MUXONE(macro, 1, 0)
// test if a boolean macro is defined to 0
#define ISZERO(macro) MUXZERO(macro, 1, 0)
// test if a macro of ANY type is defined
// NOTE1: it ONLY works inside a function, since it calls `strcmp()`
// NOTE2: macros defined to themselves (#define A A) will get wrong results
#define isdef(macro) (strcmp("" #macro, "" str(macro)) != 0)

// simplification for conditional compilation
#define __IGNORE(...)
#define __KEEP(...) __VA_ARGS__
// keep the code if a boolean macro is defined
#define IFDEF(macro, ...) MUXDEF(macro, __KEEP, __IGNORE)(__VA_ARGS__)
// keep the code if a boolean macro is undefined
#define IFNDEF(macro, ...) MUXNDEF(macro, __KEEP, __IGNORE)(__VA_ARGS__)
// keep the code if a boolean macro is defined to 1
#define IFONE(macro, ...) MUXONE(macro, __KEEP, __IGNORE)(__VA_ARGS__)
// keep the code if a boolean macro is defined to 0
#define IFZERO(macro, ...) MUXZERO(macro, __KEEP, __IGNORE)(__VA_ARGS__)

// functional-programming-like macro (X-macro)
// apply the function `f` to each element in the container `c`
// NOTE1: `c` should be defined as a list like:
//   f(a0) f(a1) f(a2) ...
// NOTE2: each element in the container can be a tuple
#define MAP(c, f) c(f)

#define BITMASK(bits) ((1ull << (bits)) - 1)
#define BITS(x, hi, lo) (((x) >> (lo)) & BITMASK((hi) - (lo) + 1)) // similar to x[hi:lo] in verilog
#define SEXT(x, len) ({ struct { int64_t n : len; } __x = { .n = x }; (uint64_t)__x.n; })
#define SEXTU(x, len) ({ struct { uint64_t n : len; } __x = { .n = x }; (uint64_t)__x.n; })

#define ROUNDUP(a, sz)   ((((uintptr_t)a) + (sz) - 1) & ~((sz) - 1))
#define ROUNDDOWN(a, sz) ((((uintptr_t)a)) & ~((sz) - 1))

#define PG_ALIGN __attribute((aligned(4096)))

#if !defined(likely)
#define likely(cond)   __builtin_expect(cond, 1)
#define unlikely(cond) __builtin_expect(cond, 0)
#endif

// for AM IOE
#define io_read(reg) \
  ({ reg##_T __io_param; \
    ioe_read(reg, &__io_param); \
    __io_param; })

#define io_write(reg, ...) \
  ({ reg##_T __io_param = (reg##_T) { __VA_ARGS__ }; \
    ioe_write(reg, &__io_param); })



#define Assert(cond, format, ...) \
  do { \
    if (!(cond)) { \
        (fflush(stdout), fprintf(stderr, ANSI_FMT(format, ANSI_FG_RED) "\n", ##  __VA_ARGS__)); \
           extern FILE* log_fp; fflush(log_fp); \
      extern void assert_fail_msg(); \
      assert_fail_msg(); \
      assert(cond); \
    } \
  } while (0)

#define panic(format, ...) Assert(0, format, ## __VA_ARGS__)

#define panic_on(cond, s) \
  ({ if (cond) { \
      putstr("AM Panic: "); putstr(s); \
      putstr(" @ " __FILE__ ":" TOSTRING(__LINE__) "  \n"); \
      halt(1); \
    } })
//#define panic(s) panic_on(1, s)
#define log_write(...) IFDEF(CONFIG_TARGET_NATIVE_ELF, \
  do { \
    extern FILE* log_fp; \
    extern bool log_enable(); \
    if (log_enable()) { \
      fprintf(log_fp, __VA_ARGS__); \
      fflush(log_fp); \
    } \
  } while (0) \
)

#define _Log(...) \
  do { \
    printf(__VA_ARGS__); \
    log_write(__VA_ARGS__); \
  } while (0)


#define Log(format, ...) \
    _Log(ANSI_FMT("[%s:%d %s] " format, ANSI_FG_BLUE) "\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)
#define LogG(format, ...) \
    _Log(ANSI_FMT("[%s:%d %s] " format, ANSI_FG_GREEN) "\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)



#endif
