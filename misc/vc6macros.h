/**
 * @file	vc6macros.h
 * @brief	VC6 ópÉ}ÉNÉç
 */

#pragma once

#ifndef _countof
//! countof
#define _countof(x)		(sizeof((x)) / sizeof((x)[0]))
#endif	// _countof

// ---- VC5

#if (_MSC_VER < 1200)

#define INT_PTR				INT			//!< INT_PTR
#define UINT_PTR			UINT		//!< UINT_PTR

#endif	// (_MSC_VER < 1200)

// ---- VC6

#if (_MSC_VER < 1300)
//! for scope
#define for					if (0 /*NEVER*/) { /* no process */ } else for

#ifdef _UNICODE
#define	_tstoi	wtoi		//!< text to int
#define _tstof	wtof		//!< text to float
#else	// _UNICODE
#define	_tstoi	atoi		//!< text to int
#define _tstof	atof		//!< text to float
#endif	// _UNICODE

typedef signed char			INT8;		//!< INT8
typedef signed short		INT16;		//!< INT16
// typedef int				INT32;
typedef __int64				INT64;		//!< INT64
typedef unsigned char		UINT8;		//!< UINT8
typedef unsigned short		UINT16;		//!< UINT16
// typedef unsigned int		UINT32;
typedef unsigned __int64	UINT64;		//!< UINT64

#ifndef LONG_PTR
#define LONG_PTR			LONG							/*!< LONG_PTR */
#endif
#ifndef DWORD_PTR
#define DWORD_PTR			DWORD							/*!< DWORD_PTR */
#endif
#ifndef GetWindowLongPtr
#define GetWindowLongPtr	GetWindowLong					/*!< Retrieves information about the specified window */
#endif
#ifndef SetWindowLongPtr
#define SetWindowLongPtr	SetWindowLong					/*!< Changes an attribute of the specified window */
#endif
#ifndef GWLP_WNDPROC
#define GWLP_WNDPROC		GWL_WNDPROC						/*!< Retrieves the pointer to the window procedure */
#endif
#ifndef GWLP_HINSTANCE
#define GWLP_HINSTANCE		GWL_HINSTANCE					/*!< Retrieves a handle to the application instance */
#endif
#ifndef GWLP_HWNDPARENT
#define GWLP_HWNDPARENT		GWL_HWNDPARENT					/*!< Retrieves a handle to the parent window */
#endif
#ifndef GWLP_USERDATA
#define GWLP_USERDATA		GWL_USERDATA					/*!< Retrieves the user data associated with the window */
#endif
#ifndef GWLP_ID
#define GWLP_ID				GWL_ID							/*!< Retrieves the identifier of the window */
#endif

#endif	// (_MSC_VER < 1300)
