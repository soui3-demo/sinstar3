/*!@file  	mem_dc.h
* @brief   	内存DC封装类
* @version 	1.0
* @author 	Baidu Type Group
* @date 	2008-10-08
*/
#pragma once


/*! @class 	CMemDC
* @version	1.0			
* @brief  	内存DC封装类
* @author	Baidu Type Group
* @date		2008-10-08
* @par		修改记录
* @par
* @par:	
*/
class CMemDC
{
public:
	/*! @fn CMemDC::CMemDC()
	* @brief 		内存DC封装类的默认构造函数
	* @return 		void
	* @author 		Baidu Type Group
	* @date 		2008-10-08
	*/
	CMemDC();

	/*! @fn CMemDC::CMemDC(HBITMAP hBitmap, HDC hdc = 0)
	 * @brief 		内存DC封装类的构造函数
	 * @param[in]	HBITMAP hBitmap 位图
	 * @param[in]	HDC hdc = 0 DC
	 * @return 		void
	 * @author 		Baidu Type Group
	 * @date 		2008-10-08
	*/
	explicit CMemDC(HBITMAP hBitmap, HDC hdc = 0);

	/*! @fn CMemDC::CMemDC(int nWidth, int nHeight, int nBitsPerPixel = 24, HDC hdc = 0)
	* @brief 		内存DC封装类的构造函数
	* @param[in]	int nWidth
	* @param[in]	int nHeight
	* @param[in]	int nBitsPerPixel = 24
	* @param[in]	HDC hdc = 0
	* @return 		void
	* @author 		Baidu Type Group
	* @date 		2008-10-08
	*/
	CMemDC(int nWidth, int nHeight, int nBitsPerPixel = 24, HDC hdc = 0);

	/*! @fn HDC Create(HBITMAP hBitmap, HDC hdc = 0)
	* @brief 		创建DC，完成初始化，该函数被构造函数调用
	* @param[in]	HBITMAP hBitmap 位图
	* @param[in]	HDC hdc = 0 DC
	* @return 		HDC DC
	* @author 		Baidu Type Group
	* @date 		2008-10-08
	*/
	HDC Create(HBITMAP hBitmap, HDC hdc = 0);

	/*! @fn HDC Create(int nWidth, int nHeight, int nBitsPerPixel = 24, HDC hdc = 0)
	* @brief 		创建DC，完成初始化，该函数被构造函数调用
	* @param[in]	int nWidth
	* @param[in]	int nHeight
	* @param[in]	int nBitsPerPixel = 24
	* @param[in]	HDC hdc = 0
	* @return 		void
	* @author 		Baidu Type Group
	* @date 		2008-10-08
	*/
	HDC Create(int nWidth, int nHeight, int nBitsPerPixel = 24, HDC hdc = 0);

	/*! @fn void CMemDC::DeleteDC()
	* @brief 		删除DC函数
	* @return 		void
	* @author 		Baidu Type Group
	* @date 		2008-10-08
	*/
	void DeleteDC();

	/*! @fn void CMemDC::Detach()
	* @brief 		分离函数
	* @return 		void
	* @author 		Baidu Type Group
	* @date 		2008-10-08
	*/
	HBITMAP Detach();

	/*! @fn operator CMemDC::HDC() const
	* @brief 		内存DC封装类的操作符函数
	* @return 		HDC 
	* @author 		Baidu Type Group
	* @date 		2008-10-08
	*/
	operator HDC () const;

	/*! @fn CMemDC::~CMemDC()
	* @brief 		内存DC封装类的析构函数
	* @return 		void
	* @author 		Baidu Type Group
	* @date 		2008-10-08
	*/
	~CMemDC();

	/*! @fn void* CMemDC::GetBits() const
	* @brief 		获取指向DIB[m_hBitmap]位数据值的指针
	* @return 		void * 返回[m_pBits]
	* @author 		Baidu Type Group
	* @date 		2008-10-08
	*/
	void* GetBits() const;

public:
	/*!<位图>*/
	HBITMAP m_hBitmap; // m_hBitmap can be accessed
	
	/*!<位图>*/
	HDC m_hDC;

private:
	/*!<在Create时保存BITMAP，以实现在DeleteDC或Detach时恢复>*/
	HBITMAP m_hOldBitmap;
	
	/*!<在CreateDIBSection时被赋值，接收一个指向DIB位数据值的指针>*/
	void* m_pBits;
};
