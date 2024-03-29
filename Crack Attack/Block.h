#ifndef BLOCK_H
#define BLOCK_H

#include "Game.h"
#include "ComboTabulator.h"

// states of blocks
#define BS_STATIC                   (1 << 0)
#define BS_SWAPPING                 (1 << 1)
#define BS_FALLING                  (1 << 2)
#define BS_DYING                    (1 << 3)
#define BS_AWAKING                  (1 << 4)
#define BS_SWAP_DIRECTION_MASK      (1 << 5)

// pop directions
#define BR_DIRECTION_1              (1 << 0)
#define BR_DIRECTION_2              (1 << 1)
#define BR_DIRECTION_3              (1 << 2)
#define BR_DIRECTION_4              (1 << 3)

class Block {
public:
    void initializeStatic ( int _x, int _y, int _flavor );
    void initializeAwaking ( int _x, int _y, int _flavor, int pop_delay,
                            int awake_delay, ComboTabulator *combo, int _pop_color );
    void timeStep (   );
    void startFalling ( ComboTabulator *combo = null, bool no_hang = false );
    void startDying ( ComboTabulator *combo, int spark_number );
    void startSwapping ( int direction );
    void finishSwapping ( int s_x );
    
    inline void beginComboInvolvement ( ComboTabulator *new_combo )
    {
        if (current_combo)
            current_combo->decrementInvolvement();
        current_combo = new_combo;
        current_combo->incrementInvolvement();
    }
    
    inline void endComboInvolvement ( ComboTabulator *old_combo )
    {
        if (current_combo && current_combo == old_combo) {
            current_combo->decrementInvolvement();
            current_combo = null;
        }
    }
    
    // free store id
    int id;
    
    // block color
    int flavor;
    
    // grid position; if between grid locations, this is the location of our
    // lowest and left most edge
    int x, y;
    
    // fine position control; GC_STEPS_PER_GRID number of increments per grid
    int f_y;
    
    // block state
    int state;
    
    // time until pop - also used for other random crap; should be renamed
    int pop_alarm;
    
    // current combo we're involved with, if any
    ComboTabulator *current_combo;
    
    // time until awakening
    int alarm;
    
    // death rotation axis
    float axis_x, axis_y;
    
    // direction of rotation upon popping
    int pop_direction;
    
    // the block color before popping
    int pop_color;
    
    // used by the extreme effects
    int X;
};

#endif
