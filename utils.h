#ifndef UTILS_H
#define UTILS_H

#include "includes.h"

class Utils {
public:
    static void centerWindowInScreen(QWidget *widget);

    // Shuffle a 1D vector <v>.
    template<typename T>
    static void shuffle(QVector<T> &v);

    // Generate a random integer in range [min, max).
    static int randomInt(int min, int max);

    // Remove all widgets from a layout.
    static void removeAllWidgets(QLayout *const layout);

    // Remove all widgets from a widget.
    static void removeAllWidgets(QWidget *const widget);
};


// Has to be implemented in header file.
template <typename T>
void Utils::shuffle(QVector<T> &v) {
    auto rng = std::default_random_engine {};
    std::shuffle(v.begin(), v.end(), rng);
}

#endif // UTILS_H
