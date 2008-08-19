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
#include <aknconsts.h>
#include <avkon.mbg>
#include <aknlists.h>
#include <eikclbd.h>
#include <gulicon.h>

#include <Python.h>
#include <symbian_python_ext_util.h>

#include "appuifwutil.h"
#include "pycallback.h"

enum ListboxType {ESingleListbox, EDoubleListbox, ESingleGraphicListbox, EDoubleGraphicListbox};

_LIT8(KAppuifwIcon, "appuifw.Icon");

class CListboxObserver;

struct Listbox2_object {
	// The following fields are required by appuifw.Application.
	// Note that PyObject_VAR_HEAD is here only to allow using of
	// this structure as a Python object. However it must always
	// be present for binary compatibility.
	PyObject_VAR_HEAD
	CEikListBox *control;
	CAppuifwEventBindingArray *event_bindings;
	// The following fields are private.
	ListboxType listbox_type;
	CListboxObserver *observer;
	CArrayPtrFlat<CGulIcon> *icons;
};

const struct _typeobject Listbox2_type={};

// appuifw.Icon object structures */

struct icon_data {
  TBuf<KMaxFileName> ob_file;
  int ob_bmpId;
  int ob_maskId;
};

struct Icon_object {
  PyObject_VAR_HEAD
  icon_data* icon;
};

_LIT(KSeparatorTab, "\t");

class CListboxObserver : public CBase, public MEikListBoxObserver
{
public:
	CListboxObserver(PyObject *aCallBack)
	{
		Py_XINCREF(aCallBack);
		iCallBack = aCallBack;
	}
	
	virtual ~CListboxObserver() {
		Py_XDECREF(iCallBack);
	}

	virtual void HandleListBoxEventL(CEikListBox* /* aListBox */, TListBoxEvent aEventType) {
		if (iCallBack && (aEventType == EEventEnterKeyPressed || 
				aEventType == EEventItemDoubleClicked))
			PyAsyncCallBack(iCallBack, Py_BuildValue("()"));
	}

private:
	PyObject *iCallBack;
};

struct Listbox2_do_init_scrollbars_params
{
	CEikListBox *control;
	CAsyncCallBack *asynccb;
};

static TInt Listbox2_do_init_scrollbars(TAny *aParams)
{
	Listbox2_do_init_scrollbars_params *params;
	
	params = (Listbox2_do_init_scrollbars_params *)aParams;
	CEikScrollBar *s;

	TRAPD(error,
		s = params->control->ScrollBarFrame()->GetScrollBarHandle(CEikScrollBar::EHorizontal);
		if (s)
			s->MakeVisible(EFalse);
		s = params->control->ScrollBarFrame()->GetScrollBarHandle(CEikScrollBar::EVertical);
		if (s)
			s->MakeVisible(EFalse);
		params->control->UpdateScrollBarsL();
	);
	
	delete params->asynccb;
	delete params;

	return error;
}

static void Listbox2_init_scrollbars(CEikListBox *aControl)
{
	CAsyncCallBack *asynccb = new CAsyncCallBack(CActive::EPriorityHigh);
	if (!asynccb)
		return;
	Listbox2_do_init_scrollbars_params *params = new Listbox2_do_init_scrollbars_params;
	if (!params) {
		delete asynccb;
		return;
	}
	params->control = aControl;
	params->asynccb = asynccb;
	TCallBack cb(Listbox2_do_init_scrollbars, params);
	asynccb->Set(cb);
	asynccb->CallBack();
}

class CSingleListbox : public CAknSingleStyleListBox
{
public:
	virtual void MakeVisible(TBool aVisible)
	{
		CEikScrollBarFrame *sbar=ScrollBarFrame();

		if (sbar) {
			TRAPD(error,
				if (aVisible) {
					sbar->SetScrollBarVisibilityL(CEikScrollBarFrame::EOn,
						CEikScrollBarFrame::EAuto);
					if (!iScrollBarsInitialized) {
						// this is done because the scrollbars can have
						// a wrong position, nothing else worked
						Listbox2_init_scrollbars(this);
						iScrollBarsInitialized = ETrue;
					}
				}
				else
					sbar->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
						CEikScrollBarFrame::EOff);
			);
		}
	}

