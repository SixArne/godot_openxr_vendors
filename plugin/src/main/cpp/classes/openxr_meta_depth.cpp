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
#include <godot_cpp/classes/open_xrapi_extension.hpp>

#include "openxr/openxr.h"

using namespace godot;

// Docs:
// https://developers.meta.com/horizon/documentation/native/android/mobile-depth

void OpenXRMetaEnvironmentDepthProvider::_bind_methods() {
	ClassDB::bind_method(D_METHOD("enable_depth", "value"), &OpenXRMetaEnvironmentDepthProvider::enable_depth);
	ClassDB::bind_method(D_METHOD("enable_hands", "value"), &OpenXRMetaEnvironmentDepthProvider::enable_hand_removal);

	ADD_SIGNAL(MethodInfo("_on_depth_provider_started", PropertyInfo(Variant::Type::NIL, "")));
	ADD_SIGNAL(MethodInfo("_on_depth_provider_stopped", PropertyInfo(Variant::Type::NIL, "")));
	ADD_SIGNAL(MethodInfo("_on_depth_provider_error", PropertyInfo(Variant::Type::STRING, "error_message")));
}

OpenXRMetaEnvironmentDepthProvider::OpenXRMetaEnvironmentDepthProvider()
{
    m_DepthExtensionWrapper = reinterpret_cast<OpenXRMetaDepthExtensionWrapper*>(Engine::get_singleton()->get_singleton("OpenXRMetaEnvironmentDepthWrapper"));
    m_DepthExtensionWrapper->connect("openxr_instance_created", callable_mp(this, &OpenXRMetaEnvironmentDepthProvider::fetch_function_pointers));
    m_DepthExtensionWrapper->connect("openxr_session_created", callable_mp(this, &OpenXRMetaEnvironmentDepthProvider::on_session_created));
    m_DepthExtensionWrapper->connect("openxr_session_destroyed", callable_mp(this, &OpenXRMetaEnvironmentDepthProvider::on_session_destroyed));
}

OpenXRMetaEnvironmentDepthProvider::~OpenXRMetaEnvironmentDepthProvider()
{
}

void OpenXRMetaEnvironmentDepthProvider::_process(double delta)
{
    m_Depth_map_was_accessed_this_frame = false;

    if (m_Is_depth_enabled)
    {
        // TODO: Enumerate here? we need to get the XrSwapchainImageBaseHeader though.....
        enumerate_depth_swapchain_images();
    }
}

void OpenXRMetaEnvironmentDepthProvider::enumerate_depth_swapchain_images()
{
    // TODO: Look into iteration, probably in process while the depth provider is working?

    // XrResult result = xrEnumerateEnvironmentDepthSwapchainImagesMETA(
    //     m_environmentDepthSwapchain,
    //     uint32_t imageCapacityInput,
    //     uint32_t* imageCountOutput,
    //     XrSwapchainImageBaseHeader* images); //STUB - This might need to be tightly linked to the actual vulkan rendering, which at first glance seems out of reach
    //                                          // for GDExtension?
}

void OpenXRMetaEnvironmentDepthProvider::on_session_destroyed()
{
    xrDestroyEnvironmentDepthProviderMETA(m_environmentDepthProvider);
    xrDestroyEnvironmentDepthSwapchainMETA(m_environmentDepthSwapchain);
}

void OpenXRMetaEnvironmentDepthProvider::on_session_created(uint64_t session)
{
    create_depth_provider(session);
}

