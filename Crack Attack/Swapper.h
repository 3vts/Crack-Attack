#ifndef SWAPPER_H
#define SWAPPER_H

#include "BlockManager.h"

#define SS_SWAPPING             (1 << 0)
#define SS_MOVE_PAUSE           (1 << 1)
#define SS_MOVE_UP              (1 << 2)
#define SS_MOVE_DOWN            (1 << 3)
#define SS_MOVE_LEFT            (1 << 4)
#define SS_MOVE_RIGHT           (1 << 5)
#define SS_MOVE_MASK            (SS_MOVE_UP | SS_MOVE_DOWN \
| SS_MOVE_LEFT | SS_MOVE_RIGHT)

#define SA_LEFT                 (1 << 0)
#define SA_RIGHT                (1 << 1)
#define SA_DISALLOWED           (1 << 2)

/* static */ class Swapper {
public:
    static void gameStart (   );
    static void timeStep (   );
    
    static inline void shiftUp (   )
    { y++; }
    
    static inline void notifyLanding ( int _x, int _y, Block &block,
                                      ComboTabulator *combo )
    {
        if (!(state & SS_SWAPPING)) return;
        if (_y - 1 != y) return;
        if (_x == x && (swap & SA_RIGHT)
            && BlockManager::flavorMatch(block, *right_block))
            right_block->beginComboInvolvement(combo);
        else if (_x == x + 1 && (swap & SA_LEFT)
                 && BlockManager::flavorMatch(block, *left_block))
            left_block->beginComboInvolvement(combo);
    }
    
    // the location of our left half
    static int x, y;
    
    // goes off when we're allowed move again
    static int move_pause_alarm;
    
    // goes off when a swap in complete
    static int swap_alarm;
    
    // swapper's state
    static int state;
    
    // factor of swap complete
    static float swap_factor;
    
    // the swapper's color;
    static int color;
    
private:
    // type of swap we're executing, if any
    static int swap;
    
    // the swapping blocks
    static Block *left_block, *right_block;
    
    // insures that the player releases a button look for another command; makes
    // the control more crisp
    static int button_down_move;
    static bool button_down_swap;
    
    // allows the user to queue up the next command before completion of
    // the current command
    static int queued_move;
    static bool queued_swap;
};

#endif
