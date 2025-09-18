//
// Created by Kai Tears on 05/08/2025.
//

#include "elements.hpp"

#include <unordered_map>

using namespace ImGui;

float get_width( )
{
    return GetContentRegionMax( ).x;
}

int mod_alpha( )
{
    return int( GetStyle( ).Alpha * 255 );
}

bool elem::internal_toggle( const char* label, bool* v )
{
    ImGuiWindow* window = GetCurrentWindow( );
    if ( window->SkipItems )
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID( label );
    const ImVec2 label_size = CalcTextSize( label, NULL, true );

    struct _state
    {
        c_animator main { 0.0f };

        c_animator outline { ImVec4( 0, 0, 0, 0 ) };
        c_animator bg { ImVec4( 0, 0, 0, 0 ) };
        c_animator pill { ImVec4( 0, 0, 0, 0 ) };
        c_animator text { ImVec4( 0, 0, 0, 0 ) };
    };

    static std::unordered_map< ImGuiID, _state > __state;
    _state& state = __state[ id ];

    const float square_sz = GetFrameHeight( );
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect total_bb( pos, pos + ImVec2( get_width( ) - style.WindowPadding.x, label_size.y + style.FramePadding.y * 2.0f ) );
    ItemSize( total_bb, style.FramePadding.y );
    if ( !ItemAdd( total_bb, id ) )
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior( total_bb, id, &hovered, &held );

    if ( pressed )
    {
        //g_ctx->audio_player->play( static_cast< int >( sound_t::toggle ) );
        ( *v ) = !( *v );
    }

    const ImRect check_bb( ImVec2( total_bb.Max.x - square_sz * 2 + 10, total_bb.Min.y + 2 ), total_bb.Max - ImVec2( 0, 2 ) );

    // window->DrawList->AddRect(total_bb.Min, total_bb.Max, ImColor(255, 0, 0));

    state.main.update< float >( ( *v ) ? 1 : 0 );

    state.pill.update< ImVec4 >( ( *v ) ? ImColor( 0, 0, 0, mod_alpha( ) ).Value : ImColor( 55, 55, 66, mod_alpha( ) ).Value );
    state.text.update< ImVec4 >( ( *v || hovered ) ? ImColor( 255, 255, 255, mod_alpha( ) ).Value : ImColor( 155, 155, 155, mod_alpha( ) ).Value );
    state.outline.update< ImVec4 >( ( *v ) ? ImColor( 255, 255, 255, mod_alpha( ) ).Value : ImColor( 55, 55, 66, mod_alpha( ) ).Value );
    state.bg.update< ImVec4 >( ( *v ) ? ImColor( 25, 25, 25, mod_alpha( ) ).Value : ImColor( 25, 25, 25, 0 ).Value );

    int vert_start_idx = window->DrawList->VtxBuffer.Size;
    window->DrawList->AddRectFilled( check_bb.Min, check_bb.Max, ImColor( state.bg.get< ImVec4 >( ) ), 666 );
    int vert_end_idx = window->DrawList->VtxBuffer.Size;

    ShadeVertsLinearColorGradientKeepAlpha( window->DrawList, vert_start_idx, vert_end_idx, check_bb.Min, check_bb.Max, ImColor( 218, 221, 216 ), ImColor( 234, 193, 251 ) );

    window->DrawList->AddRect( check_bb.Min, check_bb.Max, ImColor( state.outline.get< ImVec4 >( ) ), 666 );

    window->DrawList->AddCircleFilled( { check_bb.Min.x + 8 + ( 14 * state.main.get< float >( ) ), check_bb.GetCenter( ).y }, 5, ImColor( state.pill.get< ImVec4 >( ) ), 200 );

    const ImVec2 label_pos = ImVec2( total_bb.Min.x, total_bb.Min.y + style.FramePadding.y );

    PushStyleColor( ImGuiCol_Text, state.text.get< ImVec4 >( ) );
    if ( label_size.x > 0.0f )
        RenderText( label_pos, label );
    PopStyleColor( );

    return pressed;
}

