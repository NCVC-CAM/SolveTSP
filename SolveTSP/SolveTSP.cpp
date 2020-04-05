// SolveTSP.cpp : DLL 用の初期化処理の定義を行います。
//

#include "stdafx.h"
#include <afxdllx.h>
#include "NCVCaddin.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static AFX_EXTENSION_MODULE SolveTSPDLL = { NULL, NULL };

// 座標構造体
typedef	struct	tagTSPPT {
	CPointD		m_pt;
	BOOL		m_OriginFlag;
	BOOL		m_CheckedFlag;
} TSPPT, *LPTSPPT;

// 基本座標配列
static	CSortArray<CPtrArray, LPTSPPT>	g_arPoint,	// 座標構造体を格納する配列（メイン）
										g_arTmp,	// 座標構造体を格納する配列（一時）
										g_arBest;	// 座標構造体を格納する配列（そのときベストな）

// ｺｽﾄ計算関数
static double	GetCost(int);
// 配列初期化
static void		ClearArray(void);

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// lpReserved を使う場合はここを削除してください
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("SOLVETSP.DLL Initializing!\n");
		
		// 拡張 DLL を１回だけ初期化します。
		if (!AfxInitExtensionModule(SolveTSPDLL, hInstance))
			return 0;

		// この DLL をリソース チェインへ挿入します。
		// メモ: 拡張 DLL が MFC アプリケーションではなく
		//   MFC 標準 DLL (ActiveX コントロールのような)
		//   に暗黙的にリンクされる場合、この行を DllMain
		//   から削除して、この拡張 DLL からエクスポート
		//   された別の関数内へ追加してください。  
		//   この拡張 DLL を使用する標準 DLL はこの拡張 DLL
		//   を初期化するために明示的にその関数を呼び出します。 
		//   それ以外の場合は、CDynLinkLibrary オブジェクトは
		//   標準 DLL のリソース チェインへアタッチされず、
		//   その結果重大な問題となります。

		new CDynLinkLibrary(SolveTSPDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("SOLVETSP.DLL Terminating!\n");
		// デストラクタが呼び出される前にライブラリを終了します
		AfxTermExtensionModule(SolveTSPDLL);
	}
	return 1;   // ok
}


/////////////////////////////////////////////////////////////////////////////
// NCVC ｱﾄﾞｲﾝ関数

