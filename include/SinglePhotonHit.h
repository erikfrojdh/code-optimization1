#ifndef SINGLEPHOTONHIT_H
#define SINGLEPHOTONHIT_H
#include <cstddef>
#include <cstdint>

enum class QuadEnum : int64_t {
    TOP_LEFT = 0,
    TOP_RIGHT = 1,
    BOTTOM_LEFT = 2,
    BOTTOM_RIGHT = 3,
    UNDEFINED_QUADRANT = -1
};

#pragma pack(push, 1) //May hurt performance but needed for writing to disk
template <size_t sizex, size_t sizey>
struct SinglePhotonHit {
    int32_t iframe;              /**< frame number */
    int16_t x;                   /**< x-coordinate of the center of hit */
    int16_t y;                   /**< x-coordinate of the center of hit */
    double rms;                  /**< noise of central pixel l -- at some point it can be removed*/
    double ped;                  /**< pedestal of the central pixel -- at some point it can be removed*/
    double tot;                  /**< sum of the 3x3 cluster */
    QuadEnum quad;               /**< quadrant where the photon is located */
    double quadTot;              /**< sum of the maximum 2x2cluster */
    int32_t dHC;                 // doubleHitCheck
    int16_t dPC;                 // deadPixelCheck
    int32_t dx;                  /**< size of data cluster in x */
    int32_t dy;                  /**< size of data cluster in y */
    int32_t data[sizex * sizey]; //stack allocated data
};
#pragma pack(pop)

#pragma pack(push, 1) //May hurt performance but needed for writing to disk
template <size_t sizex, size_t sizey>
struct SmallSinglePhotonHit {
    int16_t x;     /**< x-coordinate of the center of hit */
    int16_t y;
    double tot;     /**< sum of the 3x3 cluster */
    int16_t dHC;
    uint16_t data[sizex * sizey]; //stack allocated data
};
#pragma pack(pop)

#endif
