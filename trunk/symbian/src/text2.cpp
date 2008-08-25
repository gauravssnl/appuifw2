/* Copyright 2008 Arkadiusz Wahlig (arkadiusz.wahlig@gmail.com)
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
** http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License. 
*/

#include <e32std.h>
#include <e32base.h>
#include <s32mem.h>
#include <txtfrmat.h>         // Text
#include <txtfmlyr.h>         // Text
#include <eikgted.h>          // Text
#include <eikrted.h>	      // RText
#include <txtrich.h>
#include <gdi.h>	      // RText
#include <aknutils.h>
#include <aknedsts.h>
#include <aknsconstants.h>  // for skin IDs
#include <aknsbasicbackgroundcontrolcontext.h>
#include <aknsutils.h>

#include <Python.h>
#include <symbian_python_ext_util.h>

#include "appuifwutil.h"
#include "pycallback.h"
#include "fontspec.h"
#include "colorspec.h"

class CText2RichEditor;
class CText2EditObserver;

struct Text2_object {
	// The following fields are required by appuifw.Application.
	// Note that PyObject_VAR_HEAD is here only to allow using of
	// this structure as a Python object. However it must always
	// be present for binary compatibility.
	PyObject_VAR_HEAD
	CText2RichEditor *control;
	CAppuifwEventBindingArray *event_bindings;
	// The following fields are private.
	TCharFormatMask char_format_mask;
	TCharFormat char_format;
	TInt current_style;
	CCharFormatLayer *char_format_layer;
	CParaFormatLayer *para_format_layer;
};

const struct _typeobject Text2_type={};

#define KNORMAL	   		0x00
#define KBOLD			0x01
#define KUNDERLINE 		0x02	
#define KITALIC 		0x04
#define KSTRIKETHROUGH 		0x08

#define KHIGHLIGHTNONE		0x00
#define KHIGHLIGHTROUNDED	0x10
#define KHIGHLIGHTSHADOW	0x20
#define KHIGHLIGHTNORMAL	0x40

#define HL_MASK			0x70

class CText2EditObserver : public CBase, public MEditObserver
{
public:
	CText2EditObserver(PyObject *aCallBack)
	{
		iCallBack = aCallBack;
		Py_XINCREF(aCallBack);
	} 
	
	virtual ~CText2EditObserver() {
		Py_XDECREF(iCallBack);
	}
	
	virtual void EditObserver(TInt aStart, TInt aExtent)
	{
		if (iCallBack)
			PyAsyncCallBack(iCallBack, Py_BuildValue("ii", aStart, aExtent));
	}

private:	
	PyObject *iCallBack;
};

