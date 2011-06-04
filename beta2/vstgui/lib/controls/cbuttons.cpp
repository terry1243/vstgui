//-----------------------------------------------------------------------------
// VST Plug-Ins SDK
// VSTGUI: Graphical User Interface Framework for VST plugins : 
//
// Version 4.0
//
//-----------------------------------------------------------------------------
// VSTGUI LICENSE
// (c) 2010, Steinberg Media Technologies, All Rights Reserved
//-----------------------------------------------------------------------------
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// 
//   * Redistributions of source code must retain the above copyright notice, 
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation 
//     and/or other materials provided with the distribution.
//   * Neither the name of the Steinberg Media Technologies nor the names of its
//     contributors may be used to endorse or promote products derived from this 
//     software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A  PARTICULAR PURPOSE ARE DISCLAIMED. 
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#include "cbuttons.h"
#include "../cdrawcontext.h"
#include "../cbitmap.h"
#include "../cframe.h"
#include <cmath>

namespace VSTGUI {

//------------------------------------------------------------------------
// COnOffButton
//------------------------------------------------------------------------
/*! @class COnOffButton
Define a button with 2 positions.
The bitmap includes the 2 subbitmaps (i.e the rectangle used for the display of this button is half-height of the bitmap).
When its value changes, the listener is called.
*/
//------------------------------------------------------------------------
/**
 * COnOffButton constructor.
 * @param size the size of this view
 * @param listener the listener
 * @param tag the control tag
 * @param background bitmap of the on/off button
 * @param style style, currently not used
 */
//------------------------------------------------------------------------
COnOffButton::COnOffButton (const CRect& size, CControlListener* listener, int32_t tag, CBitmap* background, int32_t style)
: CControl (size, listener, tag, background)
, style (style)
{
	setWantsFocus (true);
}

//------------------------------------------------------------------------
COnOffButton::COnOffButton (const COnOffButton& v)
: CControl (v)
, style (v.style)
{
	setWantsFocus (true);
}

//------------------------------------------------------------------------
COnOffButton::~COnOffButton ()
{}

//------------------------------------------------------------------------
void COnOffButton::draw (CDrawContext *pContext)
{
	if (pBackground)
	{
		CCoord off;

		if (value == getMax ())
			off = pBackground->getHeight () / 2;
		else
			off = 0;

		pBackground->draw (pContext, size, CPoint (0, off));
	}
	setDirty (false);
}

//------------------------------------------------------------------------
CMouseEventResult COnOffButton::onMouseDown (CPoint& where, const CButtonState& buttons)
{
	if (!(buttons & kLButton))
		return kMouseEventNotHandled;

	value = (value == getMax ()) ? getMin () : getMax ();
	invalid ();
	beginEdit ();
	valueChanged ();
	endEdit ();

	return kMouseDownEventHandledButDontNeedMovedOrUpEvents;
}

//------------------------------------------------------------------------
int32_t COnOffButton::onKeyDown (VstKeyCode& keyCode)
{
	if (keyCode.virt == VKEY_RETURN && keyCode.modifier == 0)
	{
		value = (value == getMax ()) ? getMin () : getMax ();
		invalid ();
		beginEdit ();
		valueChanged ();
		endEdit ();
		return 1;
	}
	return -1;
}

//------------------------------------------------------------------------
bool COnOffButton::sizeToFit ()
{
	if (pBackground)
	{
		CRect vs (getViewSize ());
		vs.setWidth (pBackground->getWidth ());
		vs.setHeight (pBackground->getHeight () / 2.);
		setViewSize (vs, true);
		setMouseableArea (vs);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------
// CKickButton
//------------------------------------------------------------------------
/*! @class CKickButton
Define a button with 2 states using 2 subbitmaps.
One click on it, then the second subbitmap is displayed.
When the mouse button is relaxed, the first subbitmap is framed.
*/
//------------------------------------------------------------------------
/**
 * CKickButton constructor.
 * @param size the size of this view
 * @param listener the listener
 * @param tag the control tag
 * @param background the bitmap
 * @param offset unused
 */
//------------------------------------------------------------------------
CKickButton::CKickButton (const CRect& size, CControlListener* listener, int32_t tag, CBitmap* background, const CPoint& offset)
: CControl (size, listener, tag, background)
, offset (offset)
{
	heightOfOneImage = size.height ();
	setWantsFocus (true);
}

//------------------------------------------------------------------------
/**
 * CKickButton constructor.
 * @param size the size of this view
 * @param listener the listener
 * @param tag the control tag
 * @param heightOfOneImage height of one sub bitmap in background
 * @param background the bitmap
 * @param offset of background
 */
//------------------------------------------------------------------------
CKickButton::CKickButton (const CRect& size, CControlListener* listener, int32_t tag, CCoord heightOfOneImage, CBitmap* background, const CPoint& offset)
: CControl (size, listener, tag, background)
, offset (offset)
{
	setHeightOfOneImage (heightOfOneImage);
	setWantsFocus (true);
}

//------------------------------------------------------------------------
CKickButton::CKickButton (const CKickButton& v)
: CControl (v)
, offset (v.offset)
{
	setHeightOfOneImage (v.heightOfOneImage);
	setWantsFocus (true);
}

//------------------------------------------------------------------------
CKickButton::~CKickButton ()
{}

//------------------------------------------------------------------------
void CKickButton::draw (CDrawContext *pContext)
{
	CPoint where (offset.h, offset.v);

	bounceValue ();

	if (value == getMax ())
		where.v += heightOfOneImage;

	if (pBackground)
	{
		pBackground->draw (pContext, size, where);
	}
	setDirty (false);
}

//------------------------------------------------------------------------
CMouseEventResult CKickButton::onMouseDown (CPoint& where, const CButtonState& buttons)
{
	if (!(buttons & kLButton))
		return kMouseEventNotHandled;
	fEntryState = value;
	beginEdit ();
	return onMouseMoved (where, buttons);
}

//------------------------------------------------------------------------
CMouseEventResult CKickButton::onMouseUp (CPoint& where, const CButtonState& buttons)
{
	if (value)
		valueChanged ();
	value = getMin ();  // set button to UNSELECTED state
	valueChanged ();
	if (isDirty ())
		invalid ();
	endEdit ();
	return kMouseEventHandled;
}

//------------------------------------------------------------------------
CMouseEventResult CKickButton::onMouseMoved (CPoint& where, const CButtonState& buttons)
{
	if (buttons & kLButton)
	{
		if (where.h >= size.left && where.v >= size.top  &&
			where.h <= size.right && where.v <= size.bottom)
			value = getMax ();
		else
			value = getMin ();
		
		if (isDirty ())
			invalid ();
	}
	return kMouseEventHandled;
}

//------------------------------------------------------------------------
int32_t CKickButton::onKeyDown (VstKeyCode& keyCode)
{
	if (keyCode.modifier == 0 && keyCode.virt == VKEY_RETURN)
	{
		beginEdit ();
		value = getMax ();
		invalid ();
		valueChanged ();
		return 1;
	}
	return -1;
}

//------------------------------------------------------------------------
int32_t CKickButton::onKeyUp (VstKeyCode& keyCode)
{
	if (keyCode.modifier == 0 && keyCode.virt == VKEY_RETURN)
	{
		value = getMin ();
		invalid ();
		valueChanged ();
		endEdit ();
		return 1;
	}
	return -1;
}

//------------------------------------------------------------------------
bool CKickButton::sizeToFit ()
{
	if (pBackground)
	{
		CRect vs (getViewSize ());
		vs.setHeight (heightOfOneImage);
		vs.setWidth (pBackground->getWidth ());
		setViewSize (vs, true);
		setMouseableArea (vs);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------
// CCheckBox
//------------------------------------------------------------------------
/*! @class CCheckBox
A checkbox control with a title and 3 states : checked, half checked, not checked

- if value is < 0.5 the checkbox is not checked
- if value is 0.5 the checkbox is half checked
- if value is > 0.5 the checkbox is checked

the user can only switch between checked and not checked state.

If the bitmap is set, the bitmap must contain 6 states of the checkbox in the following order:
- not checked
- half checked
- checked
- not checked hilighted
- half checked hilighted
- checked hilighted
*/
//------------------------------------------------------------------------
//------------------------------------------------------------------------
CCheckBox::CCheckBox (const CRect& size, CControlListener* listener, int32_t tag, UTF8StringPtr title, CBitmap* bitmap, int32_t style)
: CControl (size, listener, tag, bitmap)
, title (0)
, style (style)
, font (0)
, fontColor (kWhiteCColor)
, hilight (false)
{
	setTitle (title);
	setFont (kSystemFont);
	setBoxFillColor (kWhiteCColor);
	setBoxFrameColor (kBlackCColor);
	setCheckMarkColor (kRedCColor);
	font->remember ();
	setWantsFocus (true);
	if (style & kAutoSizeToFit)
		sizeToFit ();
}

//------------------------------------------------------------------------
CCheckBox::CCheckBox (const CCheckBox& checkbox)
: CControl (checkbox)
, title (0)
, style (checkbox.style)
, font (0)
, fontColor (checkbox.fontColor)
, hilight (false)
{
	setTitle (checkbox.title);
	setFont (checkbox.font);
	setBoxFillColor (checkbox.boxFillColor);
	setBoxFrameColor (checkbox.boxFrameColor);
	setCheckMarkColor (checkbox.checkMarkColor);
	font->remember ();
	setWantsFocus (true);
}

//------------------------------------------------------------------------
CCheckBox::~CCheckBox ()
{
	setTitle (0);
	setFont (0);
}

//------------------------------------------------------------------------
void CCheckBox::setTitle (UTF8StringPtr newTitle)
{
	if (title)
		delete [] title;
	title = 0;
	if (newTitle)
	{
		title = new char [strlen (newTitle) + 1];
		strcpy (title, newTitle);
	}
	if (style & kAutoSizeToFit)
		sizeToFit ();
}

//------------------------------------------------------------------------
void CCheckBox::setFont (CFontRef newFont)
{
	if (font)
		font->forget ();
	font = newFont;
	if (font)
		font->remember ();
	if (style & kAutoSizeToFit)
		sizeToFit ();
}

//------------------------------------------------------------------------
void CCheckBox::setBackground (CBitmap *background)
{
	CView::setBackground (background);
	if (style & kAutoSizeToFit)
		sizeToFit ();
}

//------------------------------------------------------------------------
void CCheckBox::setStyle (int32_t newStyle)
{
	if (style != newStyle)
	{
		style = newStyle;
		if (style & kAutoSizeToFit)
			sizeToFit ();
		invalid ();
	}
}

/// @cond ignore
//------------------------------------------------------------------------
static CCoord getFontCapHeight (CFontRef font)
{
	CCoord c = font->getSize ();
	IPlatformFont* pf = font->getPlatformFont ();
	if (pf)
	{
		CCoord capHeight = pf->getCapHeight ();
		if (capHeight <= 0)
			capHeight = pf->getAscent ();
		if (capHeight > 0)
			c = capHeight;
	}
	return c;
}
/// @endcond

//------------------------------------------------------------------------
static CCoord kCheckBoxTitleMargin = 5;

//------------------------------------------------------------------------
bool CCheckBox::sizeToFit ()
{
	if (title == 0)
		return false;
	IFontPainter* painter = font ? font->getFontPainter () : 0;
	if (painter)
	{
		CRect fitSize (size);
		if (pBackground)
		{
			fitSize.setWidth (pBackground->getWidth ());
			fitSize.setHeight (pBackground->getHeight () / 6);
		}
		else
		{
			fitSize.setWidth (fitSize.getHeight ());
		}
		fitSize.right += kCheckBoxTitleMargin;
		fitSize.right += painter->getStringWidth (0, title, true);
		setViewSize (fitSize);
		setMouseableArea (fitSize);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------
void CCheckBox::draw (CDrawContext* context)
{
	float norm = getValueNormalized ();
	CRect checkBoxSize (size);
	if (pBackground)
	{
		CPoint off;

		checkBoxSize.setWidth (pBackground->getWidth ());
		checkBoxSize.setHeight (pBackground->getHeight () / 6);

		if (norm == 0.5)
			off.y = checkBoxSize.getHeight ();
		else if (norm > 0.5)
			off.y = checkBoxSize.getHeight () * 2;
		else
			off.y = 0;
		if (hilight)
			off.y += pBackground->getHeight () / 2;

		pBackground->draw (context, checkBoxSize, off);
	}
	else
	{
		checkBoxSize.setHeight (getFontCapHeight (font) + 2);
		checkBoxSize.setWidth (checkBoxSize.getHeight ());
		checkBoxSize.offset (1, ceil ((size.getHeight () - checkBoxSize.getHeight ()) / 2));
		context->setLineWidth (1);
		context->setLineStyle (kLineSolid);
		context->setDrawMode (kAliasing);
		context->setFrameColor (boxFrameColor);
		context->setFillColor (boxFillColor);
		context->drawRect (checkBoxSize, kDrawFilledAndStroked);

		if (hilight)
		{
			CColor hilightColor = boxFrameColor;
			hilightColor.alpha /= 2;
			context->setFrameColor (hilightColor);
			CRect r (checkBoxSize);
			r.inset (1, 1);
			context->drawRect (r, kDrawStroked);
		}

		context->setDrawMode (kAntiAliasing);
		context->setFrameColor (checkMarkColor);
		context->setLineWidth (2);

		const CCoord cbInset = 2;
		
		if (style & kDrawCrossBox)
		{
			if (norm == 0.5f)
			{
				context->moveTo (CPoint (checkBoxSize.left + cbInset, checkBoxSize.top + checkBoxSize.getHeight () / 2));
				context->lineTo (CPoint (checkBoxSize.right - cbInset, checkBoxSize.top + checkBoxSize.getHeight () / 2));
			}
			else if (norm > 0.5f)
			{
				context->moveTo (CPoint (checkBoxSize.left + cbInset, checkBoxSize.top + cbInset));
				context->lineTo (CPoint (checkBoxSize.right - cbInset, checkBoxSize.bottom - cbInset));
				context->moveTo (CPoint (checkBoxSize.left + cbInset, checkBoxSize.bottom - cbInset));
				context->lineTo (CPoint (checkBoxSize.right - cbInset, checkBoxSize.top + cbInset));
			}
		}
		else
		{
			context->moveTo (CPoint (checkBoxSize.left + cbInset, checkBoxSize.top + checkBoxSize.getHeight () / 2));
			if (norm == 0.5f)
			{
				context->lineTo (CPoint (checkBoxSize.right - cbInset, checkBoxSize.top + checkBoxSize.getHeight () / 2));
			}
			else if (norm > 0.5f)
			{
				context->lineTo (CPoint (checkBoxSize.left + checkBoxSize.getWidth () / 2, checkBoxSize.bottom - cbInset));
				context->lineTo (CPoint (checkBoxSize.right + 1, checkBoxSize.top - 1));
			}
		}
	}
	
	if (title)
	{
		CPoint p (checkBoxSize.getBottomRight ());
		p.offset (kCheckBoxTitleMargin, -1);
		
		context->setFont (font);
		context->setFontColor (fontColor);
		
		context->drawString (title, p, true);
	}
	
	setDirty (false);
}

//------------------------------------------------------------------------
bool CCheckBox::getFocusPath (CGraphicsPath& outPath)
{
	if (wantsFocus ())
	{
		CCoord focusWidth = getFrame ()->getFocusWidth ();
		CRect checkBoxSize (size);
		if (pBackground)
		{
			checkBoxSize.setWidth (pBackground->getWidth ());
			checkBoxSize.setHeight (pBackground->getHeight () / 6);
		}
		else
		{
			checkBoxSize.setHeight (getFontCapHeight (font) + 2);
			checkBoxSize.setWidth (checkBoxSize.getHeight ());
			checkBoxSize.offset (1, ceil ((size.getHeight () - checkBoxSize.getHeight ()) / 2));
		}
		outPath.addRect (checkBoxSize);
		checkBoxSize.inset (-focusWidth, -focusWidth);
		outPath.addRect (checkBoxSize);
	}
	return true;
}

//------------------------------------------------------------------------
CMouseEventResult CCheckBox::onMouseDown (CPoint& where, const CButtonState& buttons)
{
	if (buttons.isLeftButton ())
	{
		beginEdit ();
		previousValue = value;
		return onMouseMoved (where, buttons);
	}
	return kMouseDownEventHandledButDontNeedMovedOrUpEvents;
}

//------------------------------------------------------------------------
CMouseEventResult CCheckBox::onMouseMoved (CPoint& where, const CButtonState& buttons)
{
	if (buttons.isLeftButton ())
	{
		bool wasHilighted = hilight;
		if (where.isInside (size))
			hilight = true;
		else
			hilight = false;
		if (wasHilighted != hilight)
			invalid ();
		return kMouseEventHandled;
	}
	return kMouseEventNotHandled;
}

//------------------------------------------------------------------------
CMouseEventResult CCheckBox::onMouseUp (CPoint& where, const CButtonState& buttons)
{
	hilight = false;
	if (where.isInside (size))
		value = (previousValue < getMax ()) ? getMax () : getMin ();
	else
		value = previousValue;
	if (isDirty ())
	{
		valueChanged ();
		invalid ();
	}
	endEdit ();
	return kMouseEventHandled;
}

//------------------------------------------------------------------------
int32_t CCheckBox::onKeyDown (VstKeyCode& keyCode)
{
	if (keyCode.virt == VKEY_RETURN && keyCode.modifier == 0)
	{
		value = (value < getMax ()) ? getMax () : getMin ();
		invalid ();
		beginEdit ();
		valueChanged ();
		endEdit ();
		return 1;
	}
	return -1;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
CTextButton::CTextButton (const CRect& size, CControlListener* listener, int32_t tag, UTF8StringPtr title)
: CKickButton (size, listener, tag, 0)
, title (title)
, font (0)
, _path (0)
, frameWidth (1.)
, roundRadius (6.)
{
	setFont (kSystemFont);
	setTextColor (kBlackCColor);
	setTextColorHighlighted (kWhiteCColor);
	setGradientStartColor (CColor (220, 220, 220, 255));
	setGradientStartColorHighlighted (CColor (180, 180, 180, 255));
	setGradientEndColor (CColor (180, 180, 180, 255));
	setGradientEndColorHighlighted (CColor (100, 100, 100, 255));
	setFrameColor (kBlackCColor);
	setFrameColorHighlighted (kBlackCColor);
}

//------------------------------------------------------------------------
bool CTextButton::removed (CView* parent)
{
	invalidPath ();
	return CKickButton::removed (parent);
}

//------------------------------------------------------------------------
void CTextButton::setViewSize (const CRect& rect, bool invalid)
{
	invalidPath ();
	CKickButton::setViewSize (rect, invalid);
}

//------------------------------------------------------------------------
void CTextButton::setTitle (UTF8StringPtr newTitle)
{
	title = newTitle;
	invalid ();
}

//------------------------------------------------------------------------
void CTextButton::setFont (CFontRef newFont)
{
	if (newFont == 0)
		return;
	if (font)
		font->forget ();
	font = newFont;
	if (font)
		font->remember ();
	invalid ();
}

//------------------------------------------------------------------------
void CTextButton::setTextColor (const CColor& color)
{
	textColor = color;
	invalid ();
}

//------------------------------------------------------------------------
void CTextButton::setGradientStartColor (const CColor& color)
{
	gradientStartColor = color;
	invalid ();
}

//------------------------------------------------------------------------
void CTextButton::setGradientEndColor (const CColor& color)
{
	gradientEndColor = color;
	invalid ();
}

//------------------------------------------------------------------------
void CTextButton::setFrameColor (const CColor& color)
{
	frameColor = color;
	invalid ();
}

//------------------------------------------------------------------------
void CTextButton::setTextColorHighlighted (const CColor& color)
{
	textColorHighlighted = color;
	invalid ();
}

//------------------------------------------------------------------------
void CTextButton::setGradientStartColorHighlighted (const CColor& color)
{
	gradientStartColorHighlighted = color;
	invalid ();
}

//------------------------------------------------------------------------
void CTextButton::setGradientEndColorHighlighted (const CColor& color)
{
	gradientEndColorHighlighted = color;
	invalid ();
}

//------------------------------------------------------------------------
void CTextButton::setFrameColorHighlighted (const CColor& color)
{
	frameColorHighlighted = color;
	invalid ();
}

//------------------------------------------------------------------------
void CTextButton::setFrameWidth (CCoord width)
{
	frameWidth = width;
	invalid ();
}

//------------------------------------------------------------------------
void CTextButton::setRoundRadius (CCoord radius)
{
	roundRadius = radius;
	invalidPath ();
	invalid ();
}

//------------------------------------------------------------------------
void CTextButton::draw (CDrawContext* context)
{
	bool highlight = value > 0.5 ? true : false;
	context->setDrawMode (kAntiAliasing);
	context->setLineWidth (frameWidth);
	context->setLineStyle (CLineStyle (CLineStyle::kLineCapRound, CLineStyle::kLineJoinRound));
	context->setFrameColor (highlight ? frameColorHighlighted : frameColor);
	CRect r (getViewSize ());
	r.inset (frameWidth / 2., frameWidth / 2.);
	CGraphicsPath* path = getPath (context);
	if (path)
	{
		CColor color1 = highlight ? gradientStartColorHighlighted : gradientStartColor;
		CColor color2 = highlight ? gradientEndColorHighlighted : gradientEndColor;
		CGradient* gradient = path->createGradient (0.2, 1, color1, color2);
		if (gradient)
		{
			context->fillLinearGradient (path, *gradient, r.getTopLeft (), r.getBottomLeft (), false);
			gradient->forget ();
		}
		else
		{
			context->setFillColor (highlight ? gradientStartColorHighlighted : gradientStartColor);
			context->drawGraphicsPath (path, CDrawContext::kPathFilled);
		}
		context->drawGraphicsPath (path, CDrawContext::kPathStroked);
	}
	else
	{
		context->setFillColor (value > 0.5 ? gradientStartColorHighlighted : gradientStartColor);
		context->drawRect (size, kDrawFilledAndStroked);
	}
	context->setFont (font);
	context->setFontColor (value > 0.5 ? textColorHighlighted : textColor);
	context->drawString (title.c_str (), size);
	setDirty (false);
}

//------------------------------------------------------------------------
int32_t CTextButton::onKeyDown (VstKeyCode& keyCode)
{
	if ((keyCode.virt == VKEY_ENTER || keyCode.virt == VKEY_RETURN) && getFrame ()->getFocusView () == this)
	{
		beginEdit ();
		value = 1;
		valueChanged ();
		value = 0;
		endEdit ();
		return 1;
	}
	return -1;
}

//------------------------------------------------------------------------
bool CTextButton::getFocusPath (CGraphicsPath& outPath)
{
	CRect r (getViewSize ());
	CCoord focusWidth = getFrame ()->getFocusWidth ();
	r.inset (-focusWidth, -focusWidth);
	outPath.addRoundRect (r, roundRadius);
	outPath.closeSubpath ();
	r = getViewSize ();
	r.inset (frameWidth / 2., frameWidth / 2.);
	outPath.addRoundRect (r, roundRadius);
	return true;
}

//------------------------------------------------------------------------
CGraphicsPath* CTextButton::getPath (CDrawContext* context)
{
	if (_path == 0)
	{
		CRect r (getViewSize ());
		r.inset (frameWidth / 2., frameWidth / 2.);
		_path = context->createRoundRectGraphicsPath (r, roundRadius);
	}
	return _path;
}

//------------------------------------------------------------------------
void CTextButton::invalidPath ()
{
	if (_path)
	{
		_path->forget ();
		_path = 0;
	}
}

} // namespace