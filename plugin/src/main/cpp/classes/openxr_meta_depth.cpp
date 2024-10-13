/**************************************************************************/
/*  openxr_meta_depth.cpp                                                 */
/**************************************************************************/
/*                       This file is part of:                            */
/*                              GODOT XR                                  */
/*                      https://godotengine.org                           */
/**************************************************************************/
/* Copyright (c) 2022-present Godot XR contributors (see CONTRIBUTORS.md) */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "classes/openxr_meta_depth.h"
#include "extensions/openxr_meta_depth_extension_wrapper.h"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>

#include "openxr/openxr.h"

using namespace godot;

void OpenXRMetaEnvironmentDepthProvider::_bind_methods() {
	ClassDB::bind_method(D_METHOD("enable_depth", "value"), &OpenXRMetaEnvironmentDepthProvider::enable_depth);

}

OpenXRMetaEnvironmentDepthProvider::OpenXRMetaEnvironmentDepthProvider()
{
    OpenXRMetaDepthExtensionWrapper* value = reinterpret_cast<OpenXRMetaDepthExtensionWrapper*>(Engine::get_singleton()->get_singleton("OpenXRMetaEnvironmentDepthWrapper"));
    value->connect("openxr_instance_created", callable_mp(this, &OpenXRMetaEnvironmentDepthProvider::fetch_function_pointers));
}

OpenXRMetaEnvironmentDepthProvider::~OpenXRMetaEnvironmentDepthProvider()
{
}

void fetch_function_pointers(uint32_t i)
{
    godot::UtilityFunctions::print("Setting function pointers");

    XrInstance instance = (XrInstance)i;

    PFN_xrCreateEnvironmentDepthProviderMETA xrCreateEnvironmentDepthProviderMETA = nullptr;
    PFN_xrDestroyEnvironmentDepthProviderMETA xrDestroyEnvironmentDepthProviderMETA = nullptr;
    PFN_xrStartEnvironmentDepthProviderMETA xrStartEnvironmentDepthProviderMETA = nullptr;
    PFN_xrStopEnvironmentDepthProviderMETA xrStopEnvironmentDepthProviderMETA = nullptr;
    PFN_xrCreateEnvironmentDepthSwapchainMETA xrCreateEnvironmentDepthSwapchainMETA = nullptr;
    PFN_xrDestroyEnvironmentDepthSwapchainMETA xrDestroyEnvironmentDepthSwapchainMETA = nullptr;
    PFN_xrEnumerateEnvironmentDepthSwapchainImagesMETA xrEnumerateEnvironmentDepthSwapchainImagesMETA = nullptr;
    PFN_xrGetEnvironmentDepthSwapchainStateMETA xrGetEnvironmentDepthSwapchainStateMETA = nullptr;
    PFN_xrAcquireEnvironmentDepthImageMETA xrAcquireEnvironmentDepthImageMETA = nullptr;
    PFN_xrSetEnvironmentDepthHandRemovalMETA xrSetEnvironmentDepthHandRemovalMETA = nullptr;

    xrGetInstanceProcAddr(
    instance,
    "xrCreateEnvironmentDepthProviderMETA",
    reinterpret_cast<PFN_xrVoidFunction*>(&xrCreateEnvironmentDepthProviderMETA));
    xrGetInstanceProcAddr(
    instance,
    "xrDestroyEnvironmentDepthProviderMETA",
    reinterpret_cast<PFN_xrVoidFunction*>(&xrDestroyEnvironmentDepthProviderMETA));
    xrGetInstanceProcAddr(
    instance,
    "xrStartEnvironmentDepthProviderMETA",
    reinterpret_cast<PFN_xrVoidFunction*>(&xrStartEnvironmentDepthProviderMETA));
    xrGetInstanceProcAddr(
    instance,
    "xrStopEnvironmentDepthProviderMETA",
    reinterpret_cast<PFN_xrVoidFunction*>(&xrStopEnvironmentDepthProviderMETA));
    xrGetInstanceProcAddr(
    instance,
    "xrCreateEnvironmentDepthSwapchainMETA",
    reinterpret_cast<PFN_xrVoidFunction*>(&xrCreateEnvironmentDepthSwapchainMETA));
    xrGetInstanceProcAddr(
    instance,
    "xrDestroyEnvironmentDepthSwapchainMETA",
    reinterpret_cast<PFN_xrVoidFunction*>(&xrDestroyEnvironmentDepthSwapchainMETA));
    xrGetInstanceProcAddr(
    instance,
    "xrEnumerateEnvironmentDepthSwapchainImagesMETA",
    reinterpret_cast<PFN_xrVoidFunction*>(&xrEnumerateEnvironmentDepthSwapchainImagesMETA));
    xrGetInstanceProcAddr(
    instance,
    "xrGetEnvironmentDepthSwapchainStateMETA",
    reinterpret_cast<PFN_xrVoidFunction*>(&xrGetEnvironmentDepthSwapchainStateMETA));
    xrGetInstanceProcAddr(
    instance,
    "xrAcquireEnvironmentDepthImageMETA",
    reinterpret_cast<PFN_xrVoidFunction*>(&xrAcquireEnvironmentDepthImageMETA));
    xrGetInstanceProcAddr(
    instance,
    "xrSetEnvironmentDepthHandRemovalMETA",
    reinterpret_cast<PFN_xrVoidFunction*>(&xrSetEnvironmentDepthHandRemovalMETA));
}

void OpenXRMetaEnvironmentDepthProvider::enable_depth(bool value)
{
    _is_depth_enabled = value;

    if (_is_depth_enabled)
    {
        godot::UtilityFunctions::print("Depth occulusion enabled");
    }
    else
    {
        godot::UtilityFunctions::print("Depth occulusion disabled");
    }
}

