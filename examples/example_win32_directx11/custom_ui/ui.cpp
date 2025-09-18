//
// Created by Kai Tears on 22/07/2025.
//

#include "ui.hpp"

#include <fstream>
#include <string>

#include "advanced/elements.hpp"

#include "../fonts/include.hpp"

#include "advanced/featurespages.hpp"

using namespace ImGui;

ImFont* logo;

void internal_scroll( const std::string& label )
{
    ImGuiWindow* window = GetCurrentWindow( );
    ImVec2 min = window->InnerRect.Min;
    ImVec2 max = window->InnerRect.Max;

    SetCursorScreenPos( min );
    InvisibleButton( label.c_str( ), max - min );

    ImGuiID id = GetID( label.c_str( ) );
    ImGuiStorage* storage = window->DC.StateStorage;

    bool dragging = storage->GetBool( id + 0, false );
    float drag_start = storage->GetFloat( id + 1, 0.0f );
    float scroll_start = storage->GetFloat( id + 2, 0.0f );

    if ( IsItemActive( ) && !dragging )
    {
        dragging = true;
        drag_start = GetMousePos( ).y;
        scroll_start = GetScrollY( );
    } else if ( !IsMouseDown( 0 ) )
    {
        dragging = false;
    }

    if ( dragging )
    {
        float delta = GetMousePos( ).y - drag_start;
        SetScrollY( scroll_start - delta );
    }

    storage->SetBool( id + 0, dragging );
    storage->SetFloat( id + 1, drag_start );
    storage->SetFloat( id + 2, scroll_start );
}

void c_user_interface::initialize( float scale )
{
    GetStyle( ).WindowRounding = 8.f;

    GetStyle( ).Colors[ ImGuiCol_Border ] = ImColor( 41, 41, 55 ).Value;
    GetStyle( ).Colors[ ImGuiCol_WindowBg ] = ImColor( 16, 16, 28 ).Value;
    GetStyle( ).Colors[ ImGuiCol_TextSelectedBg ] = { 0, 0, 0, 0 };

    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.SizePixels = 11;
    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0x0 };

    ImFontConfig main_cfg;
    // main_cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_Bold | ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_LightHinting;

    ImFontConfig mono_cfg;
    // main_cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_Monochrome;

    ImGui::GetIO( ).Fonts->TexGlyphPadding = 1;

    ImFontConfig wcfg = { };
    wcfg.OversampleH = wcfg.OversampleV = 1;
    wcfg.PixelSnapH = true;

    GetIO( ).Fonts->AddFontFromMemoryTTF( font_medium, sizeof font_medium, 14.f * scale, &main_cfg, GetIO( ).Fonts->GetGlyphRangesCyrillic( ) );
    GetIO( ).Fonts->AddFontFromMemoryTTF( font_awesome_binary, sizeof font_awesome_binary, 11 * scale, &icons_config, icon_ranges );

    logo = GetIO( ).Fonts->AddFontFromMemoryTTF( logo_ttf, sizeof logo_ttf, 48 * scale );

    ImGui::GetIO( ).FontGlobalScale = 1 / scale;

    m_tabs = {
        { crypt_string("Tab 1"), 0 },
        { crypt_string("Tab 2"), 2 } };

    m_subtabs = {
        { crypt_string( "Subtab 1" ), 0 },
        { crypt_string( "Subtab 2" ), 0 },

        { crypt_string( "Subtab 3" ), 1 } };
}

void add_rainbow_shadow( ImDrawList* draw, const ImRect& rect, int rounding )
{
    ImVec2 const p = rect.Min;
    ImVec2 const size = rect.GetSize( );

    float const time = GetTime( );
    float const speed = 0.3f;

    int const vtx_start = draw->VtxBuffer.Size;

    draw->AddShadowRect( p, p + size, IM_COL32_WHITE, 30.0f, { 0, 0 }, ImDrawFlags_ShadowCutOutShapeBackground, rounding );

    int const vtx_end = draw->VtxBuffer.Size;
    ImDrawVert* const vtx = draw->VtxBuffer.Data + vtx_start;
    ImDrawVert* const vtx_end_ptr = draw->VtxBuffer.Data + vtx_end;

    float const w = size.x;
    float const h = size.y;
    float const perimeter = ( w + h ) * 2.0f;

    for ( ImDrawVert* it = vtx; it < vtx_end_ptr; it++ )
    {
        ImVec2 const pos = it->pos;

        float t = 0.0f;
        if ( fabsf( pos.y - p.y ) < 0.5f )
            t = ( pos.x - p.x ) / perimeter;
        else if ( fabsf( pos.x - ( p.x + w ) ) < 0.5f )
            t = ( w + ( pos.y - p.y ) ) / perimeter;
        else if ( fabsf( pos.y - ( p.y + h ) ) < 0.5f )
            t = ( w + h + ( p.x + w - pos.x ) ) / perimeter;
        else if ( fabsf( pos.x - p.x ) < 0.5f )
            t = ( w + h + w + ( p.y + h - pos.y ) ) / perimeter;

        float const hue = fmodf( time * speed + t, 1.0f );
        ImU32 const col = ImColor::HSV( hue, 1.0f, 1.0f, 1.0f );

        it->col = ( it->col & 0xFF000000 ) | ( col & 0x00FFFFFF );
    }
}

