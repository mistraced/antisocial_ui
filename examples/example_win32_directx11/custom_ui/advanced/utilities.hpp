#pragma once

#include <any>
#include <type_traits>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <chrono>

#include "imgui.h"
#include "imgui_internal.h"

#define crypt_string
#define ay_obfuscate

class c_animator
{
public:
    template < typename T >
    c_animator( T initial_value );

    template < typename T >
    void set( T to_value );

    template < typename T >
    void update( T to_value, float duration = 0.10f );

    template < typename T >
    T get( ) const;

private:
    float resolve_value( float clock, float previous_value, float new_value, float duration = 0.10f );

    template < typename T >
    void update_struct( T& current, const T& target, float elapsed, float duration = 0.10f );

    std::any value;
    std::chrono::steady_clock::time_point last_update = std::chrono::steady_clock::now( );
};

static inline float ease( float time, float start_value, float change_value, float duration )
{
    return change_value * time / duration + start_value;
}

namespace
{
    template < typename, typename = void >
    struct has_x : std::false_type
    {
    };
    template < typename T >
    struct has_x< T, std::void_t< decltype( std::declval< T >( ).x ) > > : std::true_type
    {
    };

    template < typename, typename = void >
    struct has_y : std::false_type
    {
    };
    template < typename T >
    struct has_y< T, std::void_t< decltype( std::declval< T >( ).y ) > > : std::true_type
    {
    };

    template < typename, typename = void >
    struct has_z : std::false_type
    {
    };
    template < typename T >
    struct has_z< T, std::void_t< decltype( std::declval< T >( ).z ) > > : std::true_type
    {
    };

    template < typename, typename = void >
    struct has_w : std::false_type
    {
    };
    template < typename T >
    struct has_w< T, std::void_t< decltype( std::declval< T >( ).w ) > > : std::true_type
    {
    };
} // namespace

namespace c
{
    static inline ImColor alpha_interpolate( ImColor input, float alpha = ImGui::GetStyle( ).Alpha )
    {
        return { input.Value.x, input.Value.y, input.Value.z, input.Value.w * alpha };
    }

    static inline ImColor alpha_interpolate( ImVec4 input, float alpha = ImGui::GetStyle( ).Alpha )
    {
        return { input.x, input.y, input.z, input.w * alpha };
    }

    static inline ImVec4 alpha_interpolate_vec4( ImVec4 input, float alpha = ImGui::GetStyle( ).Alpha )
    {
        return { input.x, input.y, input.z, input.w * alpha };
    }

    static inline ImColor to_grayscale( const ImColor& color, float alpha = ImGui::GetStyle( ).Alpha )
    {
        float gray = 0.299f * color.Value.x + 0.587f * color.Value.y + 0.114f * color.Value.z;
        return ImColor( gray, gray, gray, color.Value.w * alpha );
    }

} // namespace c

extern template c_animator::c_animator( float );
extern template c_animator::c_animator( ImVec4 );

extern template void c_animator::set< float >( float );
extern template void c_animator::set< ImVec4 >( ImVec4 );

extern template void c_animator::update< float >( float, float );
extern template void c_animator::update< ImVec4 >( ImVec4, float );

extern template float c_animator::get< float >( ) const;
extern template ImVec4 c_animator::get< ImVec4 >( ) const;