NCADDIN BOOL NCVC_Initialize(NCVCINITIALIZE* nci)
{
	// ｱﾄﾞｲﾝの必要情報
	nci->dwSize = sizeof(NCVCINITIALIZE);
	nci->dwType = NCVCADIN_FLG_DXFEDIT;
//	nci->nToolBar = 0;
	nci->lpszMenuName[NCVCADIN_ARY_DXFEDIT] = "穴加工の最適化...";
	nci->lpszFuncName[NCVCADIN_ARY_DXFEDIT] = "SolveTSP";
	nci->lpszAddinName	= "SolveTSP";
	nci->lpszCopyright	= "MNCT Yoshiro Nose";
	nci->lpszSupport	= "http://s-gikan2.maizuru-ct.ac.jp/";

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// NCVC ｲﾍﾞﾝﾄ関数

NCADDIN void SolveTSP(void)
{
	DXFDATA		dxfData;

	// 現在アクティブなドキュメントハンドルを取得
	NCVCHANDLE	hDoc = NCVC_GetDocument(NULL);

	// 各種情報
	char	szBuf[256];
	CString	strTmp;
	NCVC_GetDocumentFileName(hDoc, szBuf, sizeof(szBuf));

	LPTSPPT		lpTsp;		// 構造体へのﾎﾟｲﾝﾀ
	int	nLayerCnt = NCVC_GetDXFLayerSize(hDoc), nDataCnt, i, j, k;
	// 配列サイズ設定
	ClearArray();
	g_arPoint.SetSize(0, 1024);
	g_arTmp.SetSize(0, 1024);
	g_arBest.SetSize(0, 1024);

	// NCVC本体からCADデータの取得
	for ( i=0; i<nLayerCnt; i++ ) {
		NCVC_GetDXFLayerData(hDoc, i, szBuf, sizeof(szBuf));
		nDataCnt = NCVC_GetDXFDataSize(hDoc, szBuf);

		strTmp=szBuf;
		if(strTmp == "TSP_MOVE"){
			NCVC_DelDXFData(hDoc, szBuf, 0, nDataCnt);
			continue;
		}

		for ( j=0; j<nDataCnt; j++ ) {
			dxfData.dwSize = sizeof(DXFDATA);
			lstrcpy(dxfData.szLayer, szBuf);
			NCVC_GetDXFData(hDoc, j, &dxfData);
			if ( dxfData.enType != DXFPOINTDATA )
				continue;
			// 点データを自前の配列に追加
			lpTsp = new TSPPT;
			lpTsp->m_pt.x = dxfData.ptS.x;
			lpTsp->m_pt.y = dxfData.ptS.y;
			lpTsp->m_OriginFlag = 0;		// ﾌﾗｸﾞの初期化
			lpTsp->m_CheckedFlag = 0;		// ﾌﾗｸﾞの初期化
			g_arPoint.Add( lpTsp );
			g_arTmp.Add( lpTsp );
			g_arBest.Add( lpTsp );
		}
	}
	// 切削原点も配列に追加
	LPDPOINT	lpptOrg = NULL;
	lpptOrg = new DPOINT;
	NCVC_GetDXFCutterOrigin(hDoc, lpptOrg);
	lpTsp = new TSPPT;
	lpTsp->m_pt.x = lpptOrg->x;
	lpTsp->m_pt.y = lpptOrg->y;
	lpTsp->m_OriginFlag = 1;	// 原点データにチェック
	lpTsp->m_CheckedFlag = 0;	// ﾌﾗｸﾞの初期化
	g_arPoint.Add( lpTsp );
	g_arTmp.Add( lpTsp );
	g_arBest.Add( lpTsp );


	//////////////////////////////////////////////
	// ＴＳＰ最適化手法による加工順の並べ替え
	//////////////////////////////////////////////

	int nNumOfData = g_arPoint.GetSize();				// 切削原点含めたﾃﾞｰﾀ数
	double nInitCost = 0, nTotalCost = 0, nTmpCost = 0; // 初期状態のｺｽﾄ，最適化後のｺｽﾄ，計算中のｺｽﾄ

	// 初期状態のｺｽﾄ（配列順にｺｽﾄ足していく）
	nInitCost = GetCost(nNumOfData);
#ifdef _DEBUG
	strTmp.Format("nNumOfData（ﾃﾞｰﾀ数） = %d\nnInitCost（配列順に足したｺｽﾄ） = %f", nNumOfData, nInitCost);
	AfxMessageBox(strTmp);
#endif

//	for ( i=0; i<=nNumOfData; i++){
//		strTmp.Format("(x, y) = (%f, %f)", g_arBest[i]->m_pt.x, g_arBest[i]->m_pt.y);
//		AfxMessageBox(strTmp);
//	}

	////////////////////////////////////////////// ﾗﾝﾀﾞﾑｻｰﾁ（しきい値はﾃﾞﾌｫﾙﾄの値+1）
	CPointD	pt;
	int nRand1, nRand2;

	// 経路をﾗﾝﾀﾞﾑにしてｺｽﾄ計算
	for (j=0, nTotalCost=nInitCost; nTotalCost>=nInitCost && j<10000; j++){
		srand( (unsigned)time( NULL ) );	// 乱数ｼﾞｪﾈﾚｰﾀの初期化（時間で）
		for ( i=0; i<nNumOfData; i++){
			nRand1 = rand() % nNumOfData;	// 乱数の発生
			nRand2 = rand() % nNumOfData;
			ASSERT( nRand1>=0 && nRand1<g_arBest.GetSize() );
			ASSERT( nRand2>=0 && nRand2<g_arBest.GetSize() );
			// nRand1とnRand2の入れ替え
			lpTsp = g_arBest.GetAt(nRand1);					// nRand1の値を退避しておいて
			g_arBest.SetAt(nRand1, g_arBest.GetAt(nRand2));	// nRand1にnRand2を入れる
			g_arBest.SetAt(nRand2, lpTsp);					// nRand2に退避した値を代入
		}
		// ﾗﾝﾀﾞﾑｻｰﾁのｺｽﾄ計算
		nTotalCost = GetCost(nNumOfData);
	}

#ifdef _DEBUG
	strTmp.Format("nTotalCost（ﾗﾝﾀﾞﾑｻｰﾁのｺｽﾄ） = %f", nTotalCost);
	AfxMessageBox(strTmp);
#endif

	//////////////////////////////////////////////  山登り法（ﾗﾝﾀﾞﾑｻｰﾁの結果を初期値に）
/*	for( g_nBestTotalCost; 1; nInitCost = g_nBestTotalCost){
		// ２点を入れ替えた経路を作成
		for ( i=0; i<nNumOfData-2; i++){
			for ( int j=i+1; j<nNumOfData-1; j++){
				g_arTmp.Copy(g_arBest);					// g_arTmpにg_arBestをｺﾋﾟｰ
				lpTsp = g_arTmp.GetAt(j);				// jの値を退避しておいて
				g_arTmp.SetAt(j, g_arTmp.GetAt(i));		// jにiを入れる
				g_arTmp.SetAt(i, lpTsp);				// iに退避した値を代入
				// ｺｽﾄの計算
				nTmpCost = 0;
				for ( i=0; i<nNumOfData-1; i++){
					pt = g_arTmp[i+1]->m_pt - g_arTmp[i]->m_pt;
					nTmpCost += GAPCALC(pt);
				}
				// 優秀な経路を記録
				if ( nTmpCost < g_nBestTotalCost ){
					g_arPoint.Copy(g_arTmp);			// g_arPointにg_arTmpをコピー
					g_nBestTotalCost = nTmpCost;
				}
			}
		}
		// もっとも優秀な経路を次の主人公に
		if ( g_nBestTotalCost < nInitCost ){
			g_arBest.Copy(g_arPoint);	// g_arBestにg_arPointをコピー
		}else{
			break;						// 優秀な経路がなければ終了
		}
	}
	strTmp.Format("g_nBestTotalCost（山登り法の最適解） = %f", g_nBestTotalCost);
	AfxMessageBox(strTmp);
*/
	//////////////////////////////////////////////  2-opt算法（ﾗﾝﾀﾞﾑｻｰﾁの結果を初期値に）

	int i0 = 0;
	double nDifCost = 0;
	CPointD	pt1, pt2, pt3, pt4, pt5, pt6;

	for (i=0; i<i0+nNumOfData; i++){
		for (j=i+2; j<i+nNumOfData-1; j++){
			pt1 =  g_arBest[i % nNumOfData]->m_pt - g_arBest[j % nNumOfData]->m_pt;
			pt2 =  g_arBest[(i+1) % nNumOfData]->m_pt - g_arBest[(j+1) % nNumOfData]->m_pt;
			pt3 =  g_arBest[i % nNumOfData]->m_pt - g_arBest[(i+1) % nNumOfData]->m_pt;
			pt4 =  g_arBest[j % nNumOfData]->m_pt - g_arBest[(j+1) % nNumOfData]->m_pt;
			nDifCost = GAPCALC(pt1) + GAPCALC(pt2) - GAPCALC(pt3) - GAPCALC(pt4);	// 距離の増分
			if (nDifCost < -0.0001){		// 改良解の発見
				for (k=0; k<(j-i)/2; k++){
					lpTsp = g_arBest.GetAt((j-k) % nNumOfData);									// j-k の値を退避しておいて
					g_arBest.SetAt((j-k) % nNumOfData, g_arBest.GetAt((i+1+k) % nNumOfData));	// j-k に i+1+k を入れる
					g_arBest.SetAt((i+1+k) % nNumOfData, lpTsp);								// i+1+k に退避した値を代入
				}
				i0 = i + 1; 
				break;
			}
		}
	}

#ifdef _DEBUG
	nTotalCost = GetCost(nNumOfData);
	strTmp.Format("nTotalCost（2-opt算法の最適解） = %f", nTotalCost);
	AfxMessageBox(strTmp);
#endif

/*	//////////////////////////////////////////////  Or-opt算法（2-opt算法の結果を初期値に）
	int h;
	i0 = 0;
	for (i=i0; i<i0+nNumOfData; i++){
//	for (i=0; i<i0+nNumOfData; i++){
		for (k=i+1; k<=i+3; k++){
			for (j=k+1; j<i+nNumOfData-1; j++){
				pt1 =  g_arBest[i % nNumOfData]->m_pt - g_arBest[(i+1) % nNumOfData]->m_pt;
				pt2 =  g_arBest[j % nNumOfData]->m_pt - g_arBest[(j+1) % nNumOfData]->m_pt;
				pt3 =  g_arBest[k % nNumOfData]->m_pt - g_arBest[(k+1) % nNumOfData]->m_pt;
				pt4 =  g_arBest[i % nNumOfData]->m_pt - g_arBest[(k+1) % nNumOfData]->m_pt;
				pt5 =  g_arBest[j % nNumOfData]->m_pt - g_arBest[k % nNumOfData]->m_pt;
				pt6 =  g_arBest[(i+1) % nNumOfData]->m_pt - g_arBest[(j+1) % nNumOfData]->m_pt;
				nDifCost = GAPCALC(pt1) + GAPCALC(pt2) + GAPCALC(pt3) - GAPCALC(pt4) - GAPCALC(pt5) - GAPCALC(pt6);	// 距離の増分
				if (nDifCost < -0.0001){
					for (h=i+1; h<=k; h++) g_arTmp.SetAt((h-i-1), g_arBest.GetAt(h % nNumOfData));
					for (h=k+1; h<=j; h++) g_arBest.SetAt((h-k+i) % nNumOfData, g_arBest.GetAt(h % nNumOfData));
					for (h=0; h<k-i; h++) g_arBest.SetAt((j-k+i+1+h) % nNumOfData, g_arTmp.GetAt(k-i-1-h));
					i0 = i + 1;
					break;
				}
				if (k == i+1) continue;
				pt4 =  g_arBest[i % nNumOfData]->m_pt - g_arBest[(k+1) % nNumOfData]->m_pt;
				pt5 =  g_arBest[j % nNumOfData]->m_pt - g_arBest[(i+1) % nNumOfData]->m_pt;
				pt6 =  g_arBest[k % nNumOfData]->m_pt - g_arBest[(j+1) % nNumOfData]->m_pt;
				nDifCost = GAPCALC(pt1) + GAPCALC(pt2) + GAPCALC(pt3) - GAPCALC(pt4) - GAPCALC(pt5) - GAPCALC(pt6);	// 距離の増分
				if (nDifCost < -0.0001){
					for (h=i+1; h<=k; h++) g_arTmp.SetAt((h-i-1), g_arBest.GetAt(h % nNumOfData));
					for (h=k+1; h<=j; h++) g_arBest.SetAt((h-k+i) % nNumOfData, g_arBest.GetAt(h % nNumOfData));
					for (h=0; h<k-i; h++) g_arBest.SetAt((j-k) % nNumOfData, g_arTmp.GetAt(k-i-1-h));
					i0 = i + 1;
					break;
				}
			}
//			if (nDifCost < -0.0001){
//				break;
//			}
		}
	}
	
	nTotalCost = GetCost(nNumOfData);
	strTmp.Format("nTotalCost（Or-opt算法の最適解） = %f", nTotalCost);
	AfxMessageBox(strTmp);
*/

	// それぞれを移動ﾚｲﾔでつながるようにﾃﾞｰﾀを登録
	dxfData.dwSize = sizeof(DXFDATA);
	dxfData.ptS.x = g_arBest[0]->m_pt.x;
	dxfData.ptS.y = g_arBest[0]->m_pt.y;

	// ２個目以降を線の移動データとして登録
	nDataCnt = g_arBest.GetSize();
	dxfData.enType = DXFLINEDATA;
	lstrcpy(dxfData.szLayer, "TSP_MOVE");
	dxfData.nLayer = DXFMOVLAYER;

	for ( i=1; i<nDataCnt; i++ ) {
		dxfData.de.ptE.x = g_arBest[i]->m_pt.x;
		dxfData.de.ptE.y = g_arBest[i]->m_pt.y;
		// 移動レイヤの線データ追加
		NCVC_AddDXFData(hDoc, &dxfData);
		// 今の終点は次の始点
		dxfData.ptS.x = dxfData.de.ptE.x;
		dxfData.ptS.y = dxfData.de.ptE.y;
	}

	// 巡回路になるように
	dxfData.de.ptE.x = g_arBest[0]->m_pt.x;
	dxfData.de.ptE.y = g_arBest[0]->m_pt.y;
	NCVC_AddDXFData(hDoc, &dxfData);

	// ﾒﾓﾘ領域解放
	ClearArray();
	if ( lpptOrg )
		delete lpptOrg;

	// 本体の再描画
	NCVC_ReDraw(hDoc);
}

// ｺｽﾄ計算関数
double GetCost(int nNumOfData)
{
	int			i;
	CPointD		pt;
	double		nTotalCost = 0;

	for ( i=0; i<nNumOfData-1; i++){
		pt = g_arBest[i+1]->m_pt - g_arBest[i]->m_pt;
		nTotalCost += GAPCALC(pt);
	}
	// 終点から原点までのｺｽﾄも追加
	pt = g_arBest[0]->m_pt - g_arBest[nNumOfData-1]->m_pt;
	nTotalCost += GAPCALC(pt);

	return nTotalCost;
}
//
void ClearArray(void)
{
	for ( int i=0; i<g_arBest.GetSize(); i++ ) {
		delete	g_arBest[i];
	}
	g_arPoint.RemoveAll();
	g_arTmp.RemoveAll();
	g_arBest.RemoveAll();
}