private:
	TBool iScrollBarsInitialized;
};

class CDoubleListbox : public CAknDoubleStyleListBox
{
public:
	virtual void MakeVisible(TBool aVisible)
	{
		CEikScrollBarFrame *sbar=ScrollBarFrame();

		if (sbar) {
			TRAPD(error,
				if (aVisible) {
					sbar->SetScrollBarVisibilityL(CEikScrollBarFrame::EOn,
						CEikScrollBarFrame::EAuto);
					if (!iScrollBarsInitialized) {
						Listbox2_init_scrollbars(this);
						iScrollBarsInitialized = ETrue;
					}
				}
				else
					sbar->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
						CEikScrollBarFrame::EOff);
			);
		}
	}

private:
	TBool iScrollBarsInitialized;
};

class CSingleGraphicListbox : public CAknSingleGraphicStyleListBox
{
public:
	virtual void MakeVisible(TBool aVisible)
	{
		CEikScrollBarFrame *sbar=ScrollBarFrame();

		if (sbar) {
			TRAPD(error,
				if (aVisible) {
					sbar->SetScrollBarVisibilityL(CEikScrollBarFrame::EOn,
						CEikScrollBarFrame::EAuto);
					if (!iScrollBarsInitialized) {
						Listbox2_init_scrollbars(this);
						iScrollBarsInitialized = ETrue;
					}
				}
				else
					sbar->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
						CEikScrollBarFrame::EOff);
			);
		}
	}

private:
	TBool iScrollBarsInitialized;
};

class CDoubleGraphicListbox : public CAknDoubleLargeStyleListBox
{
public:
	virtual void MakeVisible(TBool aVisible)
	{
		CEikScrollBarFrame *sbar=ScrollBarFrame();

		if (sbar) {
			TRAPD(error,
				if (aVisible) {
					sbar->SetScrollBarVisibilityL(CEikScrollBarFrame::EOn,
						CEikScrollBarFrame::EAuto);
					if (!iScrollBarsInitialized) {
						Listbox2_init_scrollbars(this);
						iScrollBarsInitialized = ETrue;
					}
				}
				else
					sbar->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
						CEikScrollBarFrame::EOff);
			);
		}
	}

private:
	TBool iScrollBarsInitialized;
};

static Listbox2_object *PyCObject_AsListbox2(PyObject *co)
{
	Listbox2_object *o;
	
	if (PyCObject_Check(co)) {
		o = (Listbox2_object *) PyCObject_AsVoidPtr(co);
		if (o->ob_type == &Listbox2_type)
			return o;
	}
	
	PyErr_SetString(PyExc_TypeError, "invalid Listbox2 uicontrolapi object");
	return NULL;
}

static void Listbox2_destroy(Listbox2_object *o)
{
	if (o->control) {
		switch (o->listbox_type) {
		case ESingleListbox:
			delete (CSingleListbox *)o->control;
			break;
		case EDoubleListbox:
			delete (CDoubleListbox *)o->control;
			break;
		case ESingleGraphicListbox:
			delete (CSingleGraphicListbox *)o->control;
			break;
		case EDoubleGraphicListbox:
			delete (CDoubleGraphicListbox *)o->control;
			break;
		}
	}
	if (o->event_bindings)
		delete o->event_bindings;
	if (o->observer)
		delete o->observer;
	delete o;
}