bool elem::toggle( const char* label, bool* v, const char* description, std::function< void( ) > advanced )
{
    PushStyleVar( ImGuiStyleVar_WindowPadding, { 10, 10 } );

    BeginChild( label, { GetContentRegionAvail( ).x - 10, 0 }, 0 | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_AlwaysUseWindowPadding );
    {
        ImRect const wnd_rect = GetCurrentWindow( )->Rect( );

        GetWindowDrawList( )->AddRect( wnd_rect.Min, wnd_rect.Max, c::alpha_interpolate( ImColor( 41, 43, 55 ), GetStyle( ).Alpha ), 5, 0, 1 );

        float const text_pd = 10;

        SetCursorPos( { text_pd, text_pd } );

        internal_toggle( label, v );

        SetCursorPosX( text_pd );

        PushStyleColor( ImGuiCol_Text, ImColor( 155, 155, 155 ).Value );
        TextWrapped( description );
        PopStyleColor( );

        if ( advanced )
        {
            SetCursorPosX( 0 );
            GetWindowDrawList( )->AddLine( GetCursorScreenPos( ), GetCursorScreenPos( ) + ImVec2( GetWindowSize( ).x, 0 ), c::alpha_interpolate( ImColor( 41, 43, 55 ), GetStyle( ).Alpha ) );
            Dummy( { 0, 0 } );

            PushStyleVar( ImGuiStyleVar_WindowPadding, { 0, 0 } );
            BeginChild( ( std::string( label ) + "##2" ).c_str( ), { GetContentRegionAvail( ).x, 0 }, ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_AlwaysUseWindowPadding );
            {
                advanced( );
                EndChild( );
            }
            PopStyleVar( );
        }

        EndChild( );
    }
    PopStyleVar( );

    return true;
}

bool elem::simple_category( const char* label )
{
    PushStyleVar( ImGuiStyleVar_WindowPadding, { 10, 10 } );

    BeginChild( label, { GetContentRegionAvail( ).x - 10, 0 }, 0 | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_AlwaysUseWindowPadding );
    {
        ImRect const wnd_rect = GetCurrentWindow( )->Rect( );

        GetWindowDrawList( )->AddRect( wnd_rect.Min, wnd_rect.Max, c::alpha_interpolate( ImColor( 41, 43, 55 ), GetStyle( ).Alpha ), 5, 0, 1 );

        Text( label );

        SetCursorPosX( 0 );
        GetWindowDrawList( )->AddLine( GetCursorScreenPos( ), GetCursorScreenPos( ) + ImVec2( GetWindowSize( ).x, 0 ), c::alpha_interpolate( ImColor( 41, 43, 55 ), GetStyle( ).Alpha ) );
        Dummy( { 0, 0 } );

        PushStyleVar( ImGuiStyleVar_WindowPadding, { 0, 0 } );
        BeginChild( ( std::string( label ) + "##2" ).c_str( ), { GetContentRegionAvail( ).x, 0 }, ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_AlwaysUseWindowPadding );
        {
        }
    }
    return true;
}

void elem::end_category( )
{
    PopStyleVar( );
    EndChild( );
    EndChild( );
    PopStyleVar( );
}