#ifndef EKA2
class CText2RichEditor : public CEikRichTextEditor, public MEikEdwinObserver
#else
NONSHARABLE_CLASS(CText2RichEditor) : public CEikRichTextEditor, public MEikEdwinObserver
#endif
{
public:
	CText2RichEditor() {}
	virtual void ConstructL(const CCoeControl* aParent, TInt aFlags, TBool aScrollBar,
		TBool aSkinned, PyObject *aMoveCallBack, PyObject *aEditCallBack);
	
	virtual ~CText2RichEditor()
	{
		//  iCoeEnv->ReleaseScreenFont(aFont); XXX necessary?
		delete iMyCharFormatLayer;
		delete iMyParaFormatLayer;
		if (iBgContext)
			delete iBgContext;
		Py_XDECREF(iMoveCallBack);
		Py_XDECREF(iEditCallBack);
		if (iEditObserver)
			delete iEditObserver;
	}
	
	void ReadL(PyObject*& aRval, TInt aPos = 0, TInt aLen = (-1));
	TInt WriteL(const TDesC& aText, TInt aPos = -1);
	void DelL(TInt aPos, TInt aLen);
	void ApplyFormatL(TInt aPos, TInt aLen);

	TInt Len()
	{
		return RichText()->DocumentLength();
	}
	
	void ClearL()
	{
		RichText()->Reset();
		HandleChangeL(0);
	}
  
  	TInt SetPos(TInt aPos, TBool aSelect);
    
	static TBool CheckRange(TInt& aPos, TInt& aLen, TInt aDocLen);

	void SetBold(TBool aOpt);
	void SetUnderline(TBool aOpt);
	void SetPosture(TBool aOpt);
	void SetStrikethrough(TBool aOpt);
	void SetHighlight(TInt mode);
	void SetStyle(TInt aStyle) { iCurrentStyle = aStyle; };
	TInt Style() { return iCurrentStyle; };
	void SetTextColor(TRgb color);
	TRgb TextColor() { return iCharFormat.iFontPresentation.iTextColor; };
	void SetHighlightColor(TRgb color);
	TRgb HighlightColor() { return iCharFormat.iFontPresentation.iHighlightColor; };

	void SetFont(TFontSpec&);
	
	TFontSpec& Font()
	{
		return iCharFormat.iFontSpec;
	}

	void SizeChanged()
	{
		CEikScrollBarFrame *frame=ScrollBarFrame();

		if (iBgContext) { // setup the skinned background
			TSize size=Size();
			TRect rect;
			
			AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);

			if (size.iHeight > rect.Height()) {
				// large, full
				iBgContext->SetBitmap(KAknsIIDQsnBgScreen);
			}
			else {
				// normal
				iBgContext->SetBitmap(KAknsIIDQsnBgAreaMain);
			}

			iBgContext->SetRect(Rect());
		}

		if (frame) {
			CEikScrollBar *bar=frame->VerticalScrollBar();
			if (bar) {
				TSize size=Size();

				size.iWidth = iParent->Size().iWidth - bar->ScrollBarBreadth();
				SetSizeWithoutNotification(size);
			}
		}
	
		CEikRichTextEditor::HandleSizeChangedL();
		TextLayout()->RestrictScrollToTopsOfLines(EFalse);
	}
  
	virtual void MakeVisible(TBool aVisible)
	{
		CEikScrollBarFrame *frame=ScrollBarFrame();

		if (frame) {
			TRAPD(error,
				if (aVisible)
					frame->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
						CEikScrollBarFrame::EAuto);
				else
					frame->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
						CEikScrollBarFrame::EOff);
			);
		}
	}

	// needed to draw the scrollbar background
	virtual TTypeUid::Ptr MopSupplyObject( TTypeUid aId )
	{
		if (aId.iUid == MAknsControlContext::ETypeId && iBgContext) {
			return MAknsControlContext::SupplyMopObject(aId, iBgContext);
		}
		return CEikRichTextEditor::MopSupplyObject( aId );
	}

	virtual void HandleEdwinEventL(CEikEdwin * /*aEdwin*/, TEdwinEvent aEventType)
	{
		switch (aEventType) {
			case EEventNavigation:
				if (iMoveCallBack)
					PyCallBack(iMoveCallBack, Py_BuildValue("()"));
				break;
				
			default:
				break;
		}
	}

private:
	void UpdateCharFormatLayerL();
	
	void HandleChangeL(TInt aNewCursorPos)
	{
		// TODO: Use SetAmountToFormat to improve performance?
		HandleTextChangedL();
		SetCursorPosL(aNewCursorPos, EFalse);
		//if (!IsVisible()) {   // Otherwise we occasionally pop up unwanted 
		//  int in_interpreter=(_PyThreadState_Current == PYTHON_TLS->thread_state);
		//  if (!in_interpreter)
		//    PyEval_RestoreThread(PYTHON_TLS->thread_state);
		//  // The interpreter state must be valid to use the MY_APPUI macro.
		//  CAmarettoAppUi* pyappui = MY_APPUI; 
		//  if (!in_interpreter)
		//    PyEval_SaveThread();      
		//  pyappui->RefreshHostedControl();
		//}
	}
  
	TCharFormatMask iCharFormatMask;
	TCharFormat iCharFormat;
	TInt iCurrentStyle;
	CCharFormatLayer *iMyCharFormatLayer;
	CParaFormatLayer *iMyParaFormatLayer;
	CAknsBasicBackgroundControlContext *iBgContext;
	PyObject *iMoveCallBack;
	PyObject *iEditCallBack;
	CText2EditObserver *iEditObserver;
	const CCoeControl *iParent;
};