void OpenXRMetaEnvironmentDepthProvider::aquire_depth_map()
{
    if (! m_Depth_map_was_accessed_this_frame)
    {
        XrEnvironmentDepthImageAcquireInfoMETA aquireInfo{};
        aquireInfo.type = XR_TYPE_ENVIRONMENT_DEPTH_IMAGE_ACQUIRE_INFO_META;
        aquireInfo.next = nullptr;
        aquireInfo.displayTime = (XrTime)m_DepthExtensionWrapper->get_openxr_api()->get_predicted_display_time();
        aquireInfo.space = (XrSpace)m_DepthExtensionWrapper->get_openxr_api()->get_play_space();

        XrEnvironmentDepthImageMETA imageInfo {};
        imageInfo.type = XR_TYPE_ENVIRONMENT_DEPTH_IMAGE_META;

        XrResult result = xrAcquireEnvironmentDepthImageMETA(
            m_environmentDepthProvider,
            aquireInfo,
            imageInfo);

        if (result != XrResult::XR_SUCCESS)
        {
            godot::UtilityFunctions::printerr("Failed to aquire depth map");
        }

        m_Depth_map_was_accessed_this_frame = true;
    }
}

DepthMapSize OpenXRMetaEnvironmentDepthProvider::get_depth_map_size()
{
    XrEnvironmentDepthSwapchainStateMETA depthSwapchainState{};
    depthSwapchainState.type = XR_TYPE_ENVIRONMENT_DEPTH_SWAPCHAIN_STATE_META;
    depthSwapchainState.next = nullptr;
    depthSwapchainState.width = 0;
    depthSwapchainState.height = 0;

    XrResult result = xrGetEnvironmentDepthSwapchainStateMETA(
        m_environmentDepthSwapchain,
        depthSwapchainState
    );

    if (result != XrResult::XR_SUCCESS)
    {
        godot::UtilityFunctions::printerr("Failed to get depth map size");
    }

    DepthMapSize size{};
    size.width = depthSwapchainState.width;
    size.height = depthSwapchainState.height;

    return size;
}

void OpenXRMetaEnvironmentDepthProvider::enable_hand_removal(bool value)
{
    XrEnvironmentDepthHandRemovalSetInfoMETA handRemovalSetInfo{};
    handRemovalSetInfo.type = XR_TYPE_ENVIRONMENT_DEPTH_HAND_REMOVAL_SET_INFO_META;
    handRemovalSetInfo.next = nullptr;
    handRemovalSetInfo.enabled = value;

    XrResult result = xrSetEnvironmentDepthHandRemovalMETA(
        m_environmentDepthProvider,
        handRemovalSetInfo);

    if (result != XrResult::XR_SUCCESS)
    {
        if (value)
        {
            godot::UtilityFunctions::printerr("Failed to enable hand removal");
        }
        else
        {
            godot::UtilityFunctions::printerr("Failed to disable hand removal");
        }
    }
}

void OpenXRMetaEnvironmentDepthProvider::create_depth_provider()
{
    XrEnvironmentDepthProviderCreateInfoMETA createInfo{};
    createInfo.type = XrStructureType::XR_TYPE_ENVIRONMENT_DEPTH_PROVIDER_CREATE_INFO_META;
    createInfo.next = nullptr;
    createInfo.createFlags = 0;

    XrResult result = xrCreateEnvironmentDepthProviderMETA(
        (XrSession)m_DepthExtensionWrapper->get_openxr_api()->get_session(),
        createInfo,
        m_environmentDepthProvider);

    if (result != XrResult::XR_SUCCESS)
    {
        godot::UtilityFunctions::printerr("Failed to create depth provider");
    }
    else
    {
        create_depth_swapchains();
    }
}

void OpenXRMetaEnvironmentDepthProvider::create_depth_swapchains()
{
    XrEnvironmentDepthSwapchainCreateInfoMETA depthSwapchainCreateInfo {};
    depthSwapchainCreateInfo.type = XrStructureType::XR_TYPE_ENVIRONMENT_DEPTH_SWAPCHAIN_CREATE_INFO_META;
    depthSwapchainCreateInfo.next = nullptr;
    depthSwapchainCreateInfo.createFlags = 0;

    XrResult result = xrCreateEnvironmentDepthSwapchainMETA(
        m_environmentDepthProvider,
        depthSwapchainCreateInfo,
        m_environmentDepthSwapchain);

    if (result != XrResult::XR_SUCCESS)
    {
        godot::UtilityFunctions::printerr("Failed to create depth swapchains");
    }
}