bool elem::slider_scalar( const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags )
{
    ImGuiWindow* window = GetCurrentWindow( );
    if ( window->SkipItems )
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID( label );
    const float w = get_width( );

    struct _state
    {
        c_animator text { ImVec4( 0, 0, 0, 0 ) };
        c_animator grab { 0.0f };
    };

    static std::unordered_map< ImGuiID, _state > __state;
    _state& state = __state[ id ];

    const ImVec2 label_size = CalcTextSize( label, NULL, true );
    const ImRect frame_bb( window->DC.CursorPos + ImVec2( 0, label_size.y + style.FramePadding.y ), window->DC.CursorPos + ImVec2( 0, label_size.y + style.FramePadding.y ) + ImVec2( w, style.FramePadding.y * 4.0f ) );
    const ImRect total_bb( window->DC.CursorPos, frame_bb.Max );

    const bool temp_input_allowed = ( flags & ImGuiSliderFlags_NoInput ) == 0;
    ItemSize( total_bb, style.FramePadding.y );
    if ( !ItemAdd( total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0 ) )
        return false;

    // Default format string when passing NULL
    if ( format == NULL )
        format = DataTypeGetInfo( data_type )->PrintFmt;

    const bool hovered = ItemHoverable( total_bb, id, g.LastItemData.ItemFlags );
    bool temp_input_is_active = temp_input_allowed && TempInputIsActive( id );
    if ( !temp_input_is_active )
    {
        // Tabbing or CTRL-clicking on Slider turns it into an input box
        const bool clicked = hovered && IsMouseClicked( 0, ImGuiInputFlags_None, id );
        const bool make_active = ( clicked || g.NavActivateId == id );
        if ( make_active && clicked )
            SetKeyOwner( ImGuiKey_MouseLeft, id );
        if ( make_active && temp_input_allowed )
            if ( ( clicked && g.IO.KeyCtrl ) || ( g.NavActivateId == id && ( g.NavActivateFlags & ImGuiActivateFlags_PreferInput ) ) )
                temp_input_is_active = true;

        if ( make_active && !temp_input_is_active )
        {
            SetActiveID( id, window );
            SetFocusID( id, window );
            FocusWindow( window );
            g.ActiveIdUsingNavDirMask |= ( 1 << ImGuiDir_Left ) | ( 1 << ImGuiDir_Right );
        }
    }

    if ( temp_input_is_active )
    {
        // Only clamp CTRL+Click input when ImGuiSliderFlags_ClampOnInput is set (generally via ImGuiSliderFlags_AlwaysClamp)
        const bool clamp_enabled = ( flags & ImGuiSliderFlags_ClampOnInput ) != 0;
        return TempInputScalar( total_bb, id, label, data_type, p_data, format, clamp_enabled ? p_min : NULL, clamp_enabled ? p_max : NULL );
    }

    // Slider behavior
    ImRect grab_bb;
    const bool value_changed = SliderBehavior( total_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb );

    static bool value_was_changed = false;

    if ( value_changed )
    {
        value_was_changed = true;
        MarkItemEdited( id );
    }

    if ( !IsItemActive( ) && value_was_changed )
    {
        // //g_ctx->audio_player->play( static_cast< int >( sound_t::toggle ) );
        value_was_changed = false;
    }

    state.text.update( ( IsItemActive( ) || hovered || value_changed ) ? ImColor( 255, 255, 255, mod_alpha( ) ).Value : ImColor( 155, 155, 155, mod_alpha( ) ).Value );

    window->DrawList->AddRect( frame_bb.Min + ImVec2( 0, 2 ), frame_bb.Max - ImVec2( 0, 2 ), ImColor( 55, 55, 66, mod_alpha( ) ), 12 );

    state.grab.update< float >( grab_bb.Max.x - frame_bb.Min.x );

    int vert_start_idx = window->DrawList->VtxBuffer.Size;
    window->DrawList->AddRectFilled( frame_bb.Min, { frame_bb.Min.x + state.grab.get< float >( ), frame_bb.Max.y }, ImColor( 25, 25, 25, mod_alpha( ) ), 12 );
    int vert_end_idx = window->DrawList->VtxBuffer.Size;

    ShadeVertsLinearColorGradientKeepAlpha( window->DrawList, vert_start_idx, vert_end_idx, frame_bb.Min, { grab_bb.Max.x, frame_bb.Max.y }, ImColor( 218, 221, 216 ), ImColor( 234, 193, 251 ) );

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    char value_buf[ 64 ];
    const char* value_buf_end = value_buf + DataTypeFormatString( value_buf, IM_ARRAYSIZE( value_buf ), data_type, p_data, format );
    if ( g.LogEnabled )
        LogSetNextTextDecoration( "{", "}" );

    const char* separator = ">>";
    ImVec2 const separator_size = CalcTextSize( separator );
    PushStyleColor( ImGuiCol_Text, ImColor( 65, 65, 65 ).Value );
    RenderText( total_bb.Min + ImVec2( label_size.x + 5, 0 ), separator );
    PopStyleColor( );

    PushStyleColor( ImGuiCol_Text, ImColor( 155, 155, 155 ).Value );
    RenderText( total_bb.Min + ImVec2( label_size.x + 5 + separator_size.x + 5, 0 ), value_buf );
    PopStyleColor( );

    PushStyleColor( ImGuiCol_Text, state.text.get< ImVec4 >( ) );
    if ( label_size.x > 0.0f )
        RenderText( ImVec2( total_bb.Min.x, total_bb.Min.y ), label );
    PopStyleColor( );

    return value_changed;
}

bool elem::slider_int( const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags )
{
    return slider_scalar( label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags );
}

bool elem::slider_float( const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags )
{
    return slider_scalar( label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags );
}