void c_user_interface::render( )
{
    static c_animator child_alpha { float( 0.f ) };

    PushStyleVar( ImGuiStyleVar_WindowPadding, { 0, 0 } );
    PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0 );

    SetNextWindowSize( { 620, 370 } );
    SetNextWindowPos( { ImGui::GetIO( ).DisplaySize.x / 2 - 620 / 2, ImGui::GetIO( ).DisplaySize.y / 2 - 370 / 2 }, ImGuiCond_Once );

    Begin( ay_obfuscate( "##TEKCT HAPUCAJI DMITRII UTKIN =)" ), 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus );
    {
        ImRect const wnd_rect = GetCurrentWindow( )->Rect( );
        ImDrawList* draw_list = GetWindowDrawList( );

        /*background style*/ [ wnd_rect, draw_list ]( ) -> void {
            int vert_start_idx = draw_list->VtxBuffer.Size;
            draw_list->AddRectFilled( wnd_rect.Min, wnd_rect.Max, ImColor( 16, 16, 28 ), GetStyle( ).WindowRounding );
            int vert_end_idx = draw_list->VtxBuffer.Size;

            ShadeVertsLinearColorGradientKeepAlpha( draw_list, vert_start_idx, vert_end_idx, wnd_rect.Min, wnd_rect.Max + ImVec2( 600, 600 ), ImColor( 16, 16, 28 ), ImColor( 27, 15, 61 ) );
        }( );

        float const left_side_width = 140;

        { // left side of this fucking дерьмо ебанное
            SetCursorPos( { 0, 0 } );

            BeginChild( ay_obfuscate( "##left_side" ), { left_side_width, wnd_rect.GetHeight( ) }, 0 );
            {
                PushFont( logo );
                const char* glyph = "A";
                ImVec2 const gsize = CalcTextSize( glyph );
                SetCursorPos( { left_side_width / 2 - gsize.x / 2, 20 } );

                ImRect const fill_rect = {
                    GetCursorScreenPos( ),
                    GetCursorScreenPos( ) + gsize };

                int vert_start_idx = GetWindowDrawList( )->VtxBuffer.Size;
                Text( glyph );
                int vert_end_idx = GetWindowDrawList( )->VtxBuffer.Size;

                ShadeVertsLinearColorGradientKeepAlpha( GetWindowDrawList( ), vert_start_idx, vert_end_idx, fill_rect.Min, fill_rect.Max, ImColor( 218, 221, 216 ), ImColor( 234, 193, 251 ) );

                PopFont( );

                ItemSize( { 0, 10 } );

                for ( int it { 0 }; it < m_tabs.size( ); it++ )
                {
                    ui_tab_t& tab = m_tabs[ it ];

                    ImVec2 const current_cursor = GetCursorScreenPos( );
                    ImVec2 const tab_btn_size = { left_side_width, 26 };

                    if ( InvisibleButton( tab.str_name.c_str( ), tab_btn_size ) )
                    {
                        m_current = it;
                        m_sub = tab.default_subtab;
                        child_alpha.set< float >( 0.f );
                    }

                    ImRect const tab_rect = {
                        current_cursor + ImVec2( 11, 0 ),
                        current_cursor + tab_btn_size - ImVec2( 10, 0 ) };

                    auto draw = GetWindowDrawList( );

                    tab.c_standalone.update( ( it == m_current ) ? 1.f : 0.f );

                    tab.c_text.update( ( it == m_current ) ? ImColor( 120, 123, 173 ).Value : ImColor( 220, 220, 220 ).Value );
                    tab.c_background.update( ( it == m_current ) ? ImColor( 26, 27, 47 ).Value : ImColor( 26, 27, 47, 0 ).Value );
                    tab.c_outline.update( ( it == m_current ) ? ImColor( 255, 255, 255, 8 ).Value : ImColor( 255, 255, 255, 0 ).Value );

                    int vert_start_idx = draw_list->VtxBuffer.Size;
                    draw->AddRectFilled( tab_rect.Min, tab_rect.Max, ImColor( tab.c_background.get< ImVec4 >( ) ), 4 );
                    int vert_end_idx = draw_list->VtxBuffer.Size;

                    ShadeVertsLinearColorGradientKeepAlpha( draw_list, vert_start_idx, vert_end_idx, tab_rect.Min, tab_rect.Max, ImColor( 15, 16, 34 ), ImColor( 26, 27, 47 ) );

                    draw->AddRect( tab_rect.Min, tab_rect.Max, ImColor( tab.c_outline.get< ImVec4 >( ) ), 4, 0, 1 );

                    ImVec2 const text_size = CalcTextSize( tab.str_name.c_str( ) );

                    float const text_pd = tab_rect.GetHeight( ) / 2 - text_size.y / 2;
                    draw->AddText( tab_rect.Min + ImVec2( text_pd * 2, text_pd ), ImColor( tab.c_text.get< ImVec4 >( ) ), tab.str_name.c_str( ) );
                }

                auto const render_user_info = [ & ]( const std::string& username, bool is_premium, const ImTextureID& pfp ) -> void {
                    auto* wnd = GetCurrentWindow( );
                    auto const wnd_rect = wnd->Rect( );

                    ImRect const rect = {
                        { wnd_rect.Min.x + 11, wnd_rect.Max.y - 10 - 38 },
                        wnd->Rect( ).Max - ImVec2( 10, 10 ),
                    };

                    GetWindowDrawList( )->AddShadowRect( rect.Min, rect.Max, ImColor( 0, 0, 0, 100 ), 24, { 0, 0 }, 0, 64 );

                    GetWindowDrawList( )->AddRectFilled( rect.Min, rect.Max, ImColor( 255, 255, 255, 7 ), 4 );

                    GetWindowDrawList( )->AddRect( rect.Min, rect.Max, ImColor( 27, 29, 42 ), 4 );

                    float const radius = rect.GetHeight( ) / 2 - 6;

                    GetWindowDrawList( )->AddShadowCircle( { rect.Min.x + radius + 6, rect.GetCenter( ).y }, radius, ImColor( 0, 0, 0 ), 30, { 0, 0 }, 0, 64 );

                    if (pfp)
                        GetWindowDrawList( )->AddImageRounded( pfp, { rect.Min.x + 6, rect.GetCenter( ).y - radius }, { rect.Min.x + radius * 2 + 6, rect.GetCenter( ).y + radius }, { 0, 0 }, { 1, 1 }, ImColor( 1.f, 1.f, 1.f, ImGui::GetStyle( ).Alpha ), 120 );

                    GetWindowDrawList( )->AddCircle( { rect.Min.x + radius + 6, rect.GetCenter( ).y }, radius, ImColor( 255, 255, 255 ), 64 );

                    ImVec2 const upper_text_size = CalcTextSize( username.c_str( ) );
                    std::string const type_text = is_premium ? crypt_string( "Premium" ) : crypt_string( "Free" );

                    GetWindowDrawList( )->PushClipRect( rect.Min + ImVec2( 3, 3 ), rect.Max - ImVec2( 3, 3 ) );
                    GetWindowDrawList( )->AddText( { rect.Min.x + radius * 2 + 6 * 2, rect.GetCenter( ).y - upper_text_size.y + 1 }, ImColor( 255, 255, 255 ), username.c_str( ) );
                    GetWindowDrawList( )->AddText( { rect.Min.x + radius * 2 + 6 * 2, rect.GetCenter( ).y - 1 }, is_premium ? ImColor( 245, 215, 66 ) : ImColor( 155, 155, 155 ), type_text.c_str( ) );
                    GetWindowDrawList( )->PopClipRect( );
                };

                render_user_info( "@imguiseller", true, 0);

                EndChild( );
            }

            // draw_list->AddLine( { wnd_rect.Min.x + left_side_width, wnd_rect.Min.y }, { wnd_rect.Min.x + left_side_width, wnd_rect.Max.y }, ImColor( 41, 43, 55 ), 1 );
        }

        float const top_chuj_size = 40;

        { // main shit этого дерьма
            SetCursorPos( { left_side_width, 0 } );

            BeginChild( ay_obfuscate( "##top" ), { GetContentRegionAvail( ).x, top_chuj_size }, 0 );
            {
                SetCursorPosX( GetCursorPosX( ) + 5 );

                for ( int it { 0 }; it < m_subtabs.size( ); it++ )
                {
                    ui_subtab_t& subtab = m_subtabs[ it ];

                    if ( subtab.parent_tab != m_current )
                        continue;

                    ImVec2 const current_cursor = GetCursorScreenPos( );
                    ImVec2 const tab_btn_size = { CalcTextSize( subtab.str_name.c_str( ) ).x + 20, top_chuj_size };

                    if ( InvisibleButton( subtab.str_name.c_str( ), tab_btn_size ) )
                    {
                        m_sub = it;
                        child_alpha.set< float >( 0.f );
                    }

                    ImRect const tab_rect = {
                        current_cursor + ImVec2( 0, 6 ),
                        current_cursor + tab_btn_size - ImVec2( 0, 5 ) };

                    auto* draw = GetWindowDrawList( );

                    subtab.c_standalone.update( ( it == m_sub ) ? 1.f : 0.f );

                    subtab.c_text.update( ( it == m_sub ) ? ImColor( 120, 123, 173 ).Value : ImColor( 220, 220, 220 ).Value );
                    // subtab.c_background.update( ( it == m_sub ) ? ImColor( 255, 255, 255, 4 ).Value : ImColor( 255, 255, 255, 0 ).Value );
                    // subtab.c_outline.update( ( it == m_sub ) ? ImColor( 41, 43, 55 ).Value : ImColor( 41, 43, 55, 0 ).Value );

                    // draw->AddRect( tab_rect.Min, tab_rect.Max, ImColor( subtab.c_outline.get< ImVec4 >( ) ), 4 );

                    ImVec2 const text_size = CalcTextSize( subtab.str_name.c_str( ) );

                    draw->AddText( tab_rect.GetCenter( ) - text_size / 2, ImColor( subtab.c_text.get< ImVec4 >( ) ), subtab.str_name.c_str( ) );

                    SameLine( 0, 0 );
                }

                EndChild( );
            }

            // draw_list->AddLine( { wnd_rect.Min.x + left_side_width, wnd_rect.Min.y + top_chuj_size }, { wnd_rect.Max.x, wnd_rect.Min.y + top_chuj_size }, ImColor( 41, 43, 55 ), 1 );
        }

        {
            int vert_start_idx = draw_list->VtxBuffer.Size;
            draw_list->AddLine( { wnd_rect.Min.x + left_side_width, wnd_rect.Min.y }, { wnd_rect.Min.x + left_side_width, wnd_rect.Max.y }, ImColor( 41, 43, 55 ), 1 );
            draw_list->AddLine( { wnd_rect.Min.x + left_side_width, wnd_rect.Min.y + top_chuj_size }, { wnd_rect.Max.x, wnd_rect.Min.y + top_chuj_size }, ImColor( 41, 43, 55 ), 1 );
            int vert_end_idx = draw_list->VtxBuffer.Size;

            ShadeVertsLinearColorGradientKeepAlpha( draw_list, vert_start_idx, vert_end_idx, wnd_rect.Min, wnd_rect.Max, ImColor( 41, 43, 55 ), ImColor( 21, 23, 35 ) );
        }

        child_alpha.update< float >( 1.f );

        PushStyleVar( ImGuiStyleVar_Alpha, child_alpha.get< float >( ) );

        {
            // dawg

            SetCursorPos( { left_side_width + 11, top_chuj_size + 1 + ( 35 * ( 1.0f - child_alpha.get< float >( ) ) ) } );

            float const width = GetContentRegionAvail( ).x / 2 - 0.5f;

            PushStyleVar( ImGuiStyleVar_ItemSpacing, { 10, 10 } );

            BeginChild( ( m_tabs[ m_current ].str_name + std::to_string( m_sub ) ).c_str( ), { width, GetContentRegionAvail( ).y }, ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar );
            {
                ItemSize( { 0, 0 } );

                show_features( m_current, m_sub, 1 );

                ItemSize( { 0, 0 } );

                internal_scroll( crypt_string( "##left_window" ) );

                EndChild( );
            }

            SameLine( 0, 0 );

            BeginChild( ( m_tabs[ m_current ].str_name + std::to_string( m_sub ) + "2" ).c_str( ), { width, GetContentRegionAvail( ).y }, ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar );
            {
                ItemSize( { 0, 0 } );

                show_features( m_current, m_sub, 2 );

                ItemSize( { 0, 0 } );

                internal_scroll( crypt_string( "##right_window" ) );

                EndChild( );
            }

            PopStyleVar( );
        }

        PopStyleVar( );

        {
            int vert_start_idx = draw_list->VtxBuffer.Size;
            draw_list->AddRect( wnd_rect.Min, wnd_rect.Max, ImColor( 41, 43, 55 ), GetStyle( ).WindowRounding, 0, 2 );
            int vert_end_idx = draw_list->VtxBuffer.Size;

            ShadeVertsLinearColorGradientKeepAlpha( draw_list, vert_start_idx, vert_end_idx, wnd_rect.Min, wnd_rect.Max, ImColor( 41, 43, 55 ), ImColor( 21, 23, 35 ) );
        }

        End( );
    }

    PopStyleVar( 2 );
}