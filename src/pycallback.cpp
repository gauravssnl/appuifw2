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

#include <Python.h>
#include <symbian_python_ext_util.h>

/* aCallBackTuple is a pointer to a tuple (PyObject *):
**   (callable, args_tuple, asynccallback)
** asynccallback is optional, it can be a PyCObject object
** containing a pointer to a CAsyncCallBack object which
** will be deleted.
*/
static TInt PyDoCallBack(TAny *aCallBackTuple)
{
	PyObject *callable, *args, *pyasynccb=NULL, *ret;

	PyEval_RestoreThread(PYTHON_TLS->thread_state);

	// extract callable and args from the tuple	
	if (PyArg_ParseTuple((PyObject *) aCallBackTuple, "OO|O", &callable, &args, &pyasynccb)) {	
		ret = PyEval_CallObject(callable, args); // do the call
		if (ret)
			Py_DECREF(ret);
		else
			PyErr_Print();
	}

	// if CAsyncCallBack was specified, delete it
	if (pyasynccb) {
		CAsyncCallBack *asynccb = (CAsyncCallBack *) PyCObject_AsVoidPtr(pyasynccb);
		delete asynccb;
	}

	// the caller is CAsyncCallBack so we have to decref (free) the args tuple
	Py_DECREF((PyObject *) aCallBackTuple);

	PyEval_SaveThread();

	return 0;
}

/* Calls a Python callable. NOTE! Refcount of aArgs is decreased
** by one. Use Py_BuildValue() in the call directly.
*/
TInt PyCallBack(PyObject *aCallable, PyObject *aArgs)
{
	return PyDoCallBack(Py_BuildValue("ON", aCallable, aArgs));
}

/* Asynchronously calls a Python callable. NOTE! Refcount of aArgs
** is decreased by one. Use Py_BuildValue() in the call directly.
*/
void PyAsyncCallBack(PyObject *aCallable, PyObject *aArgs)
{
	CAsyncCallBack *asynccb = new CAsyncCallBack(CActive::EPriorityHigh);
	if (!asynccb)
		return;
	TCallBack cb(PyDoCallBack, Py_BuildValue("ONN",
		aCallable, aArgs, PyCObject_FromVoidPtr(asynccb, NULL)));
	asynccb->Set(cb);
	asynccb->CallBack();
}
