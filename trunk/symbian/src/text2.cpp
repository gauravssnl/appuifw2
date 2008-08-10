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
#include <eikrted.h>
#include <txtrich.h>
#include <aknutils.h>
#include <aknedsts.h>

#include <Python.h>
#include <symbian_python_ext_util.h>

#include "appuifwutil.h"
#include "pycallback.h"

class CEdwinObserver;
class CEditObserver;

struct Text2_object {
	// The following fields are required by appuifw.Application.
	// Note that PyObject_VAR_HEAD is here only to allow using of
	// this structure as a Python object. However it must always
	// be present for binary compatibility.
	PyObject_VAR_HEAD
	CEikRichTextEditor *control;
	CAppuifwEventBindingArray *event_bindings;
	// The following fields are private.
	CEdwinObserver *edwin_observer;
	CEditObserver *edit_observer;
};

const struct _typeobject Text2_type={};

/****************** from colorspec.cpp *************************/

/*
 * Parse the given color specification.
 * Store result in *rgb.
 * 
 * Return: 1 if successful, 0 if error.
 */
 /*
static int ColorSpec_AsRgb(PyObject *color_object, TRgb *rgb)
{
	int r, g, b;
	if (PyTuple_Check(color_object)) {
		if (PyArg_ParseTuple(color_object, "iii", &r, &g, &b)) {
			*rgb = TRgb(r,g,b);
			return 1;
		}
	} else if (PyInt_Check(color_object)) {
		long x=PyInt_AS_LONG(color_object);
		// Unfortunately the Symbian TRgb color component ordering is
		// unintuitive (0x00bbggrr), so we'll just invert that here to 0x00rrggbb.
		r = (x&0xff0000) >> 16;
		g = (x&0x00ff00) >> 8;
		b = (x&0x0000ff);
		*rgb = TRgb(r, g, b);
		return 1;
	}
	PyErr_SetString(PyExc_ValueError, "invalid color specification; expected int or (r,g,b) tuple");
	return 0;
} 
*/
/***********************************************************/

#ifndef EKA2
class CRichTextEditor : public CEikRichTextEditor
#else
NONSHARABLE_CLASS(CRichTextEditor) : public CEikRichTextEditor
#endif
{
};

PyObject* Text2_create(PyObject* /*self*/, PyObject *args)
{
}








