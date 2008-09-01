/**
 * ====================================================================
 * fontspec.cpp
 *
 * Copyright (c) 2007 Nokia Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <e32std.h>
#include <e32base.h>
#include <aknutils.h>
#include <Python.h>

/*
 * Returns the TFontSpec (in twips) corresponding to the given Python form font specification object.
 * The Python font specification may be:
 *  - string: label of a font alias. 'normal', 'annotation' etc.
 *  - unicode: exact name of font to use.
 *  - tuple: font specification in the form (name, size, flags) or (name, size).
 * 
 * Return: 0 on success, -1 on error (with Python exception set)
 */
int TFontSpec_from_python_fontspec(
    PyObject *aArg,
    TFontSpec &aFontSpec,
    MGraphicsDeviceMap &aDeviceMap);

/*
 * Returns a CFont corresponding to the given Python form font specification object.
 * See TFontSpec_from_python_fontspec for the Python form font specification format.
 *
 * Return: 0 on success, -1 on error (with Python exception set)
 */
int CFont_from_python_fontspec(
    PyObject *aArg,
    CFont *&aFont,
    MGraphicsDeviceMap &aDeviceMap);

PyObject * python_fontspec_from_TFontSpec(
    const TFontSpec &aFontSpec,
    MGraphicsDeviceMap &aDeviceMap);
