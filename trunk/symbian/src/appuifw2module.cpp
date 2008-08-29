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
#include <aknnavi.h>
#include <aknnavide.h>
#include <eikspane.h>
#include <aknindicatorcontainer.h>
#include <aknnavilabel.h>
#include <eikenv.h>
#include <eiklabel.h>
#include <aknsutils.h>

#include <Python.h>
#include <symbian_python_ext_util.h>

#include "appuifw2module.h"
#include "appuifwutil.h"
#include "pycallback.h"
#include "listbox2.h"
#include "text2.h"

#include "Container.h"

const TInt KMaxNaviPaneTitleLength=32;
typedef TBuf<KMaxNaviPaneTitleLength> TNaviPaneTitle;

struct Control_object
{
	// The following fields are required by appuifw.Application.
	// Note that PyObject_VAR_HEAD is here only to allow using of
	// this structure as a Python object. However it must always
	// be present for binary compatibility.
	PyObject_VAR_HEAD
	CCoeControl *control;
	CAppuifwEventBindingArray *event_bindings;
};

#ifndef EKA2
class CAppuifw2MenuCallback : public CAmarettoCallback
#else
NONSHARABLE_CLASS(CAppuifw2MenuCallback) : public CAmarettoCallback
#endif
{
public:
	CAppuifw2MenuCallback(CAmarettoAppUi* aAppUi):
		CAmarettoCallback(aAppUi), iCallBack(NULL)
	{
		iApp = get_app();
		iAppUi->SetMenuDynInitFunc(this);
	}
	
	~CAppuifw2MenuCallback()
	{
		iAppUi->SetMenuDynInitFunc(&(get_app()->ob_data->ob_menu_cb));
		Py_XDECREF(iCallBack);
	}

	void Set(PyObject *aCallBack)
	{
		if (iCallBack) {
			Py_DECREF(iCallBack);
			iCallBack = NULL;
		}
		if (aCallBack) {
			Py_INCREF(aCallBack);
			iCallBack = aCallBack;
		}
	}
	
private:
	virtual TInt CallImpl(void* aArg)
	{
		if (iCallBack)
			PyCallBack(iCallBack, Py_BuildValue("()"));
			
		// now call the original callback
		// (it builds the API menu from appuifw.app.menu list)
		return CallImpl_Impl(aArg);
	}
	
	PyObject *iCallBack;
	struct Application_object *iApp;

  virtual TInt CallImpl_Impl(void* aStruct) {

    CAmarettoAppUi::TAmarettoMenuDynInitParams *param = (CAmarettoAppUi::TAmarettoMenuDynInitParams*)aStruct;
    TInt error = KErrNone;
    int subMenuCounter = 0;    
    int sz = PyList_Size(iApp->ob_menu); 

    for (int i = 0; i < sz; i++) {

      CEikMenuPaneItem::SData item;
	PyObject* pyitem = PyList_GetItem(iApp->ob_menu, i);
      PyObject* t = PyTuple_GetItem(pyitem, 0);
      if (!t || !PyUnicode_Check(t))
	item.iText = _L("!ERROR: BAD DATA!");
      else {
	item.iText.Copy(PyUnicode_AsUnicode(t),
			Min(PyUnicode_GetSize(t),
			    CEikMenuPaneItem::SData::ENominalTextLength));
	
	if ( param->iMenuId < R_PYTHON_SUB_MENU_00) {  /* first level menu */

	  PyObject* second = PyTuple_GetItem(pyitem, 1); 

	  if (PyTuple_Check(second)) { /* submenu item owner */
	  
	    item.iCommandId = EPythonMenuExtensionBase+i;
	    item.iCascadeId = R_PYTHON_SUB_MENU_00+subMenuCounter;
	    item.iFlags = 0;
		if (PyTuple_Size(pyitem) >= 3) {
			PyObject* third = PyTuple_GetItem(pyitem, 2);
			if (PyInt_Check(third))
				item.iFlags = PyInt_AsLong(third);
		}
	    item.iExtraText = _L("");

	    TRAP(error, (param->iMenuPane->AddMenuItemL(item) ));
	    if (error != KErrNone)
	      break;

	    iAppUi->subMenuIndex[i] = R_PYTHON_SUB_MENU_00+subMenuCounter;
	    subMenuCounter++;

	  } else { /* normal item */
	    item.iCommandId = EPythonMenuExtensionBase+i;
	    item.iCascadeId = 0;
	    item.iFlags = 0;
		if (PyTuple_Size(pyitem) >= 3) {
			PyObject* third = PyTuple_GetItem(pyitem, 2);
			if (PyInt_Check(third))
				item.iFlags = PyInt_AsLong(third);
		}
	    item.iExtraText = _L("");
	    TRAP(error, ( (param->iMenuPane)->AddMenuItemL(item)));
	    if (error != KErrNone)
	      break;
	    iAppUi->subMenuIndex[i] = 0;
	  } 

	} else if (param->iMenuId >= R_PYTHON_SUB_MENU_00) { /* submenu */

	  int subIdCounter = 0;
	  int itemIndex = 0;
	  for (int i=0; i<sz; i++) {
	    if (iAppUi->subMenuIndex[i] != 0) {
	      if (iAppUi->subMenuIndex[i] == param->iMenuId) {
		itemIndex = i;
		break;
	      }
	      else {
		PyObject* second = PyTuple_GetItem(PyList_GetItem(iApp->ob_menu, i), 1); 
		if (PyTuple_Check(second))
		  subIdCounter += PyTuple_Size(second);
	      }
	    }
	  }

	  PyObject* second = PyTuple_GetItem(PyList_GetItem(iApp->ob_menu, itemIndex), 1); 
	  int subsz = 0;
	  if (PyTuple_Check(second))
	    subsz = PyTuple_Size(second);
	  else {
	    PyErr_SetString(PyExc_TypeError, "tuple expected");
	    return -1;
	  }

	  iAppUi->aSubPane = param->iMenuPane;
	  if (error != KErrNone)
	    break;

	  for (int j=0; j<subsz; j++) {
	    CEikMenuPaneItem::SData subItem;
	    PyObject* submenu = PyTuple_GetItem(second, j);
	    PyObject* subTxt = PyTuple_GetItem(submenu, 0);
	    subItem.iText.Copy(PyUnicode_AsUnicode(subTxt),
			       Min(PyUnicode_GetSize(subTxt),
				   CEikMenuPaneItem::SData::ENominalTextLength));

	    subItem.iCommandId = EPythonMenuExtensionBase+sz+subIdCounter+j;
	    subItem.iCascadeId = 0;  

	    subItem.iFlags = 0;
		if (PyTuple_Size(submenu) >= 3) {
			PyObject* third = PyTuple_GetItem(submenu, 2);
			if (PyInt_Check(third))
				subItem.iFlags = PyInt_AsLong(third);
		}

	    subItem.iExtraText = _L("");
	    TRAP(error, ( (iAppUi->aSubPane)->AddMenuItemL(subItem)));
	    if (error != KErrNone)
	      break; 

	  }
	  break;
	} 
      }
    } 

    return error;
  }
};

