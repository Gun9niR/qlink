#include "uiconfig.h"

UiConfig::UiConfig(int windowH, int windowW): windowH(windowH),
    windowW(windowW)
{

}

int UiConfig::windowHeight() const
{
    return this->windowH;
}

int UiConfig::windowWidth() const
{
    return this->windowW;
}

// All fields are empty upon construction, need to set them afterwards.
UiConfigBuilder::UiConfigBuilder()
{

}

unique_ptr<UiConfigBuilder> UiConfigBuilder::windowSize(int h, int w)
{
    auto builder = unique_ptr<UiConfigBuilder>(new UiConfigBuilder());
    builder->windowHeight = h;
    builder->windowWidth = w;
    return builder;
}

unique_ptr<UiConfig> UiConfigBuilder::build()
{
    return unique_ptr<UiConfig>(new UiConfig(windowHeight, windowWidth));
}
