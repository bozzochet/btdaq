/* -*-C-*-
 * ams_routine.h - Header file for AMSWire Routine tables
 *
 *-----------------------------------------------------------------------
 *
 *
 *-----------------------------------------------------------------------
 */

#ifndef __ams_routines_h
#define __ams_routines_h

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************
	Definitions
 ****************************************************/

#define AMS_FRAME_SIZE			4080		/* in bytes */
#define CCSDS_HEADER_SIZE		18			/* in bytes */
#define CRC16_TAILER_SIZE		2			/* in bytes */

#define MAX_AMSW_PATH			5
#define MAX_AMSW_SIZE			8192 - 8	/* in bytes */
#define AMSW_HEADER_SIZE		14

#define MAX_CAN_SIZE			2048 - 8	/* in bytes */
#define CAN_HEADER_SIZE			sizeof(AMSCANBlockHeader)

/****************************************************
	typedef and struct
 ****************************************************/

typedef struct __AMSW_ROUTINES {
	int link;
	int npath;
	u_char path[MAX_AMSW_PATH];
} AMSWRoutines;

typedef enum __AMSW_ROUTINE_NUM {
	PATH_NO_JMDC_NEXT = 0,
	PATH_NO_JMDC_OPST = 1,
	PATH_NO_JMDC_PREV = 2,
	PATH_NO_JINJ_0 = 3,
	PATH_NO_JINJ_1 = 4,
	PATH_NO_JINJ_2 = 5,
	PATH_NO_JINJ_3 = 6,
	PATH_NO_JINF_000 = 7,
	PATH_NO_JINF_001 = 8,
	PATH_NO_JINF_002 = 9,
	PATH_NO_JINF_003 = 10,
	PATH_NO_JINF_004 = 11,
	PATH_NO_JINF_005 = 12,
	PATH_NO_JINF_006 = 13,
	PATH_NO_JINF_007 = 14,
	PATH_NO_JINF_008 = 15,
	PATH_NO_JINF_009 = 16,
	PATH_NO_JINF_010 = 17,
	PATH_NO_JINF_011 = 18,
	PATH_NO_JINF_012 = 19,
	PATH_NO_JINF_013 = 20,
	PATH_NO_JINF_014 = 21,
	PATH_NO_JINF_015 = 22,
	PATH_NO_JINF_016 = 23,
	PATH_NO_JINF_017 = 24,
	PATH_NO_JINF_018 = 25,
	PATH_NO_JINF_019 = 26,
	PATH_NO_JINF_020 = 27,
	PATH_NO_JINF_021 = 28,
	PATH_NO_JINF_022 = 29,
	PATH_NO_JINF_023 = 30,
	PATH_NO_JINF_024 = 31,
	PATH_NO_JINF_100 = 32,
	PATH_NO_JINF_101 = 33,
	PATH_NO_JINF_102 = 34,
	PATH_NO_JINF_103 = 35,
	PATH_NO_JINF_104 = 36,
	PATH_NO_JINF_105 = 37,
	PATH_NO_JINF_106 = 38,
	PATH_NO_JINF_107 = 39,
	PATH_NO_JINF_108 = 40,
	PATH_NO_JINF_109 = 41,
	PATH_NO_JINF_110 = 42,
	PATH_NO_JINF_111 = 43,
	PATH_NO_JINF_112 = 44,
	PATH_NO_JINF_113 = 45,
	PATH_NO_JINF_114 = 46,
	PATH_NO_JINF_115 = 47,
	PATH_NO_JINF_116 = 48,
	PATH_NO_JINF_117 = 49,
	PATH_NO_JINF_118 = 50,
	PATH_NO_JINF_119 = 51,
	PATH_NO_JINF_120 = 52,
	PATH_NO_JINF_121 = 53,
	PATH_NO_JINF_122 = 54,
	PATH_NO_JINF_123 = 55,
	PATH_NO_JINF_124 = 56,
	PATH_NO_JINF_200 = 57,
	PATH_NO_JINF_201 = 58,
	PATH_NO_JINF_202 = 59,
	PATH_NO_JINF_203 = 60,
	PATH_NO_JINF_204 = 61,
	PATH_NO_JINF_205 = 62,
	PATH_NO_JINF_206 = 63,
	PATH_NO_JINF_207 = 64,
	PATH_NO_JINF_208 = 65,
	PATH_NO_JINF_209 = 66,
	PATH_NO_JINF_210 = 67,
	PATH_NO_JINF_211 = 68,
	PATH_NO_JINF_212 = 69,
	PATH_NO_JINF_213 = 70,
	PATH_NO_JINF_214 = 71,
	PATH_NO_JINF_215 = 72,
	PATH_NO_JINF_216 = 73,
	PATH_NO_JINF_217 = 74,
	PATH_NO_JINF_218 = 75,
	PATH_NO_JINF_219 = 76,
	PATH_NO_JINF_220 = 77,
	PATH_NO_JINF_221 = 78,
	PATH_NO_JINF_222 = 79,
	PATH_NO_JINF_223 = 80,
	PATH_NO_JINF_224 = 81,
	PATH_NO_JINF_300 = 82,
	PATH_NO_JINF_301 = 83,
	PATH_NO_JINF_302 = 84,
	PATH_NO_JINF_303 = 85,
	PATH_NO_JINF_304 = 86,
	PATH_NO_JINF_305 = 87,
	PATH_NO_JINF_306 = 88,
	PATH_NO_JINF_307 = 89,
	PATH_NO_JINF_308 = 90,
	PATH_NO_JINF_309 = 91,
	PATH_NO_JINF_310 = 92,
	PATH_NO_JINF_311 = 93,
	PATH_NO_JINF_312 = 94,
	PATH_NO_JINF_313 = 95,
	PATH_NO_JINF_314 = 96,
	PATH_NO_JINF_315 = 97,
	PATH_NO_JINF_316 = 98,
	PATH_NO_JINF_317 = 99,
	PATH_NO_JINF_318 = 100,
	PATH_NO_JINF_319 = 101,
	PATH_NO_JINF_320 = 102,
	PATH_NO_JINF_321 = 103,
	PATH_NO_JINF_322 = 104,
	PATH_NO_JINF_323 = 105,
	PATH_NO_JINF_324 = 106,
	AMSW_PATH_ALL = 107,
} AMSWPathNo;

/****************************************************
	End
 ****************************************************/
#ifdef __cplusplus
}
#endif

#endif /* __ams_routines_h */