static void del_menucallback(CAppuifw2MenuCallback *aCallBack)
{
	delete aCallBack;
}

static PyObject *patch_menu_dyn_init_callback(PyObject* /*self*/, PyObject *args)
{
	PyObject *callback, *ccb;
	CAppuifw2MenuCallback *cb;
	
	if (!PyArg_ParseTuple(args, "O", &callback))
		return NULL;

	if (!PyCallable_Check(callback)) {
		PyErr_SetString(PyExc_TypeError, "callable expected");
		return NULL;
	}

	cb = new CAppuifw2MenuCallback(get_app()->ob_data->appui);
	if (!cb)
		return PyErr_NoMemory();		
	ccb = PyCObject_FromVoidPtr(cb, (void (*)(void *)) del_menucallback);
	
	cb->Set(callback);
		
	return ccb;
}

static PyObject *command_text(PyObject* /*self*/, PyObject *args)
{
	PyObject *pyold;
	char *ctext=NULL;
	int cmd, ctextlen=0;
	CEikButtonGroupContainer *cba;
	MEikButtonGroup *group;
	CCoeControl *button;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "i|u#", &cmd, &ctext, &ctextlen))
		return NULL;
	
	if (!(cba = CEikButtonGroupContainer::Current())) {
		PyErr_SetString(PyExc_OSError, "cannot obtain current cba");
		return NULL;
	}
	
	// get the current text, based on TSS000675
	group = cba->ButtonGroup();
	button = group->GroupControlById(cmd);
	if (button && group->IsCommandVisible(cmd)) {
		CEikLabel* label = static_cast<CEikLabel*>(button->ComponentControl(0));
		const TDesC* txt = label->Text();
		pyold = PyUnicode_FromUnicode(txt->Ptr(), txt->Length());
	}
	else {
		pyold = Py_None;
		Py_INCREF(pyold);
	}

	if (ctext) {
		TPtrC text((TUint16*)ctext, ctextlen);
	
		TRAP(error,
			cba->SetCommandL(cmd, text);
			cba->DrawDeferred();
		);
		
		if (error != KErrNone)
			return SPyErr_SetFromSymbianOSErr(error);
	}

	return pyold;
}