void CText2RichEditor::ConstructL(const CCoeControl* aParent, TInt aFlags, TBool aScrollBar,
		TBool aSkinned, PyObject *aMoveCallBack, PyObject *aEditCallBack)
{
	CEikRichTextEditor::ConstructL(aParent, 0, 0, aFlags, 0, 0);
	iParent = aParent;

	ActivateL();
  
	iMyCharFormatLayer = CCharFormatLayer::NewL();    
	iCurrentStyle = KNORMAL;
	iCharFormat.iFontPresentation.iTextColor = TRgb(0,128,0); // Default text color: dark green.
	iCharFormat.iFontPresentation.iHighlightColor = TRgb(180,180,180); // Default highlight color: gray.
	//iCharFormatMask.SetAll();  
	iCharFormatMask.SetAttrib(EAttFontHeight);
	iCharFormatMask.SetAttrib(EAttFontPosture);
	iCharFormatMask.SetAttrib(EAttFontStrokeWeight);
	iCharFormatMask.SetAttrib(EAttFontPrintPos);
	//iCharFormatMask.SetAttrib(EAttFontTypeface);
	//iCharFormatMask.SetAttrib(EAttFontBitmapStyle);
	iCharFormatMask.SetAttrib(EAttColor);

	iCharFormatMask.SetAttrib(EAttFontHighlightColor);

	iMyCharFormatLayer->SetL(iCharFormat, iCharFormatMask);
	RichText()->SetGlobalCharFormat(iMyCharFormatLayer);

	/* Set the global paragraph format. This should be more effective than setting it on every insert. */
	iMyParaFormatLayer = CParaFormatLayer::NewL();
	CParaFormat paraFormat;
	TParaFormatMask paraFormatMask;
	paraFormatMask.SetAttrib(EAttLineSpacing);
	paraFormatMask.SetAttrib(EAttLineSpacingControl);
	paraFormat.iLineSpacingControl = CParaFormat::ELineSpacingAtLeastInTwips;
	paraFormat.iLineSpacingInTwips = 30;  
	iMyParaFormatLayer->SetL(&paraFormat, paraFormatMask);    
	RichText()->SetGlobalParaFormat(iMyParaFormatLayer);

	if (aSkinned) {
		iBgContext = CAknsBasicBackgroundControlContext::NewL(
			KAknsIIDQsnBgScreen, // changed in SizeChanged()
			TRect(0, 0, 0, 0), // changed in SizeChanged()
			EFalse);
		SetSkinBackgroundControlContextL(iBgContext);

		TInt error;
		TRgb color;
		error = AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), color,
			KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6);
		if (error == KErrNone)
			SetTextColor(color);
	}

	if (aScrollBar)
		CreateScrollBarFrameL();
	
	if (aMoveCallBack) {
		iMoveCallBack = aMoveCallBack;
		Py_INCREF(aMoveCallBack);
		SetEdwinObserver(this);
	}
	
	if (aEditCallBack) {
		iEditObserver = new (ELeave) CText2EditObserver(aEditCallBack);
		RichText()->SetEditObserver(iEditObserver);
	}
}

TBool CText2RichEditor::CheckRange(TInt& aPos, TInt& aLen, TInt aDocLen)
{
  if (aPos > aDocLen)
    return EFalse;
  
  if ((aLen < 0) || ((aPos + aLen) > aDocLen))
    aLen = aDocLen - aPos;
  
  return ETrue;
}

void CText2RichEditor::ReadL(PyObject*& aRval, TInt aPos, TInt aLen)
{
  if (CheckRange(aPos, aLen, RichText()->DocumentLength()) == EFalse)
    User::Leave(KErrArgument);
  
  if ((aRval = PyUnicode_FromUnicode(NULL, aLen))) {
    TPtr buf(PyUnicode_AsUnicode(aRval), 0, aLen);
    RichText()->Extract(buf, aPos, aLen);
  }
}

