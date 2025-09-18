//
// Created by Kai Tears on 06/08/2025.
//

#include "featurespages.hpp"

#include "elements.hpp"

#include "../ui.hpp"

using namespace ImGui;

void show_features( int tab, int sub, int child )
{
    switch ( tab )
    {
    case 0: // esp
    {
        switch ( sub )
        {
        case 0:
        {
            if ( child == 1 )
            {
                static bool toggle;
                elem::toggle("Toggle example", &toggle, "With configuration!", [] {
                    static bool internal;
                    elem::internal_toggle("Internal too?", &internal);
                    static int slider;
                    elem::slider_int("And slider??", &slider, -5, 5);
                    static ImVec4 col1 = { 1, 0, 0, 1 };
                    static ImVec4 col2 = { 0, 1, 0, 1 };
                    elem::color("Colorpicker", { &col1, &col2 });
                    });
            } else if ( child == 2 )
            {
                elem::simple_category("Category example"); {
                    static bool asd;
                    elem::internal_toggle("Simple switch", &asd);
                    elem::end_category();
                }

                static bool toggle;
                elem::toggle("Another toggle", &toggle, "Lorem ipsum paster sin shluxi", [] {
                    static int combo;
                    elem::combo("Combo", &combo, {"First", "Second", "Third"});
                    });
            }
        }
        break;

        };
    }
    break;
    }
}