static CAknNavigationControlContainer *GetNaviPane()
{
	CEikStatusPane *statusPane = CEikonEnv::Static()->AppUiFactory()->StatusPane();
	return static_cast<CAknNavigationControlContainer*>(
		statusPane->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
}

static CAknNavigationDecorator *CreateNaviDecoratorL()
{
	CAknNavigationDecorator *iNaviDecorator;
	
	// Get the reference of the navi pane
	CAknNavigationControlContainer* navipane=GetNaviPane();
	iNaviDecorator = navipane->CreateNavigationLabelL();
	// Add the indicator to navi pane
	navipane->PushL(*iNaviDecorator);
	
	return iNaviDecorator;
}

static void SetNaviPaneTitleTextL(CAknNavigationDecorator *aNaviDecorator, TNaviPaneTitle& aTitle)
{
	CAknNavigationControlContainer *iNaviPane=NULL;
    
	// Get the reference of the status pane
	CEikStatusPane *statusPane = CEikonEnv::Static()->AppUiFactory()->StatusPane();
 
	if (statusPane) {
		// Get the reference of the navi pane
		iNaviPane = GetNaviPane();
        
		// Get the reference of the indicator container inside the aNaviDecorator
		CAknNaviLabel* indiContainer = 
		static_cast<CAknNaviLabel*>(aNaviDecorator->DecoratedControl());                            
         
		if (indiContainer && CEikStatusPaneBase::Current()) {
        		indiContainer->SetTextL(aTitle);
		}
	}
}

static void EnableNaviPaneArrowL(CAknNavigationDecorator *aNaviDecorator,
	CAknNavigationDecorator::TScrollButton aArrow, TBool aEnable)
{
	// Popping and pushing the decorator back seems to refresh the arrows.
	// If this is not done the arrows are drawn only on first time
	// or not at all if the decorator does not have the arrows enabled.
	CAknNavigationControlContainer* navipane=GetNaviPane();
	navipane->Pop(aNaviDecorator);

	aNaviDecorator->SetScrollButtonDimmed(aArrow, !aEnable);
	aNaviDecorator->MakeScrollButtonVisible(ETrue);
  
	navipane->PushL(*aNaviDecorator);
}

static void del_navi_decorator(CAknNavigationDecorator *decor)
{
	delete decor;
}

static PyObject *set_navi(PyObject* /*self*/, PyObject *args)
{
	char *ctext=NULL;
	int leftarrow=0, rightarrow=0;
	TInt error;
	TInt ctextlen=0;
	CAknNavigationDecorator *decor=NULL;
	
	if (!PyArg_ParseTuple(args, "u#|ii", &ctext, &ctextlen, &leftarrow, &rightarrow))
		return NULL;
	
	if (ctextlen > KMaxNaviPaneTitleLength) {
		PyErr_SetString(PyExc_ValueError, "title too long");
		return NULL;
	}

	TPtrC text((TUint16*)ctext, ctextlen);
	TNaviPaneTitle title;
	title.Append(text);

	TRAP(error,
		decor = CreateNaviDecoratorL();
		SetNaviPaneTitleTextL(decor, title);
		EnableNaviPaneArrowL(decor, CAknNavigationDecorator::ELeftButton, leftarrow);
		EnableNaviPaneArrowL(decor, CAknNavigationDecorator::ERightButton, rightarrow);
	);

	if (error != KErrNone) {
		if (decor)
			delete decor;
		return SPyErr_SetFromSymbianOSErr(error);
	}

	return PyCObject_FromVoidPtr(decor, (void (*)(void *)) del_navi_decorator);
}

PyObject* refresh(PyObject* /*self*/, PyObject* /*args*/)
{
	// calls Refresh on the appui container
	((CAmarettoContainer *)(get_app()->ob_data->appui->iContainer))->Refresh();
	
	RETURN_PYNONE;
}

PyObject* bind(PyObject* /*self*/, PyObject *args)
{
	PyObject *co, *callback;
	int key_code;
	Control_object *o;
	TInt error;
	SAppuifwEventBinding bind_info;
	
	if (!PyArg_ParseTuple(args, "OiO", &co, &key_code, &callback))
		return NULL;
		
	if (!PyCObject_Check(co)) {
		PyErr_SetString(PyExc_TypeError, "invalid uicontrolapi object");
		return NULL;
	}
	
	if (callback == Py_None)
		callback = NULL;
	else if (!PyCallable_Check(callback)) {
		PyErr_SetString(PyExc_TypeError, "callable expected");
		return NULL;
	}

	bind_info.iType = SAmarettoEventInfo::EKey;
	bind_info.iKeyEvent.iCode = key_code;
	bind_info.iKeyEvent.iModifiers = 0; // not supported by appuifw, has to be 0
	bind_info.iCb = callback;

	o = (Control_object *) PyCObject_AsVoidPtr(co);

	TRAP(error,
		o->event_bindings->InsertEventBindingL(bind_info);
	);
	
	if (error == KErrNone)
		Py_XINCREF(callback);
	
	RETURN_ERROR_OR_PYNONE(error);
}

PyObject* get_skin_color(PyObject* /*self*/, PyObject *args)
{
	int major, minor, index;
	TAknsItemID id;
	TRgb color;
	TInt error;
	
	if (!PyArg_ParseTuple(args, "iii", &major, &minor, &index))
		return NULL;
	
	id.Set(major, minor);
	
	error = AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), color,
		id, index);
	
	if (error != KErrNone)
		return SPyErr_SetFromSymbianOSErr(error);
	
	return Py_BuildValue("iii", color.Red(), color.Green(), color.Blue());
}