TInt CText2RichEditor::WriteL(const TDesC& aText, TInt aPos)
{
  TInt cpos = CursorPos();
  TInt len_written;

  if (aPos < 0)
    aPos = cpos;
  
  RMemReadStream source(aText.Ptr(), aText.Size());
  
  RichText()->ImportTextL(aPos, source,
			  CPlainText::EOrganiseByParagraph,
			  KMaxTInt, KMaxTInt, &len_written);
  RichText()->ApplyCharFormatL(iCharFormat, iCharFormatMask, aPos, len_written);

  /*
  RichText()->GetParaFormatL(&paraFormat, paraFormatMask,
			     aPos, len_written);
  paraFormatMask.ClearAll();
  RichText()->ApplyParaFormatL(&paraFormat, paraFormatMask,
			       aPos, len_written);
  */
  
  // BENCHMARKS with N70:
  // without paraformat, 13s
  //HandleTextChangedL();
  //SetCursorPosL(aPos+len_written, EFalse);      

  // without paraformat, 22s
  //SetCursorPosL(aPos+len_written, EFalse);      
  //HandleTextChangedL();

  // without paraformat, 12s
  //SetCursorPosL(aPos+len_written, EFalse);      

  // without paraformat, 3.7s
  //iLayout->SetAmountToFormat(CTextLayout::EFFormatBand);
  //SetCursorPosL(aPos+len_written, EFalse);        

  // with paraformat, 13.8s
  //HandleTextChangedL();
  //SetCursorPosL(aPos+len_written, EFalse);      

  // with paraformat, 4.6s
  //iLayout->SetAmountToFormat(CTextLayout::EFFormatBand);
  //SetCursorPosL(aPos+len_written, EFalse);        

  // with paraformat, 3.9s
  iTextView->HandleInsertDeleteL(TCursorSelection(aPos,aPos+len_written),0,ETrue);
  if (cpos > aPos)
    SetCursorPosL(cpos+len_written,EFalse);
  return aPos+len_written;
}

void CText2RichEditor::DelL(TInt aPos, TInt aLen)
{
  TInt cpos = CursorPos();
  if (CheckRange(aPos, aLen, RichText()->DocumentLength()) == EFalse)
    User::Leave(KErrArgument);
  RichText()->DeleteL(aPos, aLen);
  iTextView->HandleInsertDeleteL(TCursorSelection(aPos,aPos),aLen,ETrue);
  if (cpos > aPos)
    SetCursorPosL(cpos+aLen, EFalse);
  //HandleTextChangedL();
}

void CText2RichEditor::ApplyFormatL(TInt aPos, TInt aLen)
{
  if (CheckRange(aPos, aLen, RichText()->DocumentLength()) == EFalse)
    User::Leave(KErrArgument);

  RichText()->ApplyCharFormatL(iCharFormat, iCharFormatMask, aPos, aLen);
}

TInt CText2RichEditor::SetPos(TInt aPos, TBool aSelect)
{
  if (aPos > RichText()->DocumentLength())
    aPos = RichText()->DocumentLength();
  TRAPD(error, SetCursorPosL(aPos, aSelect));
  return error;
}

void CText2RichEditor::UpdateCharFormatLayerL(void)
{
    iMyCharFormatLayer->SetL(iCharFormat, iCharFormatMask);    
}

void CText2RichEditor::SetBold(TBool aOpt)
{
  iCharFormatMask.SetAttrib(EAttFontStrokeWeight);

  if (aOpt)
    iCharFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
  else
    iCharFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
  UpdateCharFormatLayerL();
}

void CText2RichEditor::SetUnderline(TBool aOpt)
{
  iCharFormatMask.SetAttrib(EAttFontUnderline);
  if (aOpt)
    iCharFormat.iFontPresentation.iUnderline = EUnderlineOn;
  else
    iCharFormat.iFontPresentation.iUnderline = EUnderlineOff;
  UpdateCharFormatLayerL();
}

void CText2RichEditor::SetPosture(TBool aOpt)
{
  iCharFormatMask.SetAttrib(EAttFontPosture);

  if (aOpt)
    iCharFormat.iFontSpec.iFontStyle.SetPosture(EPostureItalic);
  else
    iCharFormat.iFontSpec.iFontStyle.SetPosture(EPostureUpright);
  UpdateCharFormatLayerL();
}

void CText2RichEditor::SetStrikethrough(TBool aOpt)
{
  iCharFormatMask.SetAttrib(EAttFontStrikethrough);

  if (aOpt)
    iCharFormat.iFontPresentation.iStrikethrough = EStrikethroughOn;
  else
    iCharFormat.iFontPresentation.iStrikethrough = EStrikethroughOff;
  UpdateCharFormatLayerL();
}

void CText2RichEditor::SetHighlight(TInt mode)
{
  iCharFormatMask.SetAttrib(EAttFontHighlightStyle);

  switch (mode) {
  case KHIGHLIGHTNONE:
    iCharFormat.iFontPresentation.iHighlightStyle = TFontPresentation::EFontHighlightNone;
    break;
  case KHIGHLIGHTNORMAL:
    iCharFormat.iFontPresentation.iHighlightStyle = TFontPresentation::EFontHighlightNormal;
    break;
  case KHIGHLIGHTROUNDED:
    iCharFormat.iFontPresentation.iHighlightStyle = TFontPresentation::EFontHighlightRounded;
    break;
  case KHIGHLIGHTSHADOW:
    iCharFormat.iFontPresentation.iHighlightStyle = TFontPresentation::EFontHighlightShadow;
    break;
  }
  UpdateCharFormatLayerL();
}

