/**
 * @file 1_player_health.c
 * @brief Lesson 2, Exercise 1: one possible solution
 * @author dunamismax
 * @date 11-20-2026
 *
 * THE EXERCISE:
 * Add a `double` variable for the player's health, set it to 87.5, and print
 * it with one decimal place.
 */

#include <stdio.h>

int main(void)
{
    // 87.5 has a decimal point, so it needs a floating-point type. The
    // course uses `double` for those.
    double player_health = 87.5;

    // `%f` alone would print 87.500000. The `.1` between `%` and `f` asks
    // for exactly one digit after the decimal point.
    printf("Player health: %.1f\n", player_health);

    return 0;
}

/*
 * HOW TO COMPILE AND RUN:
 *   gcc -Wall -Wextra -std=c17 -o 1_player_health 1_player_health.c
 *   ./1_player_health
 */