bool begin_dropdown( const char* label, const char* preview_value )
{
    ImGuiWindow* window = GetCurrentWindow( );

    const ImGuiID id = window->GetID( label );

    struct _state
    {
        c_animator text { ImVec4( 0, 0, 0, 0 ) };

        bool opened, hovered = false;
        c_animator primary { 0.f };
    };

    const ImVec2 label_size = CalcTextSize( label );
    const ImVec2 preview_size = CalcTextSize( preview_value );
    const ImVec2 pos = window->DC.CursorPos;

    static std::unordered_map< ImGuiID, _state > __state;
    _state& state = __state[ id ];

    const float label_offset = label_size.y + 2;

    const float width = get_width( );

    const ImRect dropdown_rect = {
        pos + ImVec2( 0, label_size.y + 5 ),
        { pos.x + width, pos.y + 28 + label_size.y + 5 } };

    const ImRect total_rect = {
        pos,
        dropdown_rect.Max };

    ItemSize( total_rect, 0 );

    if ( !ItemAdd( total_rect, id, &total_rect ) )
        return false;

    bool hovered, held, pressed = ButtonBehavior( dropdown_rect, id, &hovered, &held );

    state.primary.update< float >( state.opened ? 1 : 0 );

    if ( pressed )
    {
        MarkItemEdited( id );
        if ( state.primary.get< float >( ) < 0.01f )
        {
            //g_ctx->audio_player->play( static_cast< int >( sound_t::toggle ) );
            state.opened = true;
        }
    }

    state.text.update( ( IsItemActive( ) || hovered || state.opened ) ? ImColor( 255, 255, 255, mod_alpha( ) ).Value : ImColor( 155, 155, 155, mod_alpha( ) ).Value );

    window->DrawList->AddRect( dropdown_rect.Min, dropdown_rect.Max, ImColor( 55, 55, 66, mod_alpha( ) ), 6 );

    const char* ang_down = "\xef\x84\x87";
    const auto ang_sz = CalcTextSize( ang_down );

    PushStyleColor( ImGuiCol_Text, state.text.get< ImVec4 >( ) );

    RenderText( total_rect.Min, label );

    RenderText( dropdown_rect.Min + ImVec2( 8, 6 ), preview_value );

    RenderText( { dropdown_rect.Max.x - 8 - ang_sz.x, dropdown_rect.Min.y + 6 }, ang_down );

    PopStyleColor( );

    if ( !state.opened && state.primary.get< float >( ) < 0.01f )
        return false;

    SetNextWindowPos( ImVec2( dropdown_rect.Min.x, dropdown_rect.Max.y + 5 * state.primary.get< float >( ) ) );
    SetNextWindowSize( ImVec2( dropdown_rect.GetWidth( ), 0 ) );

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollWithMouse;

    SetNextWindowFocus( );

    PushStyleVar( ImGuiStyleVar_WindowRounding, 6 );
    PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0 );
    PushStyleVar( ImGuiStyleVar_WindowPadding, { 10, 10 } );
    PushStyleVar( ImGuiStyleVar_Alpha, state.primary.get< float >( ) * GetStyle( ).Alpha );
    // 2 times because it's popping after this shit

    PushStyleColor( ImGuiCol_WindowBg, ImColor( 10 / 255.f, 14 / 255.f, 21 / 255.f, state.primary.get< float >( ) * GetStyle( ).Alpha ).Value );
    PushStyleColor( ImGuiCol_Border, ImColor( 25 / 255.f, 32 / 255.f, 40 / 255.f, state.primary.get< float >( ) * GetStyle( ).Alpha ).Value );

    auto ret = Begin( label, NULL, window_flags );

    PopStyleVar( 4 );
    PopStyleColor( 2 );

    PushStyleVar( ImGuiStyleVar_Alpha, state.primary.get< float >( ) * GetStyle( ).Alpha );

    state.hovered = IsWindowHovered( );

    if ( !state.hovered && state.opened && GetIO( ).MouseClicked[ 0 ] )
    {
        //g_ctx->audio_player->play( static_cast< int >( sound_t::button ) );
        state.opened = false;
    }

    return ret;
}

void end_dropdown( )
{
    PopStyleVar( );

    End( );
}

bool elem::combo( const char* label, int* value, std::vector< std::string > entries )
{
    ImGuiContext& g = *GImGui;

    const char* preview_value = entries[ *value ].c_str( );

    if ( !begin_dropdown( label, preview_value ) )
        return false;

    bool value_changed = false;
    ImGuiListClipper clipper;
    clipper.Begin( entries.size( ) );
    clipper.IncludeItemByIndex( *value );
    while ( clipper.Step( ) )
        for ( int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++ )
        {
            const char* item_text = entries[ i ].c_str( );
            if ( item_text == NULL )
                item_text = "*Unknown item*";

            PushID( i );
            const bool item_selected = ( i == *value );
            if ( selectable( item_text, item_selected ) && *value != i )
            {
                value_changed = true;
                *value = i;
            }
            if ( item_selected )
                SetItemDefaultFocus( );
            PopID( );
        }

    end_dropdown( );
    if ( value_changed )
        MarkItemEdited( g.LastItemData.ID );

    return value_changed;
}