void CText2RichEditor::SetTextColor(TRgb color)
{
  iCharFormatMask.SetAttrib(EAttColor);
  iCharFormat.iFontPresentation.iTextColor = color;
  UpdateCharFormatLayerL();
}

void CText2RichEditor::SetHighlightColor(TRgb color)
{
  iCharFormatMask.SetAttrib(EAttFontHighlightColor);
  iCharFormat.iFontPresentation.iHighlightColor = color;
  UpdateCharFormatLayerL();
}

void CText2RichEditor::SetFont(TFontSpec& aFontSpec)
{
  /* XXX Necessary???
   * 
   *   iCoeEnv = CEikRichTextEditor::ControlEnv();

  TRAPD(error,
	(aFont = iCoeEnv->CreateScreenFontL(*aFontSpec) ));

  if (error != KErrNone) {
    SPyErr_SetFromSymbianOSErr(error);
    return;
  }
*/
  iCharFormatMask.SetAttrib(EAttFontHeight);
  iCharFormatMask.SetAttrib(EAttFontPosture);
  iCharFormatMask.SetAttrib(EAttFontStrokeWeight);
  iCharFormatMask.SetAttrib(EAttFontPrintPos);
  iCharFormatMask.SetAttrib(EAttFontTypeface);
  //iCharFormatMask.SetAll();
  iCharFormat.iFontSpec = aFontSpec;
  
  UpdateCharFormatLayerL();
}

static Text2_object *PyCObject_AsText2(PyObject *co)
{
	Text2_object *o;
	
	if (PyCObject_Check(co)) {
		o = (Text2_object *) PyCObject_AsVoidPtr(co);
		if (o->ob_type == &Text2_type)
			return o;
	}
	
	PyErr_SetString(PyExc_TypeError, "invalid Text2 uicontrolapi object");
	return NULL;
}

static void Text2_destroy(Text2_object *o)
{
	if (o->control)
		delete o->control;
	if (o->event_bindings)
		delete o->event_bindings;
	delete o;
}

PyObject* Text2_create(PyObject* /*self*/, PyObject *args)
{
	int flags=EEikEdwinNoAutoSelection| // 0x00000008
		EEikEdwinAlwaysShowSelection| // 0x00001000
		EEikEdwinInclusiveSizeFixed; // 0x00000100
	int scrollbar=1, skinned=0;
	PyObject *move_callback=NULL, *edit_callback=NULL;
	Text2_object *o;
	CCoeControl *container;
	TInt error;

	if (!PyArg_ParseTuple(args, "|iiiOO", &flags, &scrollbar, &skinned,
			&move_callback, &edit_callback))
		return NULL;
	
	if (move_callback) {
		if (move_callback == Py_None)
			move_callback = NULL;
		else if (!PyCallable_Check(move_callback)) {
			PyErr_SetString(PyExc_TypeError, "move callback must be a callable");
			return NULL;
		}
	}
	
	if (edit_callback) {
		if (edit_callback == Py_None)
			edit_callback = NULL;
		else if (!PyCallable_Check(edit_callback)) {
			PyErr_SetString(PyExc_TypeError, "edit callback must be a callable");
			return NULL;
		}
	}

	o = new Text2_object;
	if (!o)
		return PyErr_NoMemory();
		
	o->control = NULL;
	o->event_bindings = NULL;

	o->event_bindings = new CAppuifwEventBindingArray;
	if (!o->event_bindings) {
		Text2_destroy(o);
		return PyErr_NoMemory();
	}
	
	o->control = new CText2RichEditor;
	if (!o->control) {
		Text2_destroy(o);
		return PyErr_NoMemory();
	}

	container = get_app()->ob_data->appui->iContainer;

	TRAP(error,
		o->control->ConstructL(container, flags, scrollbar,
			skinned, move_callback, edit_callback);
	);

	if (error != KErrNone) {
		Text2_destroy(o);
		return SPyErr_SetFromSymbianOSErr(error);
	}

	PyObject *pyfontname = Py_BuildValue("s", "dense");
	TFontSpec fontspec;
	TFontSpec_from_python_fontspec(pyfontname, fontspec,
		*CEikonEnv::Static()->ScreenDevice());
	o->control->SetFont(fontspec);
	Py_DECREF(pyfontname);
	
	o->ob_type = (struct _typeobject *) &Text2_type;

	return PyCObject_FromVoidPtr(o, (void (*)(void *)) Text2_destroy);
}

