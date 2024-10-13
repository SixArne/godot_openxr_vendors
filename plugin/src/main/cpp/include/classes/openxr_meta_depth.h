/**************************************************************************/
/*  openxr_meta_depth.h                                                   */
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

#ifndef OPENXR_META_DEPTH_H
#define OPENXR_META_DEPTH_H

#include <openxr/openxr.h>

#include <godot_cpp/classes/node.hpp>


namespace godot {

    struct DepthMapSize
    {
        uint32_t width{};
        uint32_t height{};
    };

    class OpenXRMetaEnvironmentDepthProvider : public Node {
        GDCLASS(OpenXRMetaEnvironmentDepthProvider, Node);

    public:
        OpenXRMetaEnvironmentDepthProvider();
        ~OpenXRMetaEnvironmentDepthProvider();

        void enable_depth(bool value);
        void enable_hand_removal(bool value);
        DepthMapSize get_depth_map_size();

        void _process(double delta) override;

    protected:
        static void _bind_methods();

    private:
        bool m_Is_depth_enabled{false};
        bool m_Depth_map_was_accessed_this_frame{false};

        void fetch_function_pointers();
        void create_depth_provider();
        void on_session_destroyed();
        void on_session_created(uint64_t session);
        void create_depth_swapchains();
        void enumerate_depth_swapchain_images();
        void aquire_depth_map();

        OpenXRMetaDepthExtensionWrapper* m_DepthExtensionWrapper{};

        XrEnvironmentDepthProviderMETA m_environmentDepthProvider{};
        XrEnvironmentDepthSwapchainMETA m_environmentDepthSwapchain{};

        PFN_xrCreateEnvironmentDepthProviderMETA m_xrCreateEnvironmentDepthProviderMETA{nullptr};
        PFN_xrDestroyEnvironmentDepthProviderMETA m_xrDestroyEnvironmentDepthProviderMETA{nullptr};
        PFN_xrStartEnvironmentDepthProviderMETA m_xrStartEnvironmentDepthProviderMETA{nullptr};
        PFN_xrStopEnvironmentDepthProviderMETA m_xrStopEnvironmentDepthProviderMETA{nullptr};
        PFN_xrCreateEnvironmentDepthSwapchainMETA m_xrCreateEnvironmentDepthSwapchainMETA{nullptr};
        PFN_xrDestroyEnvironmentDepthSwapchainMETA m_xrDestroyEnvironmentDepthSwapchainMETA{nullptr};
        PFN_xrEnumerateEnvironmentDepthSwapchainImagesMETA m_xrEnumerateEnvironmentDepthSwapchainImagesMETA{nullptr};
        PFN_xrGetEnvironmentDepthSwapchainStateMETA m_xrGetEnvironmentDepthSwapchainStateMETA{nullptr};
        PFN_xrAcquireEnvironmentDepthImageMETA m_xrAcquireEnvironmentDepthImageMETA{nullptr};
        PFN_xrSetEnvironmentDepthHandRemovalMETA m_xrSetEnvironmentDepthHandRemovalMETA{nullptr};

    };
} // namespace godot

#endif
