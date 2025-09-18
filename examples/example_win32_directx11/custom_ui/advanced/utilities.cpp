#include "utilities.hpp"
#include "imgui.h"
#include <cmath>
#include <algorithm>

float c_animator::resolve_value( float clock, float previous_value, float new_value, float duration )
{
    float value = std::clamp(
        ease( clock, previous_value, new_value - previous_value, duration ),
        std::min( previous_value, new_value ),
        std::max( previous_value, new_value ) );

    if ( std::abs( value - new_value ) < .001f )
        value = new_value;

    if ( std::fmod( value, 1 ) < .001f )
        value = std::floor( value );
    else if ( std::fmod( value, 1 ) > .99f )
        value = std::ceil( value );

    return value;
}

template < typename T >
void c_animator::update_struct( T& current, const T& target, float elapsed, float duration )
{
    auto resolve_comp = [ this, elapsed, duration ]( auto& cur, auto tgt ) {
        cur = resolve_value( elapsed, cur, tgt, duration );
    };

    if constexpr ( has_x< T >::value )
        resolve_comp( current.x, target.x );
    if constexpr ( has_y< T >::value )
        resolve_comp( current.y, target.y );
    if constexpr ( has_z< T >::value )
        resolve_comp( current.z, target.z );
    if constexpr ( has_w< T >::value )
        resolve_comp( current.w, target.w );
}

template < typename T >
c_animator::c_animator( T initial_value )
    : value( initial_value )
{
}

template < typename T >
void c_animator::set( T to_value )
{
    value = to_value;
    last_update = std::chrono::steady_clock::now( );
}

template < typename T >
void c_animator::update( T to_value, float duration )
{
    auto now = std::chrono::steady_clock::now( );
    float delta =
        std::chrono::duration< float >( now - last_update ).count( );
    last_update = now;

    if constexpr ( std::is_arithmetic_v< T > )
    {
        value = resolve_value( delta,
                               std::any_cast< T >( value ),
                               to_value, duration );
    } else
    {
        T current = std::any_cast< T >( value );
        update_struct( current, to_value, delta, duration );
        value = current;
    }
}

template < typename T >
T c_animator::get( ) const
{
    return std::any_cast< T >( value );
}

template c_animator::c_animator( float );
template c_animator::c_animator( ImVec4 );

template void c_animator::set< float >( float );
template void c_animator::set< ImVec4 >( ImVec4 );

template void c_animator::update< float >( float, float );
template void c_animator::update< ImVec4 >( ImVec4, float );

template float c_animator::get< float >( ) const;
template ImVec4 c_animator::get< ImVec4 >( ) const;