PyObject* Text2_clear(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		o->control->ClearL();
	)
	
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_get_text(PyObject* /*self*/, PyObject *args)
{
	PyObject *co, *r=NULL;
	int pos=0, count=-1;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "O|ii", &co, &pos, &count))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		o->control->ReadL(r, pos, count)
	)
	
	if (error != KErrNone)
		return SPyErr_SetFromSymbianOSErr(error);
	
	return r;
}

PyObject* Text2_set_text(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Py_UNICODE *ctext;
	int ctextlen;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "Ou#", &co, &ctext, &ctextlen))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		o->control->ClearL();
		o->control->WriteL(TPtrC((TUint16 *)ctext, ctextlen));
	)
	
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_add_text(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Py_UNICODE *ctext;
	int ctextlen, pos;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "Ou#", &co, &ctext, &ctextlen))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		pos = o->control->WriteL(TPtrC((TUint16 *)ctext, ctextlen));
		o->control->SetCursorPosL(pos, EFalse);
	)
	
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_insert_text(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Py_UNICODE *ctext;
	int ctextlen, pos;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "Oiu#", &co, &pos, &ctext, &ctextlen))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	if (pos < 0)
		pos = 0;
	
	TRAP(error,
		o->control->WriteL(TPtrC((TUint16 *)ctext, ctextlen), pos);
	)
	
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_delete_text(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int pos=0, count=-1;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "O|ii", &co, &pos, &count))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		o->control->DelL(pos, count);
	)
	
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_apply(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int pos=0, count=-1;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "O|ii", &co, &pos, &count))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		o->control->ApplyFormatL(pos, count);
	)
	
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_text_length(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	return Py_BuildValue("i", o->control->Len());
}

PyObject* Text2_set_pos(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int pos, select=0;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "Oi|i", &co, &pos, &select))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	error = o->control->SetPos(pos, select);
	
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_get_pos(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	return Py_BuildValue("i", o->control->CursorPos());
}

PyObject* Text2_set_focus(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int focus;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "Oi", &co, &focus))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	o->control->SetFocus(focus ? ETrue : EFalse);
	
	RETURN_PYNONE;
}

PyObject* Text2_get_focus(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	return Py_BuildValue("i", o->control->IsFocused());
}

PyObject* Text2_set_style(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int style;
	Text2_object *o;
	
	// appuifw also allows None as style, this is done in Python here
	if (!PyArg_ParseTuple(args, "Oi", &co, &style))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	o->control->SetBold(style & KBOLD ? ETrue : EFalse);
	o->control->SetUnderline(style & KUNDERLINE ? ETrue : EFalse);
	o->control->SetPosture(style & KITALIC ? ETrue : EFalse);
	o->control->SetStrikethrough(style & KSTRIKETHROUGH ? ETrue : EFalse);
	
	if (style & HL_MASK) {
		switch (style & HL_MASK) {
		case KHIGHLIGHTNORMAL:
			o->control->SetHighlight(KHIGHLIGHTNORMAL);
			break;
		case KHIGHLIGHTROUNDED:
			o->control->SetHighlight(KHIGHLIGHTROUNDED);
			break;
		case KHIGHLIGHTSHADOW:
			o->control->SetHighlight(KHIGHLIGHTSHADOW);
			break;
		default:
			PyErr_SetString(PyExc_TypeError,
				"Expected a valid combination of flags for highlight style");
			return NULL;
		}
	}
	else
		o->control->SetHighlight(KHIGHLIGHTNONE);

	if ( (style >= KNORMAL) && (style <= (KBOLD|KUNDERLINE|KITALIC|KSTRIKETHROUGH|HL_MASK)) )
		o->control->SetStyle(style);
	else {
		PyErr_SetString(PyExc_TypeError,
			"Expected a valid combination of flags for text and highlight style");
		return NULL;
	}
	if ( (style >= KNORMAL) && (style <= (KBOLD|KUNDERLINE|KITALIC|KSTRIKETHROUGH|HL_MASK)) )
		o->control->SetStyle(style);
	else {
		PyErr_SetString(PyExc_TypeError,
			"Expected a valid combination of flags for text and highlight style");
		return NULL;
	}

	RETURN_PYNONE;
}