PyObject* get_language(PyObject* /*self*/, PyObject * /*args*/)
{
	return Py_BuildValue("i", User::Language());
}


PyObject* test(PyObject* /*self*/, PyObject * /*args*/)
{
	RETURN_PYNONE;
}

static const PyMethodDef appuifw2_methods[] =
{
	{"patch_menu_dyn_init_callback", (PyCFunction)patch_menu_dyn_init_callback, METH_VARARGS},
	{"command_text", (PyCFunction)command_text, METH_VARARGS},
	{"set_navi", (PyCFunction)set_navi, METH_VARARGS},
	{"refresh", (PyCFunction)refresh, METH_NOARGS},
	{"bind", (PyCFunction)bind, METH_VARARGS},
	{"get_skin_color", (PyCFunction)get_skin_color, METH_VARARGS},
	{"get_language", (PyCFunction)get_language, METH_NOARGS},
	{"test", (PyCFunction)test, METH_NOARGS},

	{"Listbox2_create", (PyCFunction)Listbox2_create, METH_VARARGS},
	{"Listbox2_count", (PyCFunction)Listbox2_count, METH_VARARGS},
	{"Listbox2_insert", (PyCFunction)Listbox2_insert, METH_VARARGS},
	{"Listbox2_finish_insert", (PyCFunction)Listbox2_finish_insert, METH_VARARGS},
	{"Listbox2_delete", (PyCFunction)Listbox2_delete, METH_VARARGS},
	{"Listbox2_finish_delete", (PyCFunction)Listbox2_finish_delete, METH_VARARGS},
	{"Listbox2_top", (PyCFunction)Listbox2_top, METH_VARARGS},
	{"Listbox2_bottom", (PyCFunction)Listbox2_bottom, METH_VARARGS},
	{"Listbox2_current", (PyCFunction)Listbox2_current, METH_VARARGS},
	{"Listbox2_make_visible", (PyCFunction)Listbox2_make_visible, METH_VARARGS},
	{"Listbox2_select", (PyCFunction)Listbox2_select, METH_VARARGS},
	{"Listbox2_selection", (PyCFunction)Listbox2_selection, METH_VARARGS},
	{"Listbox2_clear_selection", (PyCFunction)Listbox2_clear_selection, METH_VARARGS},
	{"Listbox2_highlight_rect", (PyCFunction)Listbox2_highlight_rect, METH_VARARGS},
	{"Listbox2_empty_text", (PyCFunction)Listbox2_empty_text, METH_VARARGS},

	{"Text2_create", (PyCFunction)Text2_create, METH_VARARGS},
	{"Text2_clear", (PyCFunction)Text2_clear, METH_VARARGS},
	{"Text2_get_text", (PyCFunction)Text2_get_text, METH_VARARGS},
	{"Text2_set_text", (PyCFunction)Text2_set_text, METH_VARARGS},
	{"Text2_add_text", (PyCFunction)Text2_add_text, METH_VARARGS},
	{"Text2_insert_text", (PyCFunction)Text2_insert_text, METH_VARARGS},
	{"Text2_delete_text", (PyCFunction)Text2_delete_text, METH_VARARGS},
	{"Text2_apply", (PyCFunction)Text2_apply, METH_VARARGS},
	{"Text2_text_length", (PyCFunction)Text2_text_length, METH_VARARGS},
	{"Text2_get_pos", (PyCFunction)Text2_get_pos, METH_VARARGS},
	{"Text2_set_pos", (PyCFunction)Text2_set_pos, METH_VARARGS},
	{"Text2_get_focus", (PyCFunction)Text2_get_focus, METH_VARARGS},
	{"Text2_set_focus", (PyCFunction)Text2_set_focus, METH_VARARGS},
	{"Text2_get_style", (PyCFunction)Text2_get_style, METH_VARARGS},
	{"Text2_set_style", (PyCFunction)Text2_set_style, METH_VARARGS},
	{"Text2_get_color", (PyCFunction)Text2_get_color, METH_VARARGS},
	{"Text2_set_color", (PyCFunction)Text2_set_color, METH_VARARGS},
	{"Text2_get_highlight_color", (PyCFunction)Text2_get_highlight_color, METH_VARARGS},
	{"Text2_set_highlight_color", (PyCFunction)Text2_set_highlight_color, METH_VARARGS},
	{"Text2_get_font", (PyCFunction)Text2_get_font, METH_VARARGS},
	{"Text2_set_font", (PyCFunction)Text2_set_font, METH_VARARGS},
	{"Text2_select_all", (PyCFunction)Text2_select_all, METH_VARARGS},
	{"Text2_set_read_only", (PyCFunction)Text2_set_read_only, METH_VARARGS},
	{"Text2_get_read_only", (PyCFunction)Text2_get_read_only, METH_VARARGS},
	{"Text2_get_selection", (PyCFunction)Text2_get_selection, METH_VARARGS},
	{"Text2_set_selection", (PyCFunction)Text2_set_selection, METH_VARARGS},
	{"Text2_clear_selection", (PyCFunction)Text2_clear_selection, METH_VARARGS},
	{"Text2_set_word_wrap", (PyCFunction)Text2_set_word_wrap, METH_VARARGS},
	{"Text2_set_limit", (PyCFunction)Text2_set_limit, METH_VARARGS},
	{"Text2_undo", (PyCFunction)Text2_undo, METH_VARARGS},
	{"Text2_clear_undo", (PyCFunction)Text2_clear_undo, METH_VARARGS},
	{"Text2_set_allow_undo", (PyCFunction)Text2_set_allow_undo, METH_VARARGS},
	{"Text2_get_allow_undo", (PyCFunction)Text2_get_allow_undo, METH_VARARGS},
	{"Text2_can_undo", (PyCFunction)Text2_can_undo, METH_VARARGS},
	{"Text2_get_word_info", (PyCFunction)Text2_get_word_info, METH_VARARGS},
	{"Text2_set_case", (PyCFunction)Text2_set_case, METH_VARARGS},
	{"Text2_set_allowed_cases", (PyCFunction)Text2_set_allowed_cases, METH_VARARGS},
	{"Text2_set_input_mode", (PyCFunction)Text2_set_input_mode, METH_VARARGS},
	{"Text2_set_allowed_input_modes", (PyCFunction)Text2_set_allowed_input_modes, METH_VARARGS},
	{"Text2_set_editor_flags", (PyCFunction)Text2_set_editor_flags, METH_VARARGS},
	{"Text2_set_undo_buffer", (PyCFunction)Text2_set_undo_buffer, METH_VARARGS},
	{"Text2_can_cut", (PyCFunction)Text2_can_cut, METH_VARARGS},
	{"Text2_cut", (PyCFunction)Text2_cut, METH_VARARGS},
	{"Text2_can_copy", (PyCFunction)Text2_can_copy, METH_VARARGS},
	{"Text2_copy", (PyCFunction)Text2_copy, METH_VARARGS},
	{"Text2_can_paste", (PyCFunction)Text2_can_paste, METH_VARARGS},
	{"Text2_paste", (PyCFunction)Text2_paste, METH_VARARGS},
	{"Text2_move", (PyCFunction)Text2_move, METH_VARARGS},
	{"Text2_move_display", (PyCFunction)Text2_move_display, METH_VARARGS},
	{"Text2_set_has_changed", (PyCFunction)Text2_set_has_changed, METH_VARARGS},
	{"Text2_get_has_changed", (PyCFunction)Text2_get_has_changed, METH_VARARGS},
	{"Text2_xy2pos", (PyCFunction)Text2_xy2pos, METH_VARARGS},
	{"Text2_pos2xy", (PyCFunction)Text2_pos2xy, METH_VARARGS},
	{"Text2_get_indicator_text", (PyCFunction)Text2_get_indicator_text, METH_VARARGS},
	{"Text2_set_indicator_text", (PyCFunction)Text2_set_indicator_text, METH_VARARGS},

	{0, 0} /* sentinel */
};

DL_EXPORT(void) init_appuifw2()
{
	//PyObject* module = 
	Py_InitModule("_appuifw2", (PyMethodDef*)appuifw2_methods);
}

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
{
	return KErrNone;
}
#endif

