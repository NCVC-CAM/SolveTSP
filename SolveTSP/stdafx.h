// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���A
//            �܂��͎Q�Ɖ񐔂������A�����܂�ύX����Ȃ�
//            �v���W�F�N�g��p�̃C���N���[�h �t�@�C�����L�q���܂��B
//

#if !defined(AFX_STDAFX_H__5F22FDA2_3140_4D94_9CBD_51867921B919__INCLUDED_)
#define AFX_STDAFX_H__5F22FDA2_3140_4D94_9CBD_51867921B919__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <SDKDDKVer.h>	// ���ꂾ��XP�� SystemParametersInfo() �̒l���������Ȃ�
#define	NO_WARN_MBCS_MFC_DEPRECATION	// VS2015�`
#define _WIN32_WINNT 0x0501				// Windows XP �`
#include <WinSDKVer.h>

#define VC_EXTRALEAN		// Windows �w�b�_�[����w�ǎg�p����Ȃ��X�^�b�t�����O���܂��B

#include <afxwin.h>         // MFC �̃R�A����ѕW���R���|�[�l���g
#include <afxext.h>         // MFC �̊g������
#include <afxtempl.h>		// CMap ����ڰ�

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC �� OLE �N���X
#include <afxodlgs.h>       // MFC �� OLE �_�C�A���O �N���X
#include <afxdisp.h>        // MFC �̃I�[�g���[�V���� �N���X
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC �f�[�^�x�[�X �N���X
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO �f�[�^�x�[�X �N���X
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC �� Internet Explorer 4 �R���� �R���g���[�� �T�|�[�g
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC �� Windows �R���� �R���g���[�� �T�|�[�g
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "3Dto2D.h"
#include "CustomClass.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_STDAFX_H__5F22FDA2_3140_4D94_9CBD_51867921B919__INCLUDED_)