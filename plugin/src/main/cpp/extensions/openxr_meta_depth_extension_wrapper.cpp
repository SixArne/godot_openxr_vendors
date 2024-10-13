/**************************************************************************/
/*  openxr_fb_spatial_entity_extension_wrapper.cpp                        */
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

#include "extensions/openxr_meta_depth_extension_wrapper.h"

#include <godot_cpp/classes/open_xrapi_extension.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

OpenXRMetaDepthExtensionWrapper *OpenXRMetaDepthExtensionWrapper::singleton = nullptr;

OpenXRMetaDepthExtensionWrapper *OpenXRMetaDepthExtensionWrapper::get_singleton() {
	if (singleton == nullptr) {
		singleton = memnew(OpenXRMetaDepthExtensionWrapper());
	}
	return singleton;
}

OpenXRMetaDepthExtensionWrapper::OpenXRMetaDepthExtensionWrapper() :
		OpenXRExtensionWrapperExtension() {
	ERR_FAIL_COND_MSG(singleton != nullptr, "An OpenXRMetaDepthExtensionWrapper singleton already exists.");

	request_extensions[XR_META_ENVIRONMENT_DEPTH_EXTENSION_NAME] = &meta_environment_depth_ext;
	singleton = this;
}

OpenXRMetaDepthExtensionWrapper::~OpenXRMetaDepthExtensionWrapper() {
}

void OpenXRMetaDepthExtensionWrapper::_bind_methods() {
	ADD_SIGNAL(MethodInfo("openxr_instance_created", PropertyInfo(Variant::Type::INT, "instance")));
	ADD_SIGNAL(MethodInfo("openxr_instance_destroyed", PropertyInfo(Variant::Type::NIL, "")));
}

Dictionary OpenXRMetaDepthExtensionWrapper::_get_requested_extensions() {
	Dictionary result;
	for (auto ext : request_extensions) {
		uint64_t value = reinterpret_cast<uint64_t>(ext.value);
		result[ext.key] = (Variant)value;
	}
	return result;
}

void OpenXRMetaDepthExtensionWrapper::_on_instance_created(uint64_t instance) {
	emit_signal("openxr_instance_created", instance);    
}

void OpenXRMetaDepthExtensionWrapper::_on_instance_destroyed() {
	emit_signal("openxr_instance_destroyed");    
}

void OpenXRMetaDepthExtensionWrapper::_on_session_created(uint64_t instance) {
	emit_signal("openxr_session_created", instance);    
}

void OpenXRMetaDepthExtensionWrapper::_on_session_destroyed() {
	emit_signal("openxr_session_destroyed");    
}