void OpenXRMetaEnvironmentDepthProvider::fetch_function_pointers()
{
    godot::UtilityFunctions::print("Setting function pointers");

    XrInstance instance = (XrInstance)m_DepthExtensionWrapper->get_openxr_api()->get_instance();

    xrGetInstanceProcAddr(
        instance,
        "xrCreateEnvironmentDepthProviderMETA",
        reinterpret_cast<PFN_xrVoidFunction*>(&m_xrCreateEnvironmentDepthProviderMETA));

    xrGetInstanceProcAddr(
        instance,
        "xrDestroyEnvironmentDepthProviderMETA",
        reinterpret_cast<PFN_xrVoidFunction*>(&m_xrDestroyEnvironmentDepthProviderMETA));
    
    xrGetInstanceProcAddr(
        instance,
        "xrStartEnvironmentDepthProviderMETA",
        reinterpret_cast<PFN_xrVoidFunction*>(&m_xrStartEnvironmentDepthProviderMETA));
    
    xrGetInstanceProcAddr(
        instance,
        "xrStopEnvironmentDepthProviderMETA",
        reinterpret_cast<PFN_xrVoidFunction*>(&m_xrStopEnvironmentDepthProviderMETA));
    
    xrGetInstanceProcAddr(
        instance,
        "xrCreateEnvironmentDepthSwapchainMETA",
        reinterpret_cast<PFN_xrVoidFunction*>(&m_xrCreateEnvironmentDepthSwapchainMETA));
    
    xrGetInstanceProcAddr(
        instance,
        "xrDestroyEnvironmentDepthSwapchainMETA",
        reinterpret_cast<PFN_xrVoidFunction*>(&m_xrDestroyEnvironmentDepthSwapchainMETA));
    
    xrGetInstanceProcAddr(
        instance,
        "xrEnumerateEnvironmentDepthSwapchainImagesMETA",
        reinterpret_cast<PFN_xrVoidFunction*>(&m_xrEnumerateEnvironmentDepthSwapchainImagesMETA));
    
    xrGetInstanceProcAddr(
        instance,
        "xrGetEnvironmentDepthSwapchainStateMETA",
        reinterpret_cast<PFN_xrVoidFunction*>(&m_xrGetEnvironmentDepthSwapchainStateMETA));
    
    xrGetInstanceProcAddr(
        instance,
        "xrAcquireEnvironmentDepthImageMETA",
        reinterpret_cast<PFN_xrVoidFunction*>(&m_xrAcquireEnvironmentDepthImageMETA));
    
    xrGetInstanceProcAddr(
        instance,
        "xrSetEnvironmentDepthHandRemovalMETA",
        reinterpret_cast<PFN_xrVoidFunction*>(&m_xrSetEnvironmentDepthHandRemovalMETA));
}

void OpenXRMetaEnvironmentDepthProvider::enable_depth(bool value)
{
    m_Is_depth_enabled = value;
    XrResult result{};
     
    if (m_Is_depth_enabled)
    {
        result = xrStartEnvironmentDepthProviderMETA(
            m_environmentDepthProvider);

        emit_signal("_on_depth_provider_started");
    }
    else
    {
        result = xrStopEnvironmentDepthProviderMETA(
            m_environmentDepthProvider);

        emit_signal("_on_depth_provider_stopped");
    }

    if (result != XrResult::XR_SUCCESS)
    {
        if (m_Is_depth_enabled)
        {
            godot::UtilityFunctions::printerr("Failed to start the Depth Provider");
        }
        else
        {
            godot::UtilityFunctions::printerr("Failed to stop the Depth Provider");
        }
    }
}

