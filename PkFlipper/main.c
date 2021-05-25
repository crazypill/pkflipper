//
//  main.c
//  PkFlipper
//
//  Created by Alex Lelievre on 5/24/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <mach/mach_time.h>
#include <unistd.h>

#define num_bits        200
#define count_per_sec   20
#define count_period    60
#define sleep_period_us (1000000 / count_per_sec)
#define num_obs         (count_per_sec * count_period)

#define num_bits_as_num_longs (num_bits / 32)



int count_bits( uint32_t value )
{
    int bit_count = 0;
    for( int i = 0; i < 32; i++ )
    {
        if( value & (1 << i) )
            ++bit_count;
    }
    return bit_count;
}


uint32_t myrandom( void )
{
    return arc4random();
//    return 0xFFFFFFFF;      // for testing to prove I'm getting 200 bits when all bits are lit
}



int main( int argc, const char * argv[] )
{
    printf("PkFlipper\n");
    uint32_t seconds = 0;

    float chi_squared     = 0;
    int   bit_count       = 0;
    bool  intent_higher   = true;
    
    while( 1 )
    {
        // do this loop 20 times and wait 50ms inbetween gives us a full second of readings...
        for( int x = 0; x < count_per_sec; x++ )
        {
            bit_count = 0;
            for( int i = 0; i < num_bits_as_num_longs; i++ )
            {
                uint32_t rando = myrandom();
                bit_count += count_bits( rando );
            }

            // also add remaining 8 bits of data (200/32 has a remainder of 8)
            bit_count += count_bits( myrandom() & 0xFF );
            chi_squared += powf(bit_count - 100, 2) / 100;
            usleep( sleep_period_us );
        }

        ++seconds;

        printf( "intent: %s, chi_squared: %3.0f, seconds remaining = %d\n", intent_higher ? "higher" : "lower", chi_squared, count_period - seconds );
//        printf( "bit_count: %d, chi_squared: %4.4f, seconds remaining = %d\n", bit_count, chi_squared, 60 - seconds );

        if( seconds >= count_period )
        {
            for( int i = 1; i <= count_period; i++ )
            {
                sleep( 1 );
                printf( "last score: %3.0f, last intent: %s, next session in %d secs\n", chi_squared, intent_higher ? "higher" : "lower", count_period - i );
            }

            seconds = 0;
            chi_squared = 0;
//            intent_higher = !intent_higher;
            intent_higher = myrandom() & 1; // roll a random number and see if it's odd
        }
    }
    return 0;
}