PyObject* Listbox2_create(PyObject* /*self*/, PyObject *args)
{
	int listbox_type=ESingleListbox;
	int flags=EAknListBoxSelectionList|EAknListBoxLoopScrolling;
	PyObject *select_callback=NULL;
	Listbox2_object *o;
	CCoeControl *container;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "|iiO", &listbox_type, &flags,
			&select_callback))
		return NULL;
	
	if (select_callback) {
		if (select_callback == Py_None)
			select_callback = NULL;
		else if (!PyCallable_Check(select_callback)) {
			PyErr_SetString(PyExc_TypeError, "select callback must be a callable");
			return NULL;
		}
	}

	o = new Listbox2_object;
	if (!o)
		return PyErr_NoMemory();
		
	o->control = NULL;
	o->event_bindings = NULL;
	o->observer = NULL;
	o->icons = NULL;
	o->listbox_type = (ListboxType) listbox_type;

	o->event_bindings = new CAppuifwEventBindingArray;
	if (!o->event_bindings) {
		Listbox2_destroy(o);
		return PyErr_NoMemory();
	}
	
	switch (listbox_type) {
	case ESingleListbox:
		o->control = new CSingleListbox;
		if (flags & EAknListBoxMultipleSelection)
			o->icons = new CArrayPtrFlat<CGulIcon>(5);
		break;
	case EDoubleListbox:
		o->control = new CDoubleListbox;
		if (flags & EAknListBoxMultipleSelection)
			o->icons = new CArrayPtrFlat<CGulIcon>(5);
		break;
	case ESingleGraphicListbox:
		o->control = new CSingleGraphicListbox;
		o->icons = new CArrayPtrFlat<CGulIcon>(5);
		break;
	case EDoubleGraphicListbox:
		o->control = new CDoubleGraphicListbox;
		o->icons = new CArrayPtrFlat<CGulIcon>(5);
		break;
	default:
		Listbox2_destroy(o);
		PyErr_SetString(PyExc_ValueError, "invalid listbox type");
		return NULL;
	}

	if(!o->control) {
		Listbox2_destroy(o);
		return PyErr_NoMemory();
	}

	container = get_app()->ob_data->appui->iContainer;

	TRAP(error,
		o->control->SetContainerWindowL(*container);
		switch (listbox_type) {
		case ESingleListbox:
			((CSingleListbox *)o->control)->
				ConstructL(container, flags);
			break;
		case EDoubleListbox:
			((CDoubleListbox *)o->control)->
				ConstructL(container, flags);
			break;
		case ESingleGraphicListbox:
			((CSingleGraphicListbox *)o->control)->
				ConstructL(container, flags);
			break;
		case EDoubleGraphicListbox:
			((CDoubleGraphicListbox *)o->control)->
				ConstructL(container, flags);
			break;
		}
	);
	
	if (error != KErrNone) {
		Listbox2_destroy(o);
		return SPyErr_SetFromSymbianOSErr(error);
	}

	TRAP(error,
		o->control->CreateScrollBarFrameL(ETrue);
	);

	if (error != KErrNone) {
		Listbox2_destroy(o);
		return SPyErr_SetFromSymbianOSErr(error);
	}

	if (o->icons) {
		switch (listbox_type) {
		case ESingleListbox:
			((CSingleListbox *)o->control)->
				ItemDrawer()->ColumnData()->SetIconArray(o->icons);
			break;
		case EDoubleListbox:
			((CDoubleListbox *)o->control)->
				ItemDrawer()->ColumnData()->SetIconArray(o->icons);
			break;
		case ESingleGraphicListbox:
			((CSingleGraphicListbox *)o->control)->
				ItemDrawer()->ColumnData()->SetIconArray(o->icons);
			break;
		case EDoubleGraphicListbox:
			((CDoubleGraphicListbox *)o->control)->
				ItemDrawer()->ColumnData()->SetIconArray(o->icons);
			break;
		}			

		if (flags & EAknListBoxMultipleSelection) {
			// add mark icon
			TRAP(error,
				CFbsBitmap *markBitmap=NULL;
				CFbsBitmap *markBitmapMask=NULL;
				TRgb defaultColor=CEikonEnv::Static()->Color(EColorControlText);
			
				AknsUtils::CreateColorIconLC(AknsUtils::SkinInstance(),
					KAknsIIDQgnIndiMarkedAdd,
					KAknsIIDQsnIconColors,
					EAknsCIQsnIconColorsCG13,
					markBitmap,
					markBitmapMask,
					KAvkonBitmapFile,
					EMbmAvkonQgn_indi_marked_add,
					EMbmAvkonQgn_indi_marked_add_mask,
					defaultColor);
				o->icons->AppendL(CGulIcon::NewL(markBitmap, markBitmapMask));
				CleanupStack::Pop(2);
			);

			if (error != KErrNone) {
				Listbox2_destroy(o);
				return SPyErr_SetFromSymbianOSErr(error);
			}
		}
	}

       	if (select_callback) {
		o->observer = new CListboxObserver(select_callback);
		if (!o->observer) {
			Listbox2_destroy(o);
			return PyErr_NoMemory();
		}
	
		o->control->SetListBoxObserver(o->observer);
	}

	o->ob_type = (struct _typeobject *) &Listbox2_type;
	
	return PyCObject_FromVoidPtr(o, (void (*)(void *)) Listbox2_destroy);
}