bool elem::selectable( const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg )
{
    ImGuiWindow* window = GetCurrentWindow( );
    if ( window->SkipItems )
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    // Submit label or explicit size to ItemSize(), whereas ItemAdd() will submit a larger/spanning rectangle.
    ImGuiID id = window->GetID( label );
    ImVec2 label_size = CalcTextSize( label, NULL, true );
    ImVec2 size( size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y );
    ImVec2 pos = window->DC.CursorPos;
    pos.y += window->DC.CurrLineTextBaseOffset;
    ItemSize( size, 0.0f );

    // Fill horizontal space
    // We don't support (size < 0.0f) in Selectable() because the ItemSpacing extension would make explicitly right-aligned sizes not visibly match other widgets.
    const bool span_all_columns = ( flags & ImGuiSelectableFlags_SpanAllColumns ) != 0;
    const float min_x = span_all_columns ? window->ParentWorkRect.Min.x : pos.x;
    const float max_x = span_all_columns ? window->ParentWorkRect.Max.x : window->WorkRect.Max.x;
    if ( size_arg.x == 0.0f || ( flags & ImGuiSelectableFlags_SpanAvailWidth ) )
        size.x = ImMax( label_size.x, max_x - min_x );

    // Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
    // FIXME: Not part of layout so not included in clipper calculation, but ItemSize currently doesn't allow offsetting CursorPos.
    ImRect bb( min_x, pos.y, min_x + size.x, pos.y + size.y );
    if ( ( flags & ImGuiSelectableFlags_NoPadWithHalfSpacing ) == 0 )
    {
        const float spacing_x = span_all_columns ? 0.0f : style.ItemSpacing.x;
        const float spacing_y = style.ItemSpacing.y;
        const float spacing_L = IM_TRUNC( spacing_x * 0.50f );
        const float spacing_U = IM_TRUNC( spacing_y * 0.50f );
        bb.Min.x -= spacing_L;
        bb.Min.y -= spacing_U;
        bb.Max.x += ( spacing_x - spacing_L );
        bb.Max.y += ( spacing_y - spacing_U );
    }
    // if (g.IO.KeyCtrl) { GetForegroundDrawList()->AddRect(bb.Min, bb.Max, IM_COL32(0, 255, 0, 255)); }

    const bool disabled_item = ( flags & ImGuiSelectableFlags_Disabled ) != 0;
    const ImGuiItemFlags extra_item_flags = disabled_item ? ( ImGuiItemFlags ) ImGuiItemFlags_Disabled : ImGuiItemFlags_None;
    bool is_visible;
    if ( span_all_columns )
    {
        // Modify ClipRect for the ItemAdd(), faster than doing a PushColumnsBackground/PushTableBackgroundChannel for every Selectable..
        const float backup_clip_rect_min_x = window->ClipRect.Min.x;
        const float backup_clip_rect_max_x = window->ClipRect.Max.x;
        window->ClipRect.Min.x = window->ParentWorkRect.Min.x;
        window->ClipRect.Max.x = window->ParentWorkRect.Max.x;
        is_visible = ItemAdd( bb, id, NULL, extra_item_flags );
        window->ClipRect.Min.x = backup_clip_rect_min_x;
        window->ClipRect.Max.x = backup_clip_rect_max_x;
    } else
    {
        is_visible = ItemAdd( bb, id, NULL, extra_item_flags );
    }

    const bool is_multi_select = ( g.LastItemData.ItemFlags & ImGuiItemFlags_IsMultiSelect ) != 0;
    if ( !is_visible )
        if ( !is_multi_select || !g.BoxSelectState.UnclipMode || !g.BoxSelectState.UnclipRect.Overlaps( bb ) ) // Extra layer of "no logic clip" for box-select support (would be more overhead to add to ItemAdd)
            return false;

    const bool disabled_global = ( g.CurrentItemFlags & ImGuiItemFlags_Disabled ) != 0;
    if ( disabled_item && !disabled_global ) // Only testing this as an optimization
        BeginDisabled( );

    // FIXME: We can standardize the behavior of those two, we could also keep the fast path of override ClipRect + full push on render only,
    // which would be advantageous since most selectable are not selected.
    if ( span_all_columns )
    {
        if ( g.CurrentTable )
            TablePushBackgroundChannel( );
        else if ( window->DC.CurrentColumns )
            PushColumnsBackground( );
        g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HasClipRect;
        g.LastItemData.ClipRect = window->ClipRect;
    }

    // We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
    ImGuiButtonFlags button_flags = 0;
    if ( flags & ImGuiSelectableFlags_NoHoldingActiveID )
    {
        button_flags |= ImGuiButtonFlags_NoHoldingActiveId;
    }
    if ( flags & ImGuiSelectableFlags_NoSetKeyOwner )
    {
        button_flags |= ImGuiButtonFlags_NoSetKeyOwner;
    }
    if ( flags & ImGuiSelectableFlags_SelectOnClick )
    {
        button_flags |= ImGuiButtonFlags_PressedOnClick;
    }
    if ( flags & ImGuiSelectableFlags_SelectOnRelease )
    {
        button_flags |= ImGuiButtonFlags_PressedOnRelease;
    }
    if ( flags & ImGuiSelectableFlags_AllowDoubleClick )
    {
        button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick;
    }
    if ( ( flags & ImGuiSelectableFlags_AllowOverlap ) || ( g.LastItemData.ItemFlags & ImGuiItemFlags_AllowOverlap ) )
    {
        button_flags |= ImGuiButtonFlags_AllowOverlap;
    }

    // Multi-selection support (header)
    const bool was_selected = selected;
    if ( is_multi_select )
    {
        // Handle multi-select + alter button flags for it
        MultiSelectItemHeader( id, &selected, &button_flags );
    }

    bool hovered, held;
    bool pressed = ButtonBehavior( bb, id, &hovered, &held, button_flags );

    // Multi-selection support (footer)
    if ( is_multi_select )
    {
        MultiSelectItemFooter( id, &selected, &pressed );
    } else
    {
        // Auto-select when moved into
        // - This will be more fully fleshed in the range-select branch
        // - This is not exposed as it won't nicely work with some user side handling of shift/control
        // - We cannot do 'if (g.NavJustMovedToId != id) { selected = false; pressed = was_selected; }' for two reasons
        //   - (1) it would require focus scope to be set, need exposing PushFocusScope() or equivalent (e.g. BeginSelection() calling PushFocusScope())
        //   - (2) usage will fail with clipped items
        //   The multi-select API aim to fix those issues, e.g. may be replaced with a BeginSelection() API.
        if ( ( flags & ImGuiSelectableFlags_SelectOnNav ) && g.NavJustMovedToId != 0 && g.NavJustMovedToFocusScopeId == g.CurrentFocusScopeId )
            if ( g.NavJustMovedToId == id )
                selected = pressed = true;
    }

    // Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with keyboard/gamepad
    if ( pressed || ( hovered && ( flags & ImGuiSelectableFlags_SetNavIdOnHover ) ) )
    {
        if ( !g.NavHighlightItemUnderNav && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent )
        {
            SetNavID( id, window->DC.NavLayerCurrent, g.CurrentFocusScopeId, WindowRectAbsToRel( window, bb ) ); // (bb == NavRect)
            if ( g.IO.ConfigNavCursorVisibleAuto )
                g.NavCursorVisible = false;
        }
    }
    if ( pressed )
    {
        //g_ctx->audio_player->play( static_cast< int >( sound_t::toggle ) );
        MarkItemEdited( id );
    }

    if ( selected != was_selected )
        g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

    // Render
    if ( is_visible )
    {
        struct _state
        {
            c_animator main { ImVec4( 0, 0, 0, 0 ) };
        };

        static std::unordered_map< ImGuiID, _state > __state;
        _state& state = __state[ id ];

        state.main.update< ImVec4 >( selected ? ImColor( 255, 255, 255, mod_alpha( ) ).Value : ImColor( 155, 155, 155, mod_alpha( ) ).Value );

        PushStyleColor( ImGuiCol_Text, state.main.get< ImVec4 >( ) );

        RenderTextClipped( pos, ImVec2( ImMin( pos.x + size.x, window->WorkRect.Max.x ), pos.y + size.y ), label, NULL, &label_size, style.SelectableTextAlign, &bb );

        PopStyleColor( );
    }

    if ( span_all_columns )
    {
        if ( g.CurrentTable )
            TablePopBackgroundChannel( );
        else if ( window->DC.CurrentColumns )
            PopColumnsBackground( );
    }

    // Automatically close popups
    if ( pressed && ( window->Flags & ImGuiWindowFlags_Popup ) && !( flags & ImGuiSelectableFlags_NoAutoClosePopups ) && ( g.LastItemData.ItemFlags & ImGuiItemFlags_AutoClosePopups ) )
        CloseCurrentPopup( );

    if ( disabled_item && !disabled_global )
        EndDisabled( );

    // Selectable() always returns a pressed state!
    // Users of BeginMultiSelect()/EndMultiSelect() scope: you may call ImGui::IsItemToggledSelection() to retrieve
    // selection toggle, only useful if you need that state updated (e.g. for rendering purpose) before reaching EndMultiSelect().
    IMGUI_TEST_ENGINE_ITEM_INFO( id, label, g.LastItemData.StatusFlags );
    return pressed; //-V1020
}

