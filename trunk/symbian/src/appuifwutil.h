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

#ifndef __APPUIFWUTIL_H
#define __APPUIFWUTIL_H

#include <Python.h>
#include <Python_appui.h>
#include <coecntrl.h>

struct SAppuifwEventBinding : public SAmarettoEventInfo {
  PyObject* iCb;
};

#ifndef EKA2
class CAppuifwEventBindingArray : public CBase
#else
NONSHARABLE_CLASS(CAppuifwEventBindingArray) : public CBase
#endif
{
 public:
  CAppuifwEventBindingArray():iKey(5) {}
  ~CAppuifwEventBindingArray();
  void InsertEventBindingL(SAppuifwEventBinding&);
  TInt Callback(SAmarettoEventInfo& aEvent);

 private:
  CArrayFixSeg<SAppuifwEventBinding> iKey;
};

/*
 * Structure of Python objects that represent UI controls and
 * a check for control objects (hardcoded types -- bad)
 */

class CAppuifwEventBindingArray;

struct _control_object {
  PyObject_VAR_HEAD
  CCoeControl* ob_control;
  CAppuifwEventBindingArray* ob_event_bindings;
};

TInt AppuifwControl_Check(PyObject *obj);
struct _control_object *AppuifwControl_AsControl(PyObject *obj);

struct Application_data;
struct Application_object;

#ifndef EKA2
class CAppuifwCallback : public CAmarettoCallback
#else
NONSHARABLE_CLASS(CAppuifwCallback) : public CAmarettoCallback
#endif
{
public:
  CAppuifwCallback(Application_object* aApp, CAmarettoAppUi* aAppUi);

protected:
  Application_object* iApp;

private:
  virtual TInt CallImpl_Impl(void* aArg)=0;
  virtual TInt CallImpl(void* aArg);
};

class CAppuifwEventCallback : public CAppuifwCallback
{
public:
  CAppuifwEventCallback(Application_object* aApp,
                        CAmarettoAppUi* aAppUi);

private:
  virtual TInt CallImpl_Impl(void* aArg);
};

class CAppuifwCommandCallback : public CAppuifwCallback
{
public:
  CAppuifwCommandCallback(Application_object* aApp,
                          CAmarettoAppUi* aAppUi);

private:
  virtual TInt CallImpl_Impl(void* aArg);
};

class CAppuifwMenuCallback : public CAppuifwCallback
{
public:
  CAppuifwMenuCallback(Application_object* aApp, 
                       CAmarettoAppUi* aAppUi);

private:
  virtual TInt CallImpl_Impl(void* aStruct);
};

class CAppuifwFocusCallback : public CAppuifwCallback
{
public:
  CAppuifwFocusCallback(Application_object* aApp,
                          CAmarettoAppUi* aAppUi);
  
  ~CAppuifwFocusCallback();
  
  int Set(PyObject* aFunc);
  
  PyObject* Get();

private:
  virtual TInt CallImpl_Impl(void* aArg);

  PyObject* iFunc;
};

class CAppuifwExitKeyCallback : public CAppuifwCallback
{
public:
  CAppuifwExitKeyCallback(Application_object* aApp,
                          CAmarettoAppUi* aAppUi);
  
  ~CAppuifwExitKeyCallback();
  
  int Set(PyObject* aFunc);
  
  PyObject* Get();

private:
  virtual TInt CallImpl_Impl(void* /*aArg*/);

  PyObject* iFunc;
};

class CAppuifwTabCallback : public CAppuifwCallback
{
public:
  CAppuifwTabCallback(Application_object* aApp, CAmarettoAppUi* aAppUi);

  ~CAppuifwTabCallback();

  TInt Set(PyObject* aFunc);

private:
  virtual TInt CallImpl_Impl(void* aArg);

  PyObject* iFunc;
};

/*
 * appuifw.Application object type representation
 */

struct Application_object {
  PyObject_VAR_HEAD
  PyObject *ob_dict_attr;
  PyObject *ob_menu;
  PyObject *ob_body;
  PyObject *ob_title;
  PyObject *ob_screen;
  Application_data *ob_data;
#ifdef EKA2
  PyObject *ob_orientation;
#endif  
};

struct Application_data {
  Application_data(CAmarettoAppUi* aAppUi, Application_object* aOp);
  ~Application_data();

  CAmarettoAppUi* appui;
  TInt rsc_offset;
  CAppuifwExitKeyCallback ob_exit_key_cb;
  CAppuifwEventCallback ob_event_cb;
  CAppuifwCommandCallback ob_command_cb;
  CAppuifwMenuCallback ob_menu_cb;
  CAppuifwTabCallback ob_tab_cb;
  CAppuifwFocusCallback ob_focus_cb;
};

#define RETURN_PYNONE Py_INCREF(Py_None); return Py_None;

Application_object* get_app();

#endif /* __APPUIFWUTIL_H */