PyObject* Listbox2_count(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Listbox2_object *o;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if (!(o = PyCObject_AsListbox2(co)))
		return NULL;

	return Py_BuildValue("i", o->control->Model()->NumberOfItems());
}

PyObject* Listbox2_insert(PyObject* /*self*/, PyObject *args)
{
	PyObject *co, *pyicon=NULL;
	Py_UNICODE *ctext;
	int pos, ctextlen;
	Listbox2_object *o;
	CTextListBoxModel *model;
	CDesCArray *items;
	CGulIcon *icon=NULL;
	TBuf<((KMaxFileName+1)*2)> buf;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "Oiu#|O", &co, &pos, &ctext, &ctextlen, &pyicon))
		return NULL;
		
	if (!(o = PyCObject_AsListbox2(co)))
		return NULL;

	if (pos >= o->control->Model()->NumberOfItems())
		pos = -1;

	if (pyicon == Py_None)
		pyicon = NULL;
		
	if (pyicon) {
		if ((o->listbox_type != ESingleGraphicListbox &&
				o->listbox_type != EDoubleGraphicListbox) ||
				o->icons == NULL) {
			PyErr_SetString(PyExc_TypeError, "this listbox doesn't support icons");
			return NULL;
		}
		
		if (TPtrC8((const TUint8 *)pyicon->ob_type->tp_name).Compare(KAppuifwIcon)) {
			PyErr_SetString(PyExc_TypeError, "not an Icon object");
			return NULL;
		}

		struct icon_data *data=((struct Icon_object *)pyicon)->icon;
		
		if (data->ob_bmpId < 0 || data->ob_bmpId > KMaxTInt16) {
			PyErr_SetString(PyExc_TypeError, "invalid icon bitmap index");
			return NULL;
		}
		if (data->ob_maskId < 0 || data->ob_maskId > KMaxTInt16) {
			PyErr_SetString(PyExc_TypeError, "invalid icon mask index");
			return NULL;
		}

#if SERIES60_VERSION >= 28
		CFbsBitmap *bitmap=NULL;
		CFbsBitmap *mask=NULL;

		TRAP(error,
			AknIconUtils::CreateIconL(bitmap, mask,
				data->ob_file, data->ob_bmpId, data->ob_maskId);
			icon = CGulIcon::NewL(bitmap, mask); 
		);
#else 	     
		TRAP(error, 
			icon = CEikonEnv::Static()->CreateIconL(
				data->ob_file, data->ob_bmpId, data->ob_maskId);
		);
#endif /* SERIES60_VERSION */		

		if (error != KErrNone)
			return SPyErr_SetFromSymbianOSErr(error);
	}
	else {
		if ((o->listbox_type == ESingleGraphicListbox ||
				o->listbox_type == EDoubleGraphicListbox) &&
				o->icons != NULL) {
			PyErr_SetString(PyExc_TypeError, "this listbox requires icons");
			return NULL;
		}
	}
	
	model = (CTextListBoxModel *)o->control->Model();
	items = (CDesCArray *)model->ItemTextArray();

	if (icon) {
		// icon will be appended to icons array so we start the format
		// with icons array length
		buf.AppendNum(o->icons->Count());
	}
	buf.Append(KSeparatorTab);
	buf.Append(ctext, Min(ctextlen, KMaxFileName));

	TRAP(error,
		if (pos < 0) {
			pos = items->Count();
			items->AppendL(buf);
		}
		else
			items->InsertL(pos, buf);
		if (icon)
			o->icons->AppendL(icon);
	)
	
	if (error != KErrNone)
		return SPyErr_SetFromSymbianOSErr(error);

	return Py_BuildValue("i", pos);
}