bool elem::selectable( const char* label, bool* p_selected, ImGuiSelectableFlags flags, const ImVec2& size_arg )
{
    if ( selectable( label, *p_selected, flags, size_arg ) )
    {
        *p_selected = !*p_selected;
        return true;
    }
    return false;
}

bool elem::button( const char* label, const ImVec2& size_arg )
{
    ImGuiWindow* window = GetCurrentWindow( );
    if ( window->SkipItems )
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID( label );
    const ImVec2 label_size = CalcTextSize( label, NULL, true );

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize( size_arg, get_width( ), label_size.y + style.FramePadding.y * 6.0f );

    const ImRect bb( pos, pos + size );
    ItemSize( size, style.FramePadding.y );
    if ( !ItemAdd( bb, id ) )
        return false;

    struct _state
    {
        c_animator text { ImVec4( 0, 0, 0, 0 ) };
        c_animator fill { ImVec4( 0, 0, 0, 0 ) };
    };

    static std::unordered_map< ImGuiID, _state > __state;
    _state& state = __state[ id ];

    bool hovered, held;
    bool pressed = ButtonBehavior( bb, id, &hovered, &held, 0 );

    state.text.update( ( IsItemActive( ) || hovered ) ? ImColor( 255, 255, 255, mod_alpha( ) ).Value : ImColor( 155, 155, 155, mod_alpha( ) ).Value );

    if ( pressed )
    {
        //g_ctx->audio_player->play( static_cast< int >( sound_t::button ) );
        state.fill.set< ImVec4 >( ImVec4( 1, 1, 1, style.Alpha ) );
    }

    state.fill.update< ImVec4 >( ImVec4( 1, 1, 1, 0 ) );

    window->DrawList->AddRect( bb.Min, bb.Max, ImColor( 55, 55, 66, mod_alpha( ) ), 6 );

    int vert_start_idx = window->DrawList->VtxBuffer.Size;
    window->DrawList->AddRectFilled( bb.Min, bb.Max, ImColor( state.fill.get< ImVec4 >( ) ), 6 );
    int vert_end_idx = window->DrawList->VtxBuffer.Size;

    ShadeVertsLinearColorGradientKeepAlpha( window->DrawList, vert_start_idx, vert_end_idx, bb.Min, bb.Max, ImColor( 218, 221, 216 ), ImColor( 234, 193, 251 ) );

    PushStyleColor( ImGuiCol_Text, state.text.get< ImVec4 >( ) );
    RenderTextClipped( bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb );
    PopStyleColor( );

    return pressed;
}

