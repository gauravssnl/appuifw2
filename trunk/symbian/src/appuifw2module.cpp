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
#include <eikenv.h>
#include <eiklabel.h>

#include <Python.h>
#include <symbian_python_ext_util.h>

#include "appuifw2module.h"
#include "appuifwutil.h"
#include "pycallback.h"
#include "text.h"
#include "listbox2.h"

#include "Container.h"

const TInt KMaxNaviPaneTitleLength=32;
typedef TBuf<KMaxNaviPaneTitleLength> TNaviPaneTitle;

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

static PyObject *set_menu_init_callback(PyObject* /*self*/, PyObject *args)
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
		
		if (error != KErrNone) {
			PyErr_SetString(PyExc_OSError, "layout not available");
			return NULL;
		}
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
	iNaviDecorator = navipane->CreateEditorIndicatorContainerL();
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
        
		// Get the reference of the indicator container inside the gNaviDecorator
		CAknIndicatorContainer* indiContainer = 
		static_cast<CAknIndicatorContainer*>(aNaviDecorator->DecoratedControl());                            
         
		if (indiContainer && CEikStatusPaneBase::Current()) {
        		indiContainer->SetIndicatorValueL(
        			TUid::Uid(EAknNaviPaneEditorIndicatorMessageLength),
        			aTitle);
            
			indiContainer->SetIndicatorState(
				TUid::Uid(EAknNaviPaneEditorIndicatorMessageLength),
				EAknIndicatorStateOn);
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

static const PyMethodDef appuifw2_methods[] =
{
	{"set_menu_init_callback", (PyCFunction)set_menu_init_callback, METH_VARARGS},
	{"command_text", (PyCFunction)command_text, METH_VARARGS},
	{"set_navi", (PyCFunction)set_navi, METH_VARARGS},
	{"refresh", (PyCFunction)refresh, METH_NOARGS},

	//{"text_enable_scrollbars", (PyCFunction)text_enable_scrollbars, METH_VARARGS},
	{"text_select_all", (PyCFunction)text_select_all, METH_VARARGS},
	{"text_set_read_only", (PyCFunction)text_set_read_only, METH_VARARGS},
	{"text_is_read_only", (PyCFunction)text_is_read_only, METH_VARARGS},
	{"text_get_selection", (PyCFunction)text_get_selection, METH_VARARGS},
	{"text_set_selection", (PyCFunction)text_set_selection, METH_VARARGS},
	{"text_get_selection_len", (PyCFunction)text_get_selection_len, METH_VARARGS},
	{"text_clear_selection", (PyCFunction)text_clear_selection, METH_VARARGS},
	{"text_count_words", (PyCFunction)text_count_words, METH_VARARGS},
	{"text_set_wordwrap", (PyCFunction)text_set_wordwrap, METH_VARARGS},
	{"text_set_limit", (PyCFunction)text_set_limit, METH_VARARGS},
	{"text_undo", (PyCFunction)text_undo, METH_VARARGS},
	{"text_clear_undo", (PyCFunction)text_clear_undo, METH_VARARGS},
	{"text_set_allow_undo", (PyCFunction)text_set_allow_undo, METH_VARARGS},
	{"text_supports_undo", (PyCFunction)text_supports_undo, METH_VARARGS},
	{"text_can_undo", (PyCFunction)text_can_undo, METH_VARARGS},
	{"text_get_word_info", (PyCFunction)text_get_word_info, METH_VARARGS},
	{"text_set_case", (PyCFunction)text_set_case, METH_VARARGS},
	{"text_set_allowed_cases", (PyCFunction)text_set_allowed_cases, METH_VARARGS},
	{"text_set_input_mode", (PyCFunction)text_set_input_mode, METH_VARARGS},
	{"text_set_allowed_input_modes", (PyCFunction)text_set_allowed_input_modes, METH_VARARGS},
	{"text_set_editor_flags", (PyCFunction)text_set_editor_flags, METH_VARARGS},
	//{"text_set_alignment", (PyCFunction)text_set_alignment, METH_VARARGS},
	{"text_set_undoable", (PyCFunction)text_set_undoable, METH_VARARGS},
	{"text_can_cut", (PyCFunction)text_can_cut, METH_VARARGS},
	{"text_cut", (PyCFunction)text_cut, METH_VARARGS},
	{"text_can_copy", (PyCFunction)text_can_copy, METH_VARARGS},
	{"text_copy", (PyCFunction)text_copy, METH_VARARGS},
	{"text_can_paste", (PyCFunction)text_can_paste, METH_VARARGS},
	{"text_paste", (PyCFunction)text_paste, METH_VARARGS},
	//{"text_set_background_color", (PyCFunction)text_set_background_color, METH_VARARGS},
	{"text_set_edit_callback", (PyCFunction)text_set_edit_callback, METH_VARARGS},
	{"text_set_pos_callback", (PyCFunction)text_set_pos_callback, METH_VARARGS},
	{"text_move", (PyCFunction)text_move, METH_VARARGS},
	{"text_sel_pos", (PyCFunction)text_sel_pos, METH_VARARGS},
	{"text_set_has_changed", (PyCFunction)text_set_has_changed, METH_VARARGS},
	{"text_has_changed", (PyCFunction)text_has_changed, METH_VARARGS},
	{"text_test", (PyCFunction)text_test, METH_VARARGS},

	{"Listbox2_create", (PyCFunction)Listbox2_create, METH_VARARGS},
	{"Listbox2_count", (PyCFunction)Listbox2_count, METH_VARARGS},
	{"Listbox2_insert", (PyCFunction)Listbox2_insert, METH_VARARGS},
	{"Listbox2_delete", (PyCFunction)Listbox2_delete, METH_VARARGS},
	{"Listbox2_top", (PyCFunction)Listbox2_top, METH_VARARGS},
	{"Listbox2_bottom", (PyCFunction)Listbox2_bottom, METH_VARARGS},
	{"Listbox2_current", (PyCFunction)Listbox2_current, METH_VARARGS},
	{"Listbox2_make_visible", (PyCFunction)Listbox2_make_visible, METH_VARARGS},
	{"Listbox2_bind", (PyCFunction)Listbox2_bind, METH_VARARGS},
	{"Listbox2_select", (PyCFunction)Listbox2_select, METH_VARARGS},
	{"Listbox2_selection", (PyCFunction)Listbox2_selection, METH_VARARGS},
	{"Listbox2_clear_selection", (PyCFunction)Listbox2_clear_selection, METH_VARARGS},
	{"Listbox2_highlight_rect", (PyCFunction)Listbox2_highlight_rect, METH_VARARGS},
	{"Listbox2_empty_text", (PyCFunction)Listbox2_empty_text, METH_VARARGS},

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