PyObject* Text2_get_style(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	return Py_BuildValue("i", o->control->Style());
}


PyObject* Text2_set_color(PyObject* /*self*/, PyObject *args)
{
	PyObject *co, *color;
	Text2_object *o;
	TRgb rgb;
	
	if (!PyArg_ParseTuple(args, "OO", &co, &color))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	if (!ColorSpec_AsRgb(color, &rgb)) 
		return NULL;

	o->control->SetTextColor(rgb);
	
	RETURN_PYNONE;
}

PyObject* Text2_get_color(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	return ColorSpec_FromRgb(o->control->TextColor());
}

PyObject* Text2_set_highlight_color(PyObject* /*self*/, PyObject *args)
{
	PyObject *co, *color;
	Text2_object *o;
	TRgb rgb;
	
	if (!PyArg_ParseTuple(args, "OO", &co, &color))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	if (!ColorSpec_AsRgb(color, &rgb)) 
		return NULL;

	o->control->SetHighlightColor(rgb);
	
	RETURN_PYNONE;
}

PyObject* Text2_get_highlight_color(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	return ColorSpec_FromRgb(o->control->HighlightColor());
}

PyObject* Text2_set_font(PyObject* /*self*/, PyObject *args)
{
	PyObject *co, *font;
	Text2_object *o;
	TFontSpec fontspec;
	
	if (!PyArg_ParseTuple(args, "OO", &co, &font))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	if (TFontSpec_from_python_fontspec(font, fontspec,
			*CEikonEnv::Static()->ScreenDevice()) < 0)
		return NULL;

	o->control->SetFont(fontspec);
	
	RETURN_PYNONE;
}

PyObject* Text2_get_font(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	return python_fontspec_from_TFontSpec(o->control->Font(),
		*CEikonEnv::Static()->ScreenDevice());
}

PyObject* Text2_select_all(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		o->control->SelectAllL();
	)
	
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_set_read_only(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int read_only=1;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O|i", &co, &read_only))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	o->control->SetReadOnly(read_only ? TRUE : FALSE);

	RETURN_PYNONE;
}

PyObject* Text2_get_read_only(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	return Py_BuildValue("i", o->control->IsReadOnly() ? 1: 0);
}

PyObject* Text2_get_selection(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	return Py_BuildValue("ii", o->control->Selection().iCursorPos,
		o->control->Selection().iAnchorPos);
}

PyObject* Text2_set_selection(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int pos, anchorpos;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "Oii", &co, &pos, &anchorpos))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	if (anchorpos < 0)
		anchorpos = o->control->Len() - pos;

	if (pos < 0 || pos >= o->control->Len() ||
			anchorpos < 0 || anchorpos >= o->control->Len())
		return SPyErr_SetFromSymbianOSErr(KErrArgument);

	TRAP(error,
		o->control->SetSelectionL(pos, anchorpos);
	)
	
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_clear_selection(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		o->control->ClearSelectionL();
	)
	
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_set_word_wrap(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int word_wrap=1;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O|i", &co, &word_wrap))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	o->control->SetAvkonWrap(word_wrap ? TRUE : FALSE);

	RETURN_PYNONE;
}

PyObject* Text2_set_limit(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int limit=0;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "O|i", &co, &limit))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		o->control->SetTextLimit(limit);
	)
		
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_undo(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		o->control->UndoL();
	)
		
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_clear_undo(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		o->control->ClearUndo();
	)
		
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_set_allow_undo(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int allow=1;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "O|i", &co, &allow))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		o->control->SetAllowUndo(allow ? TRUE : FALSE);
	)
		
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_get_allow_undo(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	return Py_BuildValue("i", o->control->SupportsUndo() ? 1: 0);
}

PyObject* Text2_can_undo(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	return Py_BuildValue("i", o->control->CanUndo() ? 1: 0);
}

PyObject* Text2_get_word_info(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int pos=-1, startpos, length;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O|i", &co, &pos))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;

	if (pos < 0)
		pos = o->control->CursorPos();
	
	o->control->GetWordInfo(pos, startpos, length);
	
	return Py_BuildValue("ii", startpos, length);
}