void elem::color( const char* label, const std::vector< ImVec4* >& colors )
{
    ImGuiWindow* window = GetCurrentWindow( );
    if ( window->SkipItems )
        return;

    ImGuiContext& g = *GImGui;
    ImGuiStyle const style = g.Style;
    ImGuiID const id = window->GetID( label );
    ImVec2 const label_size = CalcTextSize( label, nullptr, true );

    struct _state
    {
        bool opened, hovered = false;
        c_animator primary { 0.0f };
        ImVec2 pos;
        ImVec4* color;
    };

    static std::unordered_map< ImGuiID, _state > __state;
    _state& state = __state[ id ];

    float const square_sz = GetFrameHeight( );

    ImVec2 const pos = window->DC.CursorPos;
    ImRect const total_bb( pos, pos + ImVec2( get_width( ), label_size.y + style.FramePadding.y * 2.0f ) );
    ItemSize( total_bb, style.FramePadding.y );
    if ( !ItemAdd( total_bb, id ) )
        return;

    auto const label_pos = ImVec2( total_bb.Min.x, total_bb.Min.y + style.FramePadding.y );

    ImVec2 const min = total_bb.Max - ImVec2( 0, square_sz - 2 );

    if ( !colors.empty( ) )
    {
        for ( int it = 0; it < colors.size( ); ++it )
        {
            if ( auto* const col = colors[ it ] )
            {
                ImRect const col_bb = {
                    ImVec2( min.x - ( square_sz ) * ( it + 1 ) - 8 * ( it ), min.y ),
                    ImVec2( min.x - ( square_sz ) * ( it ) -8 * ( it ), total_bb.Max.y - 2 ),
                };

                window->DrawList->AddRectFilled( col_bb.Min, col_bb.Max, ImColor( *col ), 3 );

                window->DrawList->AddRect( col_bb.Min, col_bb.Max, ImColor( ImColor( 55, 55, 66, mod_alpha( ) ) ), 3 );

                auto color_id = id * 10 * ( it + 1 );

                ItemAdd( col_bb, color_id );

                bool c_hovered, c_held;
                bool c_pressed = ButtonBehavior( col_bb, color_id, &c_hovered, &c_held );

                state.primary.update< float >( state.opened ? 1 : 0 );

                if ( c_pressed && state.primary.get< float >( ) < 0.01f )
                {
                    //g_ctx->audio_player->play( static_cast< int >( sound_t::toggle ) );
                    state.opened = true;
                    state.color = col;
                }
            }
        }

        const float width = square_sz * 12.0f + GetStyle( ).WindowPadding.x * 2;

        if ( !state.opened && state.primary.get< float >( ) < 0.01f )
        {
            state.pos = GetMousePos( );
        } else
        {
            if ( state.color )
            {
                SetNextWindowPos( ImVec2( state.pos.x, state.pos.y + 5 * state.primary.get< float >( ) ) );
                SetNextWindowSize( ImVec2( width, 0 ) );

                ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollWithMouse;

                SetNextWindowFocus( );

                PushStyleVar( ImGuiStyleVar_WindowRounding, 6 );
                PushStyleVar( ImGuiStyleVar_WindowBorderSize, 1 );
                PushStyleVar( ImGuiStyleVar_WindowPadding, { 10, 10 } );
                PushStyleVar( ImGuiStyleVar_Alpha, state.primary.get< float >( ) * GetStyle( ).Alpha );
                // 2 times because it's popping after this shit

                PushStyleColor( ImGuiCol_WindowBg, ImColor( 10 / 255.f, 14 / 255.f, 21 / 255.f, state.primary.get< float >( ) * GetStyle( ).Alpha ).Value );
                PushStyleColor( ImGuiCol_Border, ImColor( 25 / 255.f, 32 / 255.f, 40 / 255.f, state.primary.get< float >( ) * GetStyle( ).Alpha ).Value );

                auto ret = Begin( label, NULL, window_flags );

                PopStyleVar( 4 );
                PopStyleColor( 2 );

                PushStyleVar( ImGuiStyleVar_Alpha, state.primary.get< float >( ) * GetStyle( ).Alpha );

                const ImVec2 mouse_pos = GetMousePos( );

                state.hovered = IsWindowHovered( );

                if ( !state.hovered && state.opened && GetIO( ).MouseClicked[ 0 ] )
                {
                    //g_ctx->audio_player->play( static_cast< int >( sound_t::button ) );
                    state.opened = false;
                }

                SetNextItemWidth( width - 20 );
                ColorPicker4( "##picker", ( float* ) state.color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar, 0 );

                PopStyleVar( );

                End( );
            }
        }
    }

    if ( label_size.x > 0.0f )
        RenderText( label_pos, label );
}

void elem::preview( ImTextureID texture, int height, const std::pair< ImVec2, std::function< void( ImDrawList*, const ImVec2& ) > >& render_data )
{
    if ( !texture )
        return;

    int const width = static_cast< int >( GetContentRegionAvail( ).x );
    ImRect const rect = {
        GetCursorScreenPos( ),
        GetCursorScreenPos( ) + ImVec2( width, height ) };
    ItemSize( { static_cast< float >( width ), static_cast< float >( height ) } );

    auto* draw_list = GetWindowDrawList( );
    draw_list->AddImageRounded( texture, rect.Min, rect.Max, { 0, 0 }, { 1, 1 }, ImColor( 1.f, 1.f, 1.f, 0.9f * GetStyle( ).Alpha ), 4 );

    if ( render_data.second )
        render_data.second( draw_list, rect.Min + render_data.first );
}