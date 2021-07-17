// compiler.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���̃C���N���[�h �t�@�C���A�܂���
// �Q�Ɖ񐔂������A�����܂�ύX����Ȃ��A�v���W�F�N�g��p�̃C���N���[�h �t�@�C��
// ���L�q���܂��B
//

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>

typedef signed int			SINT;
typedef signed char			SINT8;
typedef unsigned char		UINT8;
typedef signed short		SINT16;
typedef unsigned short		UINT16;
typedef signed int			SINT32;
typedef unsigned int		UINT32;
typedef signed __int64		SINT64;
typedef unsigned __int64	UINT64;

// TODO: �v���O�����ɕK�v�Ȓǉ��w�b�_�[�������ŎQ�Ƃ��Ă��������B
#include "misc\vc6macros.h"
typedef signed __int64		int64_t;
typedef unsigned __int64	uint64_t;

#define BRESULT				BOOL
#define OEMCHAR				TCHAR
