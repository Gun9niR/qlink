#ifndef UICONFIG_H
#define UICONFIG_H

#include "includes.h"

// Stores UI information for a window. Written as a separate class for the sake
// of extendibility.
class UiConfig {
    friend class UiConfigBuilder;

private:
    const int windowH;
    const int windowW;

    UiConfig(int windowH, int windowW);

public:
    int windowHeight() const;
    int windowWidth() const;
};


// Uses builder pattern to make sure the necessary parameters are set, a bit
// over-engineering, but it's just for practice purpose.
class UiConfigBuilder {
private:
    int windowHeight;
    int windowWidth;

    UiConfigBuilder();
public:
    // windowHeight and windowWidth are necessary parameters.
    static unique_ptr<UiConfigBuilder> windowSize(int h, int w);

    unique_ptr<UiConfig> build();
};

#endif // UICONFIG_H
