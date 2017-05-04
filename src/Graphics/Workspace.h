#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <deque>
#include <vector>
#include <assert.h>
#include "../utility.h"



namespace video{

    class Window;

    /**
       @brief Represent a Workspace i.e a screen containing a set of windows.

       There is @ref _totalNumber number of Workspace wich can be switched to.

       The workspace number 0 is the kernel workspace, it is here only for debug.
       If it is active, all usermode programs are stopped and OS State can be inspected

       The Workspace contain a list of window in a precise order: the order of drawing.
       The first one has the "focus", all input goes to it(except when clicking outside).

       Workspace::draw draw the @ref active workspace.

     */

    class Workspace{
        /// Workspace 0 is kernel console, other are for user mode.
        uint _number;
        /// List of window. _win."first" has focus.
        std::deque<Window*> _wins;
        /// Total number of workspaces
        static const uint _totalNumber = 5;
        /// All workspaces are statically allocated here.
        static Workspace _elems[_totalNumber];
    public:
        /// Access Workspace number i
        static Workspace& get(uint i){
            assert(i < _totalNumber);
            return _elems[i];
        }

        static void init();
        /// The active workspace
        static uint active;
        /// Draw this Workspace on screen
        void drawMe();
        /// Draw the active workspace on screen
        static void draw(){
            assert(active < _totalNumber);
            _elems[active].drawMe();
        }
        /// Alt + Tab : cycle through windows.
        void cycle();

        /// Add a windows, it has focus by default.
        void addWin(Window* win){
            _wins.push_front(win);
        }

    };
}

#endif
