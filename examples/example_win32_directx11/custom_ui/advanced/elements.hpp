//
// Created by Kai Tears on 05/08/2025.
//

#ifndef ELEMENTS_HPP
#define ELEMENTS_HPP

#include "utilities.hpp"

#include <functional>

namespace elem
{
    extern bool toggle( const char* label, bool* v, const char* description = "missing", std::function< void( ) > advanced = { } );
    extern bool internal_toggle( const char* label, bool* v );

    extern bool simple_category( const char* label );
    extern void end_category( );

    extern bool slider_scalar( const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags );
    extern bool slider_int( const char* label, int* v, int v_min, int v_max, const char* format = 0, ImGuiSliderFlags flags = 0 );
    extern bool slider_float( const char* label, float* v, float v_min, float v_max, const char* format = 0, ImGuiSliderFlags flags = 0 );

    extern bool selectable( const char* label, bool* p_selected, ImGuiSelectableFlags flags = 0, const ImVec2& size_arg = { 0, 0 } );
    extern bool selectable( const char* label, bool selected, ImGuiSelectableFlags flags = 0, const ImVec2& size_arg = { 0, 0 } );

    extern bool combo( const char* label, int* value, std::vector< std::string > entries );

    extern bool button( const char* label, const ImVec2& size_arg = { 0, 0 } );

    extern void color( const char* label, const std::vector< ImVec4* >& colors );

    extern bool input_text( const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0 );

    extern void preview( ImTextureID texture, int height, const std::pair< ImVec2, std::function< void( ImDrawList*, const ImVec2& ) > >& render_data );
} // namespace elem

#endif // ELEMENTS_HPP
