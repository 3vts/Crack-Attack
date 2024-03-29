#ifndef GRID_H
#define GRID_H

#include <assert.h>

#include "BlockManager.h"
#include "GarbageManager.h"
#include "LevelLights.h"

// grid element states
#define GR_EMPTY                 (1 << 0)
#define GR_BLOCK                 (1 << 1)
#define GR_GARBAGE               (1 << 2)
#define GR_FALLING               (1 << 3)
#define GR_IMMUTABLE             (1 << 4)
#define GR_SHATTERING            (1 << 5)
#define GR_HANGING               (1 << 6)

// pattern types
#define PT_HORIZONTAL            (1 << 0)
#define PT_VERTICAL              (1 << 1)

class Block;
class Garbage;
class ComboTabulator;

class CheckRegistryElement {
public:
    bool mark;
    ComboTabulator *combo;
};

class GridElement {
public:
    int state;
    int resident_type;
    void *resident;
};

/* static */ class Grid {
public:
    static void gameStart (   );
    static void timeStep (   );
    static bool shiftGridUp (   );
    
    static inline void dump (   )
    {
        std::cout << '\n';
        for (int y = GC_PLAY_HEIGHT; y--; ) {
            for (int x = 0; x < GC_PLAY_WIDTH; x++)
                switch (grid[x][y].state) {
                    case GR_EMPTY: std::cout << ' '; break;
                    case GR_BLOCK: std::cout << 'B'; break;
                    case GR_GARBAGE: std::cout << '@'; break;
                    case GR_FALLING: std::cout << '*'; break;
                    case GR_IMMUTABLE: std::cout << '#'; break;
                    case GR_SHATTERING: std::cout << 'X'; break;
                    case GR_HANGING: std::cout << '+'; break;
                    default: std::cout << '!'; break;
                }
            std::cout << '\n';
        }
        std::cout << std::endl;
        
        for (int n = 0; n < GC_PLAY_WIDTH; n++)
            std::cout << blockAt(n, 0).state << std::endl;
    }
    
    static inline int stateAt ( int x, int y )
    {
        return grid[x][y].state;
    }
    
    static inline int residentTypeAt ( int x, int y )
    {
        return grid[x][y].resident_type;
    }
    
    static inline Block &blockAt ( int x, int y )
    {
        assert(grid[x][y].resident_type == GR_BLOCK);
        return *((Block *) grid[x][y].resident);
    }
    
    static inline Garbage &garbageAt ( int x, int y )
    {
#ifndef NDEBUG
        if (!(grid[x][y].resident_type == GR_GARBAGE)) {
            dump();
            DUMP(x);
            DUMP(y);
            DUMP(grid[x][y].state);
            DUMP(grid[x][y].resident_type);
            DUMP(top_occupied_row);
            DUMP(top_effective_row);
        }
#endif
        assert(grid[x][y].resident_type == GR_GARBAGE);
        assert(y < GC_PLAY_HEIGHT);
        return *((Garbage *) grid[x][y].resident);
    }
    
    static inline int flavorAt ( int x, int y )
    {
        assert(grid[x][y].state == GR_BLOCK);
        return ((Block *) grid[x][y].resident)->flavor;
    }
    
    static inline bool matchAt ( int x, int y, Block &block )
    {
        assert(grid[x][y].state == GR_BLOCK);
        return BlockManager::flavorMatch(block, *(Block *) grid[x][y].resident);
    }
    
    static inline void changeState ( int x, int y, void *resident, int state )
    {
        assert(grid[x][y].resident == resident);
        grid[x][y].state = state;
    }
    
    static inline void addBlock ( int x, int y, Block *resident, int state )
    {
        assert(x < GC_PLAY_WIDTH);
        assert(y < GC_PLAY_HEIGHT);
        assert(grid[x][y].state & GR_EMPTY);
        grid[x][y].resident = resident;
        grid[x][y].resident_type = GR_BLOCK;
        grid[x][y].state = state;
    }
    
    static inline void addGarbage ( int x, int y, Garbage *resident, int state )
    {
        assert(grid[x][y].state & GR_EMPTY);
        assert(x < GC_PLAY_WIDTH);
        assert(y < GC_PLAY_HEIGHT);
        grid[x][y].resident = resident;
        grid[x][y].resident_type = GR_GARBAGE;
        grid[x][y].state = state;
    }
    
    static inline void remove ( int x, int y, void *resident )
    {
        assert(grid[x][y].resident == resident);
        grid[x][y].resident = null;
        grid[x][y].resident_type = GR_EMPTY;
        grid[x][y].state = GR_EMPTY;
    }
    
    static inline void requestEliminationCheck ( Block &block,
                                                ComboTabulator *combo = null )
    {
        check_registry[block.id].mark = true;
        check_registry[block.id].combo = combo;
        check_count++;
    }
    
    static inline bool checkSafeHeightViolation (   )
    {
        return top_effective_row >= GC_SAFE_HEIGHT - 1;
    }
    
    static inline void notifyImpact ( int y, int height )
    {
        int impact_top = y + height - 1;
        
        if (top_effective_row < impact_top) {
            top_effective_row = impact_top;
            LevelLights::levelRaise(top_effective_row);
        }
        
        LevelLights::notifyImpact(y, height);
    }
    
    // top row with anything in it, including initially falling garbage; used to
    // determine garbage drop height; updated in Grid::timeStep() and
    // GarbageManager::newFallingGarbage()
    static int top_occupied_row;
    
    // top row that's holding blocks or landed garbage; used for level lights and
    // safe height violation; updated in Grid::timeStep() and Grid::notifyImpact()
    static int top_effective_row;
    
    static bool gray_shatter;
    
private:
    static void handleEliminationCheckRequest ( Block &block,
                                               ComboTabulator *combo );
    
    static void shatterGarbage_inline_split_ ( int x, int y, Garbage *due_to );
    static inline void shatterGarbage ( int x, int y, Garbage *due_to = null )
    {
        if (!(stateAt(x, y) & GR_GARBAGE)) return;
        shatterGarbage_inline_split_(x, y, due_to);
    }
    
    static GridElement grid[GC_PLAY_WIDTH][GC_PLAY_HEIGHT];
    static CheckRegistryElement check_registry[GC_BLOCK_STORE_SIZE];
    static int check_count;
    
    static int shatter_count;
    static int shatter_top;
    static int shatter_bottom;
};

#endif