//static CEikRichTextEditor *AppuifwControl_AsRichTextEditor(PyObject *obj)
//{
//	struct _control_object *control_object=AppuifwControl_AsControl(obj);
//	
//	if (control_object)
//		return (CEikRichTextEditor *)control_object->ob_control;
//		
//	PyErr_SetString(PyExc_TypeError, "Text UI control expected");
//	return NULL;
//}
//
//class MPyEdwinObserver : public MEikEdwinObserver
//{
//public:
//	MPyEdwinObserver() : iEdwin(NULL), iNavigationCb(NULL) {
//	}
//	
//	virtual ~MPyEdwinObserver() {
//		Detach();
//		Py_XDECREF(iNavigationCb);
//	}
//	
//	virtual void HandleEdwinEventL(CEikEdwin * /*aEdwin*/, TEdwinEvent aEventType)
//	{
//		switch (aEventType) {
//			case EEventNavigation:
//				if (iNavigationCb) {
//					PyCallBack(iNavigationCb, Py_BuildValue("()"));
//				}
//				break;
//				
//			default:
//				break;
//		}
//	}
//	
//	void SetNavigationCallBack(PyObject *aCallBack)
//	{
//		if (iNavigationCb != NULL) {
//			Py_DECREF(iNavigationCb);
//			iNavigationCb = NULL;
//		}
//		if (aCallBack != NULL && aCallBack != Py_None) {
//			Py_INCREF(aCallBack);
//			iNavigationCb = aCallBack;
//		}
//	}
//	
//	void Attach(CEikEdwin *aEdwin)
//	{
//		Detach();
//		aEdwin->SetEdwinObserver(this);
//		iEdwin = aEdwin;
//	}
//	
//	void Detach()
//	{
//		if (iEdwin) {
//			iEdwin->SetEdwinObserver(NULL);
//			iEdwin = NULL;
//		}
//	}
//
//private:	
//	CEikEdwin *iEdwin;
//	PyObject *iNavigationCb;
//};
//
//static void del_edwin_observer(MPyEdwinObserver *observer)
//{
//	delete observer;
//}
//
//class MPyEditObserver : public MEditObserver
//{
//public:
//	MPyEditObserver() : iRichText(NULL), iEditCb(NULL) {
//	}
//	
//	virtual ~MPyEditObserver() {
//		Detach();
//		Py_XDECREF(iEditCb);
//	}
//	
//	virtual void EditObserver(TInt aStart, TInt aExtent)
//	{
//		if (iEditCb) {
//			PyAsyncCallBack(iEditCb, Py_BuildValue("ii", aStart, aExtent));
//		}
//	}
//	
//	void SetEditCallBack(PyObject *aCallBack)
//	{
//		if (iEditCb != NULL) {
//			Py_DECREF(iEditCb);
//			iEditCb = NULL;
//		}
//		if (aCallBack != NULL && aCallBack != Py_None) {
//			Py_INCREF(aCallBack);
//			iEditCb = aCallBack;
//		}
//	}
//	
//	void Attach(CRichText *aRichText)
//	{
//		Detach();
//		aRichText->SetEditObserver(this);
//		iRichText = aRichText;
//	}
//	
//	void Detach()
//	{
//		if (iRichText) {
//			iRichText->SetEditObserver(NULL);
//			iRichText = NULL;
//		}
//	}
//
//private:	
//	CRichText *iRichText;
//	PyObject *iEditCb;
//};
//
//static void del_edit_observer(MPyEditObserver *observer)
//{
//	delete observer;
//}
//
////PyObject* text_enable_scrollbars(PyObject* /*self*/, PyObject *args)
///*{
//	PyObject *control;
//	int v_enable=1, h_enable=1;
//	CEikRichTextEditor *ui;
//	CEikScrollBarFrame *scrollbarframe;
//	CEikScrollBarFrame::TScrollBarVisibility v_visibility, h_visibility;
//	TRect mainpane;
//	TSize size;
//	TInt error;
//	
//	if (!PyArg_ParseTuple(args, "O|ii", &control, &v_enable, &h_enable))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//
//	TRAP(error,
//		scrollbarframe = ui->CreatePreAllocatedScrollBarFrameL();
//	)
//	if (error != KErrNone)
//		return SPyErr_SetFromSymbianOSErr(error);
//	
//	if (!AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainpane)) {
//		PyErr_SetString(PyExc_OSError, "layout not available");
//		return NULL;
//	}
//		
//	size.iWidth = mainpane.Width() - (v_enable ? CEikScrollBar::DefaultScrollBarBreadth() : 0);
//	size.iHeight = mainpane.Height() - (h_enable ? CEikScrollBar::DefaultScrollBarBreadth() : 0);
//	ui->SetSize(size);
//
//	v_visibility = (v_enable ? CEikScrollBarFrame::EOn : CEikScrollBarFrame::EOff);
//	h_visibility = (h_enable ? CEikScrollBarFrame::EOn : CEikScrollBarFrame::EOff);
//
//	TRAP(error,
//		scrollbarframe->SetScrollBarVisibilityL(h_visibility, v_visibility);
//		ui->UpdateScrollBarsL();
//		ui->HandleTextChangedL();
//	)
//	
//	RETURN_ERROR_OR_PYNONE(error);
//}*/
//
//PyObject* text_select_all(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	CEikRichTextEditor *ui;
//	TInt error;
//	
//	if (!PyArg_ParseTuple(args, "O", &control))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	TRAP(error,
//		ui->SelectAllL();
//	)
//	
//	RETURN_ERROR_OR_PYNONE(error);
//}
//
//PyObject* text_set_read_only(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	int read_only=1;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "O|i", &control, &read_only))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	ui->SetReadOnly(read_only ? TRUE : FALSE);
//
//	RETURN_PYNONE;
//}
//
//PyObject* text_is_read_only(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "O", &control))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	return Py_BuildValue("i", ui->IsReadOnly() ? 1: 0);
//}
//
//PyObject* text_get_selection(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "O", &control))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	return Py_BuildValue("ii", ui->Selection().iCursorPos,
//		ui->Selection().iAnchorPos);
//}
//
//PyObject* text_set_selection(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	int pos, anchorpos;
//	CEikRichTextEditor *ui;
//	TInt error;
//	
//	if (!PyArg_ParseTuple(args, "Oii", &control, &pos, &anchorpos))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	TRAP(error,
//		ui->SetSelectionL(pos, anchorpos);
//	)
//	
//	RETURN_ERROR_OR_PYNONE(error);
//}
//
//PyObject* text_get_selection_len(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "O", &control))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	return Py_BuildValue("i", ui->SelectionLength());
//}
//
//PyObject* text_clear_selection(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	CEikRichTextEditor *ui;
//	TInt error;
//	
//	if (!PyArg_ParseTuple(args, "O", &control))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	TRAP(error,
//		ui->ClearSelectionL();
//	)
//	
//	RETURN_ERROR_OR_PYNONE(error);
//}
//
//PyObject* text_count_words(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "O", &control))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	return Py_BuildValue("i", ui->CountWords());
//}
//
//PyObject* text_set_wordwrap(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	int wordwrap=1;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "O|i", &control, &wordwrap))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	ui->SetAvkonWrap(wordwrap ? TRUE : FALSE);
//
//	RETURN_PYNONE;
//}
//
//PyObject* text_set_limit(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	int limit=0;
//	CEikRichTextEditor *ui;
//	TInt error;
//	
//	if (!PyArg_ParseTuple(args, "O|i", &control, &limit))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	TRAP(error,
//		ui->SetTextLimit(limit);
//	)
//		
//	RETURN_ERROR_OR_PYNONE(error);
//}
//
//PyObject* text_undo(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	CEikRichTextEditor *ui;
//	TInt error;
//	
//	if (!PyArg_ParseTuple(args, "O", &control))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	TRAP(error,
//		ui->UndoL();
//	)
//		
//	RETURN_ERROR_OR_PYNONE(error);
//}
//
//PyObject* text_clear_undo(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	CEikRichTextEditor *ui;
//	TInt error;
//	
//	if (!PyArg_ParseTuple(args, "O", &control))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	TRAP(error,
//		ui->ClearUndo();
//	)
//		
//	RETURN_ERROR_OR_PYNONE(error);
//}
//
//PyObject* text_set_allow_undo(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	int allow=1;
//	CEikRichTextEditor *ui;
//	TInt error;
//	
//	if (!PyArg_ParseTuple(args, "O|i", &control, &allow))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	TRAP(error,
//		ui->SetAllowUndo(allow ? TRUE : FALSE);
//	)
//		
//	RETURN_ERROR_OR_PYNONE(error);
//}
//
//PyObject* text_supports_undo(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "O", &control))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	return Py_BuildValue("i", ui->SupportsUndo() ? 1: 0);
//}
//
//PyObject* text_can_undo(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "O", &control))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	return Py_BuildValue("i", ui->CanUndo() ? 1: 0);
//}
//
//PyObject* text_get_word_info(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	int pos=-1, startpos, length;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "O|i", &control, &pos))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//
//	if (pos < 0)
//		pos = ui->CursorPos();
//	
//	ui->GetWordInfo(pos, startpos, length);
//	
//	return Py_BuildValue("ii", startpos, length);
//}
//
//PyObject* text_set_case(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	int acase=EAknEditorTextCase;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "O|i", &control, &acase))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	ui->SetAknEditorCurrentCase(acase);
//
//	RETURN_PYNONE;
//}
//
//PyObject* text_set_allowed_cases(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	int cases=EAknEditorAllCaseModes;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "O|i", &control, &cases))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	ui->SetAknEditorPermittedCaseModes(cases);
//		
//	RETURN_PYNONE;
//}
//
//PyObject* text_set_input_mode(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	int mode=EAknEditorTextInputMode;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "O|i", &control, &mode))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	ui->SetAknEditorCurrentInputMode(mode);
//
//	RETURN_PYNONE;
//}
//
//PyObject* text_set_allowed_input_modes(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	int modes=EAknEditorAllInputModes;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "O|i", &control, &modes))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	ui->SetAknEditorAllowedInputModes(modes);
//
//	RETURN_PYNONE;
//}
//
//PyObject* text_set_editor_flags(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	int flags;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "Oi", &control, &flags))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	ui->SetAknEditorFlags(flags);
//
//	RETURN_PYNONE;
//}
//
////PyObject* text_set_alignment(PyObject* /*self*/, PyObject *args)
///*{
//	PyObject *control;
//	int align=EAknEditorAlignLeft;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "O|i", &control, &align))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	ui->SetAlignment(align);
//
//	RETURN_PYNONE;
//}*/
//
//PyObject* text_set_undoable(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	int pos, anchorpos;
//	CEikRichTextEditor *ui;
//	TInt error;
//	TBool r;
//	
//	if (!PyArg_ParseTuple(args, "Oii", &control, &pos, &anchorpos))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	TRAP(error,
//		r = ui->SetUndoBufferL(TCursorSelection(pos, anchorpos));
//	)
//	if (error != KErrNone)
//		return SPyErr_SetFromSymbianOSErr(error);
//	
//	return Py_BuildValue("i", r ? 1 : 0);
//}
//
//PyObject* text_can_cut(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "O", &control))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	return Py_BuildValue("i", ui->CcpuCanCut() ? 1 : 0);
//}
//
//PyObject* text_cut(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	CEikRichTextEditor *ui;
//	TInt error;
//	
//	if (!PyArg_ParseTuple(args, "O", &control))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	TRAP(error,
//		ui->ClipboardL(CEikEdwin::ECut);
//	)
//	
//	RETURN_ERROR_OR_PYNONE(error);
//}
//
//PyObject* text_can_copy(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "O", &control))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	return Py_BuildValue("i", ui->CcpuCanCopy() ? 1: 0);
//}
//
//PyObject* text_copy(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	CEikRichTextEditor *ui;
//	TInt error;
//	
//	if (!PyArg_ParseTuple(args, "O", &control))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	TRAP(error,
//		ui->ClipboardL(CEikEdwin::ECopy);
//	)
//	
//	RETURN_ERROR_OR_PYNONE(error);
//}
//
//PyObject* text_can_paste(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "O", &control))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	return Py_BuildValue("i", ui->CcpuCanPaste() ? 1: 0);
//}
//
//PyObject* text_paste(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	CEikRichTextEditor *ui;
//	TInt error;
//	
//	if (!PyArg_ParseTuple(args, "O", &control))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	TRAP(error,
//		ui->ClipboardL(CEikEdwin::EPaste);
//	)
//	
//	RETURN_ERROR_OR_PYNONE(error);
//}
//
////PyObject* text_set_background_color(PyObject* /*self*/, PyObject *args)
///*{
//	PyObject *control;
//	PyObject *colspec;
//	CEikRichTextEditor *ui;
//	TInt error;
//	TRgb rgb;
//	
//	if (!PyArg_ParseTuple(args, "OO", &control, &colspec))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	if (!ColorSpec_AsRgb(colspec, &rgb))
//		return NULL;
//	
//	TRAP(error,
//		ui->SetSkinBackgroundControlContextL(NULL);
//		ui->SetBackgroundColorL(rgb);
//	)
//	
//	RETURN_ERROR_OR_PYNONE(error);
//}*/
//
//PyObject* text_set_edit_callback(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	PyObject *callback;
//	CEikRichTextEditor *ui;
//	MPyEditObserver *observer;
//	
//	if (!PyArg_ParseTuple(args, "OO", &control, &callback))
//		return NULL;
//
//	if (callback != Py_None && !PyCallable_Check(callback)) {
//		PyErr_SetString(PyExc_TypeError, "callable or None expected");
//		return NULL;
//	}
//
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	if (!PyObject_HasAttrString(control, "_edit_observer")) {
//		observer = new MPyEditObserver;
//		if (!observer)
//			return PyErr_NoMemory();		
//		if (PyObject_SetAttrString(control, "_edit_observer",
//				PyCObject_FromVoidPtr(observer, 
//				(void (*)(void *)) del_edit_observer)) < 0) {
//			PyErr_SetString(PyExc_TypeError, "could not set observer attribute");
//			return NULL;
//		}
//		observer->Attach(ui->RichText());
//	}
//	
//	observer = (MPyEditObserver *) PyCObject_AsVoidPtr(PyObject_GetAttrString(control,
//		"_edit_observer"));
//	if (PyErr_Occurred()) {
//		PyErr_SetString(PyExc_TypeError, "could not get observer attribute");
//		return NULL;
//	}
//
//	observer->SetEditCallBack(callback);
//
//	RETURN_PYNONE;
//}
//
//PyObject* text_set_pos_callback(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	PyObject *callback;
//	CEikRichTextEditor *ui;
//	MPyEdwinObserver *observer;
//	
//	if (!PyArg_ParseTuple(args, "OO", &control, &callback))
//		return NULL;
//
//	if (callback != Py_None && !PyCallable_Check(callback)) {
//		PyErr_SetString(PyExc_TypeError, "callable or None expected");
//		return NULL;
//	}
//
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	if (!PyObject_HasAttrString(control, "_edwin_observer")) {
//		observer = new MPyEdwinObserver;
//		if (!observer)
//			return PyErr_NoMemory();		
//		if (PyObject_SetAttrString(control, "_edwin_observer",
//				PyCObject_FromVoidPtr(observer,
//				(void (*)(void *)) del_edwin_observer)) < 0) {
//			PyErr_SetString(PyExc_TypeError, "could not set observer attribute");
//			return NULL;
//		}
//		observer->Attach(ui);
//	}
//	
//	observer = (MPyEdwinObserver *) PyCObject_AsVoidPtr(PyObject_GetAttrString(control,
//		"_edwin_observer"));
//	if (PyErr_Occurred()) {
//		PyErr_SetString(PyExc_TypeError, "could not get observer attribute");
//		return NULL;
//	}
//
//	observer->SetNavigationCallBack(callback);
//
//	RETURN_PYNONE;
//}
//
//PyObject* text_move(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	int movement, select=0;
//	CEikRichTextEditor *ui;
//	TInt error;
//	
//	if (!PyArg_ParseTuple(args, "Oi|i", &control, &movement, &select))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	TRAP(error,
//		ui->MoveCursorL(TCursorPosition::TMovementType(movement),
//			TBool(select));
//	)
//	
//	RETURN_ERROR_OR_PYNONE(error);
//}
//
//// same as appuifw.Text.set_pos but sets select to true
//PyObject* text_sel_pos(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	int pos, select=1;
//	CEikRichTextEditor *ui;
//	TInt error;
//	
//	if (!PyArg_ParseTuple(args, "Oi|i", &control, &pos, &select))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	if (pos < 0)
//		pos = 0;
//	else if (pos > ui->RichText()->DocumentLength())
//		pos = ui->RichText()->DocumentLength();
//
//	TRAP(error,
//		ui->SetCursorPosL(pos, TBool(select));
//	)
//	
//	RETURN_ERROR_OR_PYNONE(error);
//}
//
//PyObject* text_set_has_changed(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	int changed=1;
//	CEikRichTextEditor *ui;
//	TInt error;
//	
//	if (!PyArg_ParseTuple(args, "O|i", &control, &changed))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	TRAP(error,
//		ui->RichText()->SetHasChanged(TBool(changed));
//	)
//	
//	RETURN_ERROR_OR_PYNONE(error);
//}
//
//PyObject* text_has_changed(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	CEikRichTextEditor *ui;
//	
//	if (!PyArg_ParseTuple(args, "O", &control))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	return Py_BuildValue("i", ui->RichText()->HasChanged() ? 1: 0);
//}
//
//PyObject* text_test(PyObject* /*self*/, PyObject *args)
//{
//	PyObject *control;
//	int v1=0;
//	CEikRichTextEditor *ui;
//	TInt error;
//	
//	if (!PyArg_ParseTuple(args, "O|i", &control, &v1))
//		return NULL;
//		
//	if ((ui = AppuifwControl_AsRichTextEditor(control)) == NULL)
//		return NULL;
//	
//	TRAP(error,
//		//ui->AddEdwinObserverL(observer);
//	)
//	
//	RETURN_ERROR_OR_PYNONE(error);
//}