PyObject* Listbox2_finish_insert(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Listbox2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if (!(o = PyCObject_AsListbox2(co)))
		return NULL;

	TRAP(error,
		o->control->HandleItemAdditionL();
	);

	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Listbox2_delete(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int pos=-1, count=-1;
	Listbox2_object *o;
	CTextListBoxModel *model;
	CDesCArray *items;
	TInt error, ipos;
	
	if (!PyArg_ParseTuple(args, "O|ii", &co, &pos, &count))
		return NULL;
		
	if (!(o = PyCObject_AsListbox2(co)))
		return NULL;

	model = (CTextListBoxModel *)o->control->Model();
	items = (CDesCArray *)model->ItemTextArray();

	if (pos < 0)
		pos = 0;

	if (count < 0)
		count = model->NumberOfItems()-pos;

	if (pos+count > model->NumberOfItems()) {
		PyErr_SetString(PyExc_IndexError, "index out of range");
		return NULL;
	}

	items->Delete(pos, count);

	if (o->listbox_type == ESingleGraphicListbox ||
		o->listbox_type == EDoubleGraphicListbox)
	{
		CArrayFixFlat<TInt> *used;
		
		// build an array of used icons indexes
		TRAP(error,
			used = new (ELeave) CArrayFixFlat<TInt>(5);
	
			for (int i=0; i < items->Count(); i++) {	
				TLex lex((*items)[i].Ptr());
				lex.Val(ipos);
				used->AppendL(ipos);
			}
		);
	
		if (error != KErrNone)
			RETURN_PYNONE;
	
		// walk from the back of the list and delete unused icons
		// until a used one is found
		if (o->icons && o->icons->Count() > 5) {
			// we don't want to delete icon 0 - it's the mark icon
			for (int i=o->icons->Count()-1; i >= 1; i--) {
				ipos = 0;
				for (int j=0; j < used->Count(); j++) {
					if ((*used)[j] == i) {
						ipos = 1;
						break;
					}
				}
				if (ipos)
					break;
				else
					o->icons->Delete(i);
			}
		}
	}

	RETURN_PYNONE;
}

PyObject* Listbox2_finish_delete(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Listbox2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if (!(o = PyCObject_AsListbox2(co)))
		return NULL;

	TRAP(error,
		o->control->HandleItemRemovalL();
	)
	
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* Listbox2_top(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int pos=-1, old;
	Listbox2_object *o;
	
	if (!PyArg_ParseTuple(args, "O|i", &co, &pos))
		return NULL;
		
	if (!(o = PyCObject_AsListbox2(co)))
		return NULL;

	old = o->control->TopItemIndex();

	if (pos >= 0) {
		if (pos >= o->control->Model()->NumberOfItems()) {
			PyErr_SetString(PyExc_IndexError, "index out of range");
			return NULL;
		}

		o->control->SetTopItemIndex(pos);
	}

	return Py_BuildValue("i", old);
}

PyObject* Listbox2_bottom(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Listbox2_object *o;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if (!(o = PyCObject_AsListbox2(co)))
		return NULL;

	return Py_BuildValue("i", o->control->BottomItemIndex());
}

PyObject* Listbox2_current(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int pos=-1, old;
	Listbox2_object *o;
	
	if (!PyArg_ParseTuple(args, "O|i", &co, &pos))
		return NULL;
		
	if (!(o = PyCObject_AsListbox2(co)))
		return NULL;

	old = o->control->CurrentItemIndex();

	if (pos >= 0) {
		if (pos >= o->control->Model()->NumberOfItems()) {
			PyErr_SetString(PyExc_IndexError, "index out of range");
			return NULL;
		}

		o->control->SetCurrentItemIndex(pos);
	}

	if (old < 0) {
		RETURN_PYNONE;
	}

	return Py_BuildValue("i", old);
}

PyObject* Listbox2_make_visible(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int pos;
	Listbox2_object *o;
	
	if (!PyArg_ParseTuple(args, "Oi", &co, &pos))
		return NULL;
		
	if (!(o = PyCObject_AsListbox2(co)))
		return NULL;

	if (pos < 0 || pos >= o->control->Model()->NumberOfItems()) {
		PyErr_SetString(PyExc_IndexError, "index out of range");
		return NULL;
	}

	o->control->ScrollToMakeItemVisible(pos);
	
	RETURN_PYNONE;
}

PyObject* Listbox2_select(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	int pos, select=-1, old;
	Listbox2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "Oi|i", &co, &pos, &select))
		return NULL;
		
	if (!(o = PyCObject_AsListbox2(co)))
		return NULL;
	
	if (pos < 0 || pos >= o->control->Model()->NumberOfItems()) {
		PyErr_SetString(PyExc_IndexError, "index out of range");
		return NULL;
	}

	old = o->control->View()->ItemIsSelected(pos);
	
	if (select > 0) {
		TRAP(error,
			o->control->View()->SelectItemL(pos);
		);
		if (error != KErrNone)
			return SPyErr_SetFromSymbianOSErr(error);
	}
	else if (!select)
		o->control->View()->DeselectItem(pos);
	
	return Py_BuildValue("i", old);
}

