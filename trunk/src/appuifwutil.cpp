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

#include <Python.h>
#include <symbian_python_ext_util.h>

#include "appuifwutil.h"

#define UICONTROLAPI_NAME "_uicontrolapi"

static TBool IsBoundTo(const TKeyEvent& aEv1, const TKeyEvent& aEv2)
{
	return (aEv1.iModifiers ?
		((aEv2.iModifiers & aEv1.iModifiers) &&
		(aEv2.iCode == aEv1.iCode)) :
		(aEv2.iCode == aEv1.iCode));
}

CAppuifwEventBindingArray::~CAppuifwEventBindingArray()
{
	TInt i=0;

	while (i < iKey.Count()) {
		Py_XDECREF(iKey[i].iCb);
		i++;
	}
}

void CAppuifwEventBindingArray::InsertEventBindingL(SAppuifwEventBinding& aBindInfo)
{
	TInt i=0;
  
	switch (aBindInfo.iType) {
	case SAmarettoEventInfo::EKey:
		while (i < iKey.Count()) {
			if (aBindInfo.iKeyEvent.iCode < iKey[i].iKeyEvent.iCode)
				break;
			if (IsBoundTo(aBindInfo.iKeyEvent, iKey[i].iKeyEvent)) {
				Py_XDECREF(iKey[i].iCb);
				iKey.Delete(i);
				if (aBindInfo.iCb == NULL)
					return;
				break;
			}
			i++;
		}
		iKey.InsertL(i, aBindInfo);
		break;
	default:
		User::Leave(KErrArgument);
		break;
	}
}

struct _control_object *AppuifwControl_AsControl(PyObject *obj)
{
	PyObject *controlapi_cobject=NULL;
	if (!(controlapi_cobject=PyObject_GetAttrString(obj, UICONTROLAPI_NAME)) ||
			!(PyCObject_Check(controlapi_cobject))) {
		Py_XDECREF(controlapi_cobject);
		return NULL;
	}
	struct _control_object *control_object =
  		(struct _control_object *)PyCObject_AsVoidPtr(controlapi_cobject);
	Py_DECREF(controlapi_cobject);
	return control_object;
}

TInt AppuifwControl_Check(PyObject *obj)
{
	return (AppuifwControl_AsControl(obj)) ? 1 : 0;
}

/*
 * An utility for obtaining the Application object
 */

Application_object* get_app()
{
  PyInterpreterState *interp = PyThreadState_Get()->interp;
  PyObject* m = PyDict_GetItemString(interp->modules, "_appuifw");
  
  return (Application_object*)PyDict_GetItemString(PyModule_GetDict(m),
                                                   "app");
}

//
// class CAmarettoCallback
//
/*
void CAmarettoCallback::Call(void* aArg) {
  TInt error = CallImpl(aArg);     // Enter (potentially) interpreter!
  iAppUi->ReturnFromInterpreter(error);
  User::LeaveIfError(error);
}
*/
