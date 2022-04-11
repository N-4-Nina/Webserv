#pragma once

	/* METHODS */
#define GET				1
#define	POST			2
#define DELETE			4


	/* REQUEST */
#define PARSED_TOP		1
#define PARSED_HEADERS	2
#define PARSED_CL		4
#define PARSED_HNL		8
#define PARSED_BNL		16
#define PARSED_ISMULTI	32
#define PARSED_ERROR	64


	/*	RESPONSE */
#define RES_LOCATED 	1
#define RES_ISCGI		2
#define RES_ISINDEX 	4
#define RES_INDEXDEF	8
#define RES_DEFCGI		16
#define RES_STARTED		32
#define RES_READY		64
#define RES_CLOSE		128