PyObject* Listbox2_selection(PyObject* /*self*/, PyObject *args)
{
	PyObject *co, *list;
	Listbox2_object *o;
	const CListBoxView::CSelectionIndexArray *items;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if (!(o = PyCObject_AsListbox2(co)))
		return NULL;
	
	items = o->control->SelectionIndexes();
	list = PyList_New(items->Count());
	for (int i=0; i < items->Count(); i++)
		PyList_SetItem(list, i, Py_BuildValue("i", (*items)[i]));
	return list;
}

PyObject* Listbox2_clear_selection(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Listbox2_object *o;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if (!(o = PyCObject_AsListbox2(co)))
		return NULL;
	
	o->control->ClearSelection();
	
	RETURN_PYNONE;
}

PyObject* Listbox2_highlight_rect(PyObject* /*self*/, PyObject *args)
{
	PyObject *co;
	Listbox2_object *o;
	TRect rect;
	
	if (!PyArg_ParseTuple(args, "O", &co))
		return NULL;
		
	if (!(o = PyCObject_AsListbox2(co)))
		return NULL;
	
#if S60_VERSION >= 30
	rect = o->control->HighlightRect();
#else
	rect = TRect(0, 0, 0, 0);
#endif	
	return Py_BuildValue("iiii", rect.iTl.iX, rect.iTl.iY,
		rect.Width(), rect.Height());
}

PyObject* Listbox2_empty_text(PyObject* /*self*/, PyObject *args)
{
	PyObject *co, *pyold;
	Py_UNICODE *ctext=NULL;
	int ctextlen=0;
	Listbox2_object *o;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "O|u#", &co, &ctext, &ctextlen))
		return NULL;
		
	if (!(o = PyCObject_AsListbox2(co)))
		return NULL;

	const TDesC *old = o->control->View()->EmptyListText();
	pyold = PyUnicode_FromUnicode(old->Ptr(), old->Length());
	
	if (ctext) {
		TPtrC text((TUint16*)ctext, ctextlen);
		
		TRAP(error,
			o->control->View()->SetListEmptyTextL(text);
		);
		if (error != KErrNone) {
			Py_DECREF(pyold);
			return SPyErr_SetFromSymbianOSErr(error);
		}
	}

	return pyold;
}
