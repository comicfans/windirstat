// selectobject.h - Declaration and implementation of Device Context helper classes.
//
// WinDirStat - Directory Statistics
// Copyright (C) 2003-2005 Bernhard Seifert
// Copyright (C) 2004-2006 Oliver Schneider (assarbad.net)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Author(s): - bseifert -> bseifert@users.sourceforge.net, bseifert@daccord.net
//            - assarbad -> http://assarbad.net/en/contact
//
// $Header$


// Example:
// Instead of writing 
//     CGdiObject *old = pdc->SelectObject(&brush); ...; pdc->SelectObject(old);
// we can simply write
//     CSelectObject sobrush(pdc, &brush);
// and the destructor will reselect the old object.

#pragma once


class CSelectObject
{
public:
	CSelectObject(CDC *pdc, CGdiObject *pObject)
	{ m_pOldObject = pdc->SelectObject(pObject); m_pdc = pdc; }
	~CSelectObject()
	{ m_pdc->SelectObject(m_pOldObject); }
protected:
	CDC *m_pdc;
	CGdiObject *m_pOldObject;
};

class CSelectStockObject
{
public:
	CSelectStockObject(CDC *pdc, int nIndex)
	{ m_pOldObject = pdc->SelectStockObject(nIndex); m_pdc = pdc; }
	~CSelectStockObject()
	{ m_pdc->SelectObject(m_pOldObject); }
protected:
	CDC *m_pdc;
	CGdiObject *m_pOldObject;
};

class CSetBkMode
{
public:
	CSetBkMode(CDC *pdc, int mode)
	{ m_pdc = pdc; m_oldMode = pdc->SetBkMode(mode); }
	~CSetBkMode()
	{ m_pdc->SetBkMode(m_oldMode); }
protected:
	CDC *m_pdc;
	int m_oldMode;
};

class CSetTextColor
{
public:
	CSetTextColor(CDC *pdc, COLORREF color)
	{ m_pdc = pdc; m_oldColor = pdc->SetTextColor(color); }
	~CSetTextColor()
	{ m_pdc->SetTextColor(m_oldColor); }
protected:
	CDC *m_pdc;
	COLORREF m_oldColor;
};

class CSetBkColor
{
public:
	CSetBkColor(CDC *pdc, COLORREF color)
	{ m_pdc = pdc; m_oldColor = pdc->SetBkColor(color); }
	~CSetBkColor()
	{ m_pdc->SetBkColor(m_oldColor); }
protected:
	CDC *m_pdc;
	COLORREF m_oldColor;
};

class CSaveDC
{
public:
	CSaveDC(CDC *pdc) { m_pdc = pdc; m_save = pdc->SaveDC(); }
	~CSaveDC() { m_pdc->RestoreDC(m_save); }
protected:
	CDC *m_pdc;
	int m_save;
};

inline BOOL CreateRectRgn(CRgn& rgn, CRect rc)
{
	return rgn.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
}

inline COLORREF MakeShadowColor(COLORREF c, int percent)
{
	return RGB(
		GetRValue(c) * percent / 100,
		GetGValue(c) * percent / 100,
		GetBValue(c) * percent / 100
	);
}


// $Log$
// Revision 1.6  2006/07/04 23:37:40  assarbad
// - Added my email address in the header, adjusted "Author" -> "Author(s)"
// - Added CVS Log keyword to those files not having it
// - Added the files which I forgot during last commit
//
// Revision 1.5  2006/07/04 22:49:21  assarbad
// - Replaced CVS keyword "Date" by "Header" in the file headers
//
// Revision 1.4  2006/07/04 20:45:23  assarbad
// - See changelog for the changes of todays previous check-ins as well as this one!
//
// Revision 1.3  2004/11/05 16:53:07  assarbad
// Added Date and History tag where appropriate.
//