PyObject* Text2_set_case(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int acase=EAknEditorTextCase;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O|i", &co, &acase))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	o->control->SetAknEditorCurrentCase(acase);

	RETURN_PYNONE;
}

PyObject* Text2_set_allowed_cases(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int cases=EAknEditorAllCaseModes;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O|i", &co, &cases))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	o->control->SetAknEditorPermittedCaseModes(cases);
		
	RETURN_PYNONE;
}

PyObject* Text2_set_input_mode(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int mode=EAknEditorTextInputMode;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O|i", &co, &mode))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	o->control->SetAknEditorCurrentInputMode(mode);

	RETURN_PYNONE;
}

PyObject* Text2_set_allowed_input_modes(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int modes=EAknEditorAllInputModes;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O|i", &co, &modes))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	o->control->SetAknEditorAllowedInputModes(modes);

	RETURN_PYNONE;
}

PyObject* Text2_set_editor_flags(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int flags;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "Oi", &co, &flags))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	o->control->SetAknEditorFlags(flags);

	RETURN_PYNONE;
}

PyObject* Text2_set_undo_buffer(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int pos=0, anchorpos=-1;
	Text2_object *o;
	TInt error;
	TBool r;
	
	if (!PyArg_ParseTuple(args, "O|ii", &co, &pos, &anchorpos))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;

	if (anchorpos < 0)
		anchorpos = o->control->Len() - pos;

	if (pos < 0 || pos >= o->control->Len() ||
			anchorpos < 0 || anchorpos >= o->control->Len())
		return SPyErr_SetFromSymbianOSErr(KErrArgument);
	
	TRAP(error,
		r = o->control->SetUndoBufferL(TCursorSelection(pos, anchorpos));
	)
	if (error != KErrNone)
		return SPyErr_SetFromSymbianOSErr(error);
	
	return Py_BuildValue("i", r ? 1 : 0);
}

PyObject* Text2_can_cut(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	return Py_BuildValue("i", o->control->CcpuCanCut() ? 1 : 0);
}

PyObject* Text2_cut(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		o->control->ClipboardL(CEikEdwin::ECut);
	)
	
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_can_copy(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	return Py_BuildValue("i", o->control->CcpuCanCopy() ? 1: 0);
}

PyObject* Text2_copy(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		o->control->ClipboardL(CEikEdwin::ECopy);
	)
	
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_can_paste(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	return Py_BuildValue("i", o->control->CcpuCanPaste() ? 1: 0);
}

PyObject* Text2_paste(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		o->control->ClipboardL(CEikEdwin::EPaste);
	)
	
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_move(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int movement, select=0;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "Oi|i", &co, &movement, &select))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		o->control->MoveCursorL(TCursorPosition::TMovementType(movement),
			TBool(select));
	)
	
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_move_display(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int movement;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "Oi", &co, &movement))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		o->control->MoveDisplayL(TCursorPosition::TMovementType(movement));
	)
	
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_set_has_changed(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int changed=1;
	Text2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "O|i", &co, &changed))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		o->control->RichText()->SetHasChanged(TBool(changed));
	)
	
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Text2_get_has_changed(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Text2_object *o;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	return Py_BuildValue("i", o->control->RichText()->HasChanged() ? 1: 0);
}

PyObject* Text2_xy2pos(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int x, y, pos;
	Text2_object *o;
	TInt error;
	TPoint p;
	
	if (!PyArg_ParseTuple(args, "O(ii)", &co, &x, &y))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	p = TPoint(x, y);
	TRAP(error,
		pos = o->control->TextView()->XyPosToDocPosL(p);
	)
	
	if (error != KErrNone)
		return SPyErr_SetFromSymbianOSErr(error);

	return Py_BuildValue("i", pos);
}

PyObject* Text2_pos2xy(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int pos;
	Text2_object *o;
	TInt error;
	TPoint p;
	TBool form;
	
	if (!PyArg_ParseTuple(args, "Oi", &co, &pos))
		return NULL;
		
	if ((o = PyCObject_AsText2(co)) == NULL)
		return NULL;
	
	TRAP(error,
		form = o->control->TextView()->DocPosToXyPosL(pos, p);
	)
	
	if (error != KErrNone)
		return SPyErr_SetFromSymbianOSErr(error);

	if (!form) {
		PyErr_SetString(PyExc_ValueError, "position not formatted");
		return NULL;
	}
	
	return Py_BuildValue("ii", p.iX, p.iY);
}
