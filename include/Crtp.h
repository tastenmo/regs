/**
 * @file Crtp.h
 * @author Martin Heubuch (martin.heubuch@escad.de)
 * @brief
 * @version 0.1
 * @date 2022-11-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

namespace utils
{

    template <typename T>
    struct crtp
    {
        T &underlying() { return static_cast<T &>(*this); }
        T const &underlying() const { return static_cast<T const &>(*this); }
    };

}