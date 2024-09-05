#ifndef __UI_DEFINES_H__
#define __UI_DEFINES_H__

    /* **-₋₋-TEXT VALUES-₋₋-** */
    /* ||                   || */        
    /* **-⁻⁻-TEXT VALUES-⁻⁻-** */


    /* Left X position of the properties menu */
    #define UI_TXT_LEFT_X 810

    /* Right X position of the properties menu */
    #define UI_TXT_RIGHT_X 884

    /* Table of Y positions for the texts */
    const int Y_POSES_TEXT[2][10] = {{16,56,96 ,136,176,216,256,296},
                                     {26,66,106,146,186,226,266,306}};

    /* **-₋₋-TEXT VALUES-₋₋-** */
    /* ||                   || */        
    /* **-⁻⁻-TEXT VALUES-⁻⁻-** */



    /* **-₋₋-BUTTON VALUES-₋₋-** */
    /* ||                     || */        
    /* **-⁻⁻-BUTTON VALUES-⁻⁻-** */
    
    
    /* First row Y position (up arrow) */
    #define UI_BT_ONE_UP_Y 20
    
    /* First row Y position (down arrow) */
    #define UI_BT_ONE_DOWN_Y 38


    /* Second row Y position (up arrow) */
    #define UI_BT_TWO_UP_Y (20+40)
    
    /* Second row Y position (down arrow) */
    #define UI_BT_TWO_DOWN_Y (38+40)


    /* Third row Y position (up arrow) */
    #define UI_BT_THREE_UP_Y (20+40*2)
    
    /* Third row Y position (down arrow) */
    #define UI_BT_THREE_DOWN_Y (38+40*2)


    /* Fourth row Y position (up arrow) */
    #define UI_BT_FOUR_UP_Y (20+40*3)
    
    /* Fourth row Y position (down arrow) */
    #define UI_BT_FOUR_DOWN_Y (38+40*3)


    /* Fifth row Y position (up arrow) */
    #define UI_BT_FIVE_UP_Y (20+40*4)
    
    /* Fifth row Y position (down arrow) */
    #define UI_BT_FIVE_DOWN_Y (38+40*4)


    /* Sixth row Y position (up arrow) */
    #define UI_BT_SIX_UP_Y (20+40*4)
    
    /* Sixth row Y position (down arrow) */
    #define UI_BT_SIX_DOWN_Y (38+40*4)


    /* Table of Y positions for the buttons */
    /* [...][0] - up    */
    /* [...][1] - down  */
    const int Y_POSES[2][10] = { {UI_BT_ONE_UP_Y  ,UI_BT_TWO_UP_Y  ,UI_BT_THREE_UP_Y  ,UI_BT_FOUR_UP_Y  ,UI_BT_FIVE_UP_Y  ,UI_BT_SIX_UP_Y  ,0,0,0,0},
                                 {UI_BT_ONE_DOWN_Y,UI_BT_TWO_DOWN_Y,UI_BT_THREE_DOWN_Y,UI_BT_FOUR_DOWN_Y,UI_BT_FIVE_DOWN_Y,UI_BT_SIX_DOWN_Y,0,0,0,0} };


    /* **-₋₋-BUTTON VALUES-₋₋-** */
    /* ||                     || */        
    /* **-⁻⁻-BUTTON VALUES-⁻⁻-** */


